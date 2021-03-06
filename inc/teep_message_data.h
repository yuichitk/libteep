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

#define INITIALIZE_TEEP_ARRAY(array) ( array.len = 0 )
#define INITIALIZE_TEEP_ARRAY_PTR(array) ( array->len = 0 )
#define TEEP_ARRAY_IS_NULL(array) ( (array).len == 0 )
#define INITIALIZE_TEEP_BUF(buf) ( buf.len = 0, buf.ptr = NULL )
#define INITIALIZE_TEEP_BUF_PTR(buf) ( buf->len = 0, buf->ptr = NULL )
#define TEEP_BUF_IS_NULL(buf) ( (buf).len == 0 || (buf).ptr == NULL )

/*
 * extend QCBOR_TYPE_*** as QCBOR doesn't judge wether the value is in int32_t nor uint32_t.
 * see https://github.com/laurencelundblade/QCBOR/blob/master/inc/qcbor/qcbor_decode.h for other QCBOR_TYPE_***.
 */
#ifndef QCBOR_TYPE_INT32
  #define QCBOR_TYPE_INT32 28
#endif
#ifndef QCBOR_TYPE_UINT32
  #define QCBOR_TYPE_UINT32 29
#endif


/*
 * teep-message-type
 */
typedef enum teep_type {
    TEEP_TYPE_INVALID                       = 0,
    TEEP_TYPE_QUERY_REQUEST                 = 1,
    TEEP_TYPE_QUERY_RESPONSE                = 2,
    TEEP_TYPE_UPDATE                        = 3,
    TEEP_TYPE_TEEP_SUCCESS                  = 5,
    TEEP_TYPE_TEEP_ERROR                    = 6
} teep_type_t;

/*
 * key of options
 */
typedef enum teep_options_key {
    TEEP_OPTIONS_KEY_INVALID                     = 0,
    TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES     = 1,
    TEEP_OPTIONS_KEY_CHALLENGE                   = 2,
    TEEP_OPTIONS_KEY_VERSION                     = 3,
    TEEP_OPTIONS_KEY_OCSP_DATA                   = 4,
    TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE       = 5,
    TEEP_OPTIONS_KEY_SELECTED_VERSION            = 6,
    TEEP_OPTIONS_KEY_EVIDENCE                    = 7,
    TEEP_OPTIONS_KEY_TC_LIST                     = 8,
    TEEP_OPTIONS_KEY_EXT_LIST                    = 9,
    TEEP_OPTIONS_KEY_MANIFEST_LIST               = 10,
    TEEP_OPTIONS_KEY_MSG                         = 11,
    TEEP_OPTIONS_KEY_ERR_MSG                     = 12,
    TEEP_OPTIONS_KEY_EVIDENCE_FORMAT             = 13,
    TEEP_OPTIONS_KEY_REQUESTED_TC_LIST           = 14,
    TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST            = 15,
    TEEP_OPTIONS_KEY_COMPONENT_ID                = 16,
    TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER = 17,
    TEEP_OPTIONS_KEY_HAVE_BINARY                 = 18,
    TEEP_OPTIONS_KEY_SUIT_REPORTS                = 19,
    TEEP_OPTIONS_KEY_TOKEN                       = 20
} teep_options_key_t;

/*
 * data-item-requested
 */
