/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teep_common.h"
#include "teep_message_data.h"

void teep_QCBOREncode_AddUsefulBufC(QCBOREncodeContext *pMe, UsefulBufC buf) {
    UsefulOutBuf_AppendUsefulBuf(&(pMe->OutBuf), buf);

    // QCBOR/src/qcbor_encode.c L.488: IncrementMapOrArrayCount()
    pMe->nesting.pCurrentNesting->uCount++;
}

void teep_QCBOREncode_AddUsefulBufCToMapN(QCBOREncodeContext *pMe, int64_t uLabel, UsefulBufC buf) {
    // QCBOR/inc/qcbor_encode.h L.2194: QCBOREncode_AddBytesToMapN()
    QCBOREncode_AddInt64(pMe, uLabel);
    teep_QCBOREncode_AddUsefulBufC(pMe, buf);
}

void teep_encode_add_cipher_suite(QCBOREncodeContext *pMe, teep_ciphersuite_t ciphersuite) {
    QCBOREncode_OpenArray(pMe);
    QCBOREncode_AddInt64(pMe, ciphersuite.sign);
    QCBOREncode_AddInt64(pMe, ciphersuite.encrypt);
    QCBOREncode_AddInt64(pMe, ciphersuite.mac);
    QCBOREncode_CloseArray(pMe);
}

teep_err_t teep_encode_error(const teep_error_t *teep_error, QCBOREncodeContext *context) {
    QCBOREncode_OpenMap(context);
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_TOKEN, (UsefulBufC){.ptr = teep_error->token.ptr, .len = teep_error->token.len});
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_ERR_MSG) {
        QCBOREncode_AddTextToMapN(context, TEEP_OPTIONS_KEY_ERR_MSG, (UsefulBufC){teep_error->err_msg.ptr, teep_error->err_msg.len});
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES);
        for (size_t i = 0; i < teep_error->supported_cipher_suites.len; i++) {
            teep_encode_add_cipher_suite(context, teep_error->supported_cipher_suites.items[i]);
        }
        QCBOREncode_CloseArray(context);
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_VERSION);
        for (size_t i = 0; i < teep_error->versions.len; i++) {
            QCBOREncode_AddUInt64(context, teep_error->versions.items[i]);
        }
        QCBOREncode_CloseArray(context);
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_SUIT_REPORTS) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_SUIT_REPORTS);
        for (size_t i = 0; i < teep_error->suit_reports.len; i++) {
            QCBOREncode_AddText(context, (UsefulBufC){teep_error->suit_reports.items[i].ptr, teep_error->suit_reports.items[i].len});
        }
        QCBOREncode_CloseArray(context);
    }
    QCBOREncode_CloseMap(context);

    QCBOREncode_AddUInt64(context, teep_error->err_code);
    return TEEP_SUCCESS;
}

teep_err_t teep_encode_success(const teep_success_t *teep_success, QCBOREncodeContext *context) {
    QCBOREncode_OpenMap(context);
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_TOKEN, (UsefulBufC){.ptr = teep_success->token.ptr, .len = teep_success->token.len});
    }
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_MSG) {
        QCBOREncode_AddTextToMapN(context, TEEP_OPTIONS_KEY_MSG, (UsefulBufC){teep_success->msg.ptr, teep_success->msg.len});
    }
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_SUIT_REPORTS) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_SUIT_REPORTS);
        for (size_t i = 0; i < teep_success->suit_reports.len; i++) {
            QCBOREncode_AddText(context, (UsefulBufC){teep_success->suit_reports.items[i].ptr, teep_success->suit_reports.items[i].len});
        }
        QCBOREncode_CloseArray(context);
    }
    QCBOREncode_CloseMap(context);
    return TEEP_SUCCESS;
}

teep_err_t teep_encode_update(const teep_update_t *teep_update, QCBOREncodeContext *context) {
    QCBOREncode_OpenMap(context);
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_TOKEN, (UsefulBufC){.ptr = teep_update->token.ptr, .len = teep_update->token.len});
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST);
        for (size_t i = 0; i < teep_update->unneeded_tc_list.len; i++) {
            teep_QCBOREncode_AddUsefulBufC(context, (UsefulBufC){teep_update->unneeded_tc_list.items[i].ptr, teep_update->unneeded_tc_list.items[i].len});
        }
        QCBOREncode_CloseArray(context);
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_MANIFEST_LIST) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_MANIFEST_LIST);
        for (size_t i = 0; i < teep_update->manifest_list.len; i++) {
            QCBOREncode_AddBytes(context, (UsefulBufC){teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len});
        }
        QCBOREncode_CloseArray(context);
    }
    QCBOREncode_CloseMap(context);
    return TEEP_SUCCESS;
}

