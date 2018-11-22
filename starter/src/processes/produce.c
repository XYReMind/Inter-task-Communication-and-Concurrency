// Use this to see if a number has an integer square root
// Wrote by h536wang, with help from j67cao
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
const char* My_Queue = "/lab3_my_msg_queue";
int num;
int bufferSize;
int num_p;
int num_c;

int EXT_SIG = -1;

void producer_task(int pid);
void consumer_task(int cid);

int main(int argc, char *argv[])
{
	int i;
	struct timeval tv;

	if (argc != 5) {
		printf("Usage: %s <N> <B> <P> <C>\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[1]);	/* number of items to produce   */
	bufferSize = atoi(argv[2]); /* buffer size            */
	num_p = atoi(argv[3]);  /* number of producers        */
	num_c = atoi(argv[4]);  /* number of consumers        */

    //get start time
	gettimeofday(&tv, NULL);
	g_time[0] = (tv.tv_sec) + tv.tv_usec/1000000.;

	struct mq_attr attribute;
    mqd_t qdes;    
    attribute.mq_maxmsg = bufferSize;
    attribute.mq_msgsize = sizeof(int);
    attribute.mq_flags = 0;
    attribute.mq_curmsgs = 0;

    qdes = mq_open(My_Queue, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attribute);
	
    if (qdes == -1 ) {
        perror("mq_open() Failed! Exit!");
        exit(1);
    }
		
	pid_t* producer_id = malloc(num_p * sizeof( pid_t ));
	pid_t* consumer_id = malloc(num_c * sizeof( pid_t ));

		// fork producers
    for(i = 0; i< num_p; i++){
    	producer_id[i] = fork();
			
    	if(	producer_id[i] == 0){
    		producer_task(i);
    		exit(0);
    	}
    }
		
		// fork consumers
    for(i = 0; i< num_c; i++){
    	consumer_id[i] = fork();
			
    	if(consumer_id[i] == 0){
    		consumer_task(i);
    		exit(0);
    	}	
    }
		
    int child_process;
	
		// wait for producers to finish up
    for(i = 0; i< num_p; i++){
        waitpid(producer_id[i], &child_process, 0);
    }
		
		// terminate consumers after reading the numbers 
		// since we have a while(msg != -1) in the consumer_task function
		// we do not have to check if the Buffer is empty by ourselves
    for(i = 0; i < num_c; ++i) {
			if(mq_send(qdes, (char *)&EXT_SIG, sizeof(int), 0) == -1) {
				perror("Terminate Consumer Failed! Exit!");
				exit(1);
			}
		}    
		
		// wait for consumers to finish up
    for(i = 0; i< num_c; i++){
        waitpid(consumer_id[i], &child_process, 0);
    }
		
		// close
    if(mq_close(qdes) == -1) {
        perror("mq_close() Failed! Exit!");
        exit(1);
    }
		
		// inlink
    if(mq_unlink(My_Queue) != 0){
    	perror("mq_unlink() Failed! Exit!");
        exit(1);
    }
    
		// free memory
    free(producer_id);
    free(consumer_id);
    
    //get end time
    gettimeofday(&tv, NULL);
    g_time[1] = (tv.tv_sec) + tv.tv_usec/1000000.;

    printf("System execution time: %.6lf seconds\n", \
            g_time[1] - g_time[0]);
		
    return 0;
}

void producer_task(int pid){
	int i;
	
	// open write only
	mqd_t pro_msg_queue = mq_open(My_Queue, O_WRONLY);
	if (pro_msg_queue == -1 ) {
		perror("Pro: mq_open() Failed! Exit!");
		exit(1);
	}
	
	// generate numbers and send the numbers to the queue
	for(i = 0; i < num; i++){
			if(i % num_p == pid){
				if (mq_send(pro_msg_queue, (char *)&i, sizeof(int), 0) == -1) {
						perror("Pro: mq_send() Failed! Exit!");
				}
			}
	}
	
	// close
	if (mq_close(pro_msg_queue) == -1) {
			perror("Pro: mq_close() Failed! Exit!");
			exit(1);
	}
	
  exit(0);
}

void consumer_task(int cid){
	
	// open read only
	mqd_t con_msg_queue = mq_open(My_Queue, O_RDONLY);
	if (con_msg_queue == -1 ) {
			perror("Con: mq_open() Failed! Exit!");
			exit(1);
	}
    
	int msg = 0;
	
	// while still have msg to read, do not exit the while loop
	while (msg != -1) {
		if (mq_receive(con_msg_queue, (char *)&msg, sizeof(int), 0) == -1) {
				perror("Consumer: mq_receive() Failed! Exit!");
				exit(1);
		}
		
		int root = sqrt((double)msg);
		if(root == sqrt((double)msg)) {
				printf("%d %d %d\n", cid, msg, root);
		}
	}
	
	// finish reading and checking, close
	if (mq_close(con_msg_queue) == -1) {
			perror("Consumer: mq_close() Failed! Exit!");
			exit(2);
	}
	
	exit(0);
}


