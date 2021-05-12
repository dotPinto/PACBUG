#include "pac.h"

void changemode(int dir){
  static struct termios oldt, newt;

  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}


/*
 * Funzione che permette di salvare l'ultima direzione in input e far proseguire nella stessa
 * direzione fino a nuovo input
*/
int kbhit (void){
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);

  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);

}


/*
 * Procedura che incrementa il punteggio quando pacman mangia un pallino
 * @param y coordinate di pacman
 * @param x coordinate di pacman
*/
void incrementaPunteggio(int y,int x){
  if(map[y][x]=='*'){
    pthread_mutex_lock(&mutex);
    punteggio=punteggio+100;
    mvprintw(0,(MAXX+3),"Punteggio: %d",punteggio);
    refresh();
    map[y][x]=' ';
    pthread_mutex_unlock(&mutex);
  }
}


/*
 * Funzione per verificare se la prossima coordinata di pacman o dei fantasmi è nei confini della
 * mappa
 * @param y coordinate
 * @param x coordinate
 * @return se la prossima posizione non coincide con un confine restituisce true
*/
_Bool puoi_andare(int y, int x){
  if(map[y][x]==' ' || map[y][x]=='*')
    return true;
  else if (map[y][x]=='v')
    return true;
  else
    return false;
}


/*
 * Procedura per mostrare il titolo
*/
void mainmenu(){
  int i;
  initscr();
  noecho();
  curs_set(0);

  attron(A_BLINK);
  mvprintw(10,5,".--. .   . .--.      .    .   .    .   .");
  mvprintw(11,5,"|   )|   |:          |\\  /|  / \\   |\\  |");
  mvprintw(12,5,"|--: |   || --. ____ | \\/ | /___\\  | \\ |");
  mvprintw(13,5,"|   ):   ;:   |      |    |/     \\ |  \\|");
  mvprintw(14,5,"'--'  `-'  `--'      '    '       `'   '");
  refresh();
  attroff(A_BLINK);
  mvprintw(17,14,"Un gioco creato da: ");
  mvprintw(18,16,"Susha & Pinto");

for(i=5;i<45;i++){
  mvaddch(15,0+i,ACS_CKBOARD);
  usleep(10000);
  refresh();
}
sleep(1);
erase();
mvprintw(10,5,".--. .   . .--.      .    .   .    .   .");
mvprintw(11,5,"|   )|   |:          |\\  /|  / \\   |\\  |");
mvprintw(12,5,"|--: |   || --. ____ | \\/ | /___\\  | \\ |");
mvprintw(13,5,"|   ):   ;:   |      |    |/     \\ |  \\|");
mvprintw(14,5,"'--'  `-'  `--'      '    '       `'   '");
attron(A_BLINK);
mvprintw(17,14,"PREMI QUALSIASI TASTO");
mvprintw(19,16,"PER CONTINUARE");
attroff(A_BLINK);
getch();
}


/*
 * Procedura per creare la mappa del gioco
*/
void makemap(){
  int i,j;

  start_color();
  init_pair(MURI,COLOR_BLUE,COLOR_BLACK);
  attron(COLOR_PAIR(MURI));
  attron(A_BOLD);


  for(i=0;i<MAXY;i++){
    for(j=0;j<MAXX;j++){
              switch(map[i][j]){
                  case 'x':
                  case 'X':
                      mvaddch(i,j,ACS_HLINE);
                      break;
                  case 'y':
                  case 'Y':
                      mvaddch(i,j,ACS_VLINE);
                      break;
                  case 'l':
                      mvaddch(i,j,ACS_ULCORNER);
                      break;
                  case 'L':
                      mvaddch(i, j,ACS_LLCORNER);
                      break;
                  case 'I':
                      mvaddch(i,j,ACS_URCORNER);
                      break;
                  case 'i':
                      mvaddch(i,j,ACS_LRCORNER);
                      break;
                  case 'u':
                      mvaddch(i,j,ACS_BTEE);
                      break;
                  case 'd':
                      mvaddch(i,j,ACS_TTEE);
                      break;
                  case 'e':
                      mvaddch(i,j,ACS_RTEE);
                      break;
                  case 'r':
                      mvaddch(i,j,ACS_LTEE);
                          break;
                  case '*':
                      attroff(COLOR_PAIR(MURI));
                      init_pair(WHITE,COLOR_WHITE,COLOR_BLACK);
                      attron(COLOR_PAIR(WHITE));
                      attroff(A_BOLD);
                      mvaddch(i,j,'*');
                      attroff(COLOR_PAIR(WHITE));
                      attron(COLOR_PAIR(MURI));
                      attron(A_BOLD);
                      break;
                  default:
                      mvaddch(i,j,'\t');
              }
          }
  }
  attroff(COLOR_PAIR(MURI));
  attroff(A_BOLD);

  refresh();
}


/*
 * Funzione che controlla se ci sono deviazioni o meno dei fantasmi
 * @param posizione posizione corrente
 * @param dir_corrente direzione in cui stanno andando i fantasmi
 * @return true se vi è stata almeno una deviazione
 */
