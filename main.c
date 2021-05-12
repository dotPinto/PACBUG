#include "pac.h"

int main(int argc, char const *argv[]){
  pthread_t fantar1,fantar2,fantar3;
  pthread_t checkID1,checkID2,checkID3;
  FILE* fp;
  Salvataggio s[5],s1[6];
  Salvataggio sTemp;
  punteggio = 0;
  log_space = 6;
  int i,j,max;

  printf("\nInserisci il nome del Player: ");
  scanf("%s",nickname);
  printf("\nIl tuo nome e: %s\n", nickname);
  sleep(2);

  array_fantasmi[0].id=0;
  array_fantasmi[0].colore=FANTASMA+array_fantasmi[0].id;
  array_fantasmi[0].posizione.y = -1;
  array_fantasmi[0].threadid = fantar1;
  array_fantasmi[1].id=1;
  array_fantasmi[1].colore=FANTASMA+array_fantasmi[1].id;
  array_fantasmi[1].posizione.y = -1;
  array_fantasmi[1].threadid = fantar2;
  array_fantasmi[2].id=2;
  array_fantasmi[2].colore=FANTASMA+array_fantasmi[2].id;
  array_fantasmi[2].posizione.y = -1;
  array_fantasmi[2].threadid = fantar3;

  srand(time(NULL));
  mainmenu(); //fa cose inutili

  initscr();
  noecho();
  curs_set(0);
  erase();
  makemap();

  //Avvio il thread di PACMAN gestito dall'utente
  pthread_create(&pacmanID,NULL,&pacman,NULL);

  while(punteggio<300){
    sleep(1);
  }
  //GENERAZIONE FANTASMI
  pthread_create(&array_fantasmi[0].threadid,NULL,&fantasma_aresti,(void*)&array_fantasmi[0].id);
  pthread_create(&checkID1,NULL,&controllo_vita,&(array_fantasmi[0].id));
  sleep(3);
  pthread_create(&array_fantasmi[1].threadid,NULL,&fantasma_aresti,(void*)&array_fantasmi[1].id);
  pthread_create(&checkID2,NULL,&controllo_vita,&(array_fantasmi[1].id));
  sleep(3);
  pthread_create(&array_fantasmi[2].threadid,NULL,&fantasma_aresti,(void*)&array_fantasmi[2].id);
  pthread_create(&checkID3,NULL,&controllo_vita,&(array_fantasmi[2].id));

  pthread_join(pacmanID,NULL);

  /*
  pthread_join(checkID1,NULL);
  pthread_join(checkID2,NULL);
  pthread_join(checkID3,NULL);
  */

  if(punteggio == PUNTEGGIOWIN){
      controllo_vittoria();
  }
  else
      hai_perso();

  printf("\nPremi qualsiasi tasto per continuare...");
  getchar();
  fp = fopen("classifica.bin","rb");
  if(fp == NULL){
      printf("\nSalvataggio non esistente");
      fp = fopen("classifica.bin","wb");
      if(fp == NULL)
        printf("\nErrore nella creazione del file");
      //Salvataggio
      strcpy(s[0].nickname,nickname);
      s[0].punti = punteggio;
      for(i=1;i<5;i++){
          s[i].punti = 0;
          strcpy(s[i].nickname,"NULL");
      }
      fwrite(s,sizeof(Salvataggio),5,fp);
      fclose(fp);
      printf("\nSalvataggio effettuato!");
  } else {
      fread(s,sizeof(Salvataggio),5,fp);
      for(i=0;i<5;i++){
          s1[i] = s[i];
      }
      strcpy(s1[5].nickname,nickname);
      s1[5].punti = punteggio;
      for(i=0;i<5;i++){
          max=i;
          for(j=i+1;j<6;j++){
              if(s1[j].punti > s1[max].punti)
                max = j;
          }
          sTemp = s1[max];
          s1[max] = s1[i];
          s1[i] = sTemp;
      }
      for(i=0;i<5;i++){
          s[i] = s1[i];
          printf("\n%d) %s - %d",i,s[i].nickname,s[i].punti);
      }
      fclose(fp);
      fp = fopen("classifica.bin","wb");
      if(fp == NULL)
        printf("\nErrore nell'apertura del file");
      fwrite(s,sizeof(Salvataggio),5,fp);
      fclose(fp);
      printf("\nSalvataggio effettuato!");
  }

  printf("\nChiusura del gioco...");
  return 0;
}
