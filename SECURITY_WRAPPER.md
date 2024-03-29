
# Assumption
The TAM is sending a QueryRequest message to a TEEP Agent.  
The TAM does not know the supported ciphersuite of the TEEP Agent because the negotiation has not been done at this time.

```
     +------------+           +-------------+
     | TAM        |           | TEEP Agent  |
     +------------+           +-------------+
      support {ES256,EdDSA}    support {EdDSA}

       QueryRequest ------->
```


## Target Message (QueryRequest)
```
83                  # array(3)
   01               # unsigned(1) / TEEP-TYPE-query-request /
   A3               # map(3)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
      01            # unsigned(1) / supported-cipher-suites: /
      81            # array(1)
         83         # array(3)
            26      # negative(6) / -7 = cose-alg-es256 /
            F6      # primitive(22) / null /
            F6      # primitive(22) / null /
      03            # unsigned(3) / versions: /
      81            # array(1) / [ 0 ] /
         00         # unsigned(0)
   03               # unsigned(3) / data-item-requested = attestation | trusted-components /
```

## 1. Simple COSE_Sign1(ES256)
```
D2                  # tag(18)
   84               # array(4)
      43            # bytes(3)
         A1         # map(1)
            01      # unsigned(1)
            26      # negative(6) / -7 = cose-alg-es256 /
      A0            # map(0)
      58 1F         # bytes(31) / payload: /
         8301A31450A0A1A2A3A4A5A6A7A8A9AAABACADAEAF01818326F6F603810003
      58 40         # bytes(64) / signature: /
         583A259B237E886B1C1035DF0ECEEE4A07646E27C2B0CE580745092F06876D80D7FE8BDBDB704C3B792DA8D72E7180B49C3B716ECD54FB2F4BC4D46D9F6EA4BE
```

* Pros
  - Simple, so the almost all COSE library must support this
* Cons
  - If the TEEP Agent does not support the signature algorithm (ES256), he have to send Error message with `supported-cipher-suites: [ [ cose-alg-eddsa, null, null ] ]`.

## 2. Array of the COSE_Sign1(ES256) and COSE_Sign1(EdDSA)
```
82
   D2                   # tag(18)
      84                # array(4)
         43             # bytes(3)
            A1          # map(1)
               01       # unsigned(1)
               26       # negative(6) / -7 = cose-alg-es256 /
         A0             # map(0)
         58 1F          # bytes(31) / payload: /
            8301A31450A0A1A2A3A4A5A6A7A8A9AAABACADAEAF01818326F6F603810003
         58 40          # bytes(64) / signature: /
            583A259B237E886B1C1035DF0ECEEE4A07646E27C2B0CE580745092F06876D80D7FE8BDBDB704C3B792DA8D72E7180B49C3B716ECD54FB2F4BC4D46D9F6EA4BE
   D2                   # tag(18)
      84                # array(4)
         44             # bytes(4)
            A1          # map(1)
               01       # unsigned(1)
                  38 22 # negative(34) / -35 = cose-alg-es384 /
         A0             # map(0)
         58 1F          # bytes(31) / payload: /
            8301A31450A0A1A2A3A4A5A6A7A8A9AAABACADAEAF01818326F6F603810003
         58 60          # bytes(96) / signature: /
            59077B11E81C73617B8124670A51040F9F541BEEDC2AB6E876177013FB36BC960B0DDD2B8F39BD01BBDB5CBDA6182CB56D331EE8434FB543EDFB1B904D09349E3A47F4C92F1F60ABF412FF8096184EA6F58260528874473D3F669BC3F526491C
```

* Pros
  - The TEEP Agent can choose which one to verify (as 8. Ciphersuite says)
* Cons
  - The TEEP message as payload are redundant and memory cunsuming

## 3. Array of the Detached Content COSE_Sign1(ES256) and COSE_Sign1(EdDSA)
```
83
   58 1F                # bytes(31) / payload: /
      8301A31450A0A1A2A3A4A5A6A7A8A9AAABACADAEAF01818326F6F603810003
   D2                   # tag(18)
      84                # array(4)
         43             # bytes(3)
            A1          # map(1)
               01       # unsigned(1)
               26       # negative(6) / -7 = cose-alg-es256 /
         A0             # map(0)
         F6             # primitive(22) / null /
         58 40          # bytes(64) / signature: /
            583A259B237E886B1C1035DF0ECEEE4A07646E27C2B0CE580745092F06876D80D7FE8BDBDB704C3B792DA8D72E7180B49C3B716ECD54FB2F4BC4D46D9F6EA4BE
   D2                   # tag(18)
      84                # array(4)
         44             # bytes(4)
            A1          # map(1)
               01       # unsigned(1)
                  38 22 # negative(34) / -35 = cose-alg-es384 /
         A0             # map(0)
         F6             # primitive(22) / null /
         58 60          # bytes(96) / signature: /
            59077B11E81C73617B8124670A51040F9F541BEEDC2AB6E876177013FB36BC960B0DDD2B8F39BD01BBDB5CBDA6182CB56D331EE8434FB543EDFB1B904D09349E3A47F4C92F1F60ABF412FF8096184EA6F58260528874473D3F669BC3F526491C
```

* Pros
  - The TEEP Agent can choose which one to verify (as 8. Ciphersuite says)
  - The total message length is shorter than case 2
* Cons
  - Some COSE library does not support detached content


# Another Considerations
- If we generate the signature not from the digest hash of the message but from the entire message, the more longer the payload, the more longer time may be required to calculate signature

