# lsc-projekt

## Sposób uruchomienia na Zeusie (bez Singularity, tylko MPI):
```
srun -n=10 -p plgrid-testing -t 20:00 --pty /bin/bash

module add plgrid/tools/impi

mpicc main.c -o main -lm

mpiexec -np 10 ./main
```

## Sposób zbudowania obrazu

W klatalogu znajduje się plik singularity.def. Aby zbudować z nego plik .sif, służący do uruchamiania kontenera należy użyć polecenia:
```
singularity build singularity.sif singularity.def
```

## Sposób uruchomienia na Prometeuszu

Prometeusz ma moduł z Singularity, który można załadować i wykorzystać. Przykłądowy skrypt jest wrzucony jako run_slurm.sh (należy zmienić odpowiednio parametry).
```
srun -n=10 -p plgrid-testing -t 20:00 --pty /bin/bash

module add plgrid/tools/singularity/stable
module add plgrid/tools/mpich

mpicc main.c -o main -lm

mpirun singularity exec singularity.sif /opt/main
```

## Sposób uruchomienia na AWSie

Do uruchomienia na AWSie użyty został Parallel Cluster. Aby z niego korzystaćnależy pobrać i skonfiguurować [AWS CLI](https://aws.amazon.com/cli/) oraz [AWS ParallelCluster CLI](https://aws-parallelcluster.readthedocs.io/en/latest/getting_started.html). Nastpnie wyrać następujące komendy:
```
pclusteer configure                       // konfiguracja klastra: należy wybrć odpoiwedni region, typ maszyny, ilość węzłów oraz automatyczne tworzenie VPC

pcluster create lsc_cl                    // tworzenie klastra

pcluster ssh lsc_cl -i <plik .pem z kluczem>        // łączenie się do klastra (user view)
```
Następnie należy zainstalować Singularity, roces jest przedstawiony w tym artykule: https://qywu.github.io/2020/12/09/aws-slumr-pytorch.html.

Program napisany w MPI można uruchamiać identycznie jak na Promteuszu.
