<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Success Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-08#appendix-D.5

## CBOR Diagnostic Notation
    / teep-success = /
    [
      / type: / 5 / TEEP-TYPE-teep-success = 5 /,
      / options: /
      {
        20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF'
      }
    ]


## CBOR Binary Representation
    82                  # array(2)
       05               # unsigned(5)
       A1               # map(1)
          14            # unsigned(20)
          50            # bytes(16)
             A0A1A2A3A4A5A6A7A8A9AAABACADAEAF


## Command
    echo -en "\x82\x05\xA1\x14\x50\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF" > teep_success.cbor
