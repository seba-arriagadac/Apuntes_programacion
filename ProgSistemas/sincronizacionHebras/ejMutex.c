/* Este programa acepta como argumento 0 ó 1. Lo componen dos hilos que
acceden una misma variable entera inicialmente en cero. Una hebra incrementa ese entero 2.000.000
veces y retorna el valor máximo alcanzado por la variable. La otra decrementa 2.000.000 veces el
entero, calcula el valor mínimo alcanzado por éste, espera el término de la hebra de incremento,
muestra por pantalla el valor retornado ésta y el valor final de la variable entera.
Con argumento 1 el acceso a la variable es controlado para evitar inconsistencias.
Con argumento 0, los accesos son sin  control de exclusión.
Para compilar:
$ gcc -o p4 -pthread p4.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ITERACIONES 2000000

int contador=0;
int max;
int exclusion;

pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;

void * increment(void * arg)
{
  int i;
  max=contador;
  for (i=0; i<ITERACIONES; i++){
    if (exclusion)
      pthread_mutex_lock(&mylock);
    contador++;
    if (contador > max)
      max=contador;
    if (exclusion)
      pthread_mutex_unlock(&mylock);
  }
  return (&max);
}

int main(int argc, char * argv[])
{
  int err, i;
  pthread_t tid;
  int min=contador;
  int* pStatus;
    
  exclusion = atoi(argv[1]);
    
  err = pthread_create(&tid, NULL, increment, NULL);
  if (err != 0){
    printf("can't create thread \n");
    exit(0);
  }
  for (i = 0; i <ITERACIONES; i++){
    if (exclusion)
      pthread_mutex_lock(&mylock);
    contador--;
    if (contador < min) min = contador;
    if (exclusion)
      pthread_mutex_unlock(&mylock);
  }
  pthread_join(tid, (void **)&pStatus);
  printf("El valor mínimo de contador es %i\n", min);
  printf("El valor máximo de contador retornado es %i\n", *pStatus);
  printf("El valor máximo de contador global es %i\n", max);
  printf("El valor final de contador es %i\n", contador);
}
