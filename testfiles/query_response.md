<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-10#appendix-D.3

## CBOR Diagnostic Notation
~~~~cbor-diag
/ query-response = /
[
  / type: / 2 / TEEP-TYPE-query-response /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
    / selected-cipher-suite / 5 : [ [ 18, -7 ] ] / Sign1 using ES256 /,
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
      81            # array(1)
         82         # array(2)
            12      # unsigned(18) / 18 = COSE_Sign1 /
            26      # negative(6) / -7 = cose-alg-es256 /
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
