# POSIX threads

**POSIX** significa Portable Operating System Interface (for uniX) y es un estándar orientado a facilitar la creación de aplicaciones confiables y portables.

Funciona como el *fork* osea se parte con un programa normal (una sola hebra/rutina/proceso) y dentro se crea la segunda hebra.
Todas las hebras comparten los datos globales.

## Funciones de hilos del **POSIX**
Todas las funciones de hilos de POSIX comienzan con pthread y se destacan:
| Función POSIX| Descripción|
| ----- | ---- |
| pthread_create | crea un hilo (análogo a fork) |
| pthread_equal | 	verifica igualdad de dos identificados de |
| pthread_self |	retorna ID de propio hilo (análogo a getpid) |
| pthread_exit |	termina el hilo sin terminar el proceso (análogo a exit)|
|pthread_join  |espera por el término de un hilo (análogo a waitpid)|
| pthread_cancel | Termina otro hilo  (análogo a abort)|
| pthread_detach |	Configura liberación de recursos cuando termina|
| pthread_kill |	envía una señal a un hilo |

Hay que tener cuidado con el acceso a los datos compartidos para evitar inconsistencias como en el caso del archivo

### Explicacion basica de los hilos
Los hilos (hebras o *threads*) son acciones que se realizan en paralelo entre si y que mejor manera de ejemplificar un accion que una funcion. Entonces para definir una hebra hay que asociarla a una funcion.

## Programacion con hebras
para incluir hebras en el codigo se debe agregar el include:
```c
#include <pthread.h>
```
Además las funciones a usar deben ser del tipo
```c
void * nombreFuncionParaHebra(void * arg) {
    //aqui hacer el procedimiento con variables globales
    //se puede hacer 
    //return (&variableEntero)
    //donde varibaleEntero es un entero definido commo global
}
```
(?) Se debería poder retornar otra cosa por ejemplo la dirección de un char (osea &c [con c un char]) pero pStatus tendría q ser u puntero a char (char *pStatus)
### Variables a usar
```c
int err;
pthread_t tid;
int* pStatus;

```

### Creación de hebra
```c
err = pthread_create(&tid, NULL, increment, NULL);
if (err != 0){
    printf("can't create thread \n");
    exit(0);
}
```
**Lo importante es que al escribir esta línea se va a ejecutar en paralelo lo que está dentro de la declaración de la hebra (la función), con el codigo que le sigue**
Para esperar por el termino de una hebra se hace
```c
pthread_join(tid, (void **)&pStatus);
```
En pStatus se lee el valor retornado de la hebra

Se puede ver un ejemplo donde se usan n cantidad de hebras (con n dado por el usuario) y entrega de parametros a la hebra en el archivo coreNum.c

## Ejecucion del código
Para compilar el codigo se debe hacer:
```c
gcc thread0.c -o thread0 -pthread
```

```c

```