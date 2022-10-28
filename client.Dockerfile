# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
# SPDX-License-Identifier: BSD-2-Clause
FROM debian:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y install curl git gcc make libcurl4-openssl-dev libssl-dev

RUN git clone --depth 1 https://github.com/laurencelundblade/QCBOR.git /root/QCBOR
WORKDIR /root/QCBOR
RUN make libqcbor.a install

RUN git clone --depth 1 https://github.com/laurencelundblade/t_cose.git /root/t_cose
WORKDIR /root/t_cose
RUN make -f Makefile.ossl libt_cose.a install

RUN git clone --depth 1 https://github.com/yuichitk/libteep.git /root/libteep
WORKDIR /root/libteep
RUN make -f Makefile.client

RUN apt-get -y remove curl git gcc make
RUN apt-get -y autoremove

CMD ./bin/teep_http_client ${TAM_URI}
