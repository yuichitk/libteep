/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep_examples_common.h"

uint8_t *teep_read_file(const char * file_name, size_t *size) {
    FILE* fp = fopen(file_name, "rb");
    if(fp == NULL) {
        return NULL;
    }
    const size_t MAX_FILE_SIZE = 1024;
    uint8_t *buffer = (uint8_t *)malloc(MAX_FILE_SIZE);
    *size = fread(buffer, 1, MAX_FILE_SIZE, fp);
    fclose(fp);
    return buffer;
}
