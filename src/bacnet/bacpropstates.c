/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (C) 2008 John Minack

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to:
 The Free Software Foundation, Inc.
 59 Temple Place - Suite 330
 Boston, MA  02111-1307, USA.

 As a special exception, if other files instantiate templates or
 use macros or inline functions from this file, or you compile
 this file and link it with other works to produce a work based
 on this file, this file does not by itself cause the resulting
 work to be covered by the GNU General Public License. However
 the source code for this file must still be made available in
 accordance with section (3) of the GNU General Public License.

 This exception does not invalidate any other reasons why a work
 based on this file might be covered by the GNU General Public
 License.
 -------------------------------------------
####COPYRIGHTEND####*/
#include <stdint.h>
#include "bacnet/bacdcode.h"
#include "bacnet/npdu.h"
#include "bacnet/timestamp.h"
#include "bacnet/bacpropstates.h"

/** @file bacpropstates.c  Encode/Decode BACnet Application Property States */

int bacapp_decode_property_state(uint8_t *apdu, BACNET_PROPERTY_STATE *value)
{
    int len = 0;
    uint32_t len_value_type;
    int section_length;
    uint32_t enumValue;
    uint8_t tagnum;

    section_length =
        decode_tag_number_and_value(&apdu[len], &tagnum, &len_value_type);

    if (-1 == section_length) {
        return -1;
    }
    value->tag = (BACNET_PROPERTY_STATE_TYPE)tagnum;
    len += section_length;
    switch (value->tag) {
        case BOOLEAN_VALUE:
            value->state.booleanValue = decode_boolean(len_value_type);
            break;

        case BINARY_VALUE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.binaryValue = (BACNET_BINARY_PV)enumValue;
            break;

        case EVENT_TYPE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.eventType = (BACNET_EVENT_TYPE)enumValue;
            break;

        case POLARITY:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.polarity = (BACNET_POLARITY)enumValue;
            break;

        case PROGRAM_CHANGE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.programChange = (BACNET_PROGRAM_REQUEST)enumValue;
            break;

        case PROGRAM_STATE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.programState = (BACNET_PROGRAM_STATE)enumValue;
            break;

        case REASON_FOR_HALT:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.programError = (BACNET_PROGRAM_ERROR)enumValue;
            break;

        case RELIABILITY:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.reliability = (BACNET_RELIABILITY)enumValue;
            break;

        case STATE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.state = (BACNET_EVENT_STATE)enumValue;
            break;

        case SYSTEM_STATUS:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.systemStatus = (BACNET_DEVICE_STATUS)enumValue;
            break;

        case UNITS:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.units = (BACNET_ENGINEERING_UNITS)enumValue;
            break;

        case UNSIGNED_VALUE:
            if (-1 ==
                (section_length = decode_unsigned(&apdu[len], len_value_type,
                     &value->state.unsignedValue))) {
                return -1;
            }
            break;

        case LIFE_SAFETY_MODE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.lifeSafetyMode = (BACNET_LIFE_SAFETY_MODE)enumValue;
            break;

        case LIFE_SAFETY_STATE:
            if (-1 ==
                (section_length = decode_enumerated(
                     &apdu[len], len_value_type, &enumValue))) {
                return -1;
            }
            value->state.lifeSafetyState = (BACNET_LIFE_SAFETY_STATE)enumValue;
            break;

        default:
            return -1;
    }
    len += section_length;

    return len;
}

int bacapp_decode_context_property_state(
    uint8_t *apdu, uint8_t tag_number, BACNET_PROPERTY_STATE *value)
{
    int len = 0;
    int section_length;

    if (decode_is_opening_tag_number(&apdu[len], tag_number)) {
        len++;
        section_length = bacapp_decode_property_state(&apdu[len], value);

        if (section_length == -1) {
            len = -1;
        } else {
            len += section_length;
            if (decode_is_closing_tag_number(&apdu[len], tag_number)) {
                len++;
            } else {
                len = -1;
            }
        }
    } else {
        len = -1;
    }
    return len;
}

/**
 * @brief Encode the BACnetPropertyState
 * @param apdu  Pointer to the buffer for encoding into, or NULL for length
 * @param value  Pointer to the value used for encoding
 * @return number of bytes encoded, or zero if unable to encode
 */
int bacapp_encode_property_state(uint8_t *apdu, BACNET_PROPERTY_STATE *value)
{
    int len = 0; /* length of each encoding */

    if (value) {
        switch (value->tag) {
            case BOOLEAN_VALUE:
                len = encode_context_boolean(
                    apdu, 0, value->state.booleanValue);
                break;

            case BINARY_VALUE:
                len = encode_context_enumerated(
                    apdu, 1, value->state.binaryValue);
                break;

            case EVENT_TYPE:
                len = encode_context_enumerated(
                    apdu, 2, value->state.eventType);
                break;

            case POLARITY:
                len = encode_context_enumerated(
                    apdu, 3, value->state.polarity);
                break;

            case PROGRAM_CHANGE:
                len = encode_context_enumerated(
                    apdu, 4, value->state.programChange);
                break;

            case PROGRAM_STATE:
                len = encode_context_enumerated(
                    apdu, 5, value->state.programState);
                break;

            case REASON_FOR_HALT:
                len = encode_context_enumerated(
                    apdu, 6, value->state.programError);
                break;

            case RELIABILITY:
                len = encode_context_enumerated(
                    apdu, 7, value->state.reliability);
                break;

            case STATE:
                len =
                    encode_context_enumerated(apdu, 8, value->state.state);
                break;

            case SYSTEM_STATUS:
                len = encode_context_enumerated(
                    apdu, 9, value->state.systemStatus);
                break;

            case UNITS:
                len =
                    encode_context_enumerated(apdu, 10, value->state.units);
                break;

            case UNSIGNED_VALUE:
                len = encode_context_unsigned(
                    apdu, 11, value->state.unsignedValue);
                break;

            case LIFE_SAFETY_MODE:
                len = encode_context_enumerated(
                    apdu, 12, value->state.lifeSafetyMode);
                break;

            case LIFE_SAFETY_STATE:
                len = encode_context_enumerated(
                    apdu, 13, value->state.lifeSafetyState);
                break;
            default:
                break;
        }
    }
    return len;
}
