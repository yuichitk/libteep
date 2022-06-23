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
    / token / 20 : h'A0A1',
  },
  / data-item-requested: / 3 / attestation | trusted-components /
]
~~~~


## CBOR binary Representation
~~~~
83                  # array(3)
   01               # unsigned(1) / TEEP-TYPE-query-request /
   A1               # map(3)
      14            # unsigned(20) / token: /
      42            # bytes(2)
         A0A1
   03               # unsigned(3) / attestation | trusted-components /
~~~~


## Command
    echo -en "\x83\x01\xA1\x14\x42\xA0\xA1\x03" > query_request_err3.cbor
