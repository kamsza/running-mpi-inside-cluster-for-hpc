#!/bin/bash -l
## Nazwa zlecenia
#SBATCH -J singularity_mpi
## Liczba alokowanych węzłów
#SBATCH -N 2
## Liczba zadań per węzeł (domyślnie jest to liczba alokowanych rdzeni na węźle)
#SBATCH --ntasks-per-node=2
## Maksymalny czas trwania zlecenia (format HH:MM:SS)
#SBATCH --time=00:30:00
## Nazwa grantu do rozliczenia zużycia zasobów
#SBATCH -A plgdais2
## Specyfikacja partycji
#SBATCH --partition=plgrid-short
## Plik ze standardowym wyjściem
#SBATCH --output="logs/wyjscie.out"
## Plik ze standardowym wyjściem błędów
#SBATCH --error="logs/error.err"

## przejscie do katalogu z ktorego wywolany zostal sbatch
cd $SLURM_SUBMIT_DIR
srun /bin/hostname

module add plgrid/tools/singularity/stable
module add plgrid/tools/mpich

mpirun -n 5 singularity exec singularity.sif /opt/mpitest
