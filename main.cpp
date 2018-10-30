
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib> // strtol
#include <unistd.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "timed_commit.h"


#define ITERATIONS 255000000L
#define LOG_P 2048 // multiple of 512
#define AES_KEY_BITSIZE 256 // smaller or equal to log_p
#define ERR_IO     -1
#define ERR_MALLOC -2
#define ERR_NULL   -3
#define ENOMEM -4

/*
 * 
 * */
 
void pass_crypt_commit(mpz_t k, mpz_t p ,mpz_t commitment,int log_p, long iterations){
    generate_random(k,AES_KEY_BITSIZE/8);
    generate_random_prime(p,log_p/8);
    commit(commitment,k,iterations,p);
}

int main(int argc, char *argv[]){
    
    
    std::string data_file_name("/Users/trx/projetunicorn/timed_pass/data.txt"); //fichier des données par defaut
    FILE* sortie;
    
    long iterations = ITERATIONS;
    int log_p = LOG_P;

    mpz_t k,p,timed_commitment;
    
    if(argc >1){
        for (int i = 1; i < argc;){
            //on prend le premier caractère du ième argument
            if(argv[i][0] == '-'){
                switch (argv[i][1]){
                    case 'o': {
                        data_file_name = argv[i+1];
                        i += 2;
                        break;
                    }
                    case 'i': {
                        iterations = strtol(argv[i+1], NULL, 10); //on transforme l'argument iterations, reçu en tant que string, en int
                        i += 2;
                        break;
                    }
                    case 'l': {
                        log_p = strtol(argv[i+1], NULL, 10); //pareil pour logp
                        i += 2;
                        break;
                    }
                    default: {
                        i += 1;
                    }
                }
            }
            else {
                i = i + 1;
            }
        }
    }
     
    mpz_init(k);
    mpz_init(p);
    mpz_init(timed_commitment);
    pass_crypt_commit(k,p,timed_commitment,log_p,iterations);


    /* Ouverture du fichier en écriture */
    sortie = fopen(data_file_name.c_str(), "w");
    /* on teste si l'ouverture du flot s'est bien réalisée */
    if (sortie == NULL) {
        fprintf(stderr,"Erreur: le fichier %s ne peut etre ouvert en écriture !\n", data_file_name.c_str());
        return ERR_IO; 
    }

    char* strk=mpz_get_str(NULL,36,k);

    /*gmp_printf("%Zx\n", p);
    gmp_printf("%Zx\n", timed_commitment);
    std::cout<< strk << std::endl;*/
    gmp_fprintf(sortie,"%Zx\n", p);
    gmp_fprintf(sortie,"%Zx\n", timed_commitment);
    fprintf(sortie,"%s\n", strk);

    fclose(sortie);
    
    return 0;
}

