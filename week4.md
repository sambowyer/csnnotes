# Computer Security and Networks - Week Three

## Cryptographic Protocols

### A Simple Protocol
“A” sends message “M” to “B”.   
Written as:  `Alice → Bob : “I’m Alice”``

We write down protocols as a list of messages sent between "principals":
```
1. A → B: “Hello”
2. B → A: “Offer”
3. A → B: “Accept"
```
The Attacker (Elvis) can pretend to be anyone (e.g. Alice). Here we'd write E(a) for Elvis pretending to be Alice:
`E(A) → B : “I’m Alice`

We can assume Alice and Bob share a private key (Kab) and represent symmetric key encryption with {...}<sub>Kab</sub>:
`A → B: {“I’m Alice”}_(Kab)`

Attackers cna intercept and replay messages (we assume the attacker 'owns' the network):
```
A → E(B) : {“I’m Alice”}_Kab
E(A) → B : {“I’m Alice”}_Kab
```

#### A Nonce
A nonce is a large random number which you can't guess, used to verify communications over a network.
```
1.  A → B: A
2.  B → A: {N_a}_Kab
3.  A → B: {N_a+1}_Kab , {Pay Elvis €5}_Kab
4.  A → B: A
5.  B → A: {N_a2}_Kab
6.  A → B: {N_a2+!}_Kab, {Pay Bob €5}_Kab
6'. A → B: {N_a+1}_Kab , {Pay Elvis €5}_Kab
```
If an attacker knows what has previously been sent they can replay it to exploit A and/or B.

A better protocol would be:
```
1. A → B :A, N_a
2. B → A : { N_a _}_ab
3. A → B : {N_a, Pay Elvis €5 }_Kab
```
Here N_a is encrypted *with* the 'Pay Elvis €5' message, meaning that the attacker can't find N<sub>a</sub> on it's own to form trustworthy messages.

### Key Establishment Protocol
The above protocol was possible because A and B shared a key. This is often done using a 'key establishment protocol'. A and B must either know each other's public keys or a "Trusted Third Party" (TTP).

#### The Needham-Schroeder Public Key Protocol
Assume Alice and Bob know each other's public keys:
```
1. A → B : E_B( N_a, A )
2. B → A : E_A( N_a, N_b )
3. A → B : E_B( N_b )
```
Here E_x(...) means public key encryption. After this protocol, N_a and N_b may be used to generate a symmetric key.

This is, however, vulnerable to attacks, such as this man-in-the-middle attack:
```
1. A → C : E_C( N_a, A )
    1`. C(A) → B : E_B( N_a, A )
    2`. B → C(A) : E_A( N_a, N_b )
2. C → A : E_A( N_a, N_b )
3. A → C : E_C( N_b )
    3`. C(A) → B : E_B( N_b )
```
Alice thinks they have connection with Charlie. Bob thinks it has connection with Alice, *but* it is actually with Alice.  
There is luckily a very simple fix to the original protocol for this:
```
1. A → B : E_B( N_a, A )
2. B → A : E_A( N_a, N_b, B )
3. A → B : E_B( N_b )
```
Where sending B means sending some 'identity' of B like a certified public key. Since Charlie can only replay Bob's message in 2' to Alice, Alice can't be tricked into thinking that it is speaking to Charlie (when it is actually speaking to Bob - this is the vulnerability of the previous attack).

##### Forward Secrecy
Unfortunately, this protocol isn't safe against government attackers - who could legally force people to hand over their private keys.

A protocol has “Forward Secrecy” if it keeps the message secret from an attacker who has:
- A recording of the protocol run
- The long term keys of the principals.

This provides protection against a government that can force people to give up their keys, or hackers that might steal them.

Diffie-Hellman is a good protocol that has 'forward secrecy'.

#### Station-to-Station Protocol
S<sub>x</sub>(...) means 'signed by X'.

1. A → B : g<sup>x</sup>
2. B → A : g<sup>y</sup>, { S<sub>B</sub>(g<sup>y</sup>,g<sup>x</sup>) }<sub>g<sup>xy</sup></sub>
3. A → B : { S<sub>A</sub>(g<sup>x</sup>,g<sup>y</sup>) }<sub>g<sup>xy</sup></sub>
4. B → A : { M }<sub>g<sup>xy</sup></sub>


- x,y, g<sup>xy</sup> are not stored after the protocol run.
- A&B’s keys don't let attacker read M
- STS has forward secrecy

TO be decrypted a government needs to know g<sup>xy</sup> but A and B have already 'thrown away' x and y (which were randomly generated), so the government can't possibly figure out g<sup>xy</sup>.

### Certificates
If Alice and Bob don't know each other's public keys to start with, they could either meet face-to-face and set up keys (impractical) *OR* get a trusted 3rd part (TTP) to sign their identity and public key - a certificate. *(Though TPPs have to also be concerned about government attackers.)*

#### Full Station to Station *(with certificates)*

