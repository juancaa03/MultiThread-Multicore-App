#!/bin/bash

# Lista de valores de N (70M, 80M, 90M, 100M)
n_values=("70000000" "80000000" "90000000" "100000000")

# Lista de números de hilos (1, 2, 4, 8, 16, 32, 64, 128, 256)
threads_values=("1" "2" "4" "8" "16" "32" "64" "128" "256")

# Archivo de resultados
output_file="resultados.txt"
# Limpiar archivo de resultados si ya existe
> $output_file

# Ejecutar los comandos para cada valor de N y calcular los tiempos
for n in "${n_values[@]}"; do
  # Inicializar un array para guardar los tiempos de cada número de hilos
  declare -A times
  
  # Ejecutar el comando para cada número de hilos y guardar el tiempo
  for t in "${threads_values[@]}"; do
    echo "Ejecutando: ./p $n $t"
    
    # Medir el tiempo usando /usr/bin/time
    exec_time=$( { time ./p $n $t > /dev/null; } 2>&1 | grep real | awk '{print $2}' )
    
    # Convertir el tiempo a segundos
    time_in_seconds=$(echo $exec_time | sed -e 's/m/ /' -e 's/s//')
    minutes=$(echo $time_in_seconds | cut -d ' ' -f 1)
    seconds=$(echo $time_in_seconds | cut -d ' ' -f 2)
    total_seconds=$(echo "$minutes * 60 + $seconds" | bc)
    
    # Guardar el tiempo en el array
    times[$t]=$total_seconds
    echo "$n $t $total_seconds" >> $output_file
  done
  
  # Ahora calcular el speedup para cada número de hilos (excepto 1)
  for t in "${threads_values[@]}"; do
    if [ "$t" -ne 1 ]; then
      # Calcular el speedup
      speedup=$(echo "${times[1]} / ${times[$t]}" | bc -l)
      
      # Agregar el speedup al archivo de resultados
      echo "Speedup para $n con $t hilos: $speedup" >> $output_file
    fi
  done
done

# Mostrar el contenido del archivo de resultados
cat $output_file
