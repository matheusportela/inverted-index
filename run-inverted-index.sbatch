#!/bin/bash

#SBATCH --job-name=index
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=2
#SBATCH --mem=16GB
#SBATCH --time=24:00:00

module purge
module load gcc/9.1.0
module load cmake/intel/3.11.4

unset XDG_RUNTIME_DIR
if [ "$SLURM_JOBTMP" != "" ]; then
    export XDG_RUNTIME_DIR=$SLURM_JOBTMP
fi

cd /scratch/mvp307/inverted-index
./run
