<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Update Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-10#appendix-D.4

## CBOR Diagnostic Notation
~~~~cbor-diag
/ update = /
[
  / type: / 3 / TEEP-TYPE-update /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
    / manifest-list / 10 : [ ] / array of SUIT_Envelope /
      / empty, example purpose only /
  }
]
~~~~


## CBOR Binary Representation
~~~~
82                  # array(2)
   03               # unsigned(3) / TEEP-TYPE-update /
   A2               # map(2)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
      0A            # unsigned(10) / manifest-list: /
      80            # array(0) / [] /
~~~~
