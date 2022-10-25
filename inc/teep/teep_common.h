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
    TEEP_ERR_INVALID_VALUE              = 2,
    TEEP_ERR_INVALID_LENGTH             = 3,
    TEEP_ERR_INVALID_MESSAGE_TYPE       = 4,

    TEEP_ERR_CBOR_WITHOUT_COSE          = 5,
    TEEP_ERR_VERIFICATION_FAILED        = 6,
    TEEP_ERR_SIGNING_FAILED             = 7,

    TEEP_ERR_NO_SUPPORTED_VERSION       = 8,
    TEEP_ERR_NO_SUPPORTED_CIPHERSUITE   = 9,

    TEEP_ERR_NO_MEMORY                  = 10,
    TEEP_ERR_ON_HTTP_POST               = 11,

    TEEP_ERR_UNEXPECTED_ERROR           = 29,
    TEEP_ERR_ABORT                      = 30,
    TEEP_ERR_FATAL                      = 31,
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

struct teep_cipher_suite;
bool teep_cipher_suite_is_same(struct teep_cipher_suite a, struct teep_cipher_suite b);
uint32_t teep_array_to_int32(const uint8_t *array, int32_t byte_count);
uint64_t teep_array_to_int64(const uint8_t *array);
bool teep_is_valid_mechanism(int64_t cose_mechanism_key);

#endif  // TEEP_COMMON_H
