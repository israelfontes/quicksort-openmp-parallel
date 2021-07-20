#!/bin/bash

#SBATCH --partition=cluster
#SBATCH --job-name=quicksort_i
#SBATCH --output=quicksort_i.out
#SBATCH --error=quicksort_i.err
#SBATCH --time=0-03:00
#SBATCH --hint=compute_bound

#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32

export OMP_NUM_THREADS=32

module load compilers/gnu/8.3
module load libraries/openmpi/4.0.3-pgi-19.10

#Apaga os tempos
rm times/*

#Apaga os binarios
rm bin/*

#Apagar os resultados
rm data/*

#Compila o código
gcc -g -Wall src/quicksort_openmp.c -o bin/quicksort_openmp -fopenmp -std=gnu99
gcc -g -Wall src/quicksort_sequential.c -o bin/quicksort_sequential -std=gnu99

tentativas=10 #Quantas vezes o código será executado

    for size in 3000000 4000000 5000000 6000000 #tamanho do problema
		do
               	
			echo -e "\n$size\t\t\c" >> "times/serial.txt" 

			timerSeq=0
			for tentativa in $(seq $tentativas) #Cria uma vetor de 1 até "tentativas"
			do
				#Executar o código.
				valorSeq=`./bin/quicksort_sequential $size`
				#echo $valor
				timerSeq=$((valorSeq+timerSeq))

			done

			echo -e `echo "scale=2;${timerSeq}/100/${tentativas}"|bc` >> "times/serial.txt"

		done


    for cores in 4 8 16 32 #números de cores utilizados
    do
        for size in 3000000 4000000 5000000 6000000 #tamanho do problema
        do  
            echo -e "\n$cores\t$size\t\t\c" >> "times/openmp.txt" 

            timerPth=0
            for tentativa in $(seq $tentativas) #Cria uma vetor de 1 até "tentativas"
            do
                #Executar o código.
                valorPth=`srun ./bin/quicksort_openmp $size $cores`
                #echo $valorPth
                timerPth=$((valorPth+timerPth))

            done

            echo -e `echo "scale=2;${timerPth}/100/${tentativas}"|bc` >> "times/openmp.txt"
        done
    done
exit
