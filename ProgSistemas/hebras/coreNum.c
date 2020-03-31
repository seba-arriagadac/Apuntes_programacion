/* Autor: Agustín J. González
 * Contexto: En arquitecturas de varios núcleos, varias hebras pueden ser ejecutadas en
forma   paralela.   Para   verificar   esto   en   forma   experimental, se sugiere correr una
aplicación que ejecuta una tarea paralelizable la cual se pueda descomponer en un número variable
de hebras. Luego sugiere correr esta aplicación varias veces con distintos número de hebras y en
cada caso medir su tiempo de ejecución. El análisis de estos tiempos permitiría concluir el número
de hebras paralelas que corren en una máquina.
El programa adjunto calcula la suma de los enteros del 1 al MAXNUM usando k
hebras. Cada hebra se hace cargo de una rango de números
y entregar su resultado en el parámetro retornado. La aplicación muestra por pantalla: valor de la
suma total y el número de hebras usadas. La aplicación se ejecuta usando:
$ coreNum <k>  , donde k es el número de hebras a utilizar para realizar la suma. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAXNUM 100000000LL

typedef struct  {
   long long int a, b, sum;
} PARAM;

void * partialSum(void * arg) {
  long int i;
  PARAM * task=(PARAM*)arg;
  task->sum=0;
  for (i=task->a; i<= task->b; i++)
   task->sum+=i;
  return((void*)&(task->sum));
}

int main(int argc, char * argv[]) {
  long long int err, t, k, a, workSize, sum;
  PARAM *task;
  long long int* pSum;

  k = atoi(argv[1]);  /* number of threads */
  pthread_t tid[k];

  task = (PARAM *) calloc(k, sizeof(PARAM));
  workSize = MAXNUM%k==0 ? MAXNUM/k:MAXNUM/(k-1); /* the last range could be smaller */ 
  a=0; 
  for (t=0; t<k ;t++) {
     task[t].a=a+1;
     a+=workSize;
     task[t].b= a<=MAXNUM ? a:MAXNUM;  /* to accommodate last range */
     err = pthread_create(tid+t, NULL, partialSum, task+t);
     if (err != 0)
       exit(-1);
  }
  sum=0;
  for (t=0; t<k;t++){
    pthread_join(tid[t], (void**)&pSum);
    sum+=*pSum;
  }
  printf("La suma es %lld y se usaron %lli hebras.\n", sum,k);
  long long int i = MAXNUM;
  printf("Usando la fórmula resulta: %lld \n", i*(i+1)/2);
}
