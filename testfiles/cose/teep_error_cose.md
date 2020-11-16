<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TEEP Error
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8405187BA30C656572726F7201810203830B0C0D1901C8',
        h'FB5839239F3F4CC0B333C11516B1A75ACF1BAC9B3E6B741FC52BE02D97B2FDF5809B082D16BD873F365DE6A2B90DFE18EBB929DE790989A826BAD5F24C154494'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          57                                # bytes(23)
             8405187BA30C656572726F7201810203830B0C0D1901C8
          58 40                             # bytes(64)
             FB5839239F3F4CC0B333C11516B1A75ACF1BAC9B3E6B741FC52BE02D97B2FDF5809B082D16BD873F365DE6A2B90DFE18EBB929DE790989A826BAD5F24C154494

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x57\x84\x05\x18\x7b\xa3\x0c\x65\x65\x72\x72\x6f\x72\x01\x81\x02\x03\x83\x0b\x0c\x0d\x19\x01\xc8\x58\x40\xfb\x58\x39\x23\x9f\x3f\x4c\xc0\xb3\x33\xc1\x15\x16\xb1\xa7\x5a\xcf\x1b\xac\x9b\x3e\x6b\x74\x1f\xc5\x2b\xe0\x2d\x97\xb2\xfd\xf5\x80\x9b\x08\x2d\x16\xbd\x87\x3f\x36\x5d\xe6\xa2\xb9\x0d\xfe\x18\xeb\xb9\x29\xde\x79\x09\x89\xa8\x26\xba\xd5\xf2\x4c\x15\x44\x94" > teep_error_cose.cbor

## Signing Key
    ../key/teep_agent_prime256v1.pem

## Payload CBOR definition
    ../cbor/teep_error.md
