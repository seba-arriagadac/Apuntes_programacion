# Semaforos y memoria compartida

## Analogía
### Memoria compartida
La memoria compartida es como un archivo donde se gurda unformacion, el tema es que hay q decirle cuanta informacion voy a gusrdar ahi, hay q darle un nombre para poder acceder a ella de distintos programas.
Se ba a recorrer con un puntero pero depende de lo q vaya a guardar ahí es el tipo de puntero q voy a usar.
### Semaforos
el espacio de mamoria compartida funciona como un balde con agua donde hay un llenador (productor) y un bebedor (consumidor) ambos tienen acceso al balde de agua (datos de un cierto tipo de dato) pero solo a través de un vaso (puntero al tipo de dato) entonces, básicamente pasa lo siguiente:
productor: sem_wait (espacio_para_almacenar_agua)
            (avisa que hay un espacio menos para hechar agua)
            hecha un vaso de agua
            sem_post(vasos_de_agua_disponible)
            (avisa q hay 1 vaso de agua más para sacar)

Consumidor: sem_wait(vasos_de_agua_disponible)
            (como lo va a sacer avisa q hay uno menos)
            saca el vaso de agua
            sem_post (espacio_para_almacenar_agua)
            (como sacó agua avisa q ahora hay 1 espacio más para sacar)

## el código para usar la memoria compartida con los semaforos
Para crear semaforos y memoria compatida se necesitan los siguientes include:
```c
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
```
Luego, se necesitan tener las siguiente variables a mano:
Para la memoria compartida

```c
const char *name = "/shm-example";	// file name
const int SIZE = 4096;		// file size
int shm_fd;		// file descriptor, from shm_open()
char *shm_base;	// base address, from mmap()
```
*Todos los procesos que accedan a la memoria compartida deben conocer las 2 primeras variables.
Para los semaforos
```c
const char *nameEmpty = "/EMPTY";
const char *nameFull = "/FULL";
sem_t * empty_sem, *full_sem;
```
*Todos los procesos que accedan a la memoria compartida deben conocer las 2 primeras variables.
### Para crear la zona de memoria compartida

Se crea la zona de memoria compartida:
```c

```
Se le asigna el tamaño
```c

```

### Para crear los semaforos
Se remueven los semaforos por si existen (son los mismos si estan asociados al mismo nombre)
```c
// Se remueven los semaforos por si ya existen
if (sem_unlink(nameEmpty) == -1)
    printf("Error removing %s: %s\n", nameEmpty, strerror(errno));
if (sem_unlink(nameFull) == -1)
    printf("Error removing %s: %s\n", nameEmpty, strerror(errno));
```
Se crean los semaforos
```c
// Se crealos semaforos con 0_CREATE
    if ( (empty_sem = sem_open(nameEmpty, O_CREAT, 0666, 1)) == SEM_FAILED)
        printf("Error creating %s: %s\n", nameEmpty, strerror(errno));
    if ( (full_sem = sem_open(nameFull, O_CREAT, 0666, 0)) == SEM_FAILED)
        printf("Error creating %s: %s\n", nameFull, strerror(errno));
```
### Para acceder a la memoria commpartida
```c

```
### Para utilizar los semaforos
```c
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
```

### Para cerrar el acceso a la memoria compartida
Primero se saca el mapeo de la mamoria
```c
//Se saca el mapeo
if (munmap(shm_base, SIZE) == -1) {
    printf("prod: Unmap failed: %s\n", strerror(errno));
    exit(1);
}
```
Y se cierra el acceso
```c
// Se cierra el semento de memoria pero no se elimina
  if (close(shm_fd) == -1) {
    printf("prod: Close failed: %s\n", strerror(errno));
    exit(1);
  }
```
### Para cerrar el acceso al (a los) semaforos
```c
   sem_close(empty_sem);
   sem_close(full_sem);
```
### Para eliminar la memoria compartida
```c

```
### Para eliminar los recursos de los semaforos
```c

```