_Bool controllo_deviazione(pos posizione,direzione dir_corrente){
  int contatore;

  contatore=0;
  switch (dir_corrente) {
    case SOPRA:
      if(puoi_andare(posizione.y,posizione.x-1))
      contatore++;
      if(puoi_andare(posizione.y,posizione.x+1))
      contatore++;
      if(puoi_andare(posizione.y-1,posizione.x))
      contatore++;
      break;
    case SOTTO:
      if(puoi_andare(posizione.y,posizione.x-1))
      contatore++;
      if(puoi_andare(posizione.y,posizione.x+1))
      contatore++;
      if(puoi_andare(posizione.y+1,posizione.x))
      contatore++;
      break;
    case A_SINISTRA:
      if(puoi_andare(posizione.y,posizione.x-1))
      contatore++;
      if(puoi_andare(posizione.y+1,posizione.x))
      contatore++;
      if(puoi_andare(posizione.y-1,posizione.x))
      contatore++;
      break;
    case A_DESTRA:
      if(puoi_andare(posizione.y,posizione.x+1))
      contatore++;
      if(puoi_andare(posizione.y-1,posizione.x))
      contatore++;
      if(puoi_andare(posizione.y+1,posizione.x))
      contatore++;
      break;
  }

  if(contatore>1)	// Se il fantasma può andare in più di una direzione restituisce true
    return true;
  else			// Altrimenti false
    return false;

}


/*
 * Procedura che crea il thread di pacman
*/
void * pacman(void * parametri){
  pthread_t sparoID;	// id thread sparo
  time_t interv;	// Intervallo di tempo per poter risparare
  char c;
  // Posizione iniziale di pacman
  pos_pacman.x=13;
  pos_pacman.y=23;
  num_vite = 3;
  life = 10;
  time(&interv);

  init_pair(PACMAN,COLOR_YELLOW,COLOR_BLACK);
  attron(COLOR_PAIR(PACMAN));
  pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
  mvaddch(pos_pacman.y,pos_pacman.x,'O');
  attroff(COLOR_PAIR(PACMAN));
  mvprintw(4,(MAXX+3),"LOG:");
  refresh();
  pthread_mutex_unlock(&mutex);/*Fine sezione critica*/

  pthread_mutex_lock(&mutex);
  mvprintw(2,(MAXX+3),"Vite: %d",num_vite);
  mvprintw(2,(MAXX+13),"Life: %d ",life);
  //mvprintw(2,(MAXX+22),"Sparo:"); //=28
  refresh();
  pthread_mutex_unlock(&mutex);

	do{
    c=getch();

		switch(c)	// Mi sposto in base al tasto che ho premuto
		{								// controllando ogni volta di non oltrepassare i muri
		case SU:
        while ( !kbhit() && punteggio < PUNTEGGIOWIN && num_vite > 0 && puoi_andare(pos_pacman.y-1,pos_pacman.x)){
            pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
            attron(COLOR_PAIR(PACMAN));
            mvaddch(pos_pacman.y,pos_pacman.x,' ');
            pos_pacman.y-=1;
            mvaddch(pos_pacman.y,pos_pacman.x,ACS_DARROW);
            attroff(COLOR_PAIR(PACMAN));
            refresh();
            pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
            usleep(300000);
            incrementaPunteggio(pos_pacman.y,pos_pacman.x);
        }
			break;

		case GIU:
        while ( !kbhit() && punteggio < PUNTEGGIOWIN  && num_vite > 0 && puoi_andare(pos_pacman.y+1,pos_pacman.x)){
            pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
            attron(COLOR_PAIR(PACMAN));
            mvaddch(pos_pacman.y,pos_pacman.x,' ');
      			pos_pacman.y+=1;
            mvaddch(pos_pacman.y,pos_pacman.x,ACS_UARROW);
            attroff(COLOR_PAIR(PACMAN));
            refresh();
            pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
            usleep(300000);
            incrementaPunteggio(pos_pacman.y,pos_pacman.x);
        }
			break;

		case SINISTRA:
          while ( !kbhit() && punteggio < PUNTEGGIOWIN && num_vite > 0 && puoi_andare(pos_pacman.y,pos_pacman.x-1)){
            if(pos_pacman.y == 14 && pos_pacman.x-1 == 0){
              pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
              attron(COLOR_PAIR(PACMAN));
              mvaddch(pos_pacman.y,pos_pacman.x,' ');
              pos_pacman.x=MAXX-1;
              mvaddch(pos_pacman.y,pos_pacman.x,ACS_RARROW);
              attroff(COLOR_PAIR(PACMAN));
              refresh();
              pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
              usleep(300000);
            }
            else if(puoi_andare(pos_pacman.y,pos_pacman.x-1)){
              pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
              attron(COLOR_PAIR(PACMAN));
              mvaddch(pos_pacman.y,pos_pacman.x,' ');
        			pos_pacman.x-=1;
              mvaddch(pos_pacman.y,pos_pacman.x,ACS_RARROW);
              attroff(COLOR_PAIR(PACMAN));
              refresh();
              pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
              usleep(300000);
              incrementaPunteggio(pos_pacman.y,pos_pacman.x);
            }
          }
			break;

		case DESTRA:
        while ( !kbhit() && punteggio < PUNTEGGIOWIN  && num_vite > 0 && puoi_andare(pos_pacman.y,pos_pacman.x+1)){
          if(pos_pacman.y == 14 && pos_pacman.x+1 == MAXX-1){
            pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
            attron(COLOR_PAIR(PACMAN));
            mvaddch(pos_pacman.y,pos_pacman.x,' ');
            pos_pacman.x=0;
            mvaddch(pos_pacman.y,pos_pacman.x,ACS_RARROW);
            attroff(COLOR_PAIR(PACMAN));
            refresh();
            pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
            usleep(300000);
          }
          else if(puoi_andare(pos_pacman.y,pos_pacman.x+1)){
            pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
            attron(COLOR_PAIR(PACMAN));
            mvaddch(pos_pacman.y,pos_pacman.x,' ');
      			pos_pacman.x+=1;
            mvaddch(pos_pacman.y,pos_pacman.x,ACS_LARROW);
            attroff(COLOR_PAIR(PACMAN));
            refresh();
            pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
            usleep(300000);
            incrementaPunteggio(pos_pacman.y,pos_pacman.x);
          }
        }
			break;

    case SPAZIO:

      if(time(NULL)-interv>=6){
        time(&interv);

        if(puoi_andare(pos_pacman.y-1,pos_pacman.x) && map[pos_pacman.y-1][pos_pacman.x] != '*'){
          a_bullet[3][0].y = pos_pacman.y-1;
          a_bullet[3][0].x = pos_pacman.x;
        }
        else{
          a_bullet[3][0].y = -1;
          a_bullet[3][0].x = -1;
        }
        if(puoi_andare(pos_pacman.y+1,pos_pacman.x) && map[pos_pacman.y+1][pos_pacman.x] != '*'){
          a_bullet[3][1].y = pos_pacman.y+1;
          a_bullet[3][1].x = pos_pacman.x;
        }
        else{
          a_bullet[3][1].y = -1;
          a_bullet[3][1].x = -1;
        }
        if(puoi_andare(pos_pacman.y,pos_pacman.x-1) && map[pos_pacman.y][pos_pacman.x-1] != '*'){
          a_bullet[3][2].y = pos_pacman.y;
          a_bullet[3][2].x = pos_pacman.x-1;
        }
        else{
          a_bullet[3][2].y = -1;
          a_bullet[3][2].x = -1;
        }
        if(puoi_andare(pos_pacman.y,pos_pacman.x+1) && map[pos_pacman.y][pos_pacman.x+1] != '*'){
          a_bullet[3][3].y = pos_pacman.y;
          a_bullet[3][3].x = pos_pacman.x+1;
        }
        else{
          a_bullet[3][3].y = -1;
          a_bullet[3][3].x = -1;
        }
        pthread_create(&sparoID,NULL,&sparo,NULL);
      }
      break;
    }
} while(num_vite > 0 && punteggio < PUNTEGGIOWIN);

    pthread_exit(NULL);
}


