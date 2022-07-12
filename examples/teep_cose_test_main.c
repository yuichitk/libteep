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
const unsigned char *teep_private_key = teep_agent_es256_private_key;
const unsigned char *teep_public_key = teep_agent_es256_public_key;
#else /* TEEP_ACTOR_TAM as default */
#include "tam_es256_private_key.h"
#include "tam_es256_public_key.h"
const unsigned char *teep_private_key = tam_es256_private_key;
const unsigned char *teep_public_key = tam_es256_public_key;
#endif

int main(int argc, const char * argv[]) {
    int32_t result;

    // Check arguments.
    if (argc < 2) {
        printf("%s <CBOR Input File> [<COSE Output File>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    teep_key_t key_pair;
    result = teep_key_init_es256_key_pair(teep_private_key, teep_public_key, &key_pair);

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    UsefulBuf_MAKE_STACK_UB(cbor_buf, MAX_FILE_BUFFER_SIZE);
    cbor_buf.len = read_from_file(argv[1], MAX_FILE_BUFFER_SIZE, cbor_buf.ptr);
    if (!cbor_buf.len) {
        printf("main : Failed to read CBOR file \"%s\".\n", argv[2]);
        return EXIT_FAILURE;
    }
    teep_print_hex_within_max(cbor_buf.ptr, cbor_buf.len, cbor_buf.len);
    printf("\n");

    // Create cose signed file.
    printf("main : Create signed cose file.\n");
    UsefulBuf_MAKE_STACK_UB(signed_cose, MAX_FILE_BUFFER_SIZE);
    result = teep_sign_cose_sign1(UsefulBuf_Const(cbor_buf), &key_pair, &signed_cose);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to sign. (%d)\n", result);
        return EXIT_FAILURE;
    }

    teep_print_hex_within_max(signed_cose.ptr, signed_cose.len, signed_cose.len);
    printf("\n");

    // Verify cose signed file.
    UsefulBufC returned_payload;
    result = teep_verify_cose_sign1(UsefulBuf_Const(signed_cose), &key_pair, &returned_payload);
    if (result != TEEP_SUCCESS) {
        printf("Failed to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("main : Succeed to verify. Print cose payload.\n");
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    if (argc > 2) {
        size_t write_len = write_to_file(argv[2], signed_cose.len, signed_cose.ptr);
        if (!write_len) {
            printf("main : Failed to write COSE signed CBOR to \"%s\".\n", argv[2]);
            return EXIT_FAILURE;
        }
        printf("main : Succeed to write to \"%s\".\n", argv[2]);
    }
    teep_key_free(&key_pair);

    return EXIT_SUCCESS;
}
