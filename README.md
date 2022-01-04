# lsc-projekt

## Sposób uruchomienia na Zeusie:
```
srun -N 1 --cpus-per-task=2 -p plgrid-testing -t 20:00 --pty /bin/bash

module add plgrid/tools/impi

mpicc main.c -o main -lm

mpiexec -np 1 ./main
```