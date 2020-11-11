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

// Function results
#define TEEP_NO_MEMORY                      3
#define TEEP_UNEXPECTED_ERROR               2
#define TEEP_INVALID_TYPE_OF_ARGUMENT       1
#define TEEP_SUCCESS                        0

typedef struct teep_buf {
    size_t          len;
    const uint8_t   *ptr;
} teep_buf_t;

void print_hex(const uint8_t *array, int32_t size);
void print_error_string(const char *message);
void print_debug_string(const char *message);
void print_debug_string_uint32(const char *message, uint32_t value);
uint32_t array_to_int32(const uint8_t *array, int32_t byte_count);
uint64_t array_to_int64(const uint8_t *array);

#endif  // TEEP_COMMON_H
