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

#define STAVI 0
#define CEKAJ 1

void generiraj();
void racunaj(int i);

int arg1;
int arg2;
int id;
int SemId;
long long int *broj;
long long int dv;
long long int suma;

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

void brisi() {
    shmctl(id,IPC_RMID,NULL);
    SemRemove();
    exit(0);
    }

int main(int argc, char** argv) {
    if(argc!=3) {
        printf("Ovaj program prima samo dva argumenta\n");
        exit(1);
        }
    sigset(SIGINT,brisi);
    srand(time(NULL));
    arg1 = atoi(argv[1]);
    arg2 = atoi(argv[2]);
    id = shmget(IPC_PRIVATE, sizeof(int)*arg1, 0600);
    broj = (long long int *) shmat(id,NULL,0);
    SemGet(2);
    SemSetVal(CEKAJ,1);
    SemSetVal(STAVI,0);
    if(fork()==0) {
        generiraj(arg2);
        exit(0);
        }
    for(int i=0;i<arg1;i++) {
        if(fork()==0) {
            racunaj(i+1);
            exit(0);
            }
        }
    for(int i=0;i<=arg2;i++)    wait(NULL);
    brisi();
    return 0;
    }

void generiraj(int m) {
    for(int i=0;i<m;i++) {
        *broj = rand() % 1000000000;
        printf("Generirani broj: %lli\n", *broj);
        SemOp(STAVI,-1);
        SemOp(CEKAJ,1);
        }
    *broj = -1;
    }

void racunaj(int i) {
    while(1) {
        if(*broj==-1)    break;
        suma=0;
        SemOp(CEKAJ,-1);
        printf("Proces %d je počeo s radom\n", i);
        dv = *broj;
        printf("Proces broj %d preuzeo zadatak broj %lli\n", i, dv);
        SemOp(STAVI,1);
        for(int i=1;i<=dv;i++)    suma = suma + i;
        printf("Proces %d: Zadatak=%lli suma: %lli\n", i, dv, suma);
        }
    }
