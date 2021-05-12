#!/bin/bash
#SBATCH --partition=cpsc424

#SBATCH --ntasks=30

#SBATCH --ntasks-per-node=5

#SBATCH --cpus-per-task=1

#SBATCH --mem-per-cpu=5G

#SBATCH --job-name=TestMov

#SBATCH --time=1:00:00

#SBATCH --output=%x.out

module load iomkl

make clean
make

mpiexec -n 30 ./run
