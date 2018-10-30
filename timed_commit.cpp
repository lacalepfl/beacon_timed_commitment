#include <ctime>
#include <string>
#include <iostream>

#include <string.h>

#include "gmp.h"

#include "timed_commit.h"


static void next_prime(mpz_t p, const mpz_t n) {
    if (mpz_even_p(n)) mpz_add_ui(p,n,1);
    else mpz_add_ui(p,n,2);
    while (!mpz_probab_prime_p(p, 25)) mpz_add_ui(p,p,2);
}

static void prev_prime(mpz_t p, const mpz_t n) {
    if (mpz_even_p(n)) mpz_sub_ui(p,n,1);
    else mpz_sub_ui(p,n,2);
    while (!mpz_probab_prime_p(p, 25)) mpz_sub_ui(p,p,2);
}

// the sqrt permutation as specified in the paper (returns a sqrt of either input or -input)
static void sqrt_permutation(mpz_t result, const mpz_t input, const mpz_t p, const mpz_t e) {
  mpz_t tmp;
  mpz_init(tmp);
  if (mpz_jacobi(input, p) == 1) {
    mpz_powm(tmp, input, e, p);
    if (mpz_even_p(tmp)) mpz_set(result, tmp);
    else mpz_sub(result, p, tmp);
  }
  else {
    mpz_sub(tmp, p, input);
        mpz_powm(tmp, tmp, e, p);
    if (mpz_odd_p(tmp)) mpz_set(result, tmp);
    else mpz_sub(result, p, tmp);
  }

  mpz_clear(tmp);
}

// inverse of sqrt_permutation, so basicaly computes squares
static void invert_sqrt(mpz_t result, const mpz_t input, const mpz_t p) {
  mpz_t tmp;
  mpz_init(tmp);
  if (mpz_even_p(input)) {
    mpz_mul(tmp, input, input);
    mpz_mod(result, tmp, p);
  }
  else {
    mpz_mul(tmp, input, input);
    mpz_mod(tmp, tmp, p);
    mpz_sub(result, p, tmp);  
  }

  mpz_clear(tmp);
}

// computes input1 ^ flip ^ flip ^ ... ^ flip for the minimal number of "^ flip" (at least 1, at most 2) such
// that the result is smaller than mod
static void xor_mod(mpz_t result, const mpz_t input1, const mpz_t flip, const mpz_t mod) {
    mpz_xor(result,input1,flip);
    while (mpz_cmp(result, mod) >= 0 || mpz_cmp_ui(result, 0) == 0) {
        mpz_xor(result,result,flip);
    }
}

void generate_random(mpz_t result,const int bytelength){
  unsigned char *buf = (unsigned char *)malloc(bytelength+1);
  FILE* f=fopen("/dev/urandom","rb");
  if(f){
    fread(buf,1,bytelength,f);
    mpz_import(result,bytelength,1,1,0,0,buf);
    fclose(f);
  }
  free(buf);
}

void generate_random_prime(mpz_t result, int bytelength){
  mpz_t rand;
  mpz_init(rand);
  generate_random(rand,bytelength);
  mpz_setbit(rand,bytelength*8-1);
  next_prime(result,rand);
  mpz_clear(rand);
}

void commit(mpz_t commitment, const mpz_t input, int iterations, const mpz_t p){
  mpz_t a, ones;
  mpz_init_set(a,input);

  mpz_init_set_ui(ones, 1);
  mpz_mul_2exp(ones, ones, mpz_sizeinbase(p,2) >> 1);
  mpz_sub_ui(ones, ones, 1);

  for (int i = 0; i < iterations; ++i) {
      invert_sqrt(a, a, p);
      //invert_permutation(a, a);
      xor_mod(a,a,ones,p);
  }

  mpz_set(commitment,a);

  mpz_clear(a);
  mpz_clear(ones);
}

void force_open(mpz_t opened , const mpz_t commitment, int iterations, const mpz_t p){
  mpz_t a, ones, e;
  //a est initialisé à seed
  mpz_init_set(a, commitment);

  mpz_init_set_ui(ones, 1);
  mpz_mul_2exp(ones, ones, mpz_sizeinbase(p,2) >> 1); // flip half the bits (least significant)
  mpz_sub_ui(ones, ones, 1);

  // compute the exponent for sqrt extraction

  mpz_init_set(e, p);
  mpz_add_ui(e, e, 1);
  mpz_tdiv_q_ui(e, e, 4);

  for (int i = 0; i < iterations; ++i) {
      //permutation(a, a);
      xor_mod(a,a,ones,p);
      sqrt_permutation(a, a, p, e);
  }

  //witness devient a
  mpz_set(opened, a);

  mpz_clear(a);
  mpz_clear(ones);
}
