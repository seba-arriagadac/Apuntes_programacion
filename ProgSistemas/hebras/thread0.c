/* Autor: Agustín J. González
Este programa es una variante a la pregunta 4 del 1er. certamen del 2sem 2009
Se compone de dos hilos que acceden una misma variable entera global inicialmente en cero. 
Una hebra incrementa ese entero 2.000.000 veces y retorna el valor máximo alcanzado por la variable. 
La otra decrementa 2.000.000 veces el mismo entero, calcula el valor mínimo alcanzado por éste, 
espera el término de la hebra de incremento, y muestra por pantalla el valor retornado por ésta y 
el valor final de la variable entera.

Para compilar:
$ gcc -o incre_decre  -pthread C1_2s09_p4.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ITERACIONES 2000000
int contador=0;
int max;  /* debe ser global para retornarla en thread */

void * increment(void * arg) {
  int i;
  max=contador;
  for (i=0; i<ITERACIONES; i++){
    contador++;
    if (max < contador)
      max=contador;
  }
  return (&max);
}

int decrement(void) {
  int i, min=contador;
  for (i=0; i<ITERACIONES; i++){
    contador--;
    if (contador < min)
      min=contador;
  }
  return (min);
}

int main(void) {
  int err, i;
  pthread_t tid;
  int min;
  int* pStatus;
    
  err = pthread_create(&tid, NULL, increment, NULL);
  if (err != 0){
    printf("can't create thread \n");
    exit(0);
  }
  min = decrement();
  pthread_join(tid, (void **)&pStatus);
  printf("El valor mínimo de contador es %i\n", min);
  printf("El valor máximo de contador retornado es %i\n", *pStatus);
  printf("El valor final de contador es %i\n", contador);
}
