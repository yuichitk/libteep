<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TEEP Success
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8306187BA10B6773756363657373',
        h'C3B8F3A0E99BB08C81698C1689D63C0CF0DE6699CB0F28B2123383B90B311D72D898619E7D733BC1EEEC07E6F6C585E925BBD5CD986434BD0A44FFBDACF9FD92'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          4E                                # bytes(14)
             8306187BA10B6773756363657373
          58 40                             # bytes(64)
             C3B8F3A0E99BB08C81698C1689D63C0CF0DE6699CB0F28B2123383B90B311D72D898619E7D733BC1EEEC07E6F6C585E925BBD5CD986434BD0A44FFBDACF9FD92

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x4e\x83\x06\x18\x7b\xa1\x0b\x67\x73\x75\x63\x63\x65\x73\x73\x58\x40\xc3\xb8\xf3\xa0\xe9\x9b\xb0\x8c\x81\x69\x8c\x16\x89\xd6\x3c\x0c\xf0\xde\x66\x99\xcb\x0f\x28\xb2\x12\x33\x83\xb9\x0b\x31\x1d\x72\xd8\x98\x61\x9e\x7d\x73\x3b\xc1\xee\xec\x07\xe6\xf6\xc5\x85\xe9\x25\xbb\xd5\xcd\x98\x64\x34\xbd\x0a\x44\xff\xbd\xac\xf9\xfd\x92" > teep_success_cose.cbor

## Signing Key
    ../key/teep_agent_prime256v1.pem

## Payload CBOR definition
    ../cbor/teep_success.md
