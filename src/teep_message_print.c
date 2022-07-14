/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep/teep_common.h"
#include "teep/teep_message_data.h"
#include "teep/teep_message_print.h"

#ifdef PARSE_SUIT
#include "csuit/csuit.h"
#endif

const char *teep_err_to_str(teep_err_t err) {
    switch (err) {
    case TEEP_SUCCESS:
        return "SUCCESS";
    case TEEP_ERR_INVALID_TYPE_OF_ARGUMENT:
        return "INVALID_TYPE_OF_ARGUMENT";
    case TEEP_ERR_INVALID_VALUE:
        return "INVALID_VALUE";
    case TEEP_ERR_INVALID_LENGTH:
        return "INVALID_LENGTH";
    case TEEP_ERR_INVALID_MESSAGE_TYPE:
        return "INVALID_MESSAGE_TYPE";
    case TEEP_ERR_CBOR_WITHOUT_COSE:
        return "CBOR_WITHOUT_COSE";
    case TEEP_ERR_VERIFICATION_FAILED:
        return "VERIFICATION_FAILED";
    case TEEP_ERR_SIGNING_FAILED:
        return "SIGNING_FAILED";
    case TEEP_ERR_NO_SUPPORTED_VERSION:
        return "NO_SUPPORTED_VERSION";
    case TEEP_ERR_NO_SUPPORTED_CIPHERSUITE:
        return "NO_SUPPORTED_CIPHERSUITE";
    case TEEP_ERR_NO_MEMORY:
        return "NO_MEMORY";
    case TEEP_ERR_ON_HTTP_POST:
        return "ON_HTTP_POST";
    case TEEP_ERR_UNEXPECTED_ERROR:
        return "UNEXPECTED_ERROR";
    case TEEP_ERR_ABORT:
        return "ABORT";
    case TEEP_ERR_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

teep_err_t teep_print_hex_within_max(const uint8_t *array, const size_t size, const size_t size_max) {
    if (array == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    size_t print_len = (size < size_max) ? size : size_max;
    for (size_t i = 0; i < print_len; i++) {
        printf("0x%02x ", (unsigned char)array[i]);
    }
    if (print_len < size) {
        printf("..");
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_print_hex(const uint8_t *array, const size_t size) {
    return teep_print_hex_within_max(array, size, TEEP_MAX_PRINT_BYTE_COUNT);
}

teep_err_t teep_print_text(const uint8_t *text, const size_t size) {
    if (text == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    size_t print_len = (size < TEEP_MAX_PRINT_TEXT_COUNT) ? size : TEEP_MAX_PRINT_TEXT_COUNT;
    printf("\"");
    for (size_t j = 0; j < print_len; j++) {
        putchar(text[j]);
    }
    printf("\"");
    if (print_len < size) {
        printf("..");
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_print_string(const teep_buf_t *string) {
    return teep_print_text(string->ptr, string->len);
}

const char *teep_err_code_to_str(int32_t err_code) {
    switch (err_code) {
    case TEEP_ERR_CODE_PERMANENT_ERROR:
        return "ERR_PERMANENT_ERROR";
    case TEEP_ERR_CODE_UNSUPPORTED_EXTENSION:
        return "ERR_UNSUPPORTED_EXTENSION";
    case TEEP_ERR_CODE_UNSUPPORTED_FRESHNESS_MECHANISMS:
        return "ERR_UNSUPPORTED_FRESHNESS_MECHANISMS";
    case TEEP_ERR_CODE_UNSUPPORTED_MSG_VERSION:
        return "ERR_UNSUPPORTED_MSG_VERSION";
    case TEEP_ERR_CODE_UNSUPPORTED_CIPHER_SUITES:
        return "ERR_UNSUPPORTED_CIPHER_SUITES";
    case TEEP_ERR_CODE_BAD_CERTIFICATE:
        return "ERR_BAD_CERTIFICATE";
    case TEEP_ERR_CODE_CERTIFICATE_EXPIRED:
        return "ERR_CERTIFICATE_EXPIRED";
    case TEEP_ERR_CODE_TEMPORARY_ERROR:
        return "ERR_TEMPORARY_ERROR";
    case TEEP_ERR_CODE_MANIFEST_PROCESSING_FAILED:
        return "ERR_MANIFEST_PROCESSING_FAILED";
    default:
        return "ERR_UNKNOWN";
    }
}

const char *teep_cose_algs_key_to_str(int32_t cose_algs_key) {
    switch (cose_algs_key) {
    case 0:
        return "NONE";
    case TEEP_COSE_SIGN_ES256:
        return "ES256";
    case TEEP_COSE_SIGN_EDDSA:
        return "EdDSA";
    case TEEP_COSE_SIGN_HSS_LMS:
        return "HSS-LMS";
    case TEEP_COSE_ENCRYPT_A256_GCM:
        return "AES-GCM-256";
    case TEEP_COSE_ENCRYPT_ACCM_16_64_128:
        return "AES-CCM-16-64-128";
    case TEEP_COSE_MAC_HMAC256:
        return "HMAC 256/256";
    default:
        return "UNKNOWN";
    }
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
    teep_print_hex_within_max(at, len, len);
    printf("\n");

    if (error != NULL && *error != QCBOR_SUCCESS) {
        printf("    Error! nCBORError = %d\n", *error);
    }
    if (expecting != QCBOR_TYPE_ANY && expecting != item->uDataType) {
        printf("    item->uDataType %d != %d\n", item->uDataType, expecting);
    }
}

teep_err_t teep_print_ciphersuite(const teep_ciphersuite_t *ciphersuite) {
    if (ciphersuite == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    printf("{sign: %s(%d), encrypt: %s(%d), mac: %s(%d)}",
        teep_cose_algs_key_to_str(ciphersuite->sign), ciphersuite->sign,
        teep_cose_algs_key_to_str(ciphersuite->encrypt), ciphersuite->encrypt,
        teep_cose_algs_key_to_str(ciphersuite->mac), ciphersuite->mac);
    return TEEP_SUCCESS;
}

teep_err_t teep_print_query_request(const teep_query_request_t *query_request, uint32_t indent_space) {
    if (query_request == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
    printf("%*sQueryRequest :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", query_request->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : ", indent_space + 4, "");
        teep_print_hex(query_request->token.ptr, query_request->token.len);
        printf("\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        printf("%*ssupported-cipher-suites : [\n", indent_space + 4, "");
        for (size_t i = 0; i < query_request->supported_cipher_suites.len; i++) {
            printf("%*s", indent_space + 6, "");
            result = teep_print_ciphersuite(&query_request->supported_cipher_suites.items[i]);
            if (result != TEEP_SUCCESS) {
                return result;
            }
            printf(",\n");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_FRESHNESS_MECHANISMS) {
        printf("%*ssupported-freshness-mechanisms : [ ", indent_space + 4, "");
        for (size_t i = 0; i < query_request->supported_freshness_mechanisms.len; i++) {
            printf("%u, ", query_request->supported_freshness_mechanisms.items[i]);
        }
        printf("]\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_CHALLENGE) {
        printf("%*schallenge : ", indent_space + 4, "");
        result = teep_print_hex(query_request->challenge.ptr, query_request->challenge.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        printf("%*sversions : [ ", indent_space + 4, "");
        for (size_t i = 0; i < query_request->versions.len; i++) {
            printf("%u, ", query_request->versions.items[i]);
        }
        printf("]\n");
    }
    printf("%*sdata-item-requested : %u\n", indent_space + 2, "", query_request->data_item_requested);
    return TEEP_SUCCESS;
}

teep_err_t teep_print_component_id(const teep_buf_t *component_id) {
    if (component_id == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
#ifdef PARSE_SUIT
    suit_buf_t buf = {.ptr = component_id->ptr, .len = component_id->len};
    suit_component_identifier_t identifier;
    suit_err_t suit_result = suit_set_component_identifiers(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &buf, &identifier);
    if (suit_result != SUIT_SUCCESS) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    suit_result = suit_print_component_identifier(&identifier);
    if (suit_result != SUIT_SUCCESS) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
#else
    result = teep_print_hex(component_id->ptr, component_id->len);
#endif /* PARSE_SUIT */
    return result;
}

teep_err_t teep_print_query_response(const teep_query_response_t *query_response, uint32_t indent_space) {
    if (query_response == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
    printf("%*sQueryResponse :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", query_response->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : ", indent_space + 4, "");
        teep_print_hex(query_response->token.ptr, query_response->token.len);
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE) {
        printf("%*sselected-cipher-suite : ", indent_space + 4, "");
        result = teep_print_ciphersuite(&query_response->selected_cipher_suite);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_VERSION) {
        printf("%*sselected-version : %u\n", indent_space + 4, "", query_response->selected_version);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT) {
        printf("%*sevidence-format : ", indent_space + 4, "");
        result = teep_print_string(&query_response->evidence_format);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE) {
        printf("%*sevidence : ", indent_space + 4, "");
        result = teep_print_hex(query_response->evidence.ptr, query_response->evidence.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TC_LIST) {
        printf("%*stc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < query_response->tc_list.len; i++) {
            printf("%*s{\n", indent_space + 6, "");
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_COMPONENT_ID) {
                printf("%*scomponent-id : ", indent_space + 8, "");
                result = teep_print_component_id(&query_response->tc_list.items[i].component_id);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                printf(",\n");
            }
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                if (query_response->tc_list.items[i].tc_manifest_sequence_number != TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID) {
                    printf("%*stc-manifest-sequence-number : %lu,\n", indent_space + 8, "", query_response->tc_list.items[i].tc_manifest_sequence_number);
                }
            }
            printf("%*s},\n", indent_space + 6, "");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST) {
        printf("%*srequested-tc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < query_response->requested_tc_list.len; i++) {
            printf("%*s{\n", indent_space + 6, "");
            if (query_response->requested_tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_COMPONENT_ID) {
                printf("%*scomponent-id : ", indent_space + 8, "");
                result = teep_print_component_id(&query_response->requested_tc_list.items[i].component_id);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                printf(",\n");
            }
            if (query_response->requested_tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                printf("%*stc-manifest-sequence-number : %lu,\n", indent_space + 8, "", query_response->requested_tc_list.items[i].tc_manifest_sequence_number);
            }
            if (query_response->requested_tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_HAVE_BINARY) {
                printf("%*shave-binary : %s,\n", indent_space + 8, "", (query_response->requested_tc_list.items[i].have_binary) ? "true" : "false");
            }
            printf("%*s}\n", indent_space + 6, "");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EXT_LIST) {
        printf("%*sext-list : [", indent_space + 4, "");
        for (size_t i = 0; i < query_response->ext_list.len; i++) {
            printf("%lu ", query_response->ext_list.items[i]);
        }
        printf("]\n");
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_print_update(const teep_update_t *teep_update, uint32_t indent_space, const char *ta_public_key) {
    if (teep_update == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
    printf("%*sUpdate :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_update->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : ", indent_space + 4, "");
        teep_print_hex(teep_update->token.ptr, teep_update->token.len);
        printf("\n");
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST) {
        printf("%*sunneeded-tc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_update->unneeded_tc_list.len; i++) {
            printf("%*s", indent_space + 6, "");
            result = teep_print_component_id(&teep_update->unneeded_tc_list.items[i]);
            if (result != TEEP_SUCCESS) {
                return result;
            }
            printf(",\n");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_MANIFEST_LIST) {
        printf("%*smanifest-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_update->manifest_list.len; i++) {
#ifdef PARSE_SUIT
            suit_buf_t buf = {.ptr = teep_update->manifest_list.items[i].ptr, .len = teep_update->manifest_list.items[i].len};
            suit_envelope_t envelope;
            suit_err_t suit_result = suit_set_envelope(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &buf, &envelope, ta_public_key);
            if (suit_result != SUIT_SUCCESS) {
                return TEEP_ERR_UNEXPECTED_ERROR;
            }
            suit_result = suit_print_envelope(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &envelope, indent_space + 6);
            if (result != SUIT_SUCCESS) {
                return TEEP_ERR_UNEXPECTED_ERROR;
            }
#else
            printf("%*s", indent_space + 6, "");
            result = teep_print_hex(teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len);
            if (result != TEEP_SUCCESS) {
                return result;
            }
            printf(",\n");
#endif /* PARSE_SUIT */
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_print_error(const teep_error_t *teep_error, uint32_t indent_space) {
    if (teep_error == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
    printf("%*sError :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_error->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : ", indent_space + 4, "");
        teep_print_hex(teep_error->token.ptr, teep_error->token.len);
        printf("\n");
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_ERR_MSG) {
        printf("%*serr-msg : ", indent_space + 4, "");
        result = teep_print_text(teep_error->err_msg.ptr, teep_error->err_msg.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        printf("%*ssupported-cipher-suites : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_error->supported_cipher_suites.len; i++) {
            printf("%*s", indent_space + 6, "");
            result = teep_print_ciphersuite(&teep_error->supported_cipher_suites.items[i]);
            if (result != TEEP_SUCCESS) {
                return result;
            }
            printf(",\n");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        printf("%*sversions : [ ", indent_space + 4, "");
        for (size_t i = 0; i < teep_error->versions.len; i++) {
            printf("%u, ", teep_error->versions.items[i]);
        }
        printf("]\n");
    }
    printf("%*serr-code : %s(%u)\n", indent_space + 2, "", teep_err_code_to_str(teep_error->err_code), teep_error->err_code);
    return TEEP_SUCCESS;
}

teep_err_t teep_print_success(const teep_success_t *teep_success, uint32_t indent_space) {
    if (teep_success == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result;
    printf("%*sSuccess :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_success->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : ", indent_space + 4, "");
        teep_print_hex(teep_success->token.ptr, teep_success->token.len);
        printf("\n");
    }
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_MSG) {
        printf("%*smsg : ", indent_space + 2, "");
        result = teep_print_text(teep_success->msg.ptr, teep_success->msg.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_print_message(const teep_message_t *msg, uint32_t indent_space, const char *ta_public_key) {
    if (msg == NULL) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_err_t result = TEEP_SUCCESS;
    switch (msg->teep_message.type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = teep_print_query_request(&msg->query_request, indent_space);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            result = teep_print_query_response(&msg->query_response, indent_space);
            break;
        case TEEP_TYPE_UPDATE:
            result = teep_print_update(&msg->teep_update, indent_space, ta_public_key);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            result = teep_print_success(&msg->teep_success, indent_space);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            result = teep_print_error(&msg->teep_error, indent_space);
            break;
        default:
            result = TEEP_ERR_INVALID_MESSAGE_TYPE;
            break;
    }
    return result;
}