typedef enum teep_data_item_requested {
    TEEP_DATA_ITEM_INVALID                  = 0,
    TEEP_DATA_ITEM_ATTESTATION              = 1,
    TEEP_DATA_ITEM_TRUSTED_COMPONENTS       = 2,
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
 * TEEP-err-code
 */
typedef enum teep_err_code {
    TEEP_ERR_CODE_INVALID = 0,
    TEEP_ERR_CODE_ILLEGAL_PARAMETER = 1,
    TEEP_ERR_CODE_UNSUPPORTED_EXTENSION = 2,
    TEEP_ERR_CODE_REQUEST_SIGNATURE_FAILED = 3,
    TEEP_ERR_CODE_UNSUPPORTED_MSG_VERSION = 4,
    TEEP_ERR_CODE_UNSUPPORTED_CRYPTO_ALG = 5,
    TEEP_ERR_CODE_BAD_CERTIFICATE = 6,
    TEEP_ERR_CODE_UNSUPPORTED_CERTIFICATE = 7,
    TEEP_ERR_CODE_CERTIFICATE_REVOKED = 8,
    TEEP_ERR_CODE_CERTIFICATE_EXPIRED = 9,
    TEEP_ERR_CODE_INTERNAL_ERROR = 10,
    TEEP_ERR_CODE_TC_NOT_FOUND = 12,
    TEEP_ERR_CODE_MANIFEST_PROCESSING_FAILED = 17
} teep_err_code_t;

/*
 * [ + uint32 ]
 * [ + uint64 ]
 */
#define def_teep_uint_array_with_size(content_type__, content_type_name__) \
    typedef struct {                                 \
        size_t         len;                          \
        content_type__ items[TEEP_MAX_ARRAY_LENGTH]; \
    } content_type_name__
def_teep_uint_array_with_size(uint32_t, teep_uint32_array_t);
def_teep_uint_array_with_size(uint64_t, teep_uint64_array_t);

/*
 * [ + bstr ]
 * [ + tstr ]
 */
typedef struct teep_buf_array {
    size_t         len;
    teep_buf_t     items[TEEP_MAX_ARRAY_LENGTH];
} teep_buf_array_t;

/*
 * [ + [ + bstr ] ]
 */
typedef struct teep_buf_array_array {
    size_t              len;
    teep_buf_array_t    items[TEEP_MAX_ARRAY_LENGTH];
} teep_buf_array_array_t;

#define TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID UINT64_MAX
/*
 * tc-info
 */
typedef struct teep_tc_info {
    uint64_t    contains;
    teep_buf_t  component_id;
    uint64_t    tc_manifest_sequence_number;
} teep_tc_info_t;

/*
 * [ + tc-info ]
 */
typedef struct teep_tc_info_array {
    size_t          len;
    teep_tc_info_t  items[TEEP_MAX_ARRAY_LENGTH];
} teep_tc_info_array_t;

/*
 * requested-tc-info
 */
typedef struct teep_requested_tc_info {
    uint64_t    contains;
    teep_buf_t  component_id;
    uint64_t    tc_manifest_sequence_number;
    bool        have_binary;
} teep_requested_tc_info_t;

/*
 * [ + requested-tc-info ]
 */
typedef struct teep_requested_tc_info_array {
    size_t                      len;
    teep_requested_tc_info_t    items[TEEP_MAX_ARRAY_LENGTH];
} teep_requested_tc_info_array_t;

/*
 * set bit flags to distinguish these conditions like below:
 *  1: unused because the teep-message doesn't contain the option
 *  2: 0 length array was set
 */
#define BIT(nr) (1UL << (nr))
#define TEEP_MESSAGE_CONTAINS_TYPE BIT(0)
#define TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES BIT(TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES)
#define TEEP_MESSAGE_CONTAINS_CHALLENGE BIT(TEEP_OPTIONS_KEY_CHALLENGE)
#define TEEP_MESSAGE_CONTAINS_VERSION BIT(TEEP_OPTIONS_KEY_VERSION)
#define TEEP_MESSAGE_CONTAINS_OCSP_DATA BIT(TEEP_OPTIONS_KEY_OCSP_DATA)
#define TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE BIT(TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE)
#define TEEP_MESSAGE_CONTAINS_SELECTED_VERSION BIT(TEEP_OPTIONS_KEY_SELECTED_VERSION)
#define TEEP_MESSAGE_CONTAINS_EVIDENCE BIT(TEEP_OPTIONS_KEY_EVIDENCE)
#define TEEP_MESSAGE_CONTAINS_TC_LIST BIT(TEEP_OPTIONS_KEY_TC_LIST)
#define TEEP_MESSAGE_CONTAINS_EXT_LIST BIT(TEEP_OPTIONS_KEY_EXT_LIST)
#define TEEP_MESSAGE_CONTAINS_MANIFEST_LIST BIT(TEEP_OPTIONS_KEY_MANIFEST_LIST)
#define TEEP_MESSAGE_CONTAINS_MSG BIT(TEEP_OPTIONS_KEY_MSG)
#define TEEP_MESSAGE_CONTAINS_ERR_MSG BIT(TEEP_OPTIONS_KEY_ERR_MSG)
#define TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT BIT(TEEP_OPTIONS_KEY_EVIDENCE_FORMAT)
#define TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST BIT(TEEP_OPTIONS_KEY_REQUESTED_TC_LIST)
#define TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST BIT(TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST)
#define TEEP_MESSAGE_CONTAINS_COMPONENT_ID BIT(TEEP_OPTIONS_KEY_COMPONENT_ID)
#define TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER BIT(TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER)
#define TEEP_MESSAGE_CONTAINS_HAVE_BINARY BIT(TEEP_OPTIONS_KEY_HAVE_BINARY)
#define TEEP_MESSAGE_CONTAINS_SUIT_REPORTS BIT(TEEP_OPTIONS_KEY_SUIT_REPORTS)
#define TEEP_MESSAGE_CONTAINS_TOKEN BIT(TEEP_OPTIONS_KEY_TOKEN)
/* 21 <= unused <= 61 */
#define TEEP_MESSAGE_CONTAINS_ERR_CODE BIT(62)
#define TEEP_MESSAGE_CONTAINS_DATA_ITEM_REQUESTED BIT(63)

/*
 * teep-message-header
 * XXX: each teep-message MUST have these two members
 *      at the top of itself in the same order.
 */
struct teep_message {
    teep_type_t                 type;
    uint64_t                    contains;
};

/*
 * query-request
 */
typedef struct teep_query_request {
    teep_type_t                 type;
    uint64_t                    contains;
    teep_buf_t                  token;
    teep_uint64_array_t         supported_cipher_suites;
    teep_buf_t                  challenge;
    teep_uint32_array_t         versions;
    teep_buf_t                  ocsp_data;
    // TODO :                   query-request-extensions
    // TODO :                   teep-option-extensions
    teep_data_item_requested_t  data_item_requested;
} teep_query_request_t;

/*
 * query-response
 */
typedef struct teep_query_response {
    teep_type_t                     type;
    uint64_t                        contains;
    teep_buf_t                      token;
    uint64_t                        selected_cipher_suite;
    uint32_t                        selected_version;
    teep_buf_t                      evidence_format;
    teep_buf_t                      evidence;
    teep_tc_info_array_t            tc_list;
    teep_requested_tc_info_array_t  requested_tc_list;
    teep_buf_array_t                unneeded_tc_list;
    teep_uint64_array_t             ext_list;
    // TODO :                       query-response-extensions
    // TODO :                       teep-option-extensions
} teep_query_response_t;

/*
 * update
 */
typedef struct teep_update {
    teep_type_t             type;
    uint64_t                contains;
    teep_buf_t              token;
    teep_buf_array_t        unneeded_tc_list; // [ + SUIT_Component_Identifier ]
    teep_buf_array_t        manifest_list;
    // TODO :               update-extensions
    // TODO :               teep-option-extensions
} teep_update_t;

/*
 * teep-success
 */
typedef struct teep_success {
    teep_type_t         type;
    uint64_t            contains;
    teep_buf_t          token;
    teep_buf_t          msg;
    teep_buf_array_t    suit_reports;
    // TODO :       $$teep-success-extensions
    // TODO :       $$teep-option-extensions
} teep_success_t;

/*
 * teep-error
 */
typedef struct teep_error {
    teep_type_t             type;
    uint64_t                contains;
    teep_buf_t              token;
    teep_buf_t              err_msg;
    teep_suite_array_t      supported_cipher_suites;
    teep_uint32_array_t     versions;
    teep_buf_array_t        suit_reports;
    // TODO :               $$teep-error--extensions
    // TODO :               $$teep-option-extensions
    teep_err_code_t         err_code;
} teep_error_t;

/*
 * teep-message
 */
typedef union {
    struct teep_message teep_message;
    teep_query_request_t query_request;
    teep_query_response_t query_response;
    teep_update_t teep_update;
    teep_success_t teep_success;
    teep_error_t teep_error;
} teep_message_t;

int32_t teep_set_out_of_teep_buf(QCBORDecodeContext *message, QCBORItem *item, teep_buf_t *teep_buf);
uint64_t get_teep_message_type(QCBORDecodeContext *teep_message);
int32_t teep_set_query_request(QCBORDecodeContext *teep_message, QCBORItem *item, teep_query_request_t *query_request);
int32_t teep_set_query_response(QCBORDecodeContext *teep_message, QCBORItem *item, teep_query_response_t *query_response);
int32_t teep_set_update(QCBORDecodeContext *teep_message, QCBORItem *item, teep_update_t *teep_update);
int32_t teep_set_success(QCBORDecodeContext *teep_message, QCBORItem *item, teep_success_t *teep_success);
int32_t teep_set_error(QCBORDecodeContext *teep_message, QCBORItem *item, teep_error_t *teep_error);
int32_t teep_set_message_from_bytes(const uint8_t *buf, const size_t len, teep_message_t *msg);

int32_t teep_encode_message(const teep_message_t *message, uint8_t **buf, size_t *len);

#endif  /* TEEP_MESSAGE_DATA_H */
