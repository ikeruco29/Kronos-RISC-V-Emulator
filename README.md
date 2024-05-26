
# Proyecto de fin de grado
## Emulador RISC-V "Kronos"

Este repositorio contiene el código fuente de mi proyecto de fin de grado
llamado "kronos", el cual consiste en un emulador para un procesador RISC-V que implementa una ISA rv32i.



# Instalación
Para compilar el proyecto, es necesario la herramienta "Qt Creator",
aunque al haber un archivo cmake, si tienes las librerías puedes
compilarlo manualmente.


## Documentation
En primer lugar, la aplicación cuenta con un menú de navegación superior con varias opciones: 
- Archivo: Permite cargar un programa, campaña o cerrar la aplicación.
- Generar: Permite generar una campaña para un programa específico.

El emulador tiene varios paneles para realizar diferentes acciones:

En la parte superior de la terminal (panel negro) hay cuatro botones:
- Botón stop: Para la ejecución y resetea el emulador.
- Botón pausa: Detiene la ejecución sin reiniciar el emulador.
- Botón ejecutar: Ejecuta el programa cargado en memoria.
- Botón ejecutar paso a paso: Ejecuta un ciclo de reloj.

Los demás botones son autoexplicativos.
Además, debajo del panel de la RAM, hay una caja de búsqueda. El formato para buscar es en hexadecimal, sin especificar el tipo al principio:
- Correcto: 80003020
- Incorrecto: 0x80003020
## Autor
- Iker Rodríguez Hernández

