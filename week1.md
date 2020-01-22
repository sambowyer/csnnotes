# Computer Security and Networks - Week One

## Computer Security
Correctness and efficiency against an attacker.
Decide what your assets are and the consider the impact/likelihood/risks/precautions needed for attacks.

Computer security protects the **confidentiality**, **integrity** and **availability** of your data.
These depend on the methods of transmission, storage, processing and can be achieved based on human factors, specific technologies as well as implementation of new policy and practices.

## A Threat/Attacker Model
Before building a security system, consider who the attacker/s may be, what they want, how they may attack etc.
Attackers may be:
+ Lone hackers/script kiddies: probably running known Attackers
+ Professional criminal gangs: DoS attacks, spam, phishing, ransomware etc.
+ Governments: Lots of computing power, may use wiretaps
+ ISPs: Won't break laws but do 'spy' on you and may sell/lose your data
+ Insiders

# Ciphers
Code - any way to represent data.  
Cipher - a code that is designed to be hard to ready (almost always uses a key) - e.g. Caesar/Viganere Ciphers

**_Kerckhoffs'_ Principle**: A cipher should be secure even if the attacker knows everything about it apart from the key.  

Caesar ciphers are easy to crack with brute force since only 26 possible keys. We can use frequency analysis (checking how often each letter appears) to check which characters are most likely to be s/t/a/e/... (most common letters in text).

You can create a cipher that doesn't allow frequency analysis by using a different key for each letter (like Viganere). A *one time pad* cipher is Viganere with a key equal in length to the message. This approach may use XOR (which is commutative, associative).  
If you think of applying the cipher as going through the original text XOR-ing with the key then you can do this XOR-ing again on the cipher text to get back the original text.  

This works since for all char x,y (with x being a character form the plain text and y being the corresponding key character, hence (x XOR y) is represents the corresponding cipher character):
```
x XOR x = 0,
x XOR 0 = x, and so
(x XOR y) XOR y = x XOR (y XOR y) = x XOR 0 = x
```
The problem with this is that the key needs to be as long as the message *and* you can't reuse the same key twice:
```
(x1 XOR y) XOR (x2 XOR y) = x1 XOR y XOR x2 XOR y (Associativity)
                          = x1 XOR x2 XOR y XOR y (Commutativity)
                          = x1 XOR x2             (This can be broken)
```
## Block ciphers
Modern ciphers work on blocks of text rather than on individual characters. For example AES works on blocks of 128-bits, generating 10 round keys from a single 128-bit key. It uses one permutation (ShiftRows) and three substitutions (SubBytes, MixColumns, AddRoundKey). It has no proof of being unbreakable (would require a proof that P != NP).

It works by putting character into a 4x4 square matrix and then performing the following operations:

+ "SubByte" is a fixed substitution based on matrix multiplication, one byte at a time.
+ “ShiftRows” moves the 2nd row one byte to the left, the 3rd row two bytes, and the 4th row 3 bytes.
+ “MixColumn” is a substitution of each column such that: (a0.x3 +a1.x2 + a2.x + a3) x (3.x3+x2+x+2) mod (x4+1) = ( b0.x3 + b1.x2 + b2.x+ b3 )
+ “AddRoundKey” xor’s the block with the 128-bit round key (which was generated from the main key). bi,j = ai,j xor ki,j

AES encrypts data by first generating the round keys from the main key.  
Then 9 rounds of: SubBytes ->ShiftRows -> MixColumns -> AddRoundKey  
Finally: SubBytes -> ShiftRows -> AddRoundKey

*(Java examples of AES on the CSN Canvas page)*

Other examples of modern block ciphers are DES and 3-DES. (Data Encryption Standard)

DES was designed by IBM in early 70s  - the NSA stepped in and fixed the key length at 56-bits (too short to protect against modern computing power) and added S-box substitutions (thought to be a backdoor, however, turned out to be a protection against differential cryptanalysis which the NSA knew about 20 years before it was publicly discovered in 1990).

3-DES takes three keys (if they are all set to be the same it gives you regular DES). It is expected to be secure until 2030.

## Padding

Padding is the technique of adding extra data into the message to be encrypted in case the message isn't quite the right size for the blocks needed for the encryption.
If there is 1 byte of padding needed we write '01', for 2 bytes we use '0202', 3 bytes gets '0303' etc. until the message goes to the end of a block in which case we add a block of '1616161616161616...' (16 lots of '16').  
