# Actividad 10: Concurrencia (Ejercicio 1)
### Autores
* [Daniela Vignau (A01021698)](https://github.com/dvigleo)
* [Héctor Reyes (A01339607)](https://github.com/hreyesm)

## Descripción y análisis
### Descripción del sistema
Se busca modelar un sistema basado en hilos que simule dos tipos de agentes, cada uno con su respectivo comportamiento:
* Blancanieves (1 hilo): Sirve comida a todos los enanos que se sientan en la mesa, la cual cuenta con 4 sillas. Cuando termina de servir a todos los enanos sentados en un momento dado, sale a caminar. Vuelve de caminar cuando un nuevo enano se sienta en la mesa.
* Enano (7 hilos, uno por cada enano): Al llegar a la mesa, verifica si hay una silla disponible para sentarse. Si es así, se sienta y pide a Blancanieves que le sirva la comida. Una vez que termina de comer, deja su silla.
### Análisis de los problemas de concurrencia
La región crítica del sistema comprende el proceso de asignación de sillas. Dado que hay 7 hilos que van a intentar acceder a esta región crítica, es necesario proteger los siguientes recursos compartidos:
* _table[CHAIRS]_: Se utiliza para dar a conocer el ID de un hilo de tipo enano a Blancanieves.
* _dwarfsToServe_: Indica el número de enanos que Blancanieves tiene pendientes para servir en un momento dado.

Por su parte, el hilo correspondiente a Blancanieves, al ser único durante la ejecución, no supone problemas de concurrencia notables.
## Solución
Para asegurar la concurrencia y la sincronización dentro del sistema, se optó por utilizar 1 mutex y 2 semáforos. A continuación se explica el uso que se le dio a cada uno de ellos.

### Mutex
Debido a que la región crítica está bien definida, los recursos compartidos sólo deben protegerse mediante un mutex cuando alguno de los siguientes casos se presenta:
 * Un enano escribe su ID en una posición del arreglo _table_, lo cual equivale a sentarse en una silla disponible.
 * La variable _dwarfsToServe_ es modificada. Ésta aumentará en 1 cuando un enano se sienta y disminuirá en 1 en cuanto Blancanieves le sirve la comida.
### Semáforos
* _turn_: Semáforo binario para la sincronización entre Blancanieves y los enanos. Indica a Blancanieves que hay un enano esperando a que le sirvan.

   * Se hace una llamada a _post_ en el hilo de los enanos en el momento en que un enano se sienta en una de las sillas disponibles.
   * Se hace una llamada a _wait_ en el hilo de Blancanieves para que ésta atienda al enano que la despertó.
* _chairs_: Semáforo no binario inicializado en CHAIRS (4) para llevar la cuenta del número de sillas disponibles.
   * Se hace una llamada a _post_ en el hilo de los enanos cuando un enano termina de comer y libera la silla.
   * Se hace una llamada a _wait_ dentro del _mutex_ en el hilo de los enanos para verificar que existe una silla disponible.
