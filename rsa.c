#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <gmp.h>
#include <math.h>
#include <time.h>

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	// make two large prime numbers p and q
	uint64_t nbits_low = nbits / 4;
	uint64_t nbits_high = (nbits * 3) / 4;
	uint64_t p_bits = (random() % (1 + nbits_high - nbits_low)) + nbits_low;
	uint64_t q_bits = nbits - p_bits;
	make_prime(p, p_bits, iters);
	make_prime(q, q_bits, iters);
	mpz_mul(n, p, q);
	
	//lambda(n) = lcm(p - 1, q - 1)
	mpz_t lambda, p_1, q_1, e_gcd;
	mpz_inits(lambda, p_1, q_1, e_gcd, NULL);

	mpz_sub_ui(p_1, p, 1);
	mpz_sub_ui(q_1, q, 1);

	lcm(lambda, p_1, q_1);

	while(mpz_cmp_ui(e_gcd, 1) != 0) {
		mpz_urandomb(e, state, nbits);
		gcd(e_gcd, e, lambda);
	}

	mpz_clears(lambda, p_1, q_1, e_gcd, NULL);
	return;
	
}


void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	// write n, e, and s to pbfile as hexstrings
	gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
	// write username to pbfile as is
	fprintf(pbfile, "%s\n", username);	
	return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	// scan n, e, and s from pbfile back into decimal
	gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n", n, e, s);
	// read username from pbfile
	fscanf(pbfile, "%s\n", username);
	return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	//gmp_printf("d: %Zd\n", d);
	// mod_inverse(e, lambda(n) = p-1, q-1)
	mpz_t lambda, p_1, q_1;
	mpz_inits(lambda, p_1, q_1, NULL);

	mpz_sub_ui(p_1, p, 1);
	mpz_sub_ui(q_1, q, 1);

	lcm(lambda, p_1, q_1);
	//mpz_mul(lambda, p_1, q_1);
	//gmp_printf("lambda: %Zd\n e: %Zd\n", lambda, e);
	mod_inverse(d, e, lambda); 
	mpz_clears(lambda, p_1, q_1, NULL);
	//gmp_printf("d: %Zd\n", d);
	return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	// write n and d as hexstrings to pvfile
	gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);	
	return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	// read n and d from hexstrings back into decimal from pvfile
	gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
	return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	// c = (m^e) % n
	pow_mod(c, m, e, n);
	return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	mpz_t c, m;
	mpz_inits(c, m, NULL);
	
	// finding block size
	size_t k, j;
       	k = (mpz_sizeinbase(n, 2) - 1) / 8;
	
	// allocating array of blocks
	uint8_t *block = (uint8_t *)calloc(k ,sizeof(uint8_t));
	// start of block must be 0xFF to preserve size
	block[0] = 0xFF;
	// feof() from https://stackoverflow.com/questions/1835986/how-to-use-eof-to-run-through-a-text-file-in-c
	while(!feof(infile)) {
		// Read j < k - 1 bytes from infile, place them into the allocated array from index 1.  
		// Use mpz_import to convert bytes from hex into mpz_tm
		j = fread(block + 1, sizeof(uint8_t), k - 1 ,infile);
		mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block); 
		rsa_encrypt(c, m, e, n);
		gmp_fprintf(outfile,"%Zx\n", c);
	}
	
	mpz_clears(c, m, NULL);
	free(block);
	return;
}


void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	// m = (c^d) % n
	pow_mod(m, c, d, n);
	return;
}


void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	mpz_t c, m;
	mpz_inits(c, m, NULL);
	
	size_t k, j;
	k = (mpz_sizeinbase(n, 2) - 1) / 8;
	uint8_t *block = (uint8_t *)calloc(k, sizeof(uint8_t));
	while(!feof(infile)) {
		gmp_fscanf(infile, "%Zx\n", c);
		rsa_decrypt(m, c, d, n);
		mpz_export(&block[0], &j, 1, sizeof(uint8_t), 1, 0, m); 
		fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
	}
	mpz_clears(c, m, NULL);
	free(block);
	return;
}


void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	// s = (m^d) % n
	pow_mod(s, m, d, n);
	return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	mpz_t t;
	mpz_init(t);
	pow_mod(t, s, e, n);

	if(mpz_cmp(t, m) != 0) {
		mpz_clear(t);
		return false;
	}
	mpz_clear(t);
	return true;
}
