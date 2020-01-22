# Computer Security and Networks - Week Two

## Blocking modes
Another problem we get is in the case where the message is longer than the length of one block. To deal with this we have different modes:
1. Electronic codebook mode (ECB)
    - each block is encrypted individually,
    - encrypted blocks are assembled in the same order as the plain text blocks.
    - if blocks are repeated in the plain text, this is revealed by the cipher text.
    - not too secure:
        1. Remove disk and make a copy.
        2. Replace disk in Playstation.
        3. Copy a file to the disk.
        4. Remove disk and find the bit of disk that changed (that’s your encrypted file).
        5. Copy target data to the user area.
        6. Restart the PlayStation and ask for your file back.
        7. PlayStation decrypts the file and gives you the plain text.
2. Cipher Block Chaining mode (CBC)
    - each block XOR'd with previous block, starting with a random initialisation vector (iv)
    - helps overcome replay attack
    - Suppose n blocks B1,B2,...,Bn
        - iv = random number
        - C1 = encrypt (B1 xor IV)
        - C2 = encrypt (B2 xor C1)
        - ...
        - Ci = encrypt(Bi xor C(i-1))
    - much better/more secure than ECB
    - This is an example of **probabilistic encryption** - uses random elements (e.g. the iv) to make every encryption different. This allows you to encrypt the same message with the same key without an attacker realising.
    - Playstation encrypts games (to stop them being stolen) using ECB rather than CBC because you can encrypt/decrypt in chunks, whereas with CBC you have to encrypt/decrypt the whole disk at once (since every block affects every following block).
3. Counter mode (CTR)
    - Suppose n blocks B1,B2,...,Bn
        - iv = random number
        - C1 = encrypt (B1 xor (IV))
        - C2 = encrypt (B2 xor (IV + 1))
        - C3 = encrypt (B3 xor (IV + 2))
        - ...
        - Ci = encrypt(Bi xor (IV + (i-1)))
    - Would protect against the previous PlayStation attack

### AES Cipher texts can be altered
AES encryption with a particular key maps any 128-b-t block to a 128-bit block (or 256).
AES decryption also maps any 128-bit block to a 128-bit block (or 256).

- CBC mode: any change affects all of the rest of the message.
- ECB mode: any change affects only the block.
- CTR mode: any change affects only the bits altered.

If I know enc(M1) and know M1, I can make a cipher text that decrypts to any message I want (M2):
```
Enc(M1) xor (M1 xor M2)
```

## Public Key Crypto
AES is effectively unbreakable with a 'long enough' key, but how do you get a key in the first place?

In public key encryption you have two keys: one for encryption and another one for decryption. The encryption key is made public but the decryption key is always secret.

### Diffie-Hellman
Uses two public parameters:
- generator *g* (often 260 bits long)
- prime *p* (often 1024 bits long, though might need to be 2048 bits now)

Alice and Bob pick random numbers r<sub>A</sub> and r<sub>B</sub> and find:
- t<sub>A</sub> =g<sup>r<sub>A</sub></sup> mod p, and
- t<sub>B</sub> =g<sup>r<sub>B</sub></sup> mod p
- Alice sends Bob t<sub>A</sub> and Bob sends to Alice t<sub>B</sub>.
- Alice calculates t<sub>B</sub><sup>r<sub>B</sub></sup> mod p
- Bob calculates t<sub>A</sub><sup>r<sub>B</sub></sup> mod p
- This gives the key K = g<sup>r<sub>A</sub>r<sub>B</sub></sup> mod p

An observer cannot work out r<sub>A</sub> and r<sub>B</sub> from t<sub>A</sub> and t<sub>B</sub> so the attacker cannot calculate the key. Hence we have used DH to set up a secure channel (though we should still use something else first to authenticate the person at the other end).

