#!/bin/bash
#SBATCH --job-name=lab3_mpi
#SBATCH --output=lab3_%j.out
#SBATCH --error=lab3_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=16
#SBATCH --time=01:00:00
# Если на «Сергее Королёве» нужно указать раздел очереди - раскомментируйте
# и подставьте своё (смотрите командой sinfo):
##SBATCH --partition=<имя_раздела>

# Подключите модули MPI и компилятора (точные названия: module avail)
# module load mpi/openmpi
# module load python

mpicxx -O2 -std=c++17 lab3.cpp -o lab3 || exit 1

python3 measure_mpi.py
