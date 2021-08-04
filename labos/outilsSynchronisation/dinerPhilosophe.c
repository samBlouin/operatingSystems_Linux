#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NOMBRE_DE_PHILOSOPHE 5
#define NOMBRE_DE_REPAS 2

// Tableau qui contient le nombre de repas mangés par chaque philosophe.
int nombre_repas[NOMBRE_DE_PHILOSOPHE];

// Ajoutez la structure nécessaire pour gérer la concurrence.
pthread_mutex_t mutex_fouchette[NOMBRE_DE_PHILOSOPHE];

// Fonction pour faire penser le philosophe pendant un temps aléatoire (entre 1 et 5s)
void pense(int id_philosophe){
    printf("Le philosophe %d pense.\n", id_philosophe);
    int sleep_time = (rand() % 5) + 1;
    sleep(sleep_time);
}

// Fonction pour faire manger le philosophe pendant un temps aléatoire (entre 1 et 5s)
void mange(int id_philosophe){
    printf("Le philosophe %d mange.\n", id_philosophe);
    nombre_repas[id_philosophe]++;
    int sleep_time = (rand() % 5) + 1;
    sleep(sleep_time);
}

void prendre_couverts(int id_philosophe){
    if(id_philosophe < (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE){
        pthread_mutex_lock(&mutex_fouchette[id_philosophe]); //fourchette gauche
        pthread_mutex_lock(&mutex_fouchette[ (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE ]); //fourchette droite
    }else{
        pthread_mutex_lock(&mutex_fouchette[ (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE ]); //fourchette droite
        pthread_mutex_lock(&mutex_fouchette[id_philosophe]); //fourchette gauche
    }
}
/*
Remarque : il y a 2 points clés pour éviter l'interblocage :
  1. on prend les fourchettes selon l'ordre de l'indice le plus petit à l'indice le plus grand
  2. on libere les fourchettes de facon symétrique à laquelle elles ont été prises
*/
void poser_couverts(int id_philosophe){
    if(id_philosophe < (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE){
        pthread_mutex_unlock(&mutex_fouchette[ (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE ]); //fourchette droite
        pthread_mutex_unlock(&mutex_fouchette[id_philosophe]); //fourchette gauche
    }else{
        pthread_mutex_unlock(&mutex_fouchette[id_philosophe]); //fourchette gauche
        pthread_mutex_unlock(&mutex_fouchette[ (id_philosophe+1) % NOMBRE_DE_PHILOSOPHE ]); //fourchette droite
    }
}

void *philosophe(void *arg){
    int id_philosophe = *(int *)arg;
    while(nombre_repas[id_philosophe] < NOMBRE_DE_REPAS){
        pense(id_philosophe);
        prendre_couverts(id_philosophe);
        mange(id_philosophe);
        poser_couverts(id_philosophe);
    }
    printf("Le philosophe %d sort de table.\n", id_philosophe);
}

int main(int argc, char *argv[]){
    int i;
    // Initialisation des mutex (1 par couvert)
    for (i=0; i < NOMBRE_DE_PHILOSOPHE; i++){
        pthread_mutex_init(&mutex_fouchette[i], NULL);
    }

    pthread_t phi_thread[NOMBRE_DE_PHILOSOPHE];
    int tab[NOMBRE_DE_PHILOSOPHE];

    // Création de l'ensemble des threads (1 thread par philosophe), qui exécuteront la fonction philosophe
    for (i=0; i < NOMBRE_DE_PHILOSOPHE; i++){
        tab[i]=i;
        pthread_create( phi_thread + i, NULL , philosophe, &tab[i]) ;
    }

    // Attendre que l'ensemble des threads soit terminé.
    for (i=0; i < NOMBRE_DE_PHILOSOPHE; i++){
        pthread_join( phi_thread[i], NULL ) ;
    }

    // Destruction des mutex
    for (i=0; i < NOMBRE_DE_PHILOSOPHE; i++){
        pthread_mutex_destroy(&mutex_fouchette[i]);
    }

    return 0;
}