#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define options "i:o:n:vh"

void help() {
	printf("Usage: ./decrypt [options]\n"
"  ./decrypt decrypts an input file using the specified private key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
}

int main (int argc, char **argv) {

	int opt = 0;

        // default variables:
	

	FILE *in_file = stdin;
	FILE *out_file = stdout;
	char *priv = "rsa.priv";
        bool verbose = false;

        while ((opt = getopt(argc, argv, options)) != -1) {
                switch (opt) {
                        case 'i':
                                in_file = fopen(optarg, "r");
                                break;
                        case 'o':
                                out_file = fopen(optarg, "w");
                                break;
                        case 'n':
                                priv = optarg;
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
        
        FILE *priv_file = fopen(priv, "r");
    	if (priv_file == NULL) {
        	printf("There was an error while opening %s\n", priv);
        	return 1;
    	}
    	mpz_t n, d;
    	mpz_inits(n, d, NULL);
    	rsa_read_priv(n, d, priv_file);
    	
    	if(verbose) {
    		size_t n_bits = mpz_sizeinbase(n, 2);
    		size_t d_bits = mpz_sizeinbase(d, 2);
    		gmp_printf("n - modulus (%zu bits): %Zd\nd - private exponent (%zu bits): %Zd\n", n_bits, n, d_bits, d);
	}
	rsa_decrypt_file(in_file, out_file, n, d);
	fclose(priv_file);
	fclose(in_file);
	fclose(out_file);
	mpz_clears(n, d, NULL);
	return 0;
}
