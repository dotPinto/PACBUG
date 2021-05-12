#ifndef PAC_H
#define PAC_H
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <termios.h>
#include <sys/time.h>
#include <string.h>


#define PASSO 1 		// Entita spostamento
#define SU 65 			// Freccia su
#define GIU 66 			// Freccia giu
#define SINISTRA 68		// Freccia sinsitra
#define DESTRA 67		// Freccia destra
#define SPAZIO 32		// Barra Spaziatrice
#define MAXX 28 		// Numero di colonne dello schermo
#define MAXY 31			// Numero di righe dello schermo
#define PUNTEGGIOWIN 5000
//#define PUNTEGGIOWIN 28700	// Punteggio per aver mangiato tutti i pallini
#define MAXCARATTERI 24     // Numero massimo di caratteri del nome

/**
 * Struttura per individuare una posizione della mappa
*/
typedef struct{
  int y;	// Ascissa
  int x;	// Ordinata
}pos;

/**
 * Enumerazione per riconoscere la direzione dei fantasmi e pacman
*/
typedef enum{SOPRA,SOTTO,A_DESTRA,A_SINISTRA}direzione;

/**
 * Struttura per creare un fantasma
*/
typedef struct{
  pos posizione;
  _Bool vita;
  int id;
  pthread_t threadid;
  direzione direzione;
  int colore;
  _Bool flagScontro;
}fantasmi;

typedef struct{
    char nickname[MAXCARATTERI];
    int punti;
}Salvataggio;

/**
 * Enumerazione per colorare gli oggetti
*/
typedef enum{BACK,MURI,PACMAN,WHITE,FANTASMA}color_pairs;

/**
 * Variabili globali
*/
int punteggio;
int num_fantasma;
int log_space;
pos pos_pacman;
int num_vite,life;
int intPac,intFan;
pthread_t pacmanID;
fantasmi array_fantasmi[3];
pos a_bullet[4][4];
pos oldBull[4][4];
char nickname[MAXCARATTERI];

/**
 * Mappa di gioco
*/
static char map[MAXY][MAXX] = {
  {'l', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'I', 'l', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'I'},
  {'Y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'Y'},
  {'Y', '*', 'l', 'x', 'x', 'I', '*', 'l', 'x', 'x', 'x', 'I', '*', 'y', 'y', '*', 'l', 'x', 'x', 'x', 'I', '*', 'l', 'x', 'x', 'I', '*', 'Y'},
  {'Y', '*', 'y', 'v', 'v', 'y', '*', 'y', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'y', '*', 'y', 'v', 'v', 'y', '*', 'Y'},
  {'Y', '*', 'L', 'x', 'x', 'i', '*', 'L', 'x', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'x', 'i', '*', 'L', 'x', 'x', 'i', '*', 'Y'},
  {'Y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'Y'},
  {'Y', '*', 'l', 'x', 'x', 'I', '*', 'l', 'I', '*', 'l', 'x', 'x', 'x', 'x', 'x', 'x', 'I', '*', 'l', 'I', '*', 'l', 'x', 'x', 'I', '*', 'Y'},
  {'Y', '*', 'L', 'x', 'x', 'i', '*', 'y', 'y', '*', 'L', 'x', 'x', 'I', 'l', 'x', 'x', 'i', '*', 'y', 'y', '*', 'L', 'x', 'x', 'i', '*', 'Y'},
  {'Y', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', 'Y'},
  {'L', 'x', 'x', 'x', 'x', 'I', '*', 'y', 'L', 'x', 'x', 'I', '*', 'y', 'y', '*', 'l', 'x', 'x', 'i', 'y', '*', 'l', 'x', 'x', 'x', 'x', 'i'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'l', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'I', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', 'l', 'x', 'x', 'x', 'x', 'x', 'x', 'I', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'x', 'x', 'x', 'x', 'x', 'i', '*', 'L', 'i', '*', 'y', 'v', 'v', 'v', 'v', 'v', 'v', 'y', '*', 'L', 'i', '*', 'L', 'x', 'x', 'x', 'x', 'x'},
  {'v', 'v', 'v', 'v', 'v', 'v', '*', '*', '*', '*', 'y', 'v', 'v', 'v', 'v', 'v', 'v', 'y', '*', '*', '*', '*', 'v', 'v', 'v', 'v', 'v', 'v'},
  {'x', 'x', 'x', 'x', 'x', 'I', '*', 'l', 'I', '*', 'y', 'v', 'v', 'v', 'v', 'v', 'v', 'y', '*', 'l', 'I', '*', 'l', 'x', 'x', 'x', 'x', 'x'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', 'L', 'x', 'x', 'x', 'x', 'x', 'x', 'i', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'v', 'v', 'v', 'v', 'v', 'y', '*', 'y', 'y', '*', 'l', 'x', 'x', 'x', 'x', 'x', 'x', 'I', '*', 'y', 'y', '*', 'y', 'v', 'v', 'v', 'v', 'v'},
  {'l', 'x', 'x', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'I', 'l', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'x', 'x', 'I'},
  {'Y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'Y'},
  {'Y', '*', 'l', 'x', 'x', 'I', '*', 'l', 'x', 'x', 'x', 'I', '*', 'y', 'y', '*', 'l', 'x', 'x', 'x', 'I', '*', 'l', 'x', 'x', 'I', '*', 'Y'},
  {'Y', '*', 'L', 'x', 'I', 'y', '*', 'L', 'x', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'x', 'i', '*', 'y', 'l', 'x', 'i', '*', 'Y'},
  {'Y', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', 'Y'},
  {'L', 'x', 'I', '*', 'y', 'y', '*', 'l', 'I', '*', 'l', 'x', 'x', 'x', 'x', 'x', 'x', 'I', '*', 'l', 'I', '*', 'y', 'y', '*', 'l', 'x', 'i'},
  {'l', 'x', 'i', '*', 'L', 'i', '*', 'y', 'y', '*', 'L', 'x', 'x', 'I', 'l', 'x', 'x', 'i', '*', 'y', 'y', '*', 'L', 'i', '*', 'L', 'x', 'I'},
  {'Y', '*', '*', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', 'y', 'y', '*', '*', '*', '*', '*', '*', 'Y'},
  {'Y', '*', 'l', 'x', 'x', 'x', 'x', 'i', 'L', 'x', 'x', 'I', '*', 'y', 'y', '*', 'l', 'x', 'x', 'i', 'L', 'x', 'x', 'x', 'x', 'I', '*', 'Y'},
  {'Y', '*', 'L', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'i', '*', 'L', 'i', '*', 'L', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'i', '*', 'Y'},
  {'Y', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', 'Y'},
  {'L', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'i'},
};

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //inizializzo il mutex

//INIZIO-DICHIARAZIONI
void changemode(int dir);
int kbhit (void);
void incrementaPunteggio(int y,int x);
_Bool puoi_andare(int y, int x);
void mainmenu();
void makemap();
_Bool controllo_deviazione(pos posizione,direzione dir_corrente);
void * pacman(void * parametri);
pos genera_fantasma(pos posizione);
void movimento_fantasma(int id);
void * fantasma_aresti(void * parametri);
void controllo_vittoria();
void * sparo(void * temp);
void * sparoGhost(void*temp);
void * controllo_vita(void * valore);
void hai_perso();
void scontro(fantasmi * id,int posY,int posX, fantasmi * id2, fantasmi * id3);
direzione oldDirec(direzione dir_corr);
void colpito();

//FINE-DICHIARAZIONI
#endif
