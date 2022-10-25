# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
# SPDX-License-Identifier: BSD-2-Clause
FROM debian:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y install curl git gcc make libcunit1-dev libssl-dev ruby
RUN gem install cbor-diag

RUN git clone --depth 1 https://github.com/laurencelundblade/QCBOR.git /root/QCBOR
WORKDIR /root/QCBOR
RUN make libqcbor.a install

RUN git clone --depth 1 https://github.com/laurencelundblade/t_cose.git /root/t_cose
WORKDIR /root/t_cose
RUN make -f Makefile.ossl libt_cose.a install

WORKDIR /root
COPY . ./libteep
WORKDIR /root/libteep
RUN make
RUN make -f Makefile.cose
RUN make -f Makefile.parser

CMD make test && \
    make -f Makefile.cose run && \
    make -f Makefile.parser run
