# Proyecto final del curso Estructuras de datos avanzadas (substring matching)
## Introducción  
El objetivo del proyecto es la búsqueda de una cadena de consulta en un conjunto de metadatos de papers, y el retorno de aquellos documentos que contengan la consulta en texto.  
Toda la información de como se procederá en la resolución de este problema la encontrará en el documento `saul_andersson_rojas_coila.pptx` .
## Conjunto de datos  
El conjunto de datos para la búsqueda se encuentran en [conjunto de datos](https://www.kaggle.com/Cornell-University/arxiv/).
## Formateo del conjunto de datos
Antes de indizar los documentos lo que hago es un preprocesamiento con **python** obteniendo el conjunto de datos limpio, que podrá descargar en [conjunto de datos formateado](https://drive.google.com/file/d/1Bb9Jqn16zbLK69xlGaljnsok23Vutuil/view?usp=sharing).
## Compilación del programa
Para compilar siga estos pasos:
* Descargar e instalar [cmake](https://cmake.org/download/)
* Una vez hecha la instalación dirijase a la carpeta donde se encuentra el proyecto y ejecute el siguiente comando: `cmake -S . -B build`, esto generará la carpeta `build` dentro del directorio junto con los archivos necesarios para la compilación en cualquiera de los compiladores preinstalados en el sistema, los más comunes *_Visual estudio_* para sistemas windows y *_makefiles_* para basados en Linux.
* Tendrá que copiar el [conjunto de datos formateado](https://drive.google.com/file/d/1Bb9Jqn16zbLK69xlGaljnsok23Vutuil/view?usp=sharing) dentro de la carpeta `build` y ejecutar el programa.
## Muestra de los resultados
Hemos realizado la indización de 10000 documentos

