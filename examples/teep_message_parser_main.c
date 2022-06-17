/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <string.h>
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

#if TEEP_ACTOR_AGENT == 1
#include "tam_es256_public_key.h"
#include "teep_agent_es256_private_key.h"
const char *teep_public_key = tam_es256_public_key;
const char *teep_private_key = teep_agent_es256_private_key;
#else /* TEEP_ACTOR_TAM as default */
#include "tam_es256_private_key.h"
#include "teep_agent_es256_public_key.h"
const char *teep_public_key = teep_agent_es256_public_key;
const char *teep_private_key = tam_es256_private_key;
#endif

int main(int argc, const char * argv[]) {
    int32_t result;
    teep_message_t msg = { 0 };
    const char *cbor_file_name = NULL;
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];

    if (argc < 2) {
        printf("%s <CBOR file path>\n", argv[0]);
        return EXIT_FAILURE;
    }
    cbor_file_name = argv[1];

    assert(strlen(teep_public_key) == PRIME256V1_PUBLIC_KEY_CHAR_SIZE);
    assert(strlen(teep_private_key) == PRIME256V1_PRIVATE_KEY_CHAR_SIZE);

    struct t_cose_key t_cose_public_key;
    result = create_public_key(NID_X9_62_prime256v1, teep_public_key, &t_cose_public_key);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to parse t_cose_key. (%d)\n", result);
    }
    printf("public_key : %s\n", teep_public_key);

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    size_t cbor_len = read_from_file(cbor_file_name, MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (cbor_len == 0) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(cbor_buf, cbor_len);
    printf("\n");

    // Verify cbor file.
    printf("main : Verify CBOR file.\n");
    UsefulBufC signed_cose = {cbor_buf, cbor_len};
    UsefulBufC returned_payload;
    result = verify_cose_sign1(signed_cose, &t_cose_public_key, &returned_payload);

    if (result != TEEP_SUCCESS) {
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        printf("main : Failed to verify CBOR file, treat this as raw cbor.\n");
        returned_payload.ptr = cbor_buf;
        returned_payload.len = cbor_len;
#else
        printf("main : Failed to verify CBOR file.\n");
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
        printf("main : Fail to parse CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }

    // Print teep message.
    result = teep_print_message(&msg, 2, NULL);
    if (result != TEEP_SUCCESS) {
        printf("main : Fail to print CBOR as teep-message. (err=%d)\n", result);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
