/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_COMMON_H
#define TEEP_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "qcbor/qcbor.h"

// Function results
#define TEEP_INVALID_LENGTH                 6
#define TEEP_VALUE_ERROR                    5
#define TEEP_CBOR_WITHOUT_SIGN1             4
#define TEEP_NO_MEMORY                      3
#define TEEP_UNEXPECTED_ERROR               2
#define TEEP_INVALID_TYPE_OF_ARGUMENT       1
#define TEEP_SUCCESS                        0

typedef struct teep_buf {
    size_t          len;
    const uint8_t   *ptr;
} teep_buf_t;

int32_t teep_print_hex_within_max(const uint8_t *array, const size_t size, const size_t size_max);
int32_t teep_print_hex(const uint8_t *array, size_t size);
int32_t teep_print_text(const uint8_t *text, size_t size);
void teep_print_error_string(const char *message);
void teep_print_debug_string(const char *message);
void teep_print_debug_string_uint32(const char *message, uint32_t value);
void teep_debug_print(QCBORDecodeContext *message,
                      QCBORItem *item,
                      QCBORError *error,
                      const char *func_name,
                      uint8_t expecting);
uint32_t teep_array_to_int32(const uint8_t *array, int32_t byte_count);
uint64_t teep_array_to_int64(const uint8_t *array);

#endif  // TEEP_COMMON_H
