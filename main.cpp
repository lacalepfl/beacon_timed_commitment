
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib> // strtol
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "timed_commit.h"


#define TIMEDITERATIONS 300000000000L
#define BITSIZE_QP 512
#define AES_KEY_BITSIZE 256
#define ERR_IO     -1
#define ERR_MALLOC -2
#define ERR_NULL   -3
#define ENOMEM -4

/*
 * 
 * */


int main(int argc, char *argv[]){
    
    
    std::string data_file_name("/Users/trx/projetunicorn/timed_pass/data.txt"); //fichier des données par defaut
    FILE* sortie;
    
    long iterations = TIMEDITERATIONS;
    int log_pq = BITSIZE_QP;
    long aesk_length=AES_KEY_BITSIZE;


    
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
                        log_pq = strtol(argv[i+1], NULL, 10); //pareil pour logpq
                        i += 2;
                        break;
                    }
                    case 'k': {
                        aesk_length = strtol(argv[i+1], NULL, 10);
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

    /* Erreur si le nom du fichier est null */
    if(data_file_name.c_str() ==NULL){
        return ERR_NULL;
    }

    char *N, *P, *Q, *C, *k;
    generate_commit(&N,&P,&Q,&C,&k,BITSIZE_QP,TIMEDITERATIONS,AES_KEY_BITSIZE);


    /* Ouverture du fichier en écriture */
    sortie = fopen(data_file_name.c_str(), "w");
    /* on teste si l'ouverture du flot s'est bien réalisée */
    if (sortie == NULL) {
        fprintf(stderr,"Erreur: le fichier %s ne peut etre ouvert en écriture !\n", data_file_name.c_str());
        return ERR_IO; 
    }


    fprintf(sortie, "%s\n", k);
    fprintf(sortie, "%s\n", C);
    fprintf(sortie, "%s\n", N);
    fprintf(sortie, "%s\n", P);
    fprintf(sortie, "%s\n", Q);

    fclose(sortie);
    
    return 0;
}

