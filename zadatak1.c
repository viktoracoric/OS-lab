#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void prekidna_rutina();

void obrada_prekida(int j)
{
   sigignore(SIGINT);
   if(j==2)   sigignore(SIGQUIT);
   if(j==3) {
      sigignore(SIGQUIT);
      sigignore(SIGTSTP);
   }
   printf ("Poceo obradu prekida %d\n", j);
   int brojac_pomocni = 0;
   while(brojac_pomocni<10) {
   sleep(1);
   brojac_pomocni++;
   printf("Prekid razine %d: %d/10\n",j,brojac_pomocni);
   if(j==1) {
   sigset(SIGQUIT, prekidna_rutina);
   sigset(SIGTSTP, prekidna_rutina);
   }
   if(j==2)   sigset(SIGTSTP, prekidna_rutina);
   }
   printf ("Zavrsio obradu prekida %d\n", j);
}

void prekidna_rutina (int sig)
{
   int i;
   time_t t;
   time(&t);
   printf("Prekidna rutina pozvana u: %s", ctime(&t));
   switch (sig) {
   case SIGINT:
      i = 1;
      obrada_prekida(i);
      break;
   case SIGQUIT:
      i = 2;
      obrada_prekida(i);
      break;
   case SIGTSTP:
      i = 3;
      obrada_prekida(i);
      break;
   default:
      return;
   }
   printf ("Razina prekida: %d\n", i);
}

int main (void)
{
   printf ("Poceo osnovni program\n");
   int brojac = 0;
   while(brojac<10)
{
   sleep(1);
   brojac++;
   printf("Glavni program: %d/10\n",brojac);
   sigset (SIGINT, prekidna_rutina);
   sigset (SIGQUIT, prekidna_rutina);
   sigset (SIGTSTP, prekidna_rutina);
}
   printf ("Zavrsio osnovni program\n");
   return 0;
}