/*
 * Funzione che genera casualmente la posizione di spawn dei fantasmi dai pallini
 * @param posizione posizione inizializzata dei fantasmi
 * @return posizione dello spawn dei fantasmi
*/
pos genera_fantasma(pos posizione){

  do{
    posizione.x = 0+rand()%MAXX;
    posizione.y = 0+rand()%MAXY;
  }while(map[posizione.y][posizione.x]!=' ');

  return posizione;
}


/*
 * Procedura che crea il thread di un fantasma
*/
void * fantasma_aresti(void * temp){
  int * id;
  int gen_y,gen_x;
  id = (int*) temp;

  array_fantasmi[*id].vita=true;

  pthread_mutex_lock(&mutex);
  mvprintw(log_space,(MAXX+3),"Fantasma %d spawnato!",array_fantasmi[*id].id);
  // if(log_space<=12){
  log_space+=1;
  // }
  // else
  //   log_space=6;
  refresh();
  pthread_mutex_unlock(&mutex);

  if(array_fantasmi[*id].posizione.y == -1){
    array_fantasmi[*id].posizione = genera_fantasma(array_fantasmi[*id].posizione);
  }
  else{
      gen_y=13+rand()%3;
      gen_x=11+rand()%5;
      array_fantasmi[*id].posizione.y=11;
      array_fantasmi[*id].posizione.x=13;
      pthread_mutex_lock(&mutex);
      attron(COLOR_PAIR(array_fantasmi[*id].colore));
      attron(A_BLINK);
      mvaddch(gen_y,gen_x,'M');
      attroff(A_BLINK);
      attroff(COLOR_PAIR(array_fantasmi[*id].colore));
      refresh();
      pthread_mutex_unlock(&mutex);

      sleep(1+rand()%4);

      pthread_mutex_lock(&mutex);
      mvaddch(gen_y,gen_x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_lock(&mutex);
  attron(COLOR_PAIR(array_fantasmi[*id].colore));
  mvaddch(array_fantasmi[*id].posizione.y,array_fantasmi[*id].posizione.x,'M');
  attroff(COLOR_PAIR(array_fantasmi[*id].colore));
  refresh();
  pthread_mutex_unlock(&mutex);

  sleep(2);
  if(num_vite > 0)
    movimento_fantasma(*id);

  pthread_exit(NULL);
}


/*
 * Funzione che registra la precedente direzione di un fantasma
 * @param dir_corr salva la direzione attuale prima che venga cambiata
 * @return la direzione opposta
*/
direzione oldDirec(direzione dir_corr){
  if(dir_corr == SOPRA)
    return SOTTO;
  else if(dir_corr == SOTTO)
    return SOPRA;
  else if(dir_corr == A_DESTRA)
    return A_SINISTRA;
  else
    return A_DESTRA;
}


/**
 * Procedura che regola il movimento di un fantasma e gestisce i bivi
 * @param id parametro che permette di identificare un fantasma e la sua posizione
*/
void movimento_fantasma(int id){
  int oldDir,oppDir;
  time_t interv;
  pthread_t sparoID;
  int oldPos[2];
  array_fantasmi[id].direzione = rand()%4;
  oldDir = array_fantasmi[id].direzione;
  int id2,id3;

  init_pair(FANTASMA+0,COLOR_RED,COLOR_BLACK);
  init_pair(FANTASMA+1,COLOR_CYAN,COLOR_BLACK);
  init_pair(FANTASMA+2,COLOR_GREEN,COLOR_BLACK);
  init_pair(PACMAN,COLOR_YELLOW,COLOR_BLACK);

  array_fantasmi[id].flagScontro = false;
  switch(id){
    case 0:
      id2 = 1;
      id3 = 2;
      break;
    case 1:
      id2 = 0;
      id3 = 2;
      break;
    case 2:
      id2=0;
      id3=1;
      break;
  }

  time(&interv);

   while(array_fantasmi[id].vita && num_vite > 0 && punteggio < PUNTEGGIOWIN){

       if(!array_fantasmi[id].flagScontro){
           array_fantasmi[id].direzione = rand()%4;
           oppDir = oldDirec(oldDir);
            if(num_vite<1)
             break;
            pthread_mutex_lock(&mutex);
            //debug purpose
            mvprintw((MAXY+array_fantasmi[id].id),MAXX+2,"Vita Fantasma id%d: %d   POS_Y: %d POS_X: %d ",array_fantasmi[id].id,(int)array_fantasmi[id].vita,array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x);   //debug purpose
            refresh();
            pthread_mutex_unlock(&mutex);
       } else {
           array_fantasmi[id].flagScontro = false;
       }

    switch(array_fantasmi[id].direzione){

    case SOPRA:

      while(puoi_andare(array_fantasmi[id].posizione.y-1,array_fantasmi[id].posizione.x) && array_fantasmi[id].vita && array_fantasmi[id].direzione != oppDir && punteggio < PUNTEGGIOWIN){
          scontro(&array_fantasmi[id],-1,0,&array_fantasmi[id2],&array_fantasmi[id3]);
          if(array_fantasmi[id].flagScontro){
              array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
              break;
          }

        oldDir = array_fantasmi[id].direzione;
        oldPos[0]=array_fantasmi[id].posizione.y;
        oldPos[1]=array_fantasmi[id].posizione.x;
        pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
        if (map[oldPos[0]][oldPos[1]] == '*') {
          mvaddch(oldPos[0],oldPos[1],'*');
        }
        else{
          mvaddch(oldPos[0],oldPos[1],' ');
          if(oldPos[0] == pos_pacman.y && oldPos[1] == pos_pacman.x){
             attron(COLOR_PAIR(PACMAN));
             mvaddch(oldPos[0],oldPos[1],'O');
             attroff(COLOR_PAIR(PACMAN));
          }
        }

        array_fantasmi[id].posizione.y-=1;
        attron(COLOR_PAIR(array_fantasmi[id].colore));
        mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,'M');
        attroff(COLOR_PAIR(array_fantasmi[id].colore));
        refresh();
        pthread_mutex_unlock(&mutex);/*Fine sezione critica*/
        if(array_fantasmi[id].posizione.y == pos_pacman.y && array_fantasmi[id].posizione.x == pos_pacman.x){
          colpito();
        }
        usleep(350000);
        if(controllo_deviazione(array_fantasmi[id].posizione,array_fantasmi[id].direzione)){
            if(array_fantasmi[id].flagScontro){
                array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
            }
          break;
        }
      }
      break;

    case SOTTO:

    while(puoi_andare(array_fantasmi[id].posizione.y+1,array_fantasmi[id].posizione.x) && array_fantasmi[id].vita && array_fantasmi[id].direzione != oppDir && punteggio < PUNTEGGIOWIN){
        scontro(&array_fantasmi[id],1,0,&array_fantasmi[id2],&array_fantasmi[id3]);
        if(array_fantasmi[id].flagScontro){
            array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
            break;
        }
      oldDir = array_fantasmi[id].direzione;
      oldPos[0]=array_fantasmi[id].posizione.y;
      oldPos[1]=array_fantasmi[id].posizione.x;
      pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
      if (map[oldPos[0]][oldPos[1]] == '*') {
        mvaddch(oldPos[0],oldPos[1],'*');
      }
      else{
        mvaddch(oldPos[0],oldPos[1],' ');
        if(oldPos[0] == pos_pacman.y && oldPos[1] == pos_pacman.x){
           attron(COLOR_PAIR(PACMAN));
           mvaddch(oldPos[0],oldPos[1],'O');
           attroff(COLOR_PAIR(PACMAN));
        }
      }

      array_fantasmi[id].posizione.y+=1;
      attron(COLOR_PAIR(array_fantasmi[id].colore));
      mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,'M');
      attroff(COLOR_PAIR(array_fantasmi[id].colore));
      refresh();
      pthread_mutex_unlock(&mutex);
      if(array_fantasmi[id].posizione.y == pos_pacman.y && array_fantasmi[id].posizione.x == pos_pacman.x){
          colpito();
      }
      usleep(350000);
      if(controllo_deviazione(array_fantasmi[id].posizione,array_fantasmi[id].direzione)){
          if(array_fantasmi[id].flagScontro){
              array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
          }
        break;
      }
    }
    break;

    case A_SINISTRA:

        while(puoi_andare(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x-1) && array_fantasmi[id].vita && array_fantasmi[id].direzione != oppDir && !(array_fantasmi[id].posizione.y == 14 && array_fantasmi[id].posizione.x-1 == 5) && punteggio < PUNTEGGIOWIN){
            scontro(&array_fantasmi[id],0,-1,&array_fantasmi[id2],&array_fantasmi[id3]);
            if(array_fantasmi[id].flagScontro){
                array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
                break;
            }
          oldDir = array_fantasmi[id].direzione;
          oldPos[0]=array_fantasmi[id].posizione.y;
          oldPos[1]=array_fantasmi[id].posizione.x;
          pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
          if (map[oldPos[0]][oldPos[1]] == '*') {
            mvaddch(oldPos[0],oldPos[1],'*');
          }
          else{
            mvaddch(oldPos[0],oldPos[1],' ');
            if(oldPos[0] == pos_pacman.y && oldPos[1] == pos_pacman.x){
               attron(COLOR_PAIR(PACMAN));
               mvaddch(oldPos[0],oldPos[1],'O');
               attroff(COLOR_PAIR(PACMAN));
            }
          }

          array_fantasmi[id].posizione.x-=1;
          attron(COLOR_PAIR(array_fantasmi[id].colore));
          mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,'M');
          attroff(COLOR_PAIR(array_fantasmi[id].colore));
          refresh();
          pthread_mutex_unlock(&mutex);
          if(array_fantasmi[id].posizione.y == pos_pacman.y && array_fantasmi[id].posizione.x == pos_pacman.x){
              colpito();
          }
          usleep(350000);
          if(controllo_deviazione(array_fantasmi[id].posizione,array_fantasmi[id].direzione)){
              if(array_fantasmi[id].flagScontro){
                  array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
              }
            break;
          }
        }
        break;

    case A_DESTRA:

      while(puoi_andare(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x+1) && array_fantasmi[id].vita && array_fantasmi[id].direzione != oppDir && !(array_fantasmi[id].posizione.y == 14 && array_fantasmi[id].posizione.x+1 == 22) && punteggio < PUNTEGGIOWIN){
          scontro(&array_fantasmi[id],0,1,&array_fantasmi[id2],&array_fantasmi[id3]);
          if(array_fantasmi[id].flagScontro){
              array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
              break;
          }
        oldDir = array_fantasmi[id].direzione;
        oldPos[0]=array_fantasmi[id].posizione.y;
        oldPos[1]=array_fantasmi[id].posizione.x;
        pthread_mutex_lock(&mutex);/*Inizio sezione critica*/
        if (map[oldPos[0]][oldPos[1]] == '*') {
          mvaddch(oldPos[0],oldPos[1],'*');
        }
        else{
          mvaddch(oldPos[0],oldPos[1],' ');
          if(oldPos[0] == pos_pacman.y && oldPos[1] == pos_pacman.x){
             attron(COLOR_PAIR(PACMAN));
             mvaddch(oldPos[0],oldPos[1],'O');
             attroff(COLOR_PAIR(PACMAN));
          }
        }

        array_fantasmi[id].posizione.x+=1;
        attron(COLOR_PAIR(array_fantasmi[id].colore));
        mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,'M');
        attroff(COLOR_PAIR(array_fantasmi[id].colore));
        refresh();
        pthread_mutex_unlock(&mutex);
        if(array_fantasmi[id].posizione.y == pos_pacman.y && array_fantasmi[id].posizione.x == pos_pacman.x){
            colpito();
        }
        usleep(350000);
        if(controllo_deviazione(array_fantasmi[id].posizione,array_fantasmi[id].direzione)){
            if(array_fantasmi[id].flagScontro){
                array_fantasmi[id].direzione = oldDirec(array_fantasmi[id].direzione);
            }
          break;
        }
      }
      break;
    }
    if(time(NULL)-interv >= 6 && punteggio < PUNTEGGIOWIN && array_fantasmi[id].vita){
        time(&interv);  //refresh del timer
        if(puoi_andare(array_fantasmi[id].posizione.y-1,array_fantasmi[id].posizione.x) && map[array_fantasmi[id].posizione.y-1][array_fantasmi[id].posizione.x] != '*' && array_fantasmi[id].vita){
          a_bullet[id][0].y = array_fantasmi[id].posizione.y-1;
          a_bullet[id][0].x = array_fantasmi[id].posizione.x;
        }
        else{
          a_bullet[id][0].y = -1;
          a_bullet[id][0].x = -1;
        }
        if(puoi_andare(array_fantasmi[id].posizione.y+1,array_fantasmi[id].posizione.x) && map[array_fantasmi[id].posizione.y+1][array_fantasmi[id].posizione.x] != '*' && array_fantasmi[id].vita){
          a_bullet[id][1].y = array_fantasmi[id].posizione.y+1;
          a_bullet[id][1].x = array_fantasmi[id].posizione.x;
        }
        else{
          a_bullet[id][1].y = -1;
          a_bullet[id][1].x = -1;
        }
        if(puoi_andare(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x-1) && map[array_fantasmi[id].posizione.y][array_fantasmi[id].posizione.x-1] != '*' && array_fantasmi[id].vita){
          a_bullet[id][2].y = array_fantasmi[id].posizione.y;
          a_bullet[id][2].x = array_fantasmi[id].posizione.x-1;
        }
        else{
          a_bullet[id][2].y = -1;
          a_bullet[id][2].x = -1;
        }
        if(puoi_andare(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x+1) && map[array_fantasmi[id].posizione.y][array_fantasmi[id].posizione.x+1] != '*' && array_fantasmi[id].vita){
          a_bullet[id][3].y = array_fantasmi[id].posizione.y;
          a_bullet[id][3].x = array_fantasmi[id].posizione.x+1;
        }
        else{
          a_bullet[id][3].y = -1;
          a_bullet[id][3].x = -1;
        }
        if(array_fantasmi[id].vita)
          pthread_create(&sparoID,NULL,&sparoGhost,&id);
    }
  }
  pthread_mutex_lock(&mutex);
  mvprintw((MAXY+array_fantasmi[id].id),MAXX+2,"Vita Fantasma id%d: %d",array_fantasmi[id].id,array_fantasmi[id].vita);   //debug purpose
  if(map[array_fantasmi[id].posizione.y][array_fantasmi[id].posizione.x] == ' ')
    mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,' ');
  else if(map[array_fantasmi[id].posizione.y][array_fantasmi[id].posizione.x] == '*')
    mvaddch(array_fantasmi[id].posizione.y,array_fantasmi[id].posizione.x,'*');
  refresh();
  pthread_mutex_unlock(&mutex);

  if (num_vite < 1)
    pthread_join(sparoID,NULL);
  //sperimentale
}

/**
 * Procedura che verifica se pacman ha mangiato tutti i pallini, stampa a video "HAI VINTO" e cancella la finestra
*/
void controllo_vittoria(){
  attron(A_BLINK);
  mvprintw(MAXY/2,(MAXX/2)-1,"HAI VINTO");
  mvprintw((MAXY/2)+3,(MAXX/2)-2,"Punteggio: %d",punteggio);
  attroff(A_BLINK);
  sleep(4);
  erase();
  endwin();
  curs_set(1);
}


/**
 * Procedura crea i thread degli spari che non superano i confini della mappa e vengono generati in 4 direzioni
*/
void * sparo(void * temp){
  int i,j,cont=0;

  while(cont<4 && (punteggio<PUNTEGGIOWIN)){
    cont = 0;

    if(a_bullet[3][0].y != -1){
      if(puoi_andare(a_bullet[3][0].y-1,a_bullet[3][0].x) && map[a_bullet[3][0].y-1][a_bullet[3][0].x] != '*'){
          oldBull[3][0].y=a_bullet[3][0].y;
          oldBull[3][0].x=a_bullet[3][0].x;
          a_bullet[3][0].y-=1;
          pthread_mutex_lock(&mutex);
          attron(A_BOLD);
          mvaddch(a_bullet[3][0].y,a_bullet[3][0].x,'.');
          attroff(A_BOLD);
          mvaddch(oldBull[3][0].y,oldBull[3][0].x,' ');
          refresh();
          pthread_mutex_unlock(&mutex);
          usleep(50000);
      }
      else{
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][0].y,a_bullet[3][0].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        a_bullet[3][0].y = -1;
        a_bullet[3][0].x = -1;
      }
      if(map[a_bullet[3][0].y-1][a_bullet[3][0].x] == '*'){
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][0].y,a_bullet[3][0].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        cont++;
        a_bullet[3][0].y = -1;
        a_bullet[3][0].x = -1;
      }
    }
    else
    cont++;

    if(a_bullet[3][1].y != -1){
      if(puoi_andare(a_bullet[3][1].y+1,a_bullet[3][1].x) && map[a_bullet[3][1].y+1][a_bullet[3][1].x] != '*'){
          oldBull[3][1].y=a_bullet[3][1].y;
          oldBull[3][1].x=a_bullet[3][1].x;
          a_bullet[3][1].y+=1;
          pthread_mutex_lock(&mutex);
          attron(A_BOLD);
          mvaddch(a_bullet[3][1].y,a_bullet[3][1].x,'.');
          attroff(A_BOLD);
          mvaddch(oldBull[3][1].y,oldBull[3][1].x,' ');
          refresh();
          pthread_mutex_unlock(&mutex);
          usleep(50000);
      }
      else{
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][1].y,a_bullet[3][1].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        a_bullet[3][1].y = -1;
        a_bullet[3][1].x = -1;
      }
      if(map[a_bullet[3][1].y+1][a_bullet[3][1].x] == '*'){
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][1].y,a_bullet[3][1].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        cont++;
        a_bullet[3][1].y = -1;
        a_bullet[3][1].x = -1;
      }
    }
    else
    cont++;

    if(a_bullet[3][2].y != -1){
      if(puoi_andare(a_bullet[3][2].y,a_bullet[3][2].x-1) && map[a_bullet[3][2].y][a_bullet[3][2].x-1] != '*'){
          oldBull[3][2].y=a_bullet[3][2].y;
          oldBull[3][2].x=a_bullet[3][2].x;
          a_bullet[3][2].x-=1;
          pthread_mutex_lock(&mutex);
          attron(A_BOLD);
          mvaddch(a_bullet[3][2].y,a_bullet[3][2].x,'.');
          attroff(A_BOLD);
          mvaddch(oldBull[3][2].y,oldBull[3][2].x,' ');
          refresh();
          pthread_mutex_unlock(&mutex);
          usleep(50000);
      }
      else{
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][2].y,a_bullet[3][2].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        a_bullet[3][2].y = -1;
        a_bullet[3][2].x = -1;
      }
      if(map[a_bullet[3][2].y][a_bullet[3][2].x-1] == '*'){
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][2].y,a_bullet[3][2].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        cont++;
        a_bullet[3][2].y = -1;
        a_bullet[3][2].x = -1;
      }
    }
    else
    cont++;

    if(a_bullet[3][3].y != -1){
      if(puoi_andare(a_bullet[3][3].y,a_bullet[3][3].x+1) && map[a_bullet[3][3].y][a_bullet[3][3].x+1] != '*'){
          oldBull[3][3].y=a_bullet[3][3].y;
          oldBull[3][3].x=a_bullet[3][3].x;
          a_bullet[3][3].x+=1;
          pthread_mutex_lock(&mutex);
          attron(A_BOLD);
          mvaddch(a_bullet[3][3].y,a_bullet[3][3].x,'.');
          attroff(A_BOLD);
          mvaddch(oldBull[3][3].y,oldBull[3][3].x,' ');
          refresh();
          pthread_mutex_unlock(&mutex);
          usleep(50000);
      }
      else{
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][3].y,a_bullet[3][3].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        a_bullet[3][3].y = -1;
        a_bullet[3][3].x = -1;
      }
      if(map[a_bullet[3][3].y][a_bullet[3][3].x+1] == '*'){
        pthread_mutex_lock(&mutex);
        mvaddch(a_bullet[3][3].y,a_bullet[3][3].x,' ');
        refresh();
        pthread_mutex_unlock(&mutex);
        cont++;
        a_bullet[3][3].y = -1;
        a_bullet[3][3].x = -1;
      }
    }
    else
    cont++;

    for(i=0;i<4;i++){
      for(j=0;j<3;j++){
        if(a_bullet[3][i].y != -1 && a_bullet[3][i].x != -1){
          if((a_bullet[3][i].x == array_fantasmi[j].posizione.x) && a_bullet[3][i].y == array_fantasmi[j].posizione.y && array_fantasmi[j].vita){
            pthread_mutex_lock(&mutex);
            array_fantasmi[j].vita = false;
            mvprintw((MAXY+array_fantasmi[j].id),MAXX+2,"Vita Fantasma id%d: %d",array_fantasmi[j].id,(int)array_fantasmi[j].vita);   //debug purpose
            mvaddch(a_bullet[3][i].y,a_bullet[3][i].x,' ');
            refresh();
            pthread_mutex_unlock(&mutex);
          }
        }
      }
    }
  }
  for(i=0;i<4;i++){
    if(a_bullet[3][i].y != -1 && a_bullet[3][i].x != -1){
      pthread_mutex_lock(&mutex);
      mvaddch(a_bullet[3][i].y,a_bullet[3][i].x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
    }
  }
  pthread_exit(NULL);
}


