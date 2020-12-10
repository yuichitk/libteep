/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep_common.h"

void teep_print_hex(const uint8_t *array, int32_t size) {
    for (int32_t i = 0; i < size; i++) {
        if (array[i] == 0) {
            printf("0x00 ");
            continue;
        }
        printf("%#04x ", (unsigned char)array[i]);
    }
}

void teep_print_text(const uint8_t *text, int32_t size) {
    if (size < 0) {
        return;
    }
    printf("\"%.*s\"", size, text);
}

void teep_print_error_string(const char *message) {
    printf("Error : %s\n", message);
}

void teep_print_debug_string(const char *message) {
    printf("Debug : %s\n", message);
}

void teep_print_debug_string_uint32(const char *message, uint32_t value) {
    printf("Debug : %s : %u\n", message, value);
}

void teep_debug_print(QCBORDecodeContext *message,
                      QCBORItem *item,
                      QCBORError *error,
                      const char *func_name,
                      uint8_t expecting) {
    size_t cursor = UsefulInputBuf_Tell(&message->InBuf);
    size_t len = UsefulInputBuf_GetBufferLength(&message->InBuf) - cursor;
    uint8_t *at = (uint8_t *)message->InBuf.UB.ptr + cursor;

    len = (len > 12) ? 12 : len;

    printf("DEBUG: %s\n", func_name);
    printf("msg[%ld:%ld] = ", cursor, cursor + len);
    teep_print_hex(at, len);
    printf("\n");

    if (*error != 0) {
        printf("    Error! nCBORError = %d\n", *error);
    }
    if (expecting != QCBOR_TYPE_ANY && expecting != item->uDataType) {
        printf("    item->uDataType %d != %d\n", item->uDataType, expecting);
    }
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
