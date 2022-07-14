#
# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause
#

CFLAGS	= -Wall
LDFLAGS	= -L../ -lteep -lt_cose -lqcbor -lcrypto -lcunit -lm
INC		= -I ../inc

all:
	$(CC) main.c $(CFLAGS) $(LDFLAGS)