/**
 * Procedura che crea i thread degli spari dei fantasmi e toglie le vite a pacman
*/
void * sparoGhost(void*temp){
int cont=0;
int * id = (int*)temp;
_Bool flag_hit = true;

while(cont<4 && (punteggio<PUNTEGGIOWIN) && array_fantasmi[*id].vita){
cont = 0;

if(a_bullet[*id][0].y != -1){
  oldBull[*id][0].y=a_bullet[*id][0].y;
  oldBull[*id][0].x=a_bullet[*id][0].x;
  if(oldBull[*id][0].y == pos_pacman.y && oldBull[*id][0].x == pos_pacman.x && flag_hit){
        flag_hit=false;
        a_bullet[*id][0].y = -1;
        a_bullet[*id][0].x = -1;
        colpito();
  }
  if(puoi_andare(a_bullet[*id][0].y-1,a_bullet[*id][0].x) && map[a_bullet[*id][0].y-1][a_bullet[*id][0].x] != '*' && punteggio < PUNTEGGIOWIN){
      a_bullet[*id][0].y-=1;
      pthread_mutex_lock(&mutex);
      attron(A_BOLD);
      mvaddch(a_bullet[*id][0].y,a_bullet[*id][0].x,'.');
      attroff(A_BOLD);
      mvaddch(oldBull[*id][0].y,oldBull[*id][0].x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
      usleep(50000);
  }
  else{
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][0].y,a_bullet[*id][0].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    a_bullet[*id][0].y = -1;
    a_bullet[*id][0].x = -1;
  }
  if(map[a_bullet[*id][0].y-1][a_bullet[*id][0].x] == '*'){
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][0].y,a_bullet[*id][0].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    cont++;
    a_bullet[*id][0].y = -1;
    a_bullet[*id][0].x = -1;
  }
}
  else
  cont++;
if(a_bullet[*id][1].y != -1){
  oldBull[*id][1].y=a_bullet[*id][1].y;
  oldBull[*id][1].x=a_bullet[*id][1].x;
  if(oldBull[*id][1].y == pos_pacman.y && oldBull[*id][1].x == pos_pacman.x && flag_hit){
    flag_hit=false;
    a_bullet[*id][1].y = -1;
    a_bullet[*id][1].x = -1;
    colpito();

  }
  if(puoi_andare(a_bullet[*id][1].y+1,a_bullet[*id][1].x) && map[a_bullet[*id][1].y+1][a_bullet[*id][1].x] != '*' && punteggio < PUNTEGGIOWIN){
      a_bullet[*id][1].y+=1;
      pthread_mutex_lock(&mutex);
      attron(A_BOLD);
      mvaddch(a_bullet[*id][1].y,a_bullet[*id][1].x,'.');
      attroff(A_BOLD);
      mvaddch(oldBull[*id][1].y,oldBull[*id][1].x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
      usleep(50000);
  }
  else{
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][1].y,a_bullet[*id][1].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    a_bullet[*id][1].y = -1;
    a_bullet[*id][1].x = -1;
  }
  if(map[a_bullet[*id][1].y+1][a_bullet[*id][1].x] == '*'){
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][1].y,a_bullet[*id][1].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    cont++;
    a_bullet[*id][1].y = -1;
    a_bullet[*id][1].x = -1;
  }
}
else
cont++;

