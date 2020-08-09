#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define PISI 0
#define PUN 1
#define PRAZAN 2

int arg1;
int arg2;
int id;
int SemId;

struct zajednicki {
    int ULAZ, IZLAZ;
    int M[5];
    };

struct zajednicki *podaci;

//pomoćne funkcije

void SemGet(int n) {
    SemId = semget(IPC_PRIVATE,n,0600);
    if(SemId == -1) {
        printf("Nema semafora\n");
        exit(1);
        }
    }

int SemSetVal(int SemNum, int SemVal) {
    return semctl(SemId, SemNum, SETVAL, SemVal);
    }

int SemOp(int SemNum, int SemOp) {
    struct sembuf SemBuf;
    SemBuf.sem_num = SemNum;
    SemBuf.sem_op = SemOp;
    SemBuf.sem_flg = 0;
    return semop(SemId, & SemBuf, 1);
    }

void SemRemove() {
    semctl(SemId,0,IPC_RMID,0);
    }

void brisi(int sig) {
    shmctl(id,IPC_RMID,NULL);
    SemRemove();
    exit(0);
    }

//kraj pomoćnih funkcija

void proizvodjac(int j) {
    int i=0;
    do {    
        SemOp(PUN,-1);
        SemOp(PISI,-1);
        podaci->M[podaci->ULAZ] = rand() % 1000;
        printf("Proces %d salje \"%d\"\n", j, podaci->M[podaci->ULAZ]);
        sleep(1);
        podaci->ULAZ = (podaci->ULAZ+1) % 5;
        SemOp(PISI,1);
        SemOp(PRAZAN,1);
        i = i+1;
        } while(arg2!=i);
    printf("Proizvodjac %d je zavrsio sa slanjem\n", j);
    }

void potrosac() {
    int i=0;
    int zbroj=0;
    do {
        SemOp(PRAZAN,-1);
        zbroj = zbroj + podaci->M[podaci->IZLAZ];
        printf("Potrosac prima %d\n",podaci->M[podaci->IZLAZ]);
        sleep(1);
        podaci->IZLAZ = (podaci->IZLAZ+1) % 5;
        SemOp(PUN,1);
        i=i+1;
        } while(arg2*arg1!=i);
    printf("Potrosac - zbroj iznosi: %d\n",zbroj);
    }

int main(int argc, char** argv) {
    if(argc!=3) {
        printf("Ovaj program prima samo dva argumenta\n");
        exit(1);
        }
    sigset(SIGINT,brisi);
    arg1 = atoi(argv[1]);
    arg2 = atoi(argv[2]);
    SemGet(3);
    SemSetVal(PISI,1);
    SemSetVal(PUN,5);
    SemSetVal(PRAZAN,0);
    id = shmget(IPC_PRIVATE, sizeof(struct zajednicki), 0600);
    podaci = (struct zajednicki *) shmat(id, NULL, 0);
    podaci->ULAZ=0;
    podaci->IZLAZ=0;
    for(int i=0;i<arg1;i++) {
        if(fork() == 0) {
            srand(time(NULL)+getpid());
            proizvodjac(i+1);
            exit(0);
            }
        }
    if(fork() == 0) {
        potrosac();
        exit(0);
        }
    for(int i=0;i<=arg1;i++)    wait(NULL);
    brisi(0);
    return 0;
    }
