/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_cose.h"
#include "teep_common.h"
#include "teep_examples_common.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"

#define MAX_FILE_BUFFER_SIZE                1024
#define MAX_COSE_BUFFER_SIZE                1024

int main(int argc, const char * argv[]) {
    // Check arguments.
    if (argc < 3) {
        printf("teep_cose_test <DER file path> <CBOR file path>\n");
        return EXIT_FAILURE;
    }

    // Read der file.
    printf("\nmain : Read DER file.\n");
    uint8_t der_buf[PRIME256V1_PRIVATE_KEY_DER_SIZE];
    size_t der_len = read_file(argv[1], PRIME256V1_PRIVATE_KEY_DER_SIZE, der_buf);
    if (!der_len) {
        printf("main : Can't read DER file.\n");
        return EXIT_FAILURE;
    }
    print_hex(der_buf, der_len);
    printf("\n");

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read key from DER file.\n");
    char private_key_buf[PRIME256V1_PRIVATE_KEY_CHAR_SIZE];
    char public_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE];
    read_prime256v1_key_pair(der_buf, private_key_buf, public_key_buf);
    printf("private_key_buf : %s\n", private_key_buf);
    printf("public_key_buf : %s\n\n", public_key_buf);

    // Make an ECDSA private key.
    struct t_cose_key   private_key;
    int32_t             result;
    result = create_es256_key_pair(private_key_buf, public_key_buf, &private_key);
    if (result) {
        printf("Fail make_ossl_ecdsa_key_pair : result = %d\n", result);
        return EXIT_FAILURE;
    }

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    size_t cbor_len = read_file(argv[2], MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    print_hex(cbor_buf, cbor_len);
    printf("\n");

    // Create cose signed file.
    printf("\nmain : Create signed cose file.\n");
    struct t_cose_sign1_sign_ctx    sign_ctx;
    UsefulBufC                      constructed_payload = {cbor_buf, cbor_len};
    UsefulBuf_MAKE_STACK_UB(        signed_cose_buffer, 1024);
    UsefulBufC                      signed_cose;

    t_cose_sign1_sign_init(&sign_ctx, 0, T_COSE_ALGORITHM_ES256);
    t_cose_sign1_set_signing_key(&sign_ctx,
                                 private_key,
                                 NULL_Q_USEFUL_BUF_C);
    result = t_cose_sign1_sign(&sign_ctx,
                               constructed_payload,
                               signed_cose_buffer,
                               &signed_cose);
    if (result) {
        printf("Fail t_cose_sign1_sign : result = %d\n", result);
        return EXIT_FAILURE;
    }
    print_hex(signed_cose.ptr, signed_cose.len);
    printf("\n");

    EC_KEY_free(private_key.k.key_ptr);

    // Verify cose signed file.
    UsefulBufC returned_payload;
    int32_t result_cose;
    result_cose = verify_cose_sign1(&signed_cose, public_key_buf, &returned_payload);
    if (result_cose) {
        printf("Fail to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("\nmain : Success to verify. Print cose payload.\n");
    print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    return EXIT_SUCCESS;
}
