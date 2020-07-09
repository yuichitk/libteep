<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryResponse
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8302187BA50501060B07480102030405060708088243010203430405060983010203',
        h'0EE77E0AA849DDA96820835712F5D2163FB88BB9AF1E0CD5F82A816B567ED81510CB00684BD1CC60403CB970DEF513527C4BABABBFCBA4F5FB51235C4C35E4A4'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          58 22                             # bytes(34)
             8302187BA50501060B07480102030405060708088243010203430405060983010203
          58 40                             # bytes(64)
             0EE77E0AA849DDA96820835712F5D2163FB88BB9AF1E0CD5F82A816B567ED81510CB00684BD1CC60403CB970DEF513527C4BABABBFCBA4F5FB51235C4C35E4A4

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x58\x22\x83\x02\x18\x7b\xa5\x05\x01\x06\x0b\x07\x48\x01\x02\x03\x04\x05\x06\x07\x08\x08\x82\x43\x01\x02\x03\x43\x04\x05\x06\x09\x83\x01\x02\x03\x58\x40\x0e\xe7\x7e\x0a\xa8\x49\xdd\xa9\x68\x20\x83\x57\x12\xf5\xd2\x16\x3f\xb8\x8b\xb9\xaf\x1e\x0c\xd5\xf8\x2a\x81\x6b\x56\x7e\xd8\x15\x10\xcb\x00\x68\x4b\xd1\xcc\x60\x40\x3c\xb9\x70\xde\xf5\x13\x52\x7c\x4b\xab\xab\xbf\xcb\xa4\xf5\xfb\x51\x23\x5c\x4c\x35\xe4\xa4" > query_response_cose.cbor

## Signing Key
    key/teep_agent_prime256v1.pem

## Payload CBOR definition
    cbor/query_response.md
