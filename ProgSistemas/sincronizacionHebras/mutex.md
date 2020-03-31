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
