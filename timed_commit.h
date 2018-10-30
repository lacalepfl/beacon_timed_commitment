#ifndef TIMED_COMMIT_H
#define TIMED_COMMIT_H

#include <gmp.h>

void generate_random(mpz_t result, int bytelength);
void generate_random_prime(mpz_t result, int bytelength);
void commit(mpz_t commitment, const mpz_t input, int iterations, const mpz_t p);
void force_open(mpz_t opened , const mpz_t commitment, int iterations, const mpz_t p);

#endif // TIMED_COMMIT_H
