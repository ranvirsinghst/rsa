
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#include <stdio.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <gmp.h>

#define options "b:i:n:d:s:vh"

void help() {
	printf("Usage: ./keygen [options]\n"
"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
"  the program options.\n"
"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
	return;
}

int main (int argc, char **argv) {

	int opt = 0;

	// default variables:
	
	uint64_t min_bits = 1024;
	int iters = 50;
	uint64_t seed = time(NULL);
	int verbose = 0;
	char *priv = "rsa.priv";
	char *pub = "rsa.pub";
	char *usr = getenv("USER");

	//printf("entering opt string\n");
	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'b':
				min_bits = strtoul(optarg, NULL, 10);
				break;
			case 'i':
				iters = strtoul(optarg, NULL, 10);
				break;
			case 'n':
				pub = optarg;
				break;
			case 'd':
				priv = optarg;
				break;
			case 's':
				seed = strtoul(optarg, NULL, 10);
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				help();
				return 0;
			default: help(); return 0;
		}
	}
	FILE *priv_file = fopen(priv, "w");
	FILE *pub_file = fopen(pub, "w");
	if(priv_file == NULL || pub_file == NULL) {
		printf("Could not open %s or %s.\n", pub, priv);
		return 1;
	}
	
	fchmod(fileno(priv_file), 0600);

	mpz_t s, p, q, n, e, d, m;
	mpz_inits(s, p, q, n, e, d, m, NULL);

	randstate_init(seed);
	
	mpz_set_str(m, usr, 62);
	rsa_make_pub(p, q, n, e, min_bits, iters);
	
	rsa_make_priv(d, e, p, q);
	rsa_sign(s, m, d, n);
	
	rsa_write_pub(n, e, s, usr, pub_file);
	rsa_write_priv(n, d, priv_file);


	if (verbose) {
		size_t sbits = mpz_sizeinbase(s, 2);
        	size_t pbits = mpz_sizeinbase(p, 2);
        	size_t qbits = mpz_sizeinbase(q, 2);
        	size_t nbits = mpz_sizeinbase(n, 2);
        	size_t ebits = mpz_sizeinbase(e, 2);
	        size_t dbits = mpz_sizeinbase(d, 2);
		// print username, signature s, prime p, prime q, public modulus n, public exponent e, private key d
		// print number of bits for each 
		printf("username: %s\n", usr);
		gmp_printf("user signature (%zu bits): %Zd\np (%zu bits): %Zd\nq (%zu bits): %Zd\nn - modulus (%zu bits): %Zd\ne - public exponent (%zu bits): %Zd\nd - private exponent (%zu bits): %Zd\n", sbits, s, pbits, p, qbits, q, nbits, n, ebits, e, dbits, d);
	}

	mpz_clears(s, p, q, n, e, d, m, NULL);
	fclose(pub_file);
	fclose(priv_file);
	randstate_clear();
}