if(a_bullet[*id][2].y != -1){
  oldBull[*id][2].y=a_bullet[*id][2].y;
  oldBull[*id][2].x=a_bullet[*id][2].x;
  if(oldBull[*id][2].y == pos_pacman.y && oldBull[*id][2].x == pos_pacman.x && flag_hit){
    flag_hit=false;
    a_bullet[*id][2].y = -1;
    a_bullet[*id][2].x = -1;
    colpito();
  }
  if(puoi_andare(a_bullet[*id][2].y,a_bullet[*id][2].x-1) && map[a_bullet[*id][2].y][a_bullet[*id][2].x-1] != '*' && punteggio < PUNTEGGIOWIN){
      a_bullet[*id][2].x-=1;
      pthread_mutex_lock(&mutex);
      attron(A_BOLD);
      mvaddch(a_bullet[*id][2].y,a_bullet[*id][2].x,'.');
      attroff(A_BOLD);
      mvaddch(oldBull[*id][2].y,oldBull[*id][2].x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
      usleep(50000);
  }
  else{
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][2].y,a_bullet[*id][2].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    a_bullet[*id][2].y = -1;
    a_bullet[*id][2].x = -1;
  }
  if(map[a_bullet[*id][2].y][a_bullet[*id][2].x-1] == '*'){
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][2].y,a_bullet[*id][2].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    cont++;
    a_bullet[*id][2].y = -1;
    a_bullet[*id][2].x = -1;
  }
}
else
cont++;

