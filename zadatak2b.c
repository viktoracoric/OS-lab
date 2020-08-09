#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int *polje;
int arg1;
int izmjena=1;

void *genispis(void *arg) {
    for(int i=0;i<arg1;i++) {
        polje[i] = rand() % 100;
        printf("%d. %d\n",i,polje[i]);
        }
    izmjena=0;
    return(NULL);
    }

void *dretva(void *arg) {
    int suma = 0;
    for(int i=0;i<arg1;i++) {
        suma = suma + polje[i];
        }
    printf("Suma je: %d\n",suma);
    izmjena=1;
    return(NULL);
    }

int main(int argc, char **argv) {
    if(argc != 3)   {
        printf("Ovaj program prima samo dva argumenta\n");
        exit(-1);
            }
    if(atoi(argv[1])>10) {
        printf("Prvi argument mora biti do 10\n");
        exit(-1);
        }
    pthread_t tid[2],tid2;
    pthread_create(&tid2,NULL,&genispis,NULL);
    int retval, arg2;
    srand(time(NULL));
    arg1=atoi(argv[1]);
    polje=malloc(arg1 * sizeof(polje[0]));
    arg2=atoi(argv[2]);
    for(;arg2!=0;arg2--) {
        if(izmjena==1) {
            if(pthread_create(&tid[0],NULL,&genispis,NULL)!=0) {
                printf("Greska\n");
                exit(-1);
                }
            }
        while(izmjena==1);
        if(izmjena==0) {
            if(pthread_create(&tid[1],NULL,&dretva,NULL)!=0) {
                printf("Greska\n");
                exit(-1);
                }
            }
        pthread_join(tid[0],NULL);
        pthread_join(tid[1],NULL);
        }
    return 0;
    }
