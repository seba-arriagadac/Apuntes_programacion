# Sockets
## Generalidades
### Host name
Para obtener el *Host name*
```c
int gethostname(char *name, int len);
```
que es una funcion que pone en **name** el nombre de host de largo **len**

### Transformación entre nombre y dirección de máquina

```c
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet./in.h>
    struct hostent * gethostent (void)
    struct hostent * gethostbyname(const char *name);//este es muy usado
    struct hostent * gethostbyaddr(const char * addr, int len, int type);//este igual es usado
    int sethostent(int stayopen);
    int endhostent(void);
```    

Estas funciones buscan los nombres de máquinas y sus direcciones en bases de datos que dependen de la configuración del sistema y dependen del servicio DNS.
**sethostent()** y **endhostent()** permiten abrir y cerrar la base de datos.
**gethostent()** lee la próxima entrada en la base de datos.
**gethostbyname** busca la entrada con el nombre de máquina dado.
**gethostbyaddr** busca la entrada con la dirección de máquina dada (IP).

Estas funciones retornan un puntero a la estructura con la información sobre la máquina. Si no está retornan un NULL.
```c
    struct hostent {
        char  *h_name;  /* nombre oficial */
        char  **h_aliases;  /* aliases */
        int   h_addrtype;
        int   h_length;
        char  **h_addr_list;
    }
```

### Servicios y Números de Puertas
Cada servicio (ftp, telnet, http) tiene asociado un puerto.
Los puertos son enteros sin signo de 16 bits.

Para ver los archivos se hace en la consola
```
/etc/services
```
### Orden de los Bytes en la Red
Intel (80x86, Pentium y otros) y Digital Equipment Corporation (DEC) son little-endian. Motorola y Sun SPARC son big-endian (byte más significativo en la dirección menor de memoria). 
**Solución:** se definió un orden de bytes de red (= big-endian). Así se asegura que el tráfico puede ser leído en ambas arquitecturas. 

#### Funciones para hacer las conversiones:
```c
    #include <sys/types.h>
    #include <netinet/in.h>
    u_long htonl (u_long hostlong);
    u_short htons(u_short hostshort);
    u_long ntohl(u_long netlong);
    u_long ntohs(u_short netshort);
```
    Ver páginas man en plataforma específica. Por ejemplo en aragorn año 2008, se tiene:
```c
    #include <netinet/in.h>
     uint32_t htonl(uint32_t hostlong);
     uint16_t htons(uint16_t hostshort);
     uint32_t ntohl(uint32_t netlong);
     uint16_t ntohs(uint16_t netshort);
```

gethostby* retornan enteros con orden de red.
¿Cómo tratar string? No hay problemas porq son secuencia de bits (el orden se modifica a nivel de bits)
¿Cómo tratar números en punto flotante? Aqui si hay q tener cuidado porq va a estar completamente al reves todos los bites
## Creación de sockets
El socket es el punto de comunicacion entre la capa de aplicación y la capa de transporte.
```
#include <sys/types.h>
#include <sys/socket.h>
int socket (int domain, int type, int protocol);
```
**domain** especifica la familia de direcciones en que se debe interpretar la dirección del socket. Puede ser: **AF_UNIX** , en cuyo caso la dirección es un pathname; o AF_INET , en caso de una dirección Internet IPv4, Usar AP_INET6 para IPv6. Ver más en man socket.

**type** puede ser **SOCK_STREAM**, lo cual especifica una conexión de circuito virtual. Es bidireccional y contínuo. En la Internet éste implica **TCP**. Otra opción es **SOCK_DGRAM**, lo cual especifica envío de paquetes discretos. En la Internet éste corresponde a **UDP**. Otras opciones son: SOCK_RAW para acceso a un protocolo "crudo", por ejemplo directo a IP , SOCK_RDM provee servicio garantizado pero sin comprometer orden. **Si es string o datagrama**

**protocolo** es el número del protocolo específico a utilizar del tipo señalado. Normalmente sólo un protocolo es soportado de un tipo dado dentro de una familia de protocolos. Lo mejor es dejarlo en cero y el sistema resuelve el valor que corresponde a ese protocolo según el valor dado en type.

La función **retorna** un descriptor de socket. Éste es similar a un descriptor de archivo. 

## Funciones en programación de Servidor
El servidor debe asociar el socket con un puerto y dirección de interfaz de red para que los clientes puedan acceder a él. Esto se logra con la función bind.
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int bind (int s, const sockaddr * name, int addrlen);
```
**s** es el socket ya creado,
**addrlen** es el largo de la estructura name, y
**name** es del tipo sockaddr_in
```c
    struct socketaddr_in {
        short                   sin_family; /* AF_INET */
        u_short               sin_port;    /* puerta de servicio */
        struct in_addr     sin_addr;    /* dirección asociada a la puerta */
    }
```
Las máquinas pueden tener más de una tarjeta de red; i.e. más de una dirección Internet, por ello se debe especificar la dirección sin_addr. Si queremos atender requerimientos entrando por cualquier interfaz de red, se puede usar INADDR_ANY como dirección asociada a la interfaz.

### Esperando por Conexiones
Sólo es requerido cuando el servicio es orientado al stream (TCP).
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int listen (int s, int backlog);
```
Esta función comunica al sistema operativo que el socket está listo para recibir conexiones. backlog especifica el máximo número de requerimientos de conexión que pueden estar pendientes en cada momento (<= 5). Si hay muchos tratando de conectarse, el cliente recibe un mensaje "connection refused".

### Aceptación de Conexiones
En el caso de servidores TCP
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int accept (int s, struct sockaddr *name, int *addrlen);
```
 Esta función retorna un nuevo descriptor de socket que el servidor usa para comunicarse con el cliente que fue aceptado. El OS almacena la información del cliente en name y el largo de la estructura en addrlen; es decir son valores de salida.

### Funciones Usadas en lado Cliente
#### Conexión con el servidor
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int connect (int s, struct socketaddr *name, int addrlen);
```
Esta función conecta el socket s con el servidor corriendo en la máquina y puerto especificada en name.

Esta fucnión puede ser usada en conexiones datagramas o de circuito virtual. En el primer caso esta función deja claro que los próximos datagramas están dirigidos al destino donde se hace la conexión. De otra forma la dirección destino debe ser especificada en cada llamado de envío de datagrama.

#### Funciones para Transferencia de Datos (Servicor y Cliente)
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int recv (int s, char * buf, int len, int flags);
    int send (int s, const char * buf, int len, int flags);
```
Son idénticas a read and write, sólo que ellas tienen un cuarto argumento para especificar como enviar o recibir los datos.

Cuando se usa conexión datagrama, el servidor no llama a las funciones listen y accept, y el cliente generalemnet no llama a la función connect. En estos casos se usan las funciones:
```c
    int recvfrom(int s, char * buf, int len, int flags, struct sockaddr *from, int fromlen);
    int sendto(int s, const char *buf, int len, int flags, struct sockaddr *to, int tolen);
```
#### Destrucción  o Cierre del Canal de Comunicación (Servicor y Cliente)
Es posible usar la función close(). Ésta se bloquea hasta que todos los datos sean enviados. 
Alternativamente se puede usar:
```c
    #include <sys/types.h>
    #include <sys/socket.h>
    int shutdown(int s, int how);
```
how = 0 se cierra para lectura, se puede seguir enviando.
how = 1 se cierra para escritura, se puede seguir recibiendo. Datos siendo enviados por el sistema operativo pueden ser descartados.
how = 2  equivale a shutdown(0); shutdown(1);