if(a_bullet[*id][3].y != -1){
  oldBull[*id][3].y=a_bullet[*id][3].y;
  oldBull[*id][3].x=a_bullet[*id][3].x;
  if(oldBull[*id][3].y == pos_pacman.y && oldBull[*id][3].x == pos_pacman.x && flag_hit){
    flag_hit=false;
    a_bullet[*id][3].y = -1;
    a_bullet[*id][3].x = -1;
    colpito();
  }
  if(puoi_andare(a_bullet[*id][3].y,a_bullet[*id][3].x+1) && map[a_bullet[*id][3].y][a_bullet[*id][3].x+1] != '*' && punteggio < PUNTEGGIOWIN){
      a_bullet[*id][3].x+=1;
      pthread_mutex_lock(&mutex);
      attron(A_BOLD);
      mvaddch(a_bullet[*id][3].y,a_bullet[*id][3].x,'.');
      attroff(A_BOLD);
      mvaddch(oldBull[*id][3].y,oldBull[*id][3].x,' ');
      refresh();
      pthread_mutex_unlock(&mutex);
      usleep(50000);
  }
  else{
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][3].y,a_bullet[*id][3].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    a_bullet[*id][3].y = -1;
    a_bullet[*id][3].x = -1;
  }
  if(map[a_bullet[*id][3].y][a_bullet[*id][3].x+1] == '*'){
    pthread_mutex_lock(&mutex);
    mvaddch(a_bullet[*id][3].y,a_bullet[*id][3].x,' ');
    refresh();
    pthread_mutex_unlock(&mutex);
    cont++;
    a_bullet[*id][3].y = -1;
    a_bullet[*id][3].x = -1;
  }
}
  else
  cont++;
  }
  pthread_exit(NULL);
}

