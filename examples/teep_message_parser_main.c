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

int main(int argc, const char * argv[]) {
    // Check arguments.
    if (argc < 2) {
        printf("teep_message_parser <filename>\n");
        return EXIT_FAILURE;
    }

    // Read cbor file.
    printf("Read cbor file.\n");
    size_t file_length;
    uint8_t *file_bytes = teep_read_file(argv[1], &file_length);
    if (!file_length) {
        printf("Can't read file.\n");
        return EXIT_FAILURE;
    }
    print_hex(file_bytes, file_length);
    printf("\n");

    // Verify cbor file.
    UsefulBufC signed_cose = {file_bytes, file_length};
    UsefulBufC returned_payload;
    int32_t result;
    result = verify_cose_sign1(&signed_cose, TAM_PUBLIC_KEY_PRIME256V1, &returned_payload);
    if (result) {
        printf("Fail to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("\ncose, verify : payload\n");
    print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    // Print teep message.
    print_teep_message(returned_payload.ptr, returned_payload.len);

    return EXIT_SUCCESS;
}
