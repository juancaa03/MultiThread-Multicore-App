#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void execute_command(const char *command, FILE *output, int n, int threads, double *base_time) {
    struct timespec start, end;
    double elapsed_time;

    // Iniciar medición de tiempo
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Ejecutar el comando
    printf("Ejecutando: %s\n", command);
    int ret = system(command);

    // Finalizar medición de tiempo
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (ret != 0) {
        fprintf(stderr, "Error al ejecutar el comando: %s\n", command);
        return;
    }

    // Calcular tiempo en segundos
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    // Guardar tiempo base si es el primer thread (threads == 1)
    if (threads == 1) {
        *base_time = elapsed_time;
    }

    // Calcular speedup
    double speedup = *base_time / elapsed_time;

    // Escribir resultados en el archivo
    fprintf(output, "%d %d %.6f %.6f\n", n, threads, elapsed_time, speedup);
}

int main() {
    const int ns[] = {70000000, 80000000, 90000000, 100000000};
    const int threads[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
    const int num_ns = sizeof(ns) / sizeof(ns[0]);
    const int num_threads = sizeof(threads) / sizeof(threads[0]);
    double base_time = 0.0;

    FILE *output = fopen("resultados.txt", "w");
    if (!output) {
        perror("Error al abrir el archivo de salida");
        return 1;
    }

    fprintf(output, "N Threads Tiempo(segundos) Speedup\n");

    // Ejecutar comandos
    for (int i = 0; i < num_ns; i++) {
        for (int j = 0; j < num_threads; j++) {
            char command[128];
            snprintf(command, sizeof(command), "./p %d %d", ns[i], threads[j]);
            execute_command(command, output, ns[i], threads[j], &base_time);
        }
    }

    fclose(output);
    return 0;
}
