/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "qcbor/qcbor.h"
#include "teep/teep_message_data.h"
#include "teep/teep_message_print.h"
#include "teep_examples_common.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"


#define MAX_FILE_BUFFER_SIZE                512

#if TEEP_ACTOR_AGENT == 1
#include "tam_es256_public_key.h"
const unsigned char *teep_public_key = tam_es256_public_key;
#else /* TEEP_ACTOR_TAM as default */
#include "teep_agent_es256_public_key.h"
const unsigned char *teep_public_key = teep_agent_es256_public_key;
#endif

int main(int argc, const char * argv[]) {
    int32_t result;
    teep_message_t msg = { 0 };
    const char *cbor_file_name = NULL;

    if (argc < 2) {
        printf("%s <CBOR file path>\n", argv[0]);
        return EXIT_FAILURE;
    }
    cbor_file_name = argv[1];

    teep_key_t public_key;
    result = teep_key_init_es256_public_key(teep_public_key, &public_key);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to parse t_cose_key. (%d)\n", result);
    }
    printf("public_key : ");
    teep_print_hex(teep_public_key, PRIME256V1_PUBLIC_KEY_LENGTH);
    printf("\n");

    // Read cbor file.
    UsefulBuf_MAKE_STACK_UB(cbor_buf, MAX_FILE_BUFFER_SIZE);
    printf("main : Read CBOR file.\n");
    cbor_buf.len = read_from_file(cbor_file_name, MAX_FILE_BUFFER_SIZE, cbor_buf.ptr);
    if (cbor_buf.len == 0) {
        printf("main : Failed to read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(cbor_buf.ptr, cbor_buf.len);
    printf("\n");

    // Verify cbor file.
    printf("main : Verify CBOR file.\n");
    UsefulBufC signed_cose = UsefulBuf_Const(cbor_buf);
    UsefulBufC returned_payload;
    result = teep_verify_cose_sign1(signed_cose, &public_key, &returned_payload);

    if (result != TEEP_SUCCESS) {
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        printf("main : Failed to verify CBOR file, treat this as raw cbor.\n");
        returned_payload = cbor_buf;
#else
        printf("main : Failed to verify CBOR file. %s(%d)\n", teep_err_to_str(result), result);
        return EXIT_FAILURE;
#endif
    }
    else {
        printf("main : Success to verify. Print cose payload.\n");
    }
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    // Parse teep message.
    result = teep_set_message_from_bytes(returned_payload.ptr, returned_payload.len, &msg);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to parse CBOR as teep-message. %s(%d)\n", teep_err_to_str(result), result);
        return EXIT_FAILURE;
    }

    // Print teep message.
    result = teep_print_message(&msg, 2, NULL);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to print CBOR as teep-message. %s(%d)\n", teep_err_to_str(result), result);
        return EXIT_FAILURE;
    }
    teep_free_key(&public_key);

    return EXIT_SUCCESS;
}
