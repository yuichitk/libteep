# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
# SPDX-License-Identifier: BSD-2-Clause
FROM debian:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y install curl git gcc make libcunit1-dev

WORKDIR /root
RUN curl -O https://www.openssl.org/source/openssl-3.0.5.tar.gz
RUN tar xzf openssl-3.0.5.tar.gz
WORKDIR /root/openssl-3.0.5
RUN ./config
RUN make install_sw
ENV LD_LIBRARY_PATH /usr/local/lib64
RUN ldconfig

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

RUN apt-get -y remove curl git gcc
RUN apt-get -y autoremove
RUN rm -r /root/QCBOR /root/t_cose /root/openssl-3.0.5 /root/openssl-3.0.5.tar.gz

CMD make test && make -f Makefile.cose run && make -f Makefile.parser run
