/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_COMMON_H
#define TEEP_COMMON_H

#include "qcbor/qcbor.h"

// Function results
typedef enum teep_err {
    TEEP_SUCCESS                        = 0,
    TEEP_ERR_INVALID_TYPE_OF_ARGUMENT   = 1,
    TEEP_ERR_UNEXPECTED_ERROR           = 2,
    TEEP_ERR_NO_MEMORY                  = 3,
    TEEP_ERR_CBOR_WITHOUT_SIGN1         = 4,
    TEEP_ERR_INVALID_VALUE              = 5,
    TEEP_ERR_INVALID_LENGTH             = 6,
    TEEP_ERR_INVALID_MESSAGE_TYPE       = 7,
    TEEP_ERR_ON_HTTP_POST               = 8,
    TEEP_ERR_SIGNING_FAILED             = 9,
    TEEP_ERR_NO_SUPPORTED_VERSION       = 10,
    TEEP_ERR_NO_SUPPORTED_CIPHERSUITE   = 11,
} teep_err_t;

typedef struct teep_buf {
    size_t          len;
    const uint8_t   *ptr;
} teep_buf_t;

teep_err_t teep_print_hex_within_max(const uint8_t *array, const size_t size, const size_t size_max);
teep_err_t teep_print_hex(const uint8_t *array, size_t size);
teep_err_t teep_print_text(const uint8_t *text, size_t size);
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
