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

int main(int argc, const char * argv[]) {
    // Check arguments.
    if (argc < 2) {
        printf("teep_cose_test <filename>\n");
        return EXIT_FAILURE;
    }

    // Read cbor file.
    printf("main : Read file.\n");
    size_t file_length;
    uint8_t *file_bytes = teep_read_file(argv[1], &file_length);
    if (!file_length) {
        printf("Can't read file.\n");
        return EXIT_FAILURE;
    }
    print_hex(file_bytes, file_length);
    printf("\n");

    // Make an ECDSA private key.
    struct t_cose_key   private_key;
    int32_t             result;
    result = create_es256_key_pair(TAM_PRIVATE_KEY_PRIME256V1, TAM_PUBLIC_KEY_PRIME256V1, &private_key);
    if (result) {
        printf("Fail make_ossl_ecdsa_key_pair : result = %d\n", result);
        return EXIT_FAILURE;
    }

    // Create cose signed file.
    printf("main : Create signed cose file.\n");
    struct t_cose_sign1_sign_ctx    sign_ctx;
    UsefulBufC                      constructed_payload = {file_bytes, file_length};
    UsefulBuf_MAKE_STACK_UB(        signed_cose_buffer, 300);
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
    printf("\nmain : Print signed cose bytes\n");
    print_hex(signed_cose.ptr, signed_cose.len);
    printf("\n");

    EC_KEY_free(private_key.k.key_ptr);

    // Verify cose signed file.
    UsefulBufC returned_payload;
    int32_t result_cose;
    result_cose = verify_cose_sign1(&signed_cose, TAM_PUBLIC_KEY_PRIME256V1, &returned_payload);
    if (result_cose) {
        printf("Fail to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("\nmain : Verify successs, Print cose payload\n");
    print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    return EXIT_SUCCESS;
}
