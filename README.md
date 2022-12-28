# RSA Encryption

### Building RSA Encryption
With the following files in your directory and the GMP Library installed, execute the command "make" in your terminal:
- numtheory.c
- numtheory.h
- randstate.c
- randstate.h
- rsa.c
- rsa.h
- keygen.c
- encrypt.c
- decrypt.c
- Makefile
Once the command is done running, three executables will be available: ./keygen, ./encrypt, and ./decrypt.

### Running RSA Encryption
Three different executables are available: ./keygen, ./encrypt, and ./decrypt.

./keygen: use this to generate a public and private RSA key pair
-b = minimum bits for public modulus n
-i = number of Miller-Rabin iterations
-n [pbfile]: specifies public key file
-d [pbfile]: specifies private key file
-s = random seed for initializing random state
-v enables verbose output
-h displays help and usage

./encrypt: use this with a public key to encrypt 
-i specifies input file to encrypt
-o specifies output file to write encrypted message to
-n specifies file containing public key for encryption
-v enables verbose output
-h displays help and usage

./decrypt:
-i specifies input file to decrypt
-o specifies output file to write decrypted message to
-n specifies file containing private key for decryption
-v enables verbose output
-h displays help and usage

### File Descriptions
- numtheory.c: contains numerical functions with mpz_t integers to be used in RSA encryption, decryption, and key generation operations
- numtheory.h: header file for numtheory.c
- randstate.c: establishes a random state for use in GMP library Mersenne's Twister random number generations
- randstate.h: header file for randstate.c
- rsa.c: contains functions utilized in RSA encryption, decryption, and key generation operations using functions from numtheory.c
- rsa.h: header file for rsa.c
- keygen.c: generates a public and private RSA key pair
- encrypt.c: given a public key, uses RSA encryption functions from rsa.c on a message and outputs the encrypted message to a file
- decrypt.c: given a private key, uses RSA decryption functions from rsa.c on a message and outputs the decrypted message to a file
- Makefile: compiles .c files into executables ./keygen, ./encrypt, and ./decrypt
- DESIGN.pdf: describes the design and thought process behind all assignment files pushed for asgn5
- WRITEUP.pdf: describes testing methods for files used in asgn5

