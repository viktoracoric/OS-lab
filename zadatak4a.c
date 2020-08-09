#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int x;
int br=0;
pthread_mutex_t monitor;
pthread_cond_t red;

void *barijera(void *i) {
    int broj;
    int brdr= *((int *)i);
    pthread_mutex_lock(&monitor);
    br++;
    if(br<x) {
        printf("Dretva %d unesite broj: \n", brdr);
        scanf("%d", &broj);
        pthread_cond_wait(&red,&monitor);
        }
    else {
        printf("Dretva %d unesite broj: \n", brdr);
        scanf("%d", &broj);
        br=0;
        pthread_cond_broadcast(&red);
        }
//    pthread_mutex_unlock(&monitor);

//    pthread_mutex_lock(&monitor);
    br++;
    if(br<x) {
        printf("Dretva %d uneseni broj je: %d\n", brdr, broj);
        pthread_cond_broadcast(&red);
        }
    else {
        printf("Dretva %d uneseni broj je: %d\n", brdr, broj);
        br=0;
        pthread_cond_broadcast(&red);
        }
    pthread_mutex_unlock(&monitor);
    }

void brisi() {
    pthread_mutex_destroy(&monitor);
    pthread_cond_destroy(&red);
    exit(0);
    }

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Ovaj program prima samo jedan argument\n");
        exit(-1);
        }
    sigset(SIGINT, brisi);
    pthread_mutex_init(&monitor, NULL);
    pthread_cond_init(&red, NULL);
    x = atoi(argv[1]);
    pthread_t id[x];
    for(int i=0;i<x;i++) {
        int *arg = malloc(sizeof(*arg));
        if(arg == NULL) {
            printf("Greska pri alokaciji\n");
            exit(-2);
            }
        *arg = i;
        if(pthread_create(&id[i], NULL, barijera, arg) != 0) {
            printf("Greska\n");
            exit(-3);
            }
        }
    for(int i=0;i<x;i++)    pthread_join(id[i], NULL);
    brisi();
    return 0;
    }
