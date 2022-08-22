<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Success Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-10#appendix-D.5

## CBOR Diagnostic Notation
~~~~cbor-diag
/ teep-success = /
[
  / type: / 5 / TEEP-TYPE-teep-success /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF'
  }
]
~~~~


## CBOR Binary Representation
~~~~
82                  # array(2)
   05               # unsigned(5) / TEEP-TYPE-teep-success /
   A1               # map(1)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
~~~~
