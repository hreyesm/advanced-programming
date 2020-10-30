# Actividad 10: Concurrencia (Ejercicio 1)
### Autores
* [Daniela Vignau (A01021698)](https://github.com/dvigleo)
* [Héctor Reyes (A01339607)](https://github.com/hreyesm)

## Descripción y análisis
### Descripción del sistema
Se busca modelar un sistema basado en hilos que simule dos tipos de agentes, cada uno con su respectivo comportamiento:
* <strong>Blancanieves (1 hilo):</strong> Sirve comida a todos los enanos que se sientan en la mesa, la cual cuenta con 4 sillas. Cuando termina de servir a todos los enanos sentados en un momento dado, sale a caminar. Vuelve de caminar cuando un nuevo enano se sienta en la mesa.
* <strong>Enano (7 hilos, uno por cada enano):</strong> Al llegar a la mesa, verifica si hay una silla disponible para sentarse. Si es así, se sienta y pide a Blancanieves que le sirva la comida. Una vez que termina de comer, deja su silla.
### Análisis de los problemas de concurrencia
La región crítica del sistema descrito comprende las 4 sillas. Dado que hay 7 hilos que van a intentar acceder a esta región crítica, es necesario proteger los siguientes recursos:
* <strong>_table[CHAIRS]_:</strong> Se utiliza para dar a conocer el ID de un hilo de tipo enano a Blancanieves.
* <strong>_dwarfsToServe_:</strong> Indica el número de enanos que Blancanieves tiene pendientes para servir en un momento dado.

Por su parte, cabe mencionar que el hilo correspondiente a Blancanieves, al ser único durante la ejecución, no presenta problemas de concurrencia notables.
## Solución
Para asegurar la concurrencia dentro del sistema, se optó por utilizar 1 mutex y 2 semáforos. A continuación se explica el uso que se le dio a cada uno de ellos.

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
