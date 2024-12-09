#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define N 100000000
#define INI 1500
#define MAX_THREADS 256

int p[N / 10];  // Array para almacenar los números primos
int pp = 2;     // Contador global de primos
int nn;         // Límite superior para calcular primos
int numThreads; // Número de hilos

pthread_mutex_t lock_pp; // Mutex para sincronizar el acceso a `p[]`

// Función para calcular los primos en el rango asignado a cada hilo
void *prim2ac(void *arg) {
    int thread_id = *(int *)arg;

    // Calcular el rango del hilo
    int block_size = (nn - INI + numThreads - 1) / numThreads;
    int start = INI + thread_id * block_size;
    int end = (thread_id == numThreads - 1) ? nn : start + block_size;

    // Asegurar que los hilos empiecen desde números impares
    if (start % 2 == 0) start++;

    // Buffer local para almacenar primos encontrados
    int local_count = 0;
    int *local_primes = (int *)malloc((end - start) / 2 * sizeof(int));

    // Calcular primos en el rango asignado
    for (int num = start; num < end; num += 2) {
        int is_prime = 1; // Asumir que el número es primo
        for (int i = 0; i < pp && p[i] * p[i] <= num; i++) {
            if (num % p[i] == 0) {
                is_prime = 0; // No es primo
                break;
            }
        }
        if (is_prime) {
            local_primes[local_count++] = num;
        }
    }

    // Copiar resultados locales al array global `p[]`
    pthread_mutex_lock(&lock_pp);
    for (int i = 0; i < local_count; i++) {
        p[pp++] = local_primes[i];
    }
    pthread_mutex_unlock(&lock_pp);

    free(local_primes);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <limite superior> <numero de hilos>\n", argv[0]);
        exit(1);
    }

    nn = atoi(argv[1]);
    numThreads = atoi(argv[2]);

    if (nn > N) {
        fprintf(stderr, "Error: Límite superior demasiado grande (max %d)\n", N);
        exit(1);
    }
    if (numThreads > MAX_THREADS) {
        fprintf(stderr, "Error: Número de hilos demasiado grande (max %d)\n", MAX_THREADS);
        exit(1);
    }

    // Inicializar los primeros primos
    p[0] = 2;
    p[1] = 3;
    pp = 2;

    // Llenar los primos hasta INI
    for (int num = 5; pp < INI; num += 2) {
        int is_prime = 1; // Asumir que el número es primo
        for (int i = 1; p[i] * p[i] <= num; i++) {
            if (num % p[i] == 0) {
                is_prime = 0; // No es primo
                break;
            }
        }
        if (is_prime) {
            p[pp++] = num;
        }
    }

    // Crear y lanzar hilos
    pthread_t threads[MAX_THREADS];
    int args[MAX_THREADS];
    pthread_mutex_init(&lock_pp, NULL);

    for (int i = 0; i < numThreads; i++) {
        args[i] = i;
        assert(!pthread_create(&threads[i], NULL, prim2ac, &args[i]));
        printf("Thread %d created\n", i);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < numThreads; i++) {
        assert(!pthread_join(threads[i], NULL));
        printf("Thread %d finished\n", i);
    }

    pthread_mutex_destroy(&lock_pp);

    // Mostrar resultados
    printf("Hi ha %d primers\n", pp);
    printf("Darrer primer trobat %d\n", p[pp - 1]);

    return 0;
}
