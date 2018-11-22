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

double g_time[2];
int KILLSIG = -1;
const char* QUEUE_NAME = "/lab3_queue";

int spawn_producer(int pid, int num, int num_p, int buffer_size);
int spawn_consumer(int cid, int buffer_size);
void producer_task(int id, int int_num, int num_p, int buffer_size);
void consumer_task(int cid, int buffer_size);

int main(int argc, char *argv[])
{
	int num;
	int maxmsg;
	int num_p;
	int num_c;
	int i;
	struct timeval tv;

	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce */
	maxmsg = atoi(argv[2]); /* buffer size                */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

    //get start time
	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	struct mq_attr attribute;
    mqd_t qdes;    
    attribute.mq_maxmsg = maxmsg;
    attribute.mq_msgsize = sizeof(int);
    attribute.mq_flags = 0;
    attribute.mq_curmsgs = 0;

    
    qdes = mq_open(QUEUE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attribute);
    if (qdes == -1 ) {
        perror("main: mq_open() failed");
        exit(1);
    }
	pid_t* producer_id = malloc(num_p * sizeof( pid_t ));
	pid_t* consumer_id = malloc(num_c * sizeof( pid_t ));

    for(i = 0; i< num_p; i++){
    	producer_id[i] = fork();
    	if(	producer_id[i] == 0){
    		producer_task(i, num, num_p, maxmsg);
    		exit(0);
    	}
    }
    for(i = 0; i< num_c; i++){
    	consumer_id[i] = fork();
    	if(consumer_id[i] == 0){
    		consumer_task(i, maxmsg);
    		exit(0);
    	}
    	
    }
    int child_status;
    for(i = 0; i< num_p; i++){
        waitpid(producer_id[i], &child_status, 0);
    }
    for(i = 0; i < num_c; ++i) {
		if(mq_send(qdes, (char *)&KILLSIG, sizeof(int), 0) == -1) {
			perror("Main Program: Terminate Consumer failed.");
			exit(1);
		}
	}    
    for(i = 0; i< num_c; i++){
        waitpid(consumer_id[i], &child_status, 0);
    }
    if(mq_close(qdes) == -1) {
        perror("main : mq_close() failed");
        exit(1);
    }
    if(mq_unlink(QUEUE_NAME) != 0){
    	perror("main: mq_unlink() failed");
        exit(1);
    }
    
    free(producer_id);
    free(consumer_id);
    
    //get end time
    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
    return 0;
}
void producer_task(int pid, int int_num, int num_p, int buffer_size){
	int i;
	mqd_t msg_queue = mq_open(QUEUE_NAME, O_WRONLY);
	if (msg_queue == -1 ) {
		perror("producer: mq_open() failed");
		exit(1);
	}
    for(i = 0; i < int_num; i++){
        if(i % num_p == pid){
        	if (mq_send(msg_queue, (char *)&i, sizeof(int), 0) == -1) {
            	perror("producer: mq_send() failed");
	        }
        }
    }
    if (mq_close(msg_queue) == -1) {
        perror("producer: mq_close() failed");
        exit(2);
    }
    exit(0);
}
void consumer_task(int cid, int buffer_size){
	mqd_t msg_queue = mq_open(QUEUE_NAME, O_RDONLY);
    if (msg_queue == -1 ) {
        perror("consumer: mq_open() failed");
        exit(1);
    }
    
    int msg = 0;
    
    while (msg != -1) {
        if (mq_receive(msg_queue, (char *)&msg, sizeof(int), 0) == -1) {
            perror("consumer: mq_receive() failed");
            exit(1);
        }
        int root = sqrt((double)msg);
        if(root == sqrt((double)msg)) {
            printf("%d %d %d\n", cid, msg, root);
        }
    }
    if (mq_close(msg_queue) == -1) {
        perror("consumer: mq_close() failed");
        exit(2);
    }
    exit(0);
    
}


