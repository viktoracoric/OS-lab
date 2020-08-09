#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int prisutan(int okviri[], int brOkvira, int stranica) {
	for(int i=0; i<brOkvira; i++)
		if(stranica == okviri[i])
			return 1;
	return 0;
    }

void ispis(int okviri[], int brOkvira, int zahtjev, short pogodak) {
    for(int i=0; i<brOkvira; i++) {
        if(okviri[i] == -1) printf(" - ");
		else if (okviri[i] == zahtjev && pogodak) printf("(%d)", okviri[i]);
		else if (okviri[i] == zahtjev && !pogodak) printf("[%d]", okviri[i]);
		else if (okviri[i] != zahtjev) printf(" %d ", okviri[i]);
	    }
    }

int traziPoziciju(int okviri[],int brOkvira,int zahtjevi[],int trenutna,int bs) {
    for(int i=0;i<brOkvira;i++)
        if(okviri[i] == -1) 
            return i;

    int pozicija[brOkvira];
    for(int i=0;i<brOkvira;i++) {
        pozicija[i]=100;
        for(int j=trenutna+1;j<bs;j++) {
            if(zahtjevi[j]==okviri[i]) {
                pozicija[i]=j;
                break;
                }
            }
        }

    int max=-1;
    int pPozicija=-1;
    for(int i=0;i<brOkvira;i++) {
        if(pozicija[i]>max) {
            max=pozicija[i];
            pPozicija=i;
            }
        }
    return pPozicija;
    }

int main(int argc, char **argv) {
    if(argc != 3) {
        printf("Ovaj program prima samo dva argumenta\n");
        exit(1);
        }
    int pozicija=0;
    int brOkvira = atoi(argv[1]);
    int n = atoi(argv[2]);
    int okviri[brOkvira];
    for(int i=0;i<brOkvira;i++) okviri[i]=-1;
    int zahtjevi[n];
    printf("Zahtjevi: ");
    srand(time(NULL));
    for(int i=0;i<n;i++) {
        zahtjevi[i] = (rand() % 8) + 1;
        if(i+1==n)  printf("%d\n", zahtjevi[i]);
        else    printf("%d, ", zahtjevi[i]);
        }
    short pogodak = 0;
    printf("#N");
    for(int i=1;i<=brOkvira;i++) {
        printf(" %d ", i);
        }
    printf("\n");
    for(int i=0;i<n;i++) {
        printf("%d ",zahtjevi[i]);
        if(!prisutan(okviri,brOkvira,zahtjevi[i])) {
             int pozicija = traziPoziciju(okviri,brOkvira,zahtjevi,i,n);
             okviri[pozicija]=zahtjevi[i];
             ispis(okviri,brOkvira, zahtjevi[i], pogodak);
             printf("\n");
             continue;
            }
        pogodak = 1;
        ispis(okviri,brOkvira, zahtjevi[i], pogodak);
        printf(" #pogodak\n");
        pogodak = 0;
        }
    }
