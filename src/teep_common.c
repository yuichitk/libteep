/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep/teep_common.h"
#include "teep/teep_message_data.h"

bool teep_cipher_suite_is_same(teep_cipher_suite_t a, teep_cipher_suite_t b) {
    return memcpy(&a, &b, sizeof(teep_cipher_suite_t)) ? false : true;
}

uint32_t teep_array_to_int32(const uint8_t *array, int32_t byte_count) {
    uint32_t value = 0;
    switch (byte_count) {
        case 1:
            value = (uint32_t)array[0];
            break;
        case 2:
            value = ((((uint32_t)array[0]) << 8) & 0x000000000000FF00) |
                ((((uint32_t)array[1]) << 0) & 0x00000000000000FF);
            break;
        case 4:
            value = ((((uint32_t)array[0]) << 24) & 0x00000000FF000000) |
                ((((uint32_t)array[1]) << 16) & 0x0000000000FF0000) |
                ((((uint32_t)array[2]) << 8) & 0x000000000000FF00) |
                ((((uint32_t)array[3]) << 0) & 0x00000000000000FF);
            break;
        default:
            break;
    }
    return value;
}

uint64_t teep_array_to_int64(const uint8_t *array) {
    uint64_t value = ((((uint64_t)array[0]) << 56) & 0xFF00000000000000) |
        ((((uint64_t)array[1]) << 48) & 0x00FF000000000000) |
        ((((uint64_t)array[2]) << 40) & 0x0000FF0000000000) |
        ((((uint64_t)array[3]) << 32) & 0x000000FF00000000) |
        ((((uint64_t)array[4]) << 24) & 0x00000000FF000000) |
        ((((uint64_t)array[5]) << 16) & 0x0000000000FF0000) |
        ((((uint64_t)array[6]) << 8) & 0x000000000000FF00) |
        ((((uint64_t)array[7]) << 0) & 0x00000000000000FF);
    return value;
}

bool teep_is_valid_mechanism(int64_t cose_mechanism_key) {
    switch (cose_mechanism_key) {
    case CBOR_TAG_COSE_SIGN1:
    case CBOR_TAG_SIGN:
    case CBOR_TAG_COSE_MAC0:
    case CBOR_TAG_MAC:
    case CBOR_TAG_COSE_ENCRYPT0:
    case CBOR_TAG_ENCRYPT:
        return true;
    default:
        return false;
    }
}

