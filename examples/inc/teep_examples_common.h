/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_EXAMPLES_COMMON_H
#define TEEP_EXAMPLES_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEEP_AGENT_PRIVATE_KEY_PRIME256V1 \
"60fe6dd6d85d5740a5349b6f91267e" \
"eac5ba81b8cb53ee249e4b4eb102c4" \
"76b3"

#define TEEP_AGENT_PUBLIC_KEY_PRIME256V1 \
"045886cd61dd875862e5aaa820e7a1" \
"5274c968a9bc96048ddcace32f50c3" \
"651ba39eed8125e932cd60c0ead365" \
"0d0a485cf726d378d1b016ed4298b2" \
"961e258f1b"

#define TAM_PRIVATE_KEY_PRIME256V1 \
"ac56b410b2fadbdd3411044d29c58b" \
"d4c5ea5b60e6a1601e79b8e6b6070d" \
"1bc6"

#define TAM_PUBLIC_KEY_PRIME256V1 \
"044d5e5f3367ec6e411f0ec397452a" \
"c02e6541b212761314548a62937926" \
"4c5a44308aeffc285e452ede343c0f" \
"35d21e0e2d3751f8bd32496f90af26" \
"4d686ecded"

uint8_t *teep_read_file(const char * file_name, size_t *size);

#endif  /* TEEP_EXAMPLES_COMMON_H */
