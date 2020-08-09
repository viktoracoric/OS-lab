#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct zajednickeVarijable {
    int trazim;
    int broj;
    };

struct zajednickeVarijable *zajednicke;
int x;
int id;

int max() {
    int pom=0;
    for(int i=0;i<x;i++) {
        if(zajednicke[i].broj>pom) pom=zajednicke[i].broj;
        }
    return pom;
    }

void udji_u_KO(int i) {
    zajednicke[i].trazim = 1;
    zajednicke[i].broj = max() + 1;
    zajednicke[i].trazim = 0;
    for(int j=0;j<x;j++) {
        while(zajednicke[j].trazim != 0);
        while(zajednicke[j].broj != 0 && (zajednicke[j].broj < zajednicke[i].broj || (zajednicke[j].broj == zajednicke[i].broj && j < i)));
        }
    }

void izadji_iz_KO(int i) {
    zajednicke[i].broj = 0;
    }

void proces(int i) {
    for(int k=1;k<=5;k++) {
        udji_u_KO(i);
        for(int m=1;m<=5;m++) {
            printf("Proces: %d, K.O. br: %d (%d/5)\n",i+1,k,m);
            usleep(500000);
            }
        izadji_iz_KO(i);
        }
    }

void izlaz(int sig) {
    shmdt(zajednicke);
    shmctl(id,IPC_RMID,NULL);
    exit(0);
    }

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Ovaj program prima jedan argument\n");
        exit(-1);
        }
    sigset(SIGINT,izlaz);
    x = atoi(argv[1]);
    id = shmget(IPC_PRIVATE,x*sizeof(struct zajednickeVarijable),0600);
    zajednicke = (struct zajednickeVarijable *) shmat(id,NULL,0);
    for(int i=0;i<x;i++) {
        zajednicke[i].trazim = 0;
        zajednicke[i].broj = 0;
        }
    for(int i=0;i<x;i++) {
        if(fork() == 0) {
            proces(i);
            exit(0);
            }
        }
    for(int i=0;i<x;i++)    wait(NULL);
    izlaz(0);
    return(0);
    }
