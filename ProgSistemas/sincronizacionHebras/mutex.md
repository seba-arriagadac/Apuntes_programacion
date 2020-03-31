# Sincronización de hebras
## Mutex
las funciones para el manejo de zonas de acceso exclusivo tienen el prefijo ```pthread_mutex```.

Un *mutex* es una variable especial que puede tener uno de los dos estados siguientes:
- locked (cerrado o tomado)
- unlocked (abierto o libre)

La idea de los Mutex es que sea como un semaforo y funciona como una compuerta que permite el acceso a un recurso.

| Función POSIX   | Descripción |
|----|----|
| pthread_mutex_init(...)   | 	Permite dar las condiciones iniciales a un candado mutex |
| pthread_mutex_lock(...) |	Permite solicitar acceso al mutex, el hilo se bloquea hasta su obtención |
| pthread_mutex_trylock(...) |	permite solicitar acceso al mutex,  el hilo retorna inmediatamente. El valor retornado indica si otro hilo lo tiene. |
| pthread_mutex_unlock(...) |	Permite liberar un mutex. |
| pthread_mutex_destroy(....) |	Destruye la variable (de tipo pthread_mutex_t) usada para manejo de exclusión mutua, o candado mutex |


Ejemplo: Para proteger una zona crítica, usar:
```c
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_lock(&mylock);
/* Sección crítica */
pthread_mutex_unlock(&mylock);
```
\* Si se quiere usar mutex para que 2 hebras accedan a un espacio lo ideal sería usar la línea ```c pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER; ``` afuera del *main* osea declarar mylock como variable **global**.

ver ejemplo: **ejMutex.c**

## sincronización Reader-Writer

El tema es que para leer una variable no hay mucho problema porque pueden ser varios los que leen sin causar problemas pero cuando un proceso escribe hay q tener cuidado porque mientras el proceso esta escribiendo ningun otro proceso debiera ni leer no escribir. Estos mecanismos se llaman **read-write locks** 

Para usar estos mecanismos de sincronización se usa la variable:
```c pthread_rwlock_t```.
Este tipo de candados tienen 3 estados posibles:
- Cerrado en modo lectura (Solo una hebra puede tener el candado en este modo)
- Cerrado en modo escritura (Multiples hebras pueden tener el candado en este modo)
- No cerrado

Las funciones usadas para estos candados son:

|Función POSIX |Descripción|
|----|----|
|pthread_rwlock_init (...) |permite dar las condiciones iniciales a un candado de lectura y escritura.|
|pthread_rwlock_destroy(...)|Destruye la variable (de tipo pthread_rw_lock_t) usada para manejo de exclusión mutua, o candado read-write.|
|pthread_rwlock_wrlock(...)|Permite solicitar acceso de **escritura** al candado, el hilo se bloquea hasta su obtención|
|pthread_rwlock_rdlock(...)||permite solicitar acceso de **lectura** al candado,  el hilo accede si el candado está libre o si sólo ha sido concedido para lectura.|
|pthread_rwlock_unlock(...)|Permite liberar un candado read-write.|
|othread_rwlock_tryrdlock(...) |	Permite obtener el candado (lock) si está libre y retorna cero, en otro caso retorna EBUSY|
|pthread_rwlock_trywrlock(...) |	Permite obtener el candado (lock) si está libre y retorna cero, en otro caso retorna EBUSY|

## Evitar Bloqueos Indefinidos (Deadlock)
Si varias hebras usan los mismos candados entonces q todas los accedan en el mismo orden. Sino se puede usar ```c tryXXXlock```

## Sincronización por variables de condición
Se usan cuando un hilo esera a que una variable llegue a algún valor, que supere (o baje de) un umbral o que tome deciciones dependiendo del valor de dicha varible.

La utilizacion de variables de condicion más Mutex permiten esperar por la ocurrencia de una condicón arbitraria.

**La espera por una variable de condición está libre de carreras criticas**

**La idea**:   si la condición no se cumple, en lugar de volver a consultar, quedarse bloqueado hasta que otra hebra nos informe del cambio de la variable y así volver a consultar sólo cuando tenga sentido.
Las variables de condición son del tipo (de datos) ```c pthread_cond_t```

Al igual que con los mutex se puede inicializar con ```c PTHREAD_COND_INICIALIZER```
Otra opcion es con:
Hay que recordar que hay que agregar el include
```c
#include <pthread.h>
```
Y se pueden usar las funciones:
```c
int pthread_cond_signal(pthread_cond_t *restrict cond);

int pthread_cond_broadcast(pthread_cond_t *restrict cond);
```
(Donde ambas funciones retornan 0 si esta bien)

Se usa pthread_cond_wait para esperar por un cambio que dé sentido a evaluar nuevamente la condición (tambien se puede hacer asociado a un tiempo de espera) para esto se usan las siguiente funciones:
```c
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

int pthread_cond_timewait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict timeout);
```
Y un ejemplo de esto sería
Primero, ambas hebras deben tener estas mismas variables (pueden ser globales)
```c
pthread_mutex_t    lockDeMiVariable = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t     cambioDeVariable = PTHREAD_COND_INITIALIZER;
int variable = 0;
```
Luego, en la hebra que se va a modiicar la variable se debe contener el código
```c
//...
pthread_ mutex_lock(&lockDeMiVariable);
variable++;
pthread_mutex_unlock(&lockDeMiVariable);
pthread_cond_signal(&cambioDeVariable);//aqui avisa que la variable se modificó
   //...
```
Y, en la hebra que espera por el valor de ```c variable``` se puede tener este código
```c
/* Espera por un valor superior a LIMITE en variable para poder proseguir */
    int espere = 1;
    while (espere) {  /*Espera por condición */
        pthread_ mutex_lock(&lock_de_mi_variable);
        if ( variable > LIMITE)  /* Alcanzó valor límite?*/
             espere=0;
        else
             pthread_cond_wait(&cambioDeVariable,  &lockDeMiVariable); /*espera bloqueada, pero el mutex es liberado y la otra hebra puede modificar "variable"*/
        pthread_mutex_unlock(&lock_de_mi_variable);
    }
/* hago lo que esperaba cuando variable > LIMITE */
```
o esta opcion más simple
```c
pthread_ mutex_lock(&lockDeMiVariable);
while ( variable <= LIMITE )  /* Debo seguir esperando mi condición !*/
    pthread_cond_wait(&cambioDeVariable,  &lockDeMiVariable);/*espera bloqueada*/
pthread_mutex_unlock(&lockDeMiVariable);

  /* hago lo que esperaba para variable > LIMITE */
```

## Funciones para notificar a hebras que una condición ha cambiado

Estas funciones son:

```c
int pthread_cond_signal(pthread_cond_t *restrict cond);//despertará a una hebra que espera con la misma variable de condición

int pthread_cond_broadcast(pthread_cond_t *restrict cond);//despierta a todas la hebras esperando por la condición
```