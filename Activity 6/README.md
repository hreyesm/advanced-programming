# Actividad 6: Sockets y señales
## Instrucciones para correr
### 1. Abrir terminales
Abrir cinco terminales en la misma ruta del directorio donde estén almacenados los archivos que se encuentran en el repositorio.<br>
_Nota: Es recomendable colocar las cuatro terminales de los semáforos en cada una de las esquinas de la pantallas._
### 2. Compilar programas
Compilar ambos archivos en el repositorio de la siguiente manera:
```c 
    gcc main.c -o console
    gcc semaphore.c -o semaphore
```
### 3. Ejecutar programas
<strong>Ejecutar la consola</strong><br>
El servidor debe de ser el primero en ejecutarse ya que es necesario que el socket esté prendido, esperando a que los semáforos se conecten.
```c 
    ./console 127.0.0.0.1
```
<strong>Ejecutar el primer semáforo</strong> <br>
Dado que uno de los semáforos debe de ser el primero en enviar la señal (la cual indica que su vecino se debe de poner en estado verde), se deberá incluir una bandera (-f) antes de la dirección IP.
```c 
    ./semaphore -f 127.0.0.0.1
```
<strong>Ejecutar el resto de los semáforos</strong><br>
Así como se recomienda colocar las cuatro terminales en las esquinas de la pantalla, se recomienda comenzar a ejecutar el resto de los semáforos en el orden de las manecillas de reloj.
```c 
    ./semaphore 127.0.0.0.1
```

### 4. Confirmar que la conexión con el socket fue exitosa
Si la conexión con el socket fue exitosa, entonces en cada una de las terminales aparecerá el siguiente mensaje: 
```c 
    PID = 6083
    Enter the PID of the semaphore on the right: 
```
En este momento cada uno de las terminales  de los semáforos está esperando a que el usuario ingrese el PID del semáforo a su derecha.

### 5. Ingresar los PIDs de los semáforos en las terminales
<strong>Comenzar a ingresar los PIDS se los semáforos que _no_ tienen la bandera -f</strong><br>
El usuario simplemente deberá copiar y pegar los PIDs en el orden en el que quiere que se comuniquen los semáforos. Al momento de ingresarlos, aparecerá un nuevo mensaje el cual le indicará al usuario a qué PID le estará enviando las señales.

<strong>Ingresar el PID al que el primer semáforo enviará la bandera</strong><br>
El <strong>último</strong> PID que se debe de ingresar a la consola del semáforo, es aquél que tiene la bandera, ya que en el momento en el que se presione la tecla ENTER, comenzará la ejecución del programa.

Una vez que el último PID sea ingresado, comenzará la ejecución del programa

### 6. Interrumpir los semáforos desde la consola
A partir de este momento, el usuario podrá hacer uso de Ctrl + C y Ctrl + Z para detener y reiniciar los semáforos en el último estado en el se encontraban antes de su interrupción.
