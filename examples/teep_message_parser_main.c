/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "qcbor/qcbor.h"
#include "teep_common.h"
#include "teep_cose.h"
#include "teep_message_data.h"
#include "teep_message_print.h"
#include "teep_examples_common.h"

#define MAX_FILE_BUFFER_SIZE                512

int main(int argc, const char * argv[]) {
    int32_t result;
    teep_message_t msg = { 0 };
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
    bool verify = true;
    // Check arguments.
    if (argc < 2) {
        printf("teep_message_parser <CBOR file path> [<TAM DER file path> [<TrustAnchor DER file path]]\n");
        return EXIT_FAILURE;
    }
    else if (argc == 2) {
        verify = false;
        goto skip_load_key;
    }
#else
    // Check arguments.
    if (argc < 3) {
        printf("teep_message_parser <CBOR file path> <DER file path> [<TrustAnchor DER file path]\n");
        return EXIT_FAILURE;
    }
#endif

    // Read der file.
    printf("\nmain : Read TAM DER file.\n");
    uint8_t der_buf[PRIME256V1_PUBLIC_KEY_DER_SIZE];
    size_t der_len = read_from_file(argv[2], PRIME256V1_PUBLIC_KEY_DER_SIZE, der_buf);
    if (!der_len) {
        printf("main : Can't read DER file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(der_buf, der_len);
    printf("\n");

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read public key from DER file.\n");
    char key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE];
    read_prime256v1_public_key(der_buf, key_buf);
    printf("%s\n", key_buf);

    char *trust_anchor_key = NULL;
    char ta_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE];
    if (argc > 3) {
        printf("\nmain : Read TrustAnchor DER file.\n");
        uint8_t ta_der_buf[PRIME256V1_PUBLIC_KEY_DER_SIZE];
        size_t der_len = read_from_file(argv[3], PRIME256V1_PUBLIC_KEY_DER_SIZE, ta_der_buf);
        if (!der_len) {
            printf("main : Can't read DER file.\n");
            return EXIT_FAILURE;
        }
        teep_print_hex(ta_der_buf, der_len);
        printf("\n");

        // Read key from der file.
        // This code is only available for openssl prime256v1.
        printf("\nmain : Read public key from DER file.\n");
        read_prime256v1_public_key(ta_der_buf, ta_key_buf);
        printf("%s\n", ta_key_buf);
        trust_anchor_key = ta_key_buf;
    }

#ifdef ALLOW_CBOR_WITHOUT_SIGN1
skip_load_key:
#endif
    // Read cbor file.
    printf("\nmain : Read CBOR file.\n");
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    size_t cbor_len = read_from_file(argv[1], MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(cbor_buf, cbor_len);
    printf("\n");

    UsefulBufC returned_payload;
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
    if (!verify) {
        returned_payload.ptr = cbor_buf;
        returned_payload.len = cbor_len;
        goto skip_verify_cose;
    }
#endif
    // Verify cbor file.
    printf("\nmain : Verify CBOR file.\n");
    UsefulBufC signed_cose = {cbor_buf, cbor_len};
    result = verify_cose_sign1(&signed_cose, key_buf, &returned_payload);
    if (result != TEEP_SUCCESS) {
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        printf("main : Fail to verify CBOR file, so skip this.\n");
        returned_payload.ptr = cbor_buf;
        returned_payload.len = cbor_len;
#else
        printf("main : Fail to verify CBOR file.\n");
        return EXIT_FAILURE;
#endif
    }
    else {
        printf("\nmain : Success to verify. Print cose payload.\n");
    }
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

#ifdef ALLOW_CBOR_WITHOUT_SIGN1
skip_verify_cose:
#endif
    // Print teep message.
    result = teep_set_message_from_bytes(returned_payload.ptr, returned_payload.len, &msg);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to parse CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }
    result = print_teep_message(&msg, 2, trust_anchor_key);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to print CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
