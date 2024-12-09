#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define N 100000000
#define INI 1500
#define MAX_THREADS 256

int p[N/10];
int i, nn, pp;
int numThreads;
//pthread_mutex_t lock;

void *find_primes_in_range(void *arg)
{
  int thread_id = *(int *)arg;
  int block_size = (N/10 + numThreads - 1) / numThreads;
  int start;
  if (thread_id == 0)
    start = INI + thread_id * block_size;
  else
    start = thread_id * block_size;

  int end = (thread_id == numThreads - 1) ? nn : start + block_size;

  
  int newPPindex = thread_id * block_size;
  int local_pp = 0;
  for(int num = start;num<end;num+=2)
  {
    int div = 0; // No divisible
    for (i=1; p[i]*p[i] <= num && !div;i++)
      div = div || !(num % p[i]);
    if (!div){
      if (newPPindex < (thread_id + 1) * block_size) {
        p[newPPindex++] = num;
        local_pp++;
      }
    }
  }
  // Suma de los primos encontrados por este hilo
  __sync_fetch_and_add(&pp, local_pp);
  
  pthread_exit(NULL);
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main(int na,char* arg[])
{
pthread_t threads[MAX_THREADS];
int args[MAX_THREADS];
int num, result;


if (na != 3) assert("![*] Parametros -> <Numero de datos> <Numero de threads>" == 0);
nn = atoi(arg[1]);
if (nn > N) assert("![*] Numero de datos demasiado grande -> Max 100M" == 0);
numThreads = atoi(arg[2]);
if (numThreads > MAX_THREADS || 256 % numThreads != 0 || numThreads % 2 != 0) assert("![*] Numero de threads demasiado grande -> Max 256" == 0);
//assert(nn<=N);
//pthread_mutex_init(&lock, NULL);

printf("Tots els primers fins a %d\n",nn);

p[0] = 2;
p[1] = 3;
pp = 2;
num = 5;

while (pp < INI)
{
 for (i=1; p[i]*p[i] <= num ;i++)
  if (num % p[i] == 0) break;
  if (p[i]*p[i] > num) p[pp++]=num;
  num += 2;
}

 /* Crea todos los hilos, uno a uno */
for (i = 0; i < numThreads; i++) {
	printf("MAIN: Creando hilo %d.\n", i);
	args[i] = i;
	result = pthread_create(&threads[i], NULL, find_primes_in_range, &args[i]);
	assert(!result);
}
printf("MAIN: todos los hilos creados.\n");

/* Espera a que cada hilo termine su trabajo */
for (i = 0; i < numThreads; i++) {
	result = pthread_join(threads[i], NULL);
	assert(!result);
	printf("MAIN: El hilo %d ha terminado.\n", i);
}
//pthread_mutex_destroy(&lock);

// Después de que todos los hilos terminen
qsort(p, pp, sizeof(int), compare);

printf("Hi ha %d primers\n",pp-1);
printf("Darrer primer trobat %d\n",p[pp-1]);
//for(i=0;i<pp;i++) printf("%d\n",p[i]);
// int maxValue = 0;
// for(i=0;i<pp;i++) {
//   maxValue++;
// }
// printf("Num values: %d\n", maxValue);
// for(i=0;i<pp;i++){
//   assert(p[i] < p[i+1]);
// }

exit(0);
}

