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

void generate_random(mpz_t result,const int bitlength){
  int bytelength=bitlength/8;
  bytelength+=(bitlength%8) ? 1 : 0 ;
  unsigned char *buf = (unsigned char *)malloc(bytelength+1);
  FILE* f=fopen("/dev/urandom","rb");
  if(f){
    fread(buf,1,bytelength,f);
    mpz_import(result,bytelength,1,1,0,0,buf);
    fclose(f);
  }
  free(buf);
}

void generate_random_prime(mpz_t result, int bitlength){
  mpz_t rand;
  mpz_init(rand);
  generate_random(rand,bitlength);
  mpz_setbit(rand,bitlength-1);
  next_prime(result,rand);
  mpz_clear(rand);
}

void generate_commit(char** N, char** P, char** Q, char** C, char** k, long bitlength_qp, long iterations, long aes_klength){
  mpz_t Nm,Pm,Qm,Cm,km, ktrunc, Pminus, Qminus, phiN, expf, expe;

  mpz_init(Nm);
  mpz_init(Pm);
  mpz_init(Qm);
  mpz_init(Cm);
  mpz_init(km);
  mpz_init(ktrunc);
  mpz_init(Pminus);
  mpz_init(Qminus);
  mpz_init(phiN);
  mpz_init(expf);
  mpz_init_set_ui(expe,3);

  do{
    generate_random_prime(Pm,bitlength_qp);
    generate_random_prime(Qm,bitlength_qp);

    mpz_sub_ui(Pminus, Pm, 1);
    mpz_sub_ui(Qminus, Qm, 1);

    mpz_mul(phiN, Pminus, Qminus);

  }while(mpz_gcd_ui(NULL,phiN,3)!=1);
  mpz_mul(Nm, Pm, Qm);

  generate_random(Cm,bitlength_qp);

  //compute
  mpz_powm_ui (expf, expe, iterations, phiN);
  mpz_powm (km, Cm, expf, Nm);
  //------

  mpz_tdiv_r_2exp(ktrunc,km,aes_klength);
  *k=mpz_get_str(NULL,36,ktrunc);
  *N=mpz_get_str(NULL,16,Nm);
  *P=mpz_get_str(NULL,16,Pm);
  *Q=mpz_get_str(NULL,16,Qm);
  *C=mpz_get_str(NULL,16,Cm);


  mpz_clear(Nm);
  mpz_clear(Pm);
  mpz_clear(Qm);
  mpz_clear(Cm);
  mpz_clear(km);
  mpz_clear(ktrunc);
  mpz_clear(Pminus);
  mpz_clear(Qminus);
  mpz_clear(phiN);
  mpz_clear(expf);
  mpz_clear(expe);
}

void force_open(char** k,const char* C,const char* N, long iterations, long aes_klength){
  mpz_t Nm,Pm,Qm,Cm,km, ktrunc, expf, expe;

  mpz_init_set_str(Nm,N,16);
  mpz_init_set_str(Cm,C,16);
  mpz_init(km);
  mpz_init(ktrunc);
  mpz_init(expf);
  mpz_init_set_ui(expe,3);

  //compute
  mpz_pow_ui (expf, expe, iterations);
  mpz_powm (km, Cm, expf, Nm);
  //------

  mpz_tdiv_r_2exp(ktrunc,km,aes_klength);
  *k=mpz_get_str(NULL,36,ktrunc);


  mpz_clear(Nm);
  mpz_clear(Cm);
  mpz_clear(km);
  mpz_clear(ktrunc);
  mpz_clear(expf);
  mpz_clear(expe);
}
