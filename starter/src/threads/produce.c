// Use this to see if a number has an integer square root
#define EPS 1.E-7

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

double g_time[2];
int num;
int maxmsg;
int num_p;
int num_c;
int *buf;
int buf_size;
sem_t items;
sem_t free_spaces;
pthread_mutex_t mutex;

void *producer(void *arg);
void *consumer(void *arg);
void buf_send(int i);
int buf_receive();
void busy_loop(int iters);

int main(int argc, char *argv[])
{
	int i;
	struct timeval tv;

	if (argc != 5)
	{
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		return 1;
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

	/* Initialize mutex & semaphores */
	pthread_mutex_init(&mutex, NULL);
	sem_init(&items, 0, 0);
	sem_init(&free_spaces, 0, maxmsg);

	/* Start time */
	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec / 1000000.;

	/* Open queue of size maxmsg */
	buf = malloc(sizeof(int) * maxmsg);

	/* Initialize threads */
	pthread_t producers[num_p];
	pthread_t consumers[num_c];

	/* Spawn producers */
	for (int i = 0; i < num_p; ++i)
	{
		int *arg = malloc(sizeof(int));
		*arg = i;
		pthread_create(&producers[i], NULL, producer, arg);
	}

	/* Spawn consumers */
	for (int i = 0; i < num_c; ++i)
	{
		int *arg = malloc(sizeof(int));
		*arg = i;
		pthread_create(&consumers[i], NULL, consumer, arg);
	}

	/* Wait for all producers to finish */
	for (int i = 0; i < num_p; ++i)
	{
		pthread_join(producers[i], NULL);
	}

	/* Send killsig */
	for (i = 0; i < num_c; ++i)
	{
		buf_send(-1);
	}

	/* Wait for all consumers to finish */
	for (int i = 0; i < num_c; ++i)
	{
		pthread_join(consumers[i], NULL);
	}

	/* Close queue */
	free(buf);

	/* Cleanup */
	pthread_mutex_destroy(&mutex);
	sem_destroy(&items);
	sem_destroy(&free_spaces);

	/* Stop time */
	gettimeofday(&tv, NULL);
	g_time[1] = (tv.tv_sec) + tv.tv_usec / 1000000.;

	printf("System execution time: %.6lf seconds\n",
		   g_time[1] - g_time[0]);
	return 0;
}

/* Produce numbers as its task */
void *producer(void *arg)
{
	int id = *(int *)arg;
	/* Produce numbers i where i % P = id */
	for (int i = id % num_p; i < num; i += num_p)
	{
		buf_send(i);
	}
	free(arg);
	pthread_exit(0);
}

/* Remove numbers from the buffer and check if it has an integer square root, if so print to the terminal */
void *consumer(void *arg)
{
	int id = *(int *)arg;
	int msg;
	while (1)
	{
		/* Get signal from the message queue */
		msg = buf_receive();
		if (msg < 0) /* killsig received */
		{
			break;
		}
		else
		{
			/* Check for square root */
			// busy_loop(50000);
			int root = sqrt((double)msg);
			if ((double)msg - root * root < EPS)
			{
				printf("%d %d %d\n", id, msg, root);
			}
		}
	}
	free(arg);
	pthread_exit(0);
}

void buf_send(int i)
{
	/* Send item into the buffer */
	sem_wait(&free_spaces);
	pthread_mutex_lock(&mutex);
	buf[buf_size] = i;
	++buf_size;
	pthread_mutex_unlock(&mutex);
	sem_post(&items);
	return;
}

int buf_receive()
{
	/* Receive item from the buffer */
	sem_wait(&items);
	pthread_mutex_lock(&mutex);
	int i = buf[buf_size - 1];
	--buf_size;
	pthread_mutex_unlock(&mutex);
	sem_post(&free_spaces);
	return i;
}

void busy_loop(int iters)
{
  volatile int sink;
  do
  {
    sink = 0;
  } while (--iters > 0);
  (void)sink;
}
