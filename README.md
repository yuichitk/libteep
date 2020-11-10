# [libteep](https://github.com/yuichitk/libteep/)
**libcteep** is a C library for encoding/decoding [IETF TEEP Protocol](https://tools.ietf.org/html/draft-ietf-teep-protocol) messages.

## Overview
 - Implemented C-native data representation.
 - Using [QCBOR](https://github.com/laurencelundblade/QCBOR) and [t_cose](https://github.com/laurencelundblade/t_cose) for decoding signed CBOR binary data.
 - There are 3 sample codes for interoperability testing.
   1. Verifying and Decoding TEEP Protocol message binary.
   2. Signing and Encoding TEEP Protocol message binary.
   3. TEEP/HTTP Client Communicating with TEEP/HTTP Server using TEEP-over-HTTP.
   - The above TEEP Protocol message is protected by the COSE_Sign1 structure.

## Getting started
Installing [QCBOR](https://github.com/laurencelundblade/QCBOR).
```bash
git clone https://github.com/laurencelundblade/QCBOR.git
cd QCBOR
make install
```

Installing [t_cose](https://github.com/laurencelundblade/t_cose).
```bash
git clone https://github.com/laurencelundblade/t_cose.git
cd t_cose
make -f Makefile.ossl install
```

Make and run sample codes you need.

1. teep_message_parser
```bash
make -f Makefile.parser
./teep_message_parser ./testfiles/key/tam_prime256v1_pub.der ./testfiles/query_request_cose.cbor
./teep_message_parser ./testfiles/key/teep_agent_prime256v1_pub.der ./testfiles/query_response_cose.cbor
./teep_message_parser ./testfiles/key/tam_prime256v1_pub.der ./testfiles/trusted_app_install_with_manifest_cose.cbor
```

2. teep_cose_test
```bash
make -f Makefile.cose
./teep_cose_test ./testfiles/key/tam_prime256v1.der ./testfiles/cbor/query_request.cbor
./teep_cose_test ./testfiles/key/teep_agent_prime256v1.der ./testfiles/cbor/query_response.cbor
./teep_cose_test ./testfiles/key/tam_prime256v1.der ./testfiles/cbor/trusted_app_install_with_manifest.cbor
```

3. teep_http_client
```bash
make -f Makefile.client
./testfiles/tam_server.sh
./teep_http_client
```

* In some cases, teep_http_client Requires to install libcurl.
```bash
sudo apt-get install libcurl4-openssl-dev
```

## TEEP Protocol Message Examples
The following description Markdown and CBOR files are compliant with [draft-ietf-teep-protocol-04](https://tools.ietf.org/html/draft-ietf-teep-protocol-04).
- QueryRequest
  - [query_request.md](https://github.com/yuichitk/libteep/blob/master/testfiles/query_request.md)
  - [query_request.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_request.cbor)

- QueryResponse
  - [query_response.md](https://github.com/yuichitk/libteep/blob/master/testfiles/query_response.md)
  - [query_response.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_response.cbor)

- TrustedAppInstall
  - [trusted_app_install.md](https://github.com/yuichitk/libteep/blob/master/testfiles/trusted_app_install.md)
  - [trusted_app_install.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/trusted_app_install.cbor)

- Success
  - [teep_success.md](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_success.md)
  - [teep_success.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_success.cbor)

- Error
  - [teep_error.md](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_error.md)
  - [teep_error.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_error.cbor)

## License and Copyright
BSD 2-Clause License

Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
