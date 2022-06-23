<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-08#appendix-D.1

## CBOR Diagnostic Notation
~~~~
/ query-request = /
[
  / type: / 1 / TEEP-TYPE-query-request /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
    / versions / 3 : [ 1 ]
  },
  / data-item-requested: / 3 / attestation | trusted-components /
]
~~~~


## CBOR binary Representation
~~~~
83                  # array(3)
   01               # unsigned(1) / TEEP-TYPE-query-request /
   A2               # map(2)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
      03            # unsigned(3) / versions: /
      81            # array(1) / [ 0 ] /
         01         # unsigned(1)
   03               # unsigned(3) / attestation | trusted-components /
~~~~


## Command
    echo -en "\x83\x01\xA2\x14\x50\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\x03\x81\x01\x03" > query_request_err1.cbor
