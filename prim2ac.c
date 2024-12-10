#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define N 100000000
#define INI 1500
#define MAX_THREADS 256

int p[N / 10]; // Array para almacenar los números primos
int pp = 2;    // Contador global de primos
int nn;        // Límite superior para calcular primos
int numThreads; // Número de hilos

typedef struct {
  int *local_primes; // Array local de primos por hilo
  int local_count;   // Número de primos encontrados por hilo
} ThreadData;

ThreadData thread_data[MAX_THREADS]; // Datos de cada hilo

// Función para calcular los primos en el rango asignado a cada hilo
void *prim2ac(void *arg) {
  int thread_id = *(int *)arg;

  // Calcular el rango del hilo
  int block_size = (nn - INI + numThreads - 1) / numThreads;
  int start = INI + thread_id * block_size;
  int end = (thread_id == numThreads - 1) ? nn : start + block_size;

  // Asegurar que los hilos empiecen desde números impares
  if (start % 2 == 0)
    start++;

  // Buffer local para almacenar primos encontrados
  thread_data[thread_id].local_primes = (int *)malloc((end - start) / 2 * sizeof(int));
  if (!thread_data[thread_id].local_primes) {
    fprintf(stderr, "Error al asignar memoria para los primos locales del hilo %d\n", thread_id);
    pthread_exit(NULL);
  }
  thread_data[thread_id].local_count = 0;

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
      thread_data[thread_id].local_primes[thread_data[thread_id].local_count++] = num;
    }
  }

  pthread_exit(NULL);
}

// Función para eliminar duplicados de un array ordenado
void remove_duplicates(int *arr, int *size) {
  int i, j = 0;
  for (i = 1; i < *size; i++) {
    if (arr[i] != arr[j]) {
      j++;
      arr[j] = arr[i];
    }
  }
  *size = j + 1;
}

// Ordenar el array global `p[]`
int compare_ints(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Uso: %s <número de datos> <número de hilos>\n", argv[0]);
    exit(1);
  }

  nn = atoi(argv[1]);
  if (nn > N) {
    fprintf(stderr, "Error: Número de datos demasiado grande (max %d)\n", N);
    exit(1);
  }
  numThreads = atoi(argv[2]);
  if (numThreads > MAX_THREADS || (numThreads != 1 && numThreads != 2 && numThreads != 4 && numThreads != 8 && numThreads != 16 && numThreads != 32 && numThreads != 64 && numThreads != 128 && numThreads != 256)) {
    fprintf(stderr, "Error: Número de hilos incorrecto, debe ser: 1, 2, 4, 8, 16, 32, 64, 128 o 256\n");
    exit(1);
  }

  // Inicializar los primeros primos
  p[0] = 2;
  p[1] = 3;

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

  // Fusionar resultados locales en el array global `p`
  for (int i = 0; i < numThreads; i++) {
    for (int j = 0; j < thread_data[i].local_count; j++) {
      p[pp++] = thread_data[i].local_primes[j];
    }
    free(thread_data[i].local_primes); // Liberar memoria local
  }

  qsort(p, pp, sizeof(int), compare_ints);

  // Eliminar duplicados
  remove_duplicates(p, &pp);

  // Mostrar resultados
  printf("Hi ha %d primers\n", pp-1);
  printf("Darrer primer trobat %d\n", p[pp - 1]);

  return 0;
}
