#!/bin/bash
cd ../gantt-gplot
chmod +x ./generate_gantt_chart
chmod +x ./gantt.py
cd ../schedsim

if(rm -r result); then
	echo "Borramos la carpeta result."
fi

mkdir result
echo "Creamos la carpeta result"

echo "Introduzca el nombre de fichero: "
read fichero

while [ ! -e examples/$fichero ]; do
	echo "El fichero no existe."	
	echo "Introduzca el nombre de fichero: "
	read fichero
done

echo "Introduzca un número entre 1 y 8 de CPUs: "
read nCPUs

while [ $nCPUs -gt 8 ] || [ $nCPUs -lt 1 ]; do
	echo "Introduzca un número entre 1 y 8 de CPUs: "
	read nCPUs
done

avaliableSchedulerList=( "RR" "SJF" "PRIO" "FCFS" )
for avaliableScheduler in ${avaliableSchedulerList[@]}; do
	for CPU in `seq 1 $nCPUs`; do
		echo "Ejecutando $avaliableScheduler con $CPU CPUs: " 
		if [ "$avaliableScheduler" == "PRIO" ]; then
			./schedsim -i examples/$fichero -n $CPU -s $avaliableScheduler -p
		else
			./schedsim -i examples/$fichero -n $CPU -s $avaliableScheduler
		fi

		for ((i=0; i<$CPU; i++)); do
			echo "Moviendo CPU-$CPU-_$i.log a la carpeta result..."
			mv CPU_$i.log result/$avaliableScheduler-CPU-$CPU-_$i.log
		done
		
		cd ../gantt-gplot
		for ((j=0; j<$CPU; j++)); do
			echo "Generando gráfica para $avaliableScheduler-CPU-$CPU-_$j.log..."
			./generate_gantt_chart ../schedsim/result/$avaliableScheduler-CPU-$CPU-_$j.log
		done
		cd ../schedsim
	done
done