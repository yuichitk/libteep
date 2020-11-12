/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_MESSAGE_DATA_H
#define TEEP_MESSAGE_DATA_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "qcbor/qcbor.h"

#ifndef TEEP_MAX_ARRAY_LENGTH
  #define TEEP_MAX_ARRAY_LENGTH               20
#endif

#define INITIALIZE_TEEP_ARRAY(array) ( array.len = SIZE_MAX )
#define TEEP_ARRAY_IS_NULL(array) ( (array).len == SIZE_MAX )
#define TEEP_ARRAY_IS_NOT_NULL(array) ( (array.len < SIZE_MAX) )
#define INITIALIZE_TEEP_BUF(buf) ( buf.len = SIZE_MAX, buf.ptr = NULL )
#define TEEP_BUF_IS_NULL(buf) ( (buf).len == SIZE_MAX )
#define TEEP_BUF_IS_NOT_NULL(buf) ( (buf).len < SIZE_MAX )

#define TEEP_INVALID_VERSION                0

/*
 * teep-message-type
 */
typedef enum teep_type {
    TEEP_TYPE_INVALID                       = 0,
    TEEP_TYPE_QUERY_REQUEST                 = 1,
    TEEP_TYPE_QUERY_RESPONSE                = 2,
    TEEP_TYPE_INSTALL                       = 3,
    TEEP_TYPE_DELETE                        = 4,
    TEEP_TYPE_TEEP_SUCCESS                  = 5,
    TEEP_TYPE_TEEP_ERROR                    = 6
} teep_type_t;

/*
 * key of options
 */
typedef enum teep_options_key {
    TEEP_OPTIONS_KEY_INVALID                 = 0,
    TEEP_OPTIONS_KEY_CIPHER_SUITES           = 1,
    TEEP_OPTIONS_KEY_NONCE                   = 2,
    TEEP_OPTIONS_KEY_VERSIONS                = 3,
    TEEP_OPTIONS_KEY_OSCP_DATA               = 4,
    TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE   = 5,
    TEEP_OPTIONS_KEY_SELECTED_VERSION        = 6,
    TEEP_OPTIONS_KEY_EAT                     = 7,
    TEEP_OPTIONS_KEY_TA_LIST                 = 8,
    TEEP_OPTIONS_KEY_EXT_LIST                = 9,
    TEEP_OPTIONS_KEY_MANIFEST_LIST           = 10,
    TEEP_OPTIONS_KEY_MSG                     = 11,
    TEEP_OPTIONS_KEY_ERR_MSG                 = 12
} teep_options_key_t;

/*
 * data-item-requested
 */
typedef enum teep_data_item_requested {
    TEEP_DATA_ITEM_INVALID                  = 0,
    TEEP_DATA_ITEM_ATTESTATION              = 1,
    TEEP_DATA_ITEM_TRUSTED_APPS             = 2,
    TEEP_DATA_ITEM_EXTENTIONS               = 4,
    TEEP_DATA_ITEM_SUIT_COMMANDS            = 8
} teep_data_item_requested_t;

/*
 * TEEP-suite
 */
typedef enum teep_suite {
    TEEP_SUITE_INVALID                                       = 0,
    TEEP_SUITE_AES_CCM_16_64_128_HMAC256_256_X25519_EdDSA    = 1,
    TEEP_SUITE_AES_CCM_16_64_128_HMAC256_256_P_256_ES256     = 2
} teep_suite_t;

/*
 * [ + suite ]
 */
typedef struct teep_suite_array {
    size_t         len;
    teep_suite_t   items[TEEP_MAX_ARRAY_LENGTH];
} teep_suite_array_t;

/*
 * [ + uint ]
 */
typedef struct teep_uint_array {
    size_t     len;
    uint32_t   items[TEEP_MAX_ARRAY_LENGTH];
} teep_uint_array_t;

/*
 * [ + bstr ]
 * [ + tstr ]
 */
typedef struct teep_buf_array {
    size_t         len;
    teep_buf_t     items[TEEP_MAX_ARRAY_LENGTH];
} teep_buf_array_t;

/*
 * query-request
 */
typedef struct teep_query_request {
    teep_type_t                type;
    uint32_t                   token;
    teep_uint_array_t          supported_suites;
    teep_suite_t               suite;
    teep_buf_t		           nonce;
    teep_uint_array_t          version;
    teep_buf_t		           ocsp_data;
    // TODO :                  query-request-extensions
    // TODO :                  teep-option-extensions
    teep_data_item_requested_t data_item_requested;
} teep_query_request_t;

/*
 * query-response
 */
typedef struct teep_query_response {
    teep_type_t         type;
    uint32_t            token;
    teep_suite_t        suite;
    uint32_t            version;
    teep_buf_t          eat;
    teep_buf_array_t    ta_list;
    teep_uint_array_t   ext_info;
    // TODO :           query-response-extensions
    // TODO :           teep-option-extensions
} teep_query_response_t;

/*
 * trusted-app-install
 */
typedef struct teep_install {
    teep_type_t             type;
    uint32_t                token;
    teep_buf_array_t        manifest_list;
    // TODO :               trusted-app-install-extensions
    // TODO :               teep-option-extensions
} teep_install_t;

/*
 * trusted-app-delete
 */
typedef struct teep_delete {
    teep_type_t             type;
    uint32_t                token;
    teep_buf_array_t        ta_list;
    // TODO : $$trusted-app-delete-extensions
    // TODO : $$teep-option-extensions
} teep_delete_t;

/*
 * teep-success
 */
typedef struct teep_success {
    teep_type_t     type;
    uint32_t        token;
    teep_buf_t      msg;
    // TODO :       $$teep-success-extensions
    // TODO :       $$teep-option-extensions
} teep_success_t;

/*
 * teep-error
 */
typedef struct teep_error {
    teep_type_t            type;
    uint32_t               token;
    teep_buf_t             err_msg;
    teep_suite_array_t     suite;
    teep_uint_array_t      version;
    // TODO :              $$teep-error--extensions
    // TODO :              $$teep-option-extensions
    uint32_t               err_code;
} teep_error_t;

uint64_t get_teep_message_type(QCBORDecodeContext *teep_message);
int32_t set_teep_query_request(QCBORDecodeContext *teep_message, teep_query_request_t *query_request);
int32_t set_teep_query_response(QCBORDecodeContext *teep_message, teep_query_response_t *query_response);
int32_t set_teep_install(QCBORDecodeContext *teep_message, teep_install_t *app_install);
int32_t set_teep_delete(QCBORDecodeContext *teep_message, teep_delete_t *app_delete);
int32_t set_teep_success(QCBORDecodeContext *teep_message, teep_success_t *success);
int32_t set_teep_error(QCBORDecodeContext *teep_message, teep_error_t *error);

#endif  /* TEEP_MESSAGE_DATA_H */
