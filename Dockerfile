# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
# SPDX-License-Identifier: BSD-2-Clause
FROM ubuntu

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y upgrade
RUN apt-get -y install build-essential git
RUN apt-get -y install libssl-dev libcurl4-openssl-dev

WORKDIR /root
RUN git clone --depth 1 https://github.com/laurencelundblade/QCBOR.git
WORKDIR ./QCBOR
RUN make install

WORKDIR /root
RUN git clone --depth 1 https://github.com/laurencelundblade/t_cose.git
WORKDIR ./t_cose
RUN make -f Makefile.ossl install

WORKDIR /root
RUN ldconfig
COPY . ./libteep
WORKDIR ./libteep
RUN make -f Makefile.client -B

RUN apt-get -y purge build-essential git
RUN rm -r /root/QCBOR
RUN rm -r /root/t_cose

CMD ./teep_http_client ${TAM_URI}
