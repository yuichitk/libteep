/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <assert.h>
#include "qcbor/qcbor.h"
#include "teep_common.h"
#include "teep_cose.h"
#include "teep_message_data.h"
#include "teep_message_print.h"
#include "teep_examples_common.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"

#define MAX_FILE_BUFFER_SIZE                512

int main(int argc, const char * argv[]) {
    int32_t result;
    teep_message_t msg = { 0 };
    const char *cbor_file_name = NULL;
    const char *teep_ver_key_name = NULL;
    const char *teep_sig_key_name = NULL;
    const char *suit_ver_key_name = NULL;
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    char teep_ver_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE + 1];
    char teep_sig_key_buf[PRIME256V1_PRIVATE_KEY_CHAR_SIZE + 1];
    char suit_ver_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE + 1];

#ifdef ALLOW_CBOR_WITHOUT_SIGN1
    if (argc < 2) {
        printf("teep_message_parser <CBOR file path> [<TEEP Verification DER file path> [<TEEP Sign DER file path [<SUIT Verification DER file path>]]]\n");
        return EXIT_FAILURE;
    }
#else
    if (argc < 3) {
        printf("teep_message_parser <CBOR file path> <TEEP Verification DER file path> [<TEEP Sign DER file path> [<SUIT Verification DER file path>]]\n");
        return EXIT_FAILURE;
    }
#endif

    // Check arguments.
    if (argc > 4) {
        argc = 4;
    }
    switch(argc) {
        case 4:
            suit_ver_key_name = argv[4];
        case 3:
            teep_sig_key_name = argv[3];
        case 2:
            teep_ver_key_name = argv[2];
            cbor_file_name = argv[1];
            break;
        default:
            return EXIT_FAILURE;
    }
#ifndef ALLOW_CBOR_WITHOUT_SIGN1
    assert(teep_ver_key_name != NULL);
#endif
    assert(cbor_file_name != NULL);

    if (teep_sig_key_name != NULL) {
        printf("main : Read TEEP Private&Public Key Pair.\n");
        result = read_char_key_pair_from_der(teep_sig_key_name, teep_sig_key_buf, teep_ver_key_buf);
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to load key.\n");
            return EXIT_FAILURE;
        }
    }
    else if (teep_ver_key_name != NULL) {
        printf("main : Read TEEP Public Key.\n");
        result = read_char_public_key_from_der(teep_ver_key_name, teep_ver_key_buf);
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to load key.\n");
            return EXIT_FAILURE;
        }
    }

    if (suit_ver_key_name != NULL) {
        printf("main : Read SUIT TrustAnchor Public Key.\n");
        result = read_char_public_key_from_der(suit_ver_key_name, suit_ver_key_buf);
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to load key.\n");
            return EXIT_FAILURE;
        }
    }

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    size_t cbor_len = read_from_file(cbor_file_name, MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(cbor_buf, cbor_len);
    printf("\n");

    // Verify cbor file.
    UsefulBufC returned_payload;
    if (teep_ver_key_name == NULL) {
        returned_payload.ptr = cbor_buf;
        returned_payload.len = cbor_len;
        goto skip_verify_cose;
    }
    printf("main : Verify CBOR file.\n");
    UsefulBufC signed_cose = {cbor_buf, cbor_len};
    result = verify_cose_sign1(&signed_cose, teep_ver_key_buf, &returned_payload);
    if (result != TEEP_SUCCESS) {
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        printf("main : Fail to verify CBOR file, treat this as raw cbor.\n");
        returned_payload.ptr = cbor_buf;
        returned_payload.len = cbor_len;
#else
        printf("main : Fail to verify CBOR file.\n");
        return EXIT_FAILURE;
#endif
    }
    else {
        printf("main : Success to verify. Print cose payload.\n");
    }
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

skip_verify_cose:
    // Parse teep message.
    result = teep_set_message_from_bytes(returned_payload.ptr, returned_payload.len, &msg);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to parse CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }

    // Print teep message.
    result = print_teep_message(&msg, 2, (suit_ver_key_name == NULL) ? NULL : suit_ver_key_buf);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to print CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }

    // Check whether input cbor payload and encoded cbor is the same
    uint8_t cbor_encoded_buf[MAX_FILE_BUFFER_SIZE];
    uint8_t *encoded_ptr = cbor_encoded_buf;
    size_t encoded_len = MAX_FILE_BUFFER_SIZE;
    result = teep_encode_message(&msg, &encoded_ptr, &encoded_len);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to encode CBOR from teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }
    if (encoded_len != returned_payload.len || memcmp(returned_payload.ptr, encoded_ptr, encoded_len)) {
        printf("main : Fail to encode to the same teep-message.\n");
        teep_print_hex_within_max(returned_payload.ptr, returned_payload.len, returned_payload.len);
        printf("\n");

        teep_print_hex_within_max(encoded_ptr, encoded_len, encoded_len);
        printf("\n");
        return EXIT_FAILURE;
    }
    printf("main : Success to encode to the same binary.\n\n");

    /* NOTE:
        There is no way to check the COSE_Sign1ed output is the same to the original
        because it generates different outputs for the same input.
    */
    return EXIT_SUCCESS;
}