teep_err_t teep_encode_query_response(const teep_query_response_t *query_response, QCBOREncodeContext *context) {
    QCBOREncode_OpenMap(context);
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_TOKEN, (UsefulBufC){.ptr = query_response->token.ptr, .len = query_response->token.len});
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE) {
        QCBOREncode_AddInt64(context, TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE);
        teep_encode_add_cipher_suite(context, query_response->selected_cipher_suite);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_VERSION) {
        QCBOREncode_AddUInt64ToMapN(context, TEEP_OPTIONS_KEY_SELECTED_VERSION, query_response->selected_version);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT) {
        QCBOREncode_AddTextToMapN(context, TEEP_OPTIONS_KEY_EVIDENCE_FORMAT, (UsefulBufC){query_response->evidence_format.ptr, query_response->evidence_format.len});
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_EVIDENCE, (UsefulBufC){query_response->evidence.ptr, query_response->evidence.len});
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TC_LIST) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_TC_LIST);
        for (size_t i = 0; i < query_response->tc_list.len; i++) {
            QCBOREncode_OpenMap(context);
            teep_QCBOREncode_AddUsefulBufCToMapN(context, TEEP_OPTIONS_KEY_COMPONENT_ID, (UsefulBufC){query_response->tc_list.items[i].component_id.ptr, query_response->tc_list.items[i].component_id.len});

            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                QCBOREncode_AddUInt64ToMapN(context, TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER, query_response->tc_list.items[i].tc_manifest_sequence_number);
            }
            QCBOREncode_CloseMap(context);
        }
        QCBOREncode_CloseArray(context);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_REQUESTED_TC_LIST);
        for (size_t i = 0; i < query_response->requested_tc_list.len; i++) {
            QCBOREncode_OpenMap(context);
            teep_QCBOREncode_AddUsefulBufCToMapN(context, TEEP_OPTIONS_KEY_COMPONENT_ID, (UsefulBufC){query_response->requested_tc_list.items[i].component_id.ptr, query_response->requested_tc_list.items[i].component_id.len});

            if (query_response->requested_tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                QCBOREncode_AddUInt64ToMapN(context, TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER, query_response->requested_tc_list.items[i].tc_manifest_sequence_number);
            }
            if (query_response->requested_tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_HAVE_BINARY) {
                QCBOREncode_AddBoolToMapN(context, TEEP_OPTIONS_KEY_HAVE_BINARY, query_response->requested_tc_list.items[i].have_binary);
            }
            QCBOREncode_CloseMap(context);
        }
        QCBOREncode_CloseArray(context);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST);
        for (size_t i = 0; i < query_response->unneeded_tc_list.len; i++) {
            teep_QCBOREncode_AddUsefulBufC(context, (UsefulBufC){query_response->unneeded_tc_list.items[i].ptr, query_response->unneeded_tc_list.items[i].len});
        }
        QCBOREncode_CloseArray(context);
    }
    QCBOREncode_CloseMap(context);
    return TEEP_SUCCESS;
}

teep_err_t teep_encode_query_request(const teep_query_request_t *query_request, QCBOREncodeContext *context) {
    QCBOREncode_OpenMap(context);
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_TOKEN, (UsefulBufC){.ptr = query_request->token.ptr, .len = query_request->token.len});
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES);
        for (size_t i = 0; i < query_request->supported_cipher_suites.len; i++) {
            teep_encode_add_cipher_suite(context, query_request->supported_cipher_suites.items[i]);
        }
        QCBOREncode_CloseArray(context);
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_CHALLENGE) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_CHALLENGE, (UsefulBufC){query_request->challenge.ptr, query_request->challenge.len});
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        QCBOREncode_OpenArrayInMapN(context, TEEP_OPTIONS_KEY_VERSION);
        for (size_t i = 0; i < query_request->versions.len; i++) {
            QCBOREncode_AddUInt64(context, query_request->versions.items[i]);
        }
        QCBOREncode_CloseArray(context);
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_OCSP_DATA) {
        QCBOREncode_AddBytesToMapN(context, TEEP_OPTIONS_KEY_OCSP_DATA, (UsefulBufC){query_request->ocsp_data.ptr, query_request->ocsp_data.len});
    }
    QCBOREncode_CloseMap(context);

    if (query_request->contains & TEEP_MESSAGE_CONTAINS_DATA_ITEM_REQUESTED) {
        return TEEP_ERR_INVALID_VALUE;
    }
    QCBOREncode_AddUInt64(context, query_request->data_item_requested);
    return TEEP_SUCCESS;
}

teep_err_t teep_encode_message(const teep_message_t *message, void **buf, size_t *len) {
    teep_err_t result;
    QCBOREncodeContext context;
    QCBOREncode_Init(&context, (UsefulBuf){.ptr = *buf, .len = *len});
    QCBOREncode_OpenArray(&context);
    QCBOREncode_AddUInt64(&context, message->teep_message.type);

    switch(message->teep_message.type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = teep_encode_query_request(&message->query_request, &context);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            result = teep_encode_query_response(&message->query_response, &context);
            break;
        case TEEP_TYPE_UPDATE:
            result = teep_encode_update(&message->teep_update, &context);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            result = teep_encode_success(&message->teep_success, &context);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            result = teep_encode_error(&message->teep_error, &context);
            break;
        default:
            result = TEEP_ERR_INVALID_MESSAGE_TYPE;
    }
    QCBOREncode_CloseArray(&context);
    UsefulBufC tmp;
    QCBORError error = QCBOREncode_Finish(&context, &tmp);
    if (error != QCBOR_SUCCESS) {
        result = TEEP_ERR_UNEXPECTED_ERROR;
    }

    if (result != TEEP_SUCCESS) {
        *len = 0;
        return result;
    }

    *len = tmp.len;
    *buf = (uint8_t *)tmp.ptr;
    return result;
}


