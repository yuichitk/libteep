#
# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause
#

NAME	= libteep
CC		= gcc
CFLAGS	= -Wall -g -fPIC
LDFLAGS	= -lt_cose -lqcbor
INC		= -I ./inc
SRCS	= src/teep_common.c src/teep_cose.c src/teep_message_decode.c src/teep_message_print.c
PUBLIC_INTERFACE	= inc/teep.h inc/teep_common.h inc/teep_message_data.h inc/teep_cose.h inc/teep_message_print.h
OBJDIR	= ./obj
OBJS	= $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRCS)))

.PHONY: all so install uninstall clean

all: $(NAME).a

so: $(NAME).so

$(NAME).a: $(OBJS)
	$(AR) -r $@ $^

$(NAME).so: $(OBJS)
	$(CC) -shared $^ $(CFLAGS) $(INC) -o $@

$(OBJDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

define install-header
	install -m 644 $1 $2/$(nodir $1)

endef

install: $(NAME).a $(PUBLIC_INTERFACE)
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(NAME).a $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/include/teep
	$(foreach header,$(PUBLIC_INTERFACE),$(call install-header,$(header),$(DESTDIR)$(PREFIX)/include/teep))

install_so: $(NAME).so
	install -m 755 $(NAME).so $(DESTDIR)$(PREFIX)/lib/$(NAME).so.1.0.0
	ln -sf $(NAME).so.1 $(DESTDIR)$(PREFIX)/lib/$(NAME).so
	ln -sf $(NAME).so.1.0.0 $(DESTDIR)$(PREFIX)/lib/$(NAME).so.1

uninstall: $(NAME).a $(PUBLIC_INTERFACE)
	$(RM) -d $(DESTDIR)$(PREFIX)/include/teep/*
	$(RM) -d $(DESTDIR)$(PREFIX)/include/teep/
	$(RM) $(addprefix $(DESTDIR)$(PREFIX)/lib/, \
		$(NAME).a $(NAME).so $(NAME).so.1 $(NAME).so.1.0.0)

clean:
	rm -f $(OBJS) $(NAME).a $(NAME).so

