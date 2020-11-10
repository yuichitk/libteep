<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8401187BA405010248010203040506070803830B0C0D044301020302',
        h'1A9FAC210E9E968FF00778E426F3B869A9BA0177CF3DE8F7D8C657EB659BACC9523E9580CC74E5745826872817BBF2B23586A4E57202EBEC30ECED8BB7D04C08'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          58 1C                             # bytes(28)
             8401187BA405010248010203040506070803830B0C0D044301020302
          58 40                             # bytes(64)
             1A9FAC210E9E968FF00778E426F3B869A9BA0177CF3DE8F7D8C657EB659BACC9523E9580CC74E5745826872817BBF2B23586A4E57202EBEC30ECED8BB7D04C08

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x58\x1c\x84\x01\x18\x7b\xa4\x05\x01\x02\x48\x01\x02\x03\x04\x05\x06\x07\x08\x03\x83\x0b\x0c\x0d\x04\x43\x01\x02\x03\x02\x58\x40\x1a\x9f\xac\x21\x0e\x9e\x96\x8f\xf0\x07\x78\xe4\x26\xf3\xb8\x69\xa9\xba\x01\x77\xcf\x3d\xe8\xf7\xd8\xc6\x57\xeb\x65\x9b\xac\xc9\x52\x3e\x95\x80\xcc\x74\xe5\x74\x58\x26\x87\x28\x17\xbb\xf2\xb2\x35\x86\xa4\xe5\x72\x02\xeb\xec\x30\xec\xed\x8b\xb7\xd0\x4c\x08" > query_request_cose.cbor

## Signing Key
    key/tam_prime256v1.pem

## Payload CBOR definition
    cbor/query_request.md
