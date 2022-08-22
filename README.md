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

### Parse & Encode tests
```
docker build -t libteep .
docker run -t libteep
```

### TEEP over HTTP Client test
Use [tamproto](https://github.com/ko-isobe/tamproto) as a TAM.
```
git clone https://github.com/ko-isobe/tamproto.git ../tamproto
docker-compose up
```

## TEEP Protocol Message Examples
The following description Markdown and CBOR files are compliant with [draft-ietf-teep-protocol-10](https://tools.ietf.org/html/draft-ietf-teep-protocol-10).
- QueryRequest
  - [query_request.md](https://github.com/yuichitk/libteep/blob/master/testfiles/query_request.md)
  - [query_request.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_request.cbor)
  - [query_request_cose.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_request_cose.cbor)

- QueryResponse
  - [query_response.md](https://github.com/yuichitk/libteep/blob/master/testfiles/query_response.md)
  - [query_response.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_response.cbor)
  - [query_response_cose.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/query_response_cose.cbor)

- Update
  - [update.md](https://github.com/yuichitk/libteep/blob/master/testfiles/update.md)
  - [update.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/update.cbor)
  - [update_cose.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/update_cose.cbor)

- Success
  - [teep_success.md](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_success.md)
  - [teep_success.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_success.cbor)
  - [teep_success_cose.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_success_cose.cbor)

- Error
  - [teep_error.md](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_error.md)
  - [teep_error.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_error.cbor)
  - [teep_error_cose.cbor](https://github.com/yuichitk/libteep/blob/master/testfiles/teep_error_cose.cbor)


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
