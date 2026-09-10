# \# Actividad 2.6: Problema Práctico en OpenMP

# 

# \## 1. Nombre de la práctica

# Búsqueda Exhaustiva Paralela con OpenMP\*\*

# 

# &#x20;Integrantes

# \*\*Alumno:\*\* Julio Abraham Angulo Diaz

# \*\*Profesor:\*\* Jose Navarro Rios

# \*\*Materia:\*\* Programación Paralela

# \*\*Institución:\*\* Centro Universitario de Tonalá (CUTonalá) - Universidad de Guadalajara

# 

# \## 3. Descripción de la solución

# El proyecto resuelve el problema de ruptura de claves alfanuméricas mediante búsqueda exhaustiva (fuerza bruta). Calcula el tamaño total del espacio de búsqueda ($36^{\\text{longitud}}$) y evalúa cada combinación posible. Para optimizar el uso de memoria y evitar la creación previa de millones de cadenas, convierte sobre la marcha el índice numérico de la iteración a su equivalente alfanumérico en base 36.

# 

# \## 4. Caracteres utilizados

# El espacio de búsqueda está constituido por un conjunto constante de \*\*36 caracteres\*\*:

# \* \*\*Letras mayúsculas:\*\* `A-Z` (26 caracteres)

# \* \*\*Dígitos numéricos:\*\* `0-9` (10 caracteres)

# \* \*\*Conjunto:\*\* `{"A", "B", ..., "Z", "0", "1", ..., "9"}`

# 

# \## 5. Algoritmos implementados

# \*\*Búsqueda Secuencial:\*\* Evalúa linealmente el espacio de combinaciones desde el índice `0` hasta $36^N - 1$ utilizando un único hilo de procesamiento.

# \*\*Búsqueda Paralela Distribución por Rangos:\*\* Divide el espacio total de combinaciones de forma equitativa entre los hilos disponibles (`total / num\_hilos`). Cada hilo ejecuta de forma independiente la búsqueda en su rango asignado `\[inicio, fin]`.

# 

# \## 6. Directivas y funciones de OpenMP utilizadas

# \* `#pragma omp parallel`: Define la región paralela e inicia la ejecución multihilo para la distribución por rangos.

# \* `#pragma omp critical`: Define una sección crítica de acceso exclusivo para garantizar que solo un hilo a la vez actualice la variable global con su identificador cuando encuentra la clave, evitando \*\*condiciones de carrera\*\* (\*race conditions\*).

# \* `omp\_get\_thread\_num()`: Obtiene el ID del hilo actual para calcular sus límites de búsqueda `inicio` y `fin`.

# \* `omp\_get\_num\_threads()`: Obtiene la cantidad total de hilos para dividir el espacio de trabajo en bloques (\*chunks\*).

# \* `omp\_get\_wtime()`: Mide con alta precisión el tiempo de inicio y fin de la ejecución para calcular el tiempo total y el \*Speedup\*.

# 

# \## 7. Instrucciones básicas de compilación y ejecución

# 

# \### Compilación (GCC / g++)

# Asegúrate de incluir la bandera `-fopenmp` al compilar:

# ```bash

# g++ -fopenmp main.cpp -o fuerza\_bruta

