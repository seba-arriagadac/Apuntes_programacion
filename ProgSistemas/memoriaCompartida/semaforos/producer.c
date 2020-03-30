/**
 * Simple program demonstrating shared memory in POSIX systems.
 * This is the producer process that writes to the shared memory region.
 *
 * @author Agustín J. González 
 * One line at the time is read from stdin by producer and written in the shared memory.
 * Then the consumer reads each line and displays it.
 * This a variant of a very similar program by Silberschatz, Galvin, and Gagne
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 *
 * modifications by dheller@cse.psu.edu, 31 Jan. 2014
 * modifications by Agustín J. González, 14 Oct. 2019
 *    gcc producer.c -lpthread -lrt
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include <semaphore.h> 

void display(char *prog, char *bytes, int n);

int main(void)  {
  const char *name = "/shm-example";	// file name
  const int SIZE = 4096;		// file size
  const char *nameEmpty = "/EMPTY";
  const char *nameFull = "/FULL";

  int shm_fd;		// file descriptor, from shm_open()
  int nc;
  char *shm_base;	// base address, from mmap()

  sem_t * empty_sem, *full_sem;

  // Se crea la zona de memoria
  shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    printf("prod: Shared memory failed: %s\n", strerror(errno));
    exit(1);
  }

  // Se le asigna un tamaño a la memoria compartida
  ftruncate(shm_fd, SIZE);

  // Se mapea la memoria compartida
  shm_base = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_base == MAP_FAILED) {
    printf("prod: Map failed: %s\n", strerror(errno));
    close(shm_fd);
    shm_unlink(name);   /* close and shm_unlink?*/
    exit(1);
  }
    //--------------------------------------
    //-aqui se crean los semaforos
    //---------------------------------------

    // Se remueven los semaforos por si ya existen
    if (sem_unlink(nameEmpty) == -1)
        printf("Error removing %s: %s\n", nameEmpty, strerror(errno));
    if (sem_unlink(nameFull) == -1)
        printf("Error removing %s: %s\n", nameEmpty, strerror(errno));

    // Se crealos semaforos con 0_CREATE
    if ( (empty_sem = sem_open(nameEmpty, O_CREAT, 0666, 1)) == SEM_FAILED)
        printf("Error creating %s: %s\n", nameEmpty, strerror(errno));
    if ( (full_sem = sem_open(nameFull, O_CREAT, 0666, 0)) == SEM_FAILED)
        printf("Error creating %s: %s\n", nameFull, strerror(errno));

    do {
      // Se decrementa semaforo EspacioParaEscribir (SemEmpty)
      if (sem_wait(empty_sem)!=0) 
          printf("Error waiting %s\n",strerror(errno));
      else {
         /**
          * Write to the mapped shared memory region.
          */
           display("prod_before", shm_base, 64);
           nc = read(STDIN_FILENO, shm_base, SIZE-1);
           shm_base[nc] ='\0';  /* to make it NULL termited string */
           display("prod_after", shm_base, 64);
           //Se incrementa semaforo elementosParaLeer (SemFull)
           if (sem_post(full_sem)!=0)
             printf("Error posting %s\n",strerror(errno));
       }
   } while (strlen(shm_base) > 0);
   sem_close(empty_sem);
   sem_close(full_sem);
  //Se saca el mapeo
  if (munmap(shm_base, SIZE) == -1) {
    printf("prod: Unmap failed: %s\n", strerror(errno));
    exit(1);
  }

  // Se cierra el semento de memoria pero no se elimina
  if (close(shm_fd) == -1) {
    printf("prod: Close failed: %s\n", strerror(errno));
    exit(1);
  }

  return 0;
}

void display(char *prog, char *bytes, int n)
{
  printf("display: %s\n", prog);
  for (int i = 0; i < n; i++) 
    { printf("%02x%c", bytes[i], ((i+1)%16) ? ' ' : '\n'); }
  printf("\n");
}

