#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int x;
int y;
int id;

struct memorija {
    pthread_mutex_t monitor;
    pthread_mutexattr_t dijeljeniM;
    pthread_cond_t red[2];
    pthread_condattr_t dijeljeniR;
    int siti;
    int br[2];
    int ceka[2];
    };

struct memorija *podaci;

void ispis(int vrsta, char stanje[]) {
    printf("Red Linux: ");
    for(int i=0;i<y;i++) {
        if(i<podaci->ceka[0]) printf("L");
        else printf("-");
        }
    printf(" Red Microsoft: ");
    for(int i=0;i<y;i++) {
        if(i<podaci->ceka[1]) printf("M");
        else printf("-");
        }
    printf(" Restoran: ");
    if(vrsta==0 || podaci->br[0]>0) {
        for(int i=0;i<podaci->br[0];i++)    printf("L");
        }
    if(vrsta==1 || podaci->br[1]>0) {
        for(int i=0;i<podaci->br[1];i++)    printf("M");
        }
    printf(" --> ");
    if(vrsta==0)    printf("L %s\n", stanje);
    else    printf("M %s\n", stanje);
    }

void udji(int vrsta) {
    pthread_mutex_lock(&podaci->monitor);
    podaci->ceka[vrsta]++;
    while(podaci->br[1-vrsta]>0 || ((podaci->siti >= x) && (podaci->ceka[1-vrsta]>0))) {
        ispis(vrsta, "u red cekanja");
        pthread_cond_wait(&podaci->red[vrsta], &podaci->monitor);
        }
    podaci->br[vrsta]++;
    podaci->ceka[vrsta]--;
    if(podaci->ceka[1-vrsta]>0) podaci->siti++;
    ispis(vrsta, "u restoran");
    pthread_mutex_unlock(&podaci->monitor);
    }

void izadji(int vrsta) {
    pthread_mutex_lock(&podaci->monitor);
    podaci->br[vrsta]--;
    if(podaci->br[vrsta] == 0) {
        podaci->siti = 0;
        pthread_cond_broadcast(&podaci->red[1-vrsta]);
        }
    ispis(vrsta, "iz restorana");
    pthread_mutex_unlock(&podaci->monitor);
    }

void programer(int vrsta) {
    usleep(rand()%100);
    udji(vrsta);
    usleep(rand()%100);
    izadji(vrsta);
    }

void start() {
    pthread_mutexattr_init(&podaci->dijeljeniM);
    pthread_mutexattr_setpshared(&podaci->dijeljeniM, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&podaci->monitor, &podaci->dijeljeniM);
    pthread_condattr_init(&podaci->dijeljeniR);
    pthread_condattr_setpshared(&podaci->dijeljeniR, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&podaci->red[0], &podaci->dijeljeniR);
    pthread_cond_init(&podaci->red[1], &podaci->dijeljeniR);
    }

void brisi() {
    pthread_mutexattr_destroy(&podaci->dijeljeniM);
    pthread_mutex_destroy(&podaci->monitor);
    pthread_condattr_destroy(&podaci->dijeljeniR);
    pthread_cond_destroy(&podaci->red[0]);
    pthread_cond_destroy(&podaci->red[1]);
    shmdt(podaci);
    shmctl(id,IPC_RMID,NULL);
    exit(0);
    }

int main(int argc, char** argv) {
    if(argc != 3) {
        printf("Ovaj program prima dva argumenta\n");
        exit(-1);
        }
    sigset(SIGINT, brisi);
    x = atoi(argv[1]);
    y = atoi(argv[2]);
    id = shmget(IPC_PRIVATE, sizeof(struct memorija), 0600);
    podaci = (struct memorija *) shmat(id, NULL, 0);
    podaci->siti=0;
    podaci->br[0] = 0;
    podaci->br[1] = 0;
    podaci->ceka[0] = 0;
    podaci->ceka[1] = 0;
    start();
    for(int i=0;i<y*2;i++) {
        if(fork() == 0) {
            programer(i%2);
            exit(0);
            }
        }
    for(int i=0;i<y*2;i++)  wait(NULL);
    brisi();
    return 0;
    }