1. A → B : g<sup>x</sup>
2. B → A : g<sup>y</sup>, Cert<sub>B</sub>, { S<sub>B</sub>(g<sup>y</sup>,g<sup>x</sup>) }<sub>g<sup>xy</sup></sub>
3. A → B : Cert<sub>A</sub>, { S<sub>A</sub>(g<sup>x</sup>,g<sup>y</sup>) }<sub>g<sup>xy</sup></sub>

The “full” STS protocol add certificates for A & B. These contain their public key signed by a TTP,
so Alice and Bob don’t need to know each others public keys.

#### The Needham-Schroeder Key Establishment Protocol
A and B use trusted 3rd party S to establish a key Kab:
```
1. A → S: A, B, N_a
2. S → A: {N_a ,B, K_ab, {K_ab , A}_Kbs}_Kas
3. A → B:{K_ab ,A}K_bs
4. B → A: {N_b}K_ab
5. A → B: {N_b+1}K_ab
```
Forward secrecy here relies on an attacker(e.g. government) not having access to K<sub>as</sub> or K<sub>bs</sub>.
```
1. A → S: A,B,N_a
2. S → A: {N_a ,B,K_ab,{K_ab ,A}_Kbs}_Kas
3. A → B: {K_ab ,A}_Kbs
4. B → A: {N_b}_Kab
5. A → B: {N_b+1}_Kab   

... much later ...

1'. A → B: {K_ab ,A}_Kbs
2’. B → A: {N_b}_Kab
3’. A → B: {N_b+1}Kab
```

### Key Establishment Goals
- **Key Freshness**: the key established is new either from some trusted 3rd party
or because it uses a new nonce.
- **Key Exclusivity**: the key is only known to the principals in the protocol.
- **Good Key**: the key is both fresh and exclusive

### Authentication Goals
**Far-end Operative**: A knows that “B” is currently active.

For instance B might have signed a nonce generated by A e.g.
```
1. A → B: N_a
2. B → A: Sign_B (N_a)
```
Not enough on its own, e.g. N.S. protocol.

**Once Authentication**: A knows that B wishes to communicate with A
For instance, B might have the name A in a message e.g.
```
1. B → A:Sign_B (A)
```

Both of these together give...

**Entity Authentication**: A knows that B is currently active and wants to communicate with A.
e.g.
```
1. A → B:Na
2. B → A:Sign_B (A,N_a)
```

### The Highest Goal
A protocol provides Mutual Belief in a Key K for Alice with respect to Bob if, after running the protocol Bob can be sure that:
- “K” is a good key with for Alive and Bob
- Alice can be sure that Bob wishes to
communicate with Alice using “K”
- Alice knows that Bob believes that “K” is a good key for “B”.

## The TLS & TOR Protocols

### The SSL/TLS Protocol
- The Secure Sockets Layer (SSL) protocol was renamed the Transport Layer Security (TLS) protocol.
- It provides encrypted socket communication and authentication, based on public keys.
- It may use a range of ciphers (RSA,DES,DH,..)
    - These are negotiated at the start of the run.

#### X.509 Standard for Certificates
- X.509 certificates contain a subject, subject’s public key, Issuer name, etc
- The issuer signs the hash of all the data
- To check a certificate I hash all the data and check the issuers public key.
- If I have the issuer’s public key, and trust the issuer, I can then be sure of the subject’s public key.

#### The Internet Protocol Stack
Most of the time the stack looks like:
- Application - (Applications e.g. browser)
- -> Transport - TCP (dest IP, dest port, src IP, src port)
- -> Network - Internet Protocol (IP address)
- -> Link/Hardware - Ethernet + MAC address

But with TLS, you have a TLS layer in between the Application layer and the Transport level. The encryption is transparent to the Application layer.

#### TLS Protocol
The actual main TLS protocol is as follows:

1. C → S: N<sub>C</sub>
2. S → C: N<sub>S</sub> ,Cert<sub>S</sub>
3. C → S: E<sub>S</sub>(K_seed), {Hash1}<sub>K<sub>CS</sub></sub>
4. S → C: {Hash2}<sub>K<sub>CS</sub></sub>

Hash 1 = #(N<sub>C</sub>,N<sub>S</sub>, E<sub>S</sub>(K_seed))  
Hash 2 = #(N<sub>C</sub>,N<sub>S</sub>, E<sub>S</sub>(K_seed), {Hash1}<sub>K<sub>CS</sub></sub> )  
K<sub>CS</sub> is a session key based on N<sub>C</sub>, N<sub>S</sub> & K_seed,
All data is then encrypted with K<sub>CS</sub> and hashed for integrity.

##### TLS-DHE
A variant uses Diffie-Hellman for “forward secrecy” (if someone gets the servers key later they can’t go back and break a recording of the traffic):
1. C → S: N<sub>C</sub>
2. S → C: N<sub>S</sub> , g<sup>x</sup>, Cert<sub>S</sub> , Sign<sub>S</sub>(#(N<sub>C</sub>,N<sub>S</sub>,g<sup>x</sup>))
3. C → S: g<sup>y</sup>, {#(All previous messages)}<sub>K<sub>CS</sub></sub>
4. S → C:: {#(All previous messages)}<sub>K<sub>CS</sub>
</sub>  

KCS is a session key based on NC, NS & gxy,
