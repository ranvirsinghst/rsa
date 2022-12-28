
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define options "i:o:n:vh"

void help() {
	printf("Usage: ./encrypt [options]\n"
"  ./encrypt encrypts an input file using the specified public key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
}

int main (int argc, char **argv) {

	int opt = 0;
	// default variables:


        FILE *in_file = stdin;
        FILE *out_file = stdout;
        char *pub = "rsa.pub";
        bool verbose = false;
        char *user[1024];

        while ((opt = getopt(argc, argv, options)) != -1) {
                switch (opt) {
                        case 'i':
                                in_file = fopen(optarg, "r");
                                break;
                        case 'o':
                                out_file = fopen(optarg, "w");
                                break;
                        case 'n':
                                pub = optarg;
                                break;
                        case 'v':
				verbose = true;
                                break;
                        case 'h':
				help();
				return 0;
                        default:
                        	help();
                        	return 0;
                }
        }
        
        FILE *pub_file = fopen(pub, "r");
    	if (pub_file == NULL) {
        	printf("There was an error while opening %s\n", pub);
        	return 1;
    	}
    	
    	mpz_t m, n, e, s;
    	mpz_inits(m, n, e, s, NULL);
    	rsa_read_pub(n, e, s, (char *)user, pub_file);
        mpz_set_str(m, (char *)user, 62);
    	if(verbose) {
    		size_t n_bits = mpz_sizeinbase(n, 2);
    		size_t e_bits = mpz_sizeinbase(e, 2);
    		size_t s_bits = mpz_sizeinbase(s, 2);
    		printf("username: %s\n", (char *)user);
    		gmp_printf("user signature (%zu bits): %Zd\nn - modulus (%zu bits): %Zd\ne - public exponent (%zu bits): %Zd\n", s_bits, s, n_bits, n, e_bits, e);
	}
	
	if(rsa_verify(m, s, e, n) == false) {
		printf("./encrypt: Couldn't verify user signature - exiting!\n");
		return 1;
	}
	
	rsa_encrypt_file(in_file, out_file, n, e);
	
	fclose(pub_file);
	fclose(in_file);
	fclose(out_file);
	mpz_clears(m, n, e, s, NULL);
	return 0;
}
