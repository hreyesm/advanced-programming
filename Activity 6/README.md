# Actividad 6: Sockets y señales
## Instrucciones para correr
1. Compilar ambos archivos en el repositorio de la siguiente manera:
```c 
    gcc main.c -o console
    gcc semaphore.c -o semaphore
```
2. Abrir cinco terminales en la misma ruta del directorio donde estén almacenados los archivos. 
3. En una terminal ejecutar el siguiente comando:
```c 
    ./console 127.0.0.0.1
```
4. El semáforo que se encargará de enviar la primera señal, se deberá ejecutar colocando una _flag_ (-f) antes de la dirección IP.
```c 
    ./semaphore -f 127.0.0.0.1
```
5. Correr en las siguientes (4)
