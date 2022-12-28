#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <gmp.h>
#include <time.h>
#include <stdbool.h>

void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t temp, a2, b2;
	mpz_inits(temp, a2, b2, NULL);
	mpz_set(a2, a);
	mpz_set(b2, b);
	while (mpz_sgn(b2) != 0) {
		mpz_set(temp, b2); // temp = b
		mpz_mod(b2, a2, b2); // b = a % b
		mpz_set(a2, temp); // a = temp (old b)
	}
	mpz_set(d, a2); // d = a
	
	mpz_clears(temp, a2, b2, NULL); // clearing used mpz_t vars
	return;
}

void lcm(mpz_t d, mpz_t a, mpz_t b) {
	// formula is |ab|/gcd(d,a,b)
	mpz_t term, abs_term, gcd_a_b;
	mpz_inits(term, abs_term, gcd_a_b, NULL);
	mpz_mul(term, a, b); // term = a * b
	mpz_abs(abs_term, term); // abs_term = |term|
	gcd(gcd_a_b, a, b); // gcd_a_b = gcd(a, b)
	mpz_cdiv_q(d, abs_term, gcd_a_b); // d = |term|/gcd(a, b)
	mpz_clears(term, abs_term, gcd_a_b, NULL);
	return;
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
	mpz_t r, r_prime, t, t_prime, q, qxr, qxt, temp_r, temp_t;
	mpz_inits(r, r_prime, t, t_prime, q, qxr, qxt, temp_r, temp_t, NULL); // t = 0;
	
	mpz_set(r, n); // r = n
	
	mpz_set(r_prime, a); // r' = a

	mpz_set_ui(t_prime, 1); // t' = 1
	while (mpz_cmp_ui(r_prime, 0) != 0) {
		mpz_fdiv_q(q, r, r_prime); // q = r/r'
		mpz_set(temp_r, r_prime);

		mpz_mul(qxr, q, r_prime); // qxr = q * r'
		mpz_sub(r_prime, r, qxr); // r' = r - (q * r')
		mpz_set(r, temp_r);
		
		mpz_set(temp_t, t_prime); // t = t'

		mpz_mul(qxt, q, t_prime); // qxt = q * t'
		mpz_sub(t_prime, t, qxt); // t' = t - (q * t')
		mpz_set(t, temp_t);
	}
	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_set_ui(o, 0); // o = 0
		mpz_clears(r, r_prime, t, t_prime, q, qxr, qxt, NULL);
		return;
	}
	if (mpz_cmp_ui(t, 0) < 0) {	
		mpz_add(t, t, n); // o = t + n
	}
	mpz_set(o, t);
	mpz_clears(r, r_prime, t, t_prime, q, qxr, qxt, NULL);
	return;
}
/*
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
	mpz_set_ui(o, 1); // o = 1
	mpz_t p, dd;
	mpz_inits(p, dd, NULL);
	mpz_set(p, a);
	mpz_set(dd, d);

	while (mpz_cmp_d(dd, 0) > 0) {
		if (mpz_odd_p(dd)) {
			// o  = (o * p) % n
			mpz_mul(o, o, p);
			mpz_mod(o, o, n);
		}
		// p = (p^2) % n
		mpz_mul(p, p, p);
		mpz_mod(p, p, n);
		// d = d/2
		mpz_fdiv_q_ui(dd, dd, 2);
	}

	mpz_clears(p, dd, NULL);
	return;
}
*/

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
	mpz_t v, p, dd;
	mpz_inits(v, p, dd, NULL);
	mpz_set_ui(v, 1);
	mpz_set(p, a);
	mpz_set(dd, d);
	
	while(mpz_cmp_ui(dd, 0) == 1) {
		if(mpz_odd_p(dd)) {
			mpz_mul(v, v, p);
			mpz_mod(v, v, n);
		}
		mpz_mul(p, p, p);
		mpz_mod(p, p, n);
		mpz_fdiv_q_ui(dd, dd, 2);
	}
	mpz_set(o, v);
}	

bool is_prime(mpz_t n, uint64_t iters) {
	mpz_t s, r, n_1, a, y, n_4, j, s_1, two;
	mpz_inits(s, r, n_1, a, y, n_4, j, s_1, two, NULL);
	
	mpz_sub_ui(n_1, n, 1); // n_1 = n - 1
	mpz_set(r, n_1); // r = n - 1

	while (mpz_even_p(r)) {
		mpz_fdiv_q_ui(r, r, 2); // n_minus /= 2
		mpz_add_ui(s, s, 1);
	}

	for (uint64_t i = 0; i < iters; i++) {
		//printf("outer for: %lu\n", i);
		mpz_set_ui(two, 2);
		// random integer between 2 and n-2
		mpz_sub_ui(n_4, n, 4);
		mpz_urandomm(a, state, n_4);
		mpz_add_ui(a, a, 2);
		// store power_mod in y
		pow_mod(y, a, r, n); // perform pow_mod on a, r, and n, store result in y

		if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_1) <= 0)) {
			mpz_set_ui(j, 1);
			mpz_sub_ui(s_1, s, 1);
			while ((mpz_cmp(j, s_1) <= 0) && (mpz_cmp(y, n_1))) {
				//printf("inner while\n");
				pow_mod(y, y, two, n);
				if (mpz_cmp_ui(y, 1) == 0) {
					mpz_clears(s, r, n_1, a, y, n_4, j, s_1, NULL);
					//gmp_printf("%Zd is not prime\n", n);
					return false;
				}
				//printf("adding 1 to j\n");
				mpz_add_ui(j, j, 1);
			}
			if (mpz_cmp(y, n_1) != 0) {
				mpz_clears(s, r, n_1, a, y, n_4, j, s_1, two, NULL);
				return false;
			}
		}
	}
	mpz_clears(s, r, n_1, a, y, n_4, j, s_1, two, NULL);
	
	return true;
}


void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	mpz_t increment_2, potential, mask;
	mpz_inits(increment_2, potential, mask, NULL);
	mpz_set_ui(increment_2, 2);
	mpz_pow_ui(mask, increment_2, bits);
	while (true) {
        	mpz_urandomb(potential, state, bits);
        	mpz_add(potential, potential, mask);
        	if (is_prime(potential, iters) == true) {
            		break;
        	}
    	}
    	mpz_set(p, potential);
    	mpz_clears(increment_2, potential, mask, NULL);
    	return;
}
