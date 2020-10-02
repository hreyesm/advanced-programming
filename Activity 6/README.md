# Actividad 6: Sockets y señales
### Autores
* [Daniela Vignau (A01021698)](https://github.com/dvigleo)
* [Héctor Reyes (A01339607)](https://github.com/hreyesm)
## Instrucciones de ejecución
### 1. Abrir ventanas de terminal
Abrir cinco ventanas de terminal (una correspondiente a la consola de control y cuatro a los semáforos) en el mismo directorio donde están almacenados los archivos que se encuentran en este repositorio.<br />
_Nota: Se recomienda colocar las cuatro ventanas para los semáforos en cada una de las esquinas de la pantalla._
### 2. Compilar archivos
Compilar ambos archivos de la siguiente manera:
```c
    gcc main.c -o console
    gcc semaphore.c -o semaphore
```
### 3. Inicializar consola de control y semáforos
<strong>Inicializar la consola de control</strong>
La consola de control (servidor) debe ser el primer proceso en ejecutarse, ya que es necesario que espere a que los semáforos (clientes) terminen de conectarse a través del socket previo a mostrar su información.<br />
```c 
    ./console 127.0.0.0.1
```
<strong>Inicializar el primer semáforo</strong>
Dado que uno de los semáforos debe enviar la primera señal, la cual indicará a su vecino que cambie su estado de _RED_ a _GREEN_, debe agregarse el argumento _-f_ antes de la dirección IP.
```c 
    ./semaphore -f 127.0.0.0.1
```
<strong>Inicializar el resto de los semáforos</strong>
De la misma forma en que se recomienda colocar los cuatro semáforos en cada una de las esquinas de la pantalla, se recomienda inicializar el resto de los semáforos en sentido horario.
```c 
    ./semaphore 127.0.0.0.1
```

### 4. Verificar que la conexión a la consola de control fue exitosa
Si la conexión a la consola de control fue exitosa, cada uno de los semáforos mostrará el siguiente mensaje: 
```c 
    PID: <PID>
    Enter the PID of the semaphore on the right: 
```
A partir de ese momento, cada uno de los semáforos esperará a que el usuario ingrese el PID del semáforo a su derecha.

### 5. Conectar los semáforos entre sí
<strong>Ingresar en cada semáforo el PID del semáforo a su derecha (para los semáforos que no reciben el argumento _-f_)</strong>
El usuario deberá copiar y pegar en cada semáforo, uno a uno, el PID del semáforo a su derecha en el orden en que quiere que se comuniquen los semáforos. Al ingresar un PID, el semáforo indicará al usuario a qué PID le estará enviando las señales.

<strong>Ingresar en el semáforo que recibe el argumento _-f_ el PID del semáforo a su derecha</strong>
El <strong>último</strong> semáforo para el cual se debe ingresar el PID del semáforo a su derecha es aquél que recibe el argumento _-f_, ya que en el momento en que se presione la tecla `Enter`, comenzará la comunicación entre procesos.

### 6. Interrumpir los semáforos desde la consola
Una vez conectados los semáforos entre sí, el usuario podrá usar `Ctrl + C` y `Ctrl + Z` para cambiar el estado de los semáforos mediante interrupciones o para reestablecerlos al último estado en que se encontraban antes de ser interrumpidos.
