# Computer Security and Networks - Week Three

## The TLS & Tor Protocols (continued)

### Weaknesses in TLS

- Configuration weaknesses:
    - Cipher downgrading
    - Self signed certificates
- Direct attacks against implementations:
    - Apple’s goto fail bug.
    - LogJam attack
    - HeartBleed

#### Configuration Weaknesses

##### Cipher Downgrading

There are cipher suites for TLS, some include encryption, some include authentication and some include both. A client and server will have different suites available to them, but will have to use one that they have in common

If an attacker owns the network, they can remove suites with strong encryption, forcing clients to connect with weaker encryption (so long as their system supports that weaker suite).

##### Self Signed Certificates
If a client accepts a self signed certificate (because its easier than maintaining a set of certificates), they might as well not be using TLS at all - it is easy to then form a man-in-the-middle attack.

####  Direct attacks against implementations

##### Apple's TLS Problem

```
...
 if (...)
    goto fail;
    goto fail; //this always runs
...
```

Apple's TLS implementation once had a bug where there was a rogue 'goto fail' that ended up being executed before all of the vital checks where done. This had the effect of essentially changing the TLS-DHE protocol from this:

1. C → S: N<sub>C</sub>
2. S → C: N<sub>S</sub> , g<sup>x</sup>, Cert<sub>S</sub> , Sign<sub>S</sub>(#(N<sub>C</sub>,N<sub>S</sub>,g<sup>x</sup>))
3. C → S: g<sup>y</sup>, {#(All previous messages)}<sub>K<sub>CS</sub></sub>
4. S → C:: {#(All previous messages)}<sub>K<sub>CS</sub>
</sub>  

to this:

1. C → S: N<sub>C</sub>
2. S → C: N<sub>S</sub> , g<sup>x</sup>, Cert<sub>S</sub> , True
3. C → S: g<sup>y</sup>, {#(All previous messages)}<sub>K<sub>CS</sub></sub>
4. S → C:: {#(All previous messages)}<sub>K<sub>CS</sub>
</sub>  

Which effectively ruins the security of the protocol.

##### LogJam

Edward Snowden's leaks showed that the NSA regularly used main-in-the-middle (*MitM*) attacks on Diffie-Hellman exchanging protocols which set up DH keys with a much smaller prime than expected (e.g. 512 bits rather than 1024 or 2048). If the client didn't check this, the NSA could then (still as the MitM), easily crack the encryption, since the prime is now small enough for cracking to be computationally viable.

##### HeartBleed

HeartBleed was a programming error in OpenSSL.
TLS clients can request and 'heart beat' from the server to make sure the connection is still open. The attack occurs when the client requested a much longer message and would be given parts of the servers memory. This memory could sometimes contain the server's key.

### TLS 1.3
- Newest standard, ratified August 2018
- Removes obsolete cryptographic protocols
- Simplified handshake => efficiency gain
- Forward secrecy mandatory
- Intercepting TLS connections now only possible as active attacker performing MITM attack

## Tor - (anonymity across networks)

We have very little privacy online, some ways to help this are:

#### Proxy: Hotspot Shield VPN

Use a 'Shotspot Shield' to connect to that will connect to the servers you need on your behalf, so those servers never see you IP

#### Virtual Private Networks (VPNs)
Securely (with certificates and encryption) connects you to another network. You can route all of your traffic via a VPN to provide anonymity to the servers you connnect to. *However*, there is no anonymity to the VPN - you have to trust them.

#### Tor: Onion Routing
You get the best anonymity by routing your traffic via a number of proxies. Onion routing ensures that your message really is routed through the proxies that you want.

In Tor, each proxy only learns the IP of the proxy before and the proxy after it (source IP known only to the first node, the destination IP only known to the last node). The public key of each proxy is known. The user picks 3 proxies, and is anonymous as they aren't all corrupt.

##### Hidden Servers
Servers can run Tor to 'hide' themselves from the rest of the internet.

*But*, anonymity does not equal security - webservers running on Tor can still be attacked (and have been by the FBI to implant malware).

## Automated Protocol Verification
*(Not examinable)*

Basically trying to formally prove the effectiveness/security of a given protocol.
