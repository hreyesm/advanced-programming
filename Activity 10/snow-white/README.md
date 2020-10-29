# Actividad 10: Concurrencia (Ejercicio 1)
### Autores
* [Daniela Vignau (A01021698)](https://github.com/dvigleo)
* [Héctor Reyes (A01339607)](https://github.com/hreyesm)

## Análisis sobre los problemas de concurrencia
La región crítica del problema descrito es la mesa con las 4 sillas
* Al ser 7 hilos los que van a intentar acceder a dicha región crítica, se deben proteger las variables que:
    * se utilizan para mandar el ID del enano a Blancanieves (_table[CHAIRS]_)
    * indican el número de enanos que Blancanieves tiene esperando por comer (_dwarfsToServe_)
* Por otra parte, Blancanieves, al ser un único hilo, no existen problemas de concurrencia notables.

## Solución seleccionada
Para poder resolverlo dados los anteriormente mencioandos problemas de concurrencia, se propone utilizar 1 mutex y 2 semáforos. A continuación se explicará el uso que se le dará a cada uno de ellos dentro del programa.

### Mutex
Dado que la región crítica está bastante bien definida, solo es necesario la implementación de un solo mutex para proteger cuando:
    * Un enano a la vez escriba su ID sobre el arreglo _table_ 
    * Se modifique la variable global _dwarfsToServe_. 
        * Esta última variable aumentará en el hilo de los enanos y disminuirá en cuanto Blancanieves lo atienda.
### Semáforos
Se decidió utilizar semáforos porque la implementación de estos es bastante más sencilla y comprensible que la de las variables de condición. 
* _turn_: Semáforo binario que se utilizará para indicar a Blancanieves que hay un enano esperando a ser atendido. 
    * Se hará un _post_ en el hilo de los enanos en el momento en el que un enano se siente en una de las sillas disponibles
    * Se hara un _wait_ en el hilo de Blancanieves para que lo comience a atender
* _chairs_: Semáforo inicializado en CHAIRS (4) que se utilizará para llevar la cuenta del número de sillas disponibles.
    * Se hace un _wait_ dentro del _mutex_ en el hilo de los enanos para verificar que existe una silla disponible
    * Se hace un _post_ en el hilo de los enanos una vez que el enano terminó de comer y libera la silla