<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Update Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-08#appendix-D.4

## CBOR Diagnostic Notation
    / update = /
    [
      / type: / 3 / TEEP-TYPE-update = 3 /,
      / options: /
      {
        / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
        / manifest-list / 10 : [ ] / array of SUIT_Envelope /
          / empty, example purpose only /
      }
    ]


## CBOR Binary Representation
    82                  # array(2)
       03               # unsigned(3)
       A2               # map(2)
          14            # unsigned(20)
          50            # bytes(16)
             A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
          0A            # unsigned(10)
          80            # array(0)


## Command
    echo -en "\x82\x03\xA2\x14\x50\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\x0A\x80" > update.cbor