### Elgamal
Elgamal is Diffie-Hellman turned into public key scheme. It uses a fixed g and p.
- Alice picks r<sub>A</sub> as private key
- t<sub>A</sub> = g<sup>r<sub>A</sub></sup> mod p is the public key
- Encryption of message M: Bob chooses r<sub>B</sub>, sends (g<sup>r<sub>B</sub></sup> mod p, M * t<sub>A</sub><sup>r<sub>B</sub></sup> mod p)

### RSA
Public key system carefully generates e, d and n. Public key is (e,n) and the private key is (d,n).
Based on the fact that there is no (classical) polynomial algorithm for factoring products of (two) very large primes (though Shor's quantum algorithm could potentially break this). A recent paper said that n should be 2048 bits since a nation state may have the power to break RSA encryption with an n of length 1024-bit.
RSA is not probabilistic so if you encrypt a file with lots of repeated data in it, you  may end up seeing repetition in the encrypted file too.

## Using Public Key Crypto
Public key crypto is much slower than symmetric key. So instead of just using public key crypto, systems usually:
- make a new symmetric keys
- encrypt that with the public key
- then encrypt the message with the symmetric key.

### Signatures
Using RSA: E<sub>pub</sub>(D<sub>priv</sub>(M)) = M
This can be used to sign messages with a private key - this can then be verified with the public key. Hopefully any real crypto suite will not use the same key for encryption and signing (as this could be used to trick people into decrypting when they think they're signing).

### Saving keys
We want to protect read access to private keys and make sure the public ones are real.
KeyStore (for which a Java class exists) provides password protected storage for keys. The 'keytool' command can be used to generate keys outside Java.
A KeyStore holds password protected private and public keys as certificates.

### Certificates
A public key certificate binds a public key to an identity.
As well as a public key it contains a name, e-mail address, etc.
It is signed, with the private key, and anyone else that trusts it.
Key servers store public key certificates – E.g. http://pgp.mit.edu, http://keyserver.pgp.com
Many clients can automatically search a key server for unknown e-mail addresses.
But beware, there is no guarantee the key is not a fake.

## Hashes and Message Authentication Codes (MACs)

### Hashes
A hash of any message is a short string generated from that message. It is always the same and any small change makes the hash totally different. It is very hard to go from the hash to the message *AND* it is very unlikely that any two different messages have the same hash.

Hashes can be used for *signing*, instead of the RSA technique from above ^^^. Instead of signing the whole message, you can first form the hash (of the original unencrypted message) and sign that hash. This is very secure and allows you to check whether the message has been altered in any way (since a small change in the message leads to a big change in the hash).

Alice has a signing key Ks and wants to sign message M.
Detached Signature: D<sub>Ks</sub>(#(M)).

Hashing is used in:
- Download/Message verification
- Tying parts of a message together (hash the whole message)
- Hash message, then sign the hash.
- Protect Passwords
    - Store the hash, not the password

Attacks on hashes:
- Preimage Attack: Find a message for a given hash: very hard.
- Prefix Collision Attack: a collision attack where the attacker can pick a prefix for the message.
- Collision Attack: Find two “random” messages with the same hash.

Main 'families' of hashes are SHA (best) - current SHA3 is best - and MD (e.g. MD4 & MD5), though MD is only useful when we only care about preimage attacks or integrity from transmission erros (not attacks). Also MD6 is new and seems good & fast but was beaten by another hash to become 'SHA3'.

### Message Authentication Codes
MACs are hashes with a key. Written as MAC<sub>key</sub>(M). You can only make or check the hash if you know the key - this stops guessing attacks.
MACs are sometimes used for authentication (e.g. in Alice and Bank share keyA, Alice sends to the bank:
“Pay Bob £10”,MAC<sub>keyA</sub>(“Pay Bob £10”))
There's a possible 'length extension attack' on MACs - adding data to a MAC without knowing the key.

To make a MAC we need:
- A key
- A Short string from a long message
- Single bit change in message = totally different hash.
- Hard to go from the hash to the message.
- Unlikely that any two different messages have the same hash.