/**
 * Procedura che crea i thread per far respawnare i fantasmi
*/
void * controllo_vita(void * valore){
  int *id;
  id = (int*) valore;
  log_space = 6;

  while(num_vite > 0 && punteggio < PUNTEGGIOWIN){
    pthread_join(array_fantasmi[*id].threadid,NULL);
    if(num_vite > 0){
      pthread_create(&(array_fantasmi[*id].threadid),NULL,&fantasma_aresti,&(array_fantasmi[*id].id));
      pthread_mutex_lock(&mutex);
      mvprintw(log_space,(MAXX+27),"Fantasma %d re-spawnato!",array_fantasmi[*id].id);
      refresh();
      pthread_mutex_unlock(&mutex);
    }
 }
  pthread_exit(NULL);
}
/*controllo_vita aspetta la terminazione di uno dei fantasmi e lo riavvia dandoli la posizione al centro dello schermo*/

/**
 * Procedura che fa "rimbalzare" i fantasmi quando si incontrano
 * @param id id del fantasma da far "rimbalzare"
*/
void scontro(fantasmi * id,int posY,int posX, fantasmi * id2, fantasmi * id3){
        if(id->posizione.y+(posY) == id2->posizione.y && id->posizione.x+(posX) == id2->posizione.x){
            id->flagScontro = true;
            id2->flagScontro = true;
        }
        else if (id->posizione.y+(posY) == id3->posizione.y && id->posizione.x+(posX) == id3->posizione.x){
            id->flagScontro = true;
            id3->flagScontro = true;
        }
}

/**
 * Procedura che fa finire il gioco chiudendo i thread e stampa a video "you died"
*/
void hai_perso(){
    sleep(1);
    erase();
    attron(A_BLINK);
    mvprintw(10,5,"__  ______  __  __   ____  ______________ ");
    mvprintw(11,5,"\\ \\/ / __ \\/ / / /  / __ \\/  _/ ____/ __ \\");
    mvprintw(12,5,"\\  / / / / / / /  / / / // // __/ / / / /");
    mvprintw(13,5," / / /_/ / /_/ /  / /_/ _/ // /___/ /_/ / ");
    mvprintw(14,5,"/_/\\____/\\____/  /_____/___/_____/_____/  ");
    attroff(A_BLINK);
    mvprintw(26,16,"Made by Susha & Pinto");
    refresh();
    sleep(10);
    erase();
    endwin();
    curs_set(1);

}

/**
 * Procedura che toglie delle vite se pacman viene colpito
 */
void colpito(){
    init_pair(PACMAN,COLOR_YELLOW,COLOR_BLACK);

    if(num_vite > 0){
      life--;
    }
    if(life == 0){
        num_vite--;
        if(num_vite > 0){
            life = 10;
        }
    }
    pthread_mutex_lock(&mutex);
    attron(COLOR_PAIR(PACMAN));
    mvaddch(pos_pacman.y,pos_pacman.x,'O');
    attroff(COLOR_PAIR(PACMAN));
    mvprintw(2,(MAXX+3),"Vite: %d",num_vite);
    mvprintw(2,(MAXX+13),"Life: %d ",life);
    refresh();
    pthread_mutex_unlock(&mutex);
}

//if movimento array_fantasmi = pos_pacman
//fantasma non si sposta o leva una vita a pacman
