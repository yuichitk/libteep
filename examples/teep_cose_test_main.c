/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_cose.h"
#include "teep_common.h"
#include "teep_examples_common.h"

#define MAX_FILE_BUFFER_SIZE                1024

#if TEEP_ACTOR_AGENT == 1
#include "teep_agent_es256_private_key.h"
#include "teep_agent_es256_public_key.h"
const char *teep_private_key = teep_agent_es256_private_key;
const char *teep_public_key = teep_agent_es256_public_key;
#else /* TEEP_ACTOR_TAM as default */
#include "tam_es256_private_key.h"
#include "tam_es256_public_key.h"
const char *teep_private_key = tam_es256_private_key;
const char *teep_public_key = tam_es256_public_key;
#endif

int main(int argc, const char * argv[]) {
    int32_t result;

    // Check arguments.
    if (argc < 3) {
        printf("teep_cose_test <DER file path> <CBOR file path>\n");
        return EXIT_FAILURE;
    }

    // Read der file.
    //char private_key_buf[PRIME256V1_PRIVATE_KEY_CHAR_SIZE + 1];
    //char public_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE + 1];
    /*
    printf("\nmain : Read keys from DER file.\n");
    result = read_char_key_pair_from_der(argv[1], private_key_buf, public_key_buf);
    if (result != TEEP_SUCCESS) {
        printf("Failed to load key pair.\n");
        return EXIT_FAILURE;
    }
    printf("private_key_buf : %s\n", private_key_buf);
    printf("public_key_buf : %s\n\n", public_key_buf);
    */
    struct t_cose_key t_cose_key_pair;
    result = create_key_pair(NID_X9_62_prime256v1, teep_private_key, teep_public_key, &t_cose_key_pair);

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    UsefulBuf_MAKE_STACK_UB(cbor_buf, MAX_FILE_BUFFER_SIZE);
    cbor_buf.len = read_from_file(argv[2], MAX_FILE_BUFFER_SIZE, cbor_buf.ptr);
    if (!cbor_buf.len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex_within_max(cbor_buf.ptr, cbor_buf.len, cbor_buf.len);
    printf("\n");

    // Create cose signed file.
    printf("main : Create signed cose file.\n");
    UsefulBuf_MAKE_STACK_UB(signed_cose, MAX_FILE_BUFFER_SIZE);
    result = sign_cose_sign1(UsefulBuf_Const(cbor_buf), &t_cose_key_pair, T_COSE_ALGORITHM_ES256, &signed_cose);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to sign. (%d)\n", result);
        return EXIT_FAILURE;
    }

    teep_print_hex_within_max(signed_cose.ptr, signed_cose.len, signed_cose.len);
    printf("\n");

    // Verify cose signed file.
    UsefulBufC returned_payload;
    result = verify_cose_sign1(UsefulBuf_Const(signed_cose), &t_cose_key_pair, &returned_payload);
    if (result != TEEP_SUCCESS) {
        printf("Fail to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("main : Success to verify. Print cose payload.\n");
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    if (argc > 3) {
        size_t write_len = write_to_file(argv[3], signed_cose.len, signed_cose.ptr);
        if (!write_len) {
            printf("main : Can't write COSE signed CBOR file.%s\n", argv[3]);
            return EXIT_FAILURE;
        }
        printf("main : Success to write to \"%s\".\n", argv[3]);
    }

    return EXIT_SUCCESS;
}
