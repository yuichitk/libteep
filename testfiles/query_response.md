<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-08#appendix-D.3

## CBOR Diagnostic Notation
~~~~
/ query-response = /
[
  / type: / 2 / TEEP-TYPE-query-response /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
    / selected-cipher-suite / 5 : [ -7, null, null ] / only use ES256 /,
    / selected-version / 6 : 0,
    / tc-list / 8 : [
      {
        / component-id / 16 : [ h'0102030405060708090A0B0C0D0E0F' ]
      },
      {
        / component-id / 16 : [ h'1102030405060708090A0B0C0D0E0F' ]
      }
    ]
  }
]
~~~~


## CBOR binary Representation
~~~~
82                  # array(2)
   02               # unsigned(2) / TEEP-TYPE-query-response /
   A4               # map(4)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
      05            # unsigned(5) / selected-cipher-suite: /
      83            # array(3)
         26         # negative(6) / -7 = cose-alg-es256 /
         F6         # primitive(22) / null /
         F6         # primitive(22) / null /
      06            # unsigned(6) / selected-version: /
      00            # unsigned(0)
      08            # unsigned(8) / tc-list: /
      82            # array(2)
         A1         # map(1)
            10      # unsigned(16) / component-id: /
            81      # array(1)
               4F   # bytes(15)
                  0102030405060708090A0B0C0D0E0F
         A1         # map(1)
            10      # unsigned(16) / component-id: /
            81      # array(1)
               4F   # bytes(15)
                  1102030405060708090A0B0C0D0E0F
~~~~


## Command
    echo -en "\x82\x02\xA4\x14\x50\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\x05\x83\x26\xF6\xF6\x06\x00\x08\x82\xA1\x10\x81\x4F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\xA1\x10\x81\x4F\x11\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > query_response.cbor
