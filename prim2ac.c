#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#define N 100000000
#define INI 1500
#define MAX_THREADS 256

int p[N/10];
int i, nn,pp;
int numThreads;
pthread_mutex_t lock;

void *find_primes_in_range(void *arg)
{
  int thread_id = *(int *)arg;
  int range_size = (nn - INI + numThreads - 1) / numThreads;//nn / numThreads;
  int start = INI + thread_id * range_size;
  int end = (thread_id == numThreads - 1) ? nn : start + range_size;

  int *local_p = malloc(sizeof(int) * (nn / numThreads));
  int local_pp = 0;

  for(int num = start;num<end;num+=2) 
  {
    int div = 0; // No divisible
    for (i=1; p[i]*p[i] <= num && !div;i++)
      div = div || !(num % p[i]);
    if (!div) local_p[local_pp++]=num;
  }

  pthread_mutex_lock(&lock);
  for (int x = 0; x < local_pp; x++) {
    if(pp < N/10) {
      p[pp++] = local_p[x];
    } else {
      printf("No se puede acceder al array");
    }
  }
  printf("PP: %d\n", pp-1);
  pthread_mutex_unlock(&lock);

  free(local_p);
  pthread_exit(NULL);
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

pthread_mutex_init(&lock, NULL);

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
printf("PP MAIN: %d\n", pp-1);


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
pthread_mutex_destroy(&lock);

printf("Hi ha %d primers\n",pp-1);
printf("Darrer primer trobat %d\n",p[pp-1]);
//for(i=0;i<pp;i++) printf("%d\n",p[i]);
exit(0);
}

