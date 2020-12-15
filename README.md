# MedianFilterParalel
Median Filter using OpenMPI

https://repl.it/@TheLe01/MedianFilter#image.h

gcc -o main main.c -fopenmp



## BENCHMARKING ##

Testes usando código sequencial

| Tamanho FIltro   |  Duração em s |
|------------------|:-------------:|
|       3          |    0:079      |
|       5          |    0:176      |
|       7          |    0:290      |

Testes usando código paralelo (2 threads)

| Tamanho FIltro   |  Duração em s |
|------------------|:-------------:|
|       3          |    0:051      |
|       5          |    0:103      |
|       7          |    0:156      |

