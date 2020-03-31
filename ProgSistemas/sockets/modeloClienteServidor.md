# Modelo Cliente-Servidor y Multiplexión de I/O (select)

El modelo **cliente-servidor** (similar al del teléfono) se usa en muchas aplicaciones. 
Otro modelo **peer-to-peer**, el cual se ve por ejemplo en aplicaciones tipo Messanger y Skype. En esta sección veremos las bases de comunicación de estos sistemas.

## Clasificación de Servidores
Por la forma como un servidor atiende las peticiones entrantes
### Servidor Iterativo

- Espera por la llegada de un cliente
- Procesa el requerimiento de un cliente
- Envía la respuesta a ese cliente
- Regresa al primer paso

Como consecuencia de esta interacción *sólo un cliente puede ser atendido a la vez*.
Este tipo de servidor solo atiende un requerimiento por vez.

Éste es el caso típico de servidores UDP (UDPServer.c (**ver reusePort**)  UDPClient.c). Como no hay una conexión establecida, los requerimientos de varios clientes se pueden intercalar conservando "atómicamente" las secuencias requerimiento y respuesta para cada cliente.

Un servidor Iterativo basado en TCP es **menos conveniente**. En este caso hay una conexión con el cliente y sólo podemos atender a otro cliente cuando el previo ha concluido todas sus interacciones requerimiento-respuesta. La postergación de los otros clientes es mayor comparada con servidor UDP.

### Servidor Concurrente: trabaja así:

- Espera por la llegada de un cliente
- Inicia un servidor para manejar los requerimientos del cliente. Esto involucra la creación de **un nuevo proceso o hebra**. Cuando el cliente se va (termina) el proceso o hebra también termina.
- Regresa al primer paso.

Éste es el caso típico de servidores **TCP** (TCPServer.c  TCPClient.c) con procesos o hebras a cargo de cada cliente. El paso tres es alcanzado tan pronto como se inicia la atención del Cliente, por lo tanto múltiples clientes pueden ser atendidos en forma concurrente. 

Destacar:

- Valores de puertas asignadas cuando se establece una conexión. La 4-tupla (LocalPort, LocalIP, remotePort, RemoteIP) define cada conexión TCP.
- Llamados para obtener información sobre la conexión: gethostname(), gethostbyname(), inet_ntoa(), inet_addr(), getsockname(), getpeername(), setsockopt(). Notar que algunas de estas funciones (Ej. gethostbyname()) no garantizan re-entrancia. En páginas man correspondiente buscar por "reentrant"
- Configuración de puerto como reusable. El cliente o servidor que termina primero deja tomado el puerto por un tiempo. Si el servidor termina primero, no podremos usar el mismo puerto por un rato (~90 [s]).

Una situación intermedia es creada **cuando un servidor TCP atiende múltiples clientes en forma secuencial por requerimiento y no por conexión. Es un caso parecido al servidor Iterativo UDP, pero implementado con TCP**. En este caso tenemos:

- Espera por la llegada de un cliente o nuevo requerimiento de un cliente ya conectado.
- Acepta la conexión de un nuevo cliente o procesa requerimiento de uno ya conectado
- Si se atendió un requerimiento, envía la respuesta al cliente que envió el requerimiento
- Regresa al primer paso

Para poder esperar requerimientos que pueden llegar por múltiples descriptores, se dispone de la función select().
### Técnicas que se parecen a **select** son:

- Creación de múltiples procesos y cada uno atiende cada descriptor. Esta opción genera complicaciones cuando hay datos  compartidos entre las conexiones. Por ejemplo en un chat en que participan varios. Lo que uno recibe se debe enviar al resto, pero los otros están en otros procesos.
- El uso de hilos es similar al caso anterior. Aquí los datos se pueden compartir, pero se deben eliminar carreras críticas o inconsitencias cuando se requiere comunicación entre los distintos hilos.
- Uso de I/O sin bloqueo (nonbloking I/O). En este caso el servidor hace una encuesta descriptor por descriptor identificando si hay algún requerimiento: Si no lo hay, se retorna inmediatamente y se consulta el otro descriptor. Conduce a sistemas con "Busy-wait" y por ello esta opción no es recomendada.
- Uso de I/O asíncrono. Se anuncia deseo de hacer una lectura o escritura, luego el proceso es interrumpido cuando tiene algo para leer o escribir. Esta opción no es soportada por todos los sistemas operativos.
- En resumen, veremos el select dado que es soportado más ampliamente, y es simple pues en todo momento tenemos un sólo hilo (secuencia en ejecución)


Las funciones **select** y **poll** proveen mecanismos **para que los programas chequeen un grupo de descriptores y sepan cuándo existe entradas, salidas, o alguna condición de excepción en alguno de ellos**.

    #include <sys/types.h>
    #include <sys/time.h>

    int select ( int maxfd, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval * timeout_ptr );

    Macros para manipular el conjunto de descriptores:
    void FD_SET ( int fd, fd_set * fdset);  /* agrega un descriptor al conjunto de descriptores */
    void FD_CLR ( int fd, fd_set * fdset); /* remueve un sescriptor del conjunto */
    int FD_ISSET ( int fd, fd_set * fdset);  /* retorna verdadero si el descriptor es parte del conjunto */
    void FD_ZERO ( fd_set * fdset);  /* desactiva todos los descriptores del conjunto pasados como argumento equivale a remover los elementos del conjunto */

    struct timeval {
        long tv_sec;    /* segundos*/
        long tv_usec;    /* micro segundos */
    }
     

Valores para timeout_ptr

    NULL : Espera por siempre. Espera infinita por actividad en alguno de los descriptores. Esta espera puede ser interrumpida por la llegada de una señal.
    tv_sec = tv_usec = 0:    No espera. Todos los descriptores son chequeados y se retorna inmediatamente.
    tv_sec != 0 ó tv_usec != 0 : Se espera hasta el tiempo indicado. Si hay actividad antes, se retorna inmediatamente, sino se retorna como el caso anterior al término del timeout.

Valores retornado por select:

    -1: Ocurrió un error. Por ejemplo si llegó una señal.
     0: no hay actividad en ninguno de los descriptores.
     n: número de descriptores que presentan actividad.

La función pselect es una variante de select.
int   pselect(int   n,   fd_set   *readfds,  fd_set  *writefds,  fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask);
La principal diferencia con select es el tipo de dato para especificar el tiempo de espera. Ahora el tiempo es especificado en segundos y nanosegundos. Si el último argumento es null, el comportamiento es como en select. Si es no nulo, este valor define máscara de señales que serán aceptadas mientras se espera.

Ejemplos

     Servidor Iterativo Cliente
    Leyendo entradas de múltiples terminales (o ventanas xterm o telnet) select
