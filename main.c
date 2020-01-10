#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t full;
sem_t empty;
int in;
int out;

int insert_item(buffer_item item);
int remove_item(buffer_item* item);

void* producer (void* param);
void* consumer (void* param);

int main(int argc, char* argv[]){

	if(argc!=4){
		printf("usage: pc <length of run in seconds> <number of producers> <number of consumers>\n");
		exit(1);
	}

	//get cmd line args
	int how_long=atoi(argv[1]);
	int num_prods=atoi(argv[2]);
	int num_cons=atoi(argv[3]);

	//init buffer & locks
	in=0;
	out=0;
	sem_init(&full,0,0);
	sem_init(&empty,0,BUFFER_SIZE);
	pthread_mutex_init(&mutex,NULL);
	int i;
	for(i=0;i<BUFFER_SIZE;i++){
		buffer[i]=-1;
	}

	//create producer threads
	pthread_t* producers=malloc(sizeof(pthread_t)*num_prods);
	for (i=0;i<num_prods;i++){
		pthread_create(&producers[i],NULL,producer,NULL);
	}

	//create consumer threads
	pthread_t* consumers=malloc(sizeof(pthread_t)*num_cons);
	for (i=0;i<num_cons;i++){
		pthread_create(&consumers[i],NULL,consumer,NULL);
	}

	//sleep as specified
	sleep(how_long);

	//exit
	return 0;

}

void print_buffer(){
	int i;
	printf("\t BUFFER:");fflush(stdout);
	for (i=0;i<BUFFER_SIZE;i++){
		printf(" %10d",buffer[i]);fflush(stdout);
	}
	printf("\n");fflush(stdout);fflush(stdout);
}




void* producer (void* param){

	buffer_item item;

	while(true){
		//sleep for random time
		sleep(rand()%10);
		//generate item
		item=rand();
		if(insert_item(item)){
			//error out
			printf("error, can't insert item\n");fflush(stdout);
			exit(1);
		}else{
			printf("producer produced %d\n",(int)item);fflush(stdout);
		}
	}
}



void* consumer(void* param){

buffer_item item;

	while(true){
		//sleep for random time
		sleep(rand()%10);
		if(remove_item(&item)){
			//error out
			printf("error, can't remove item\n");fflush(stdout);
			exit(1);
		}else{
			printf("consumer consumed %d\n",item);fflush(stdout);
		}
	}

}

int insert_item(buffer_item item){
		//wait for space
		sem_wait(&empty);
		//aquire lock
		pthread_mutex_lock(&mutex);
		//start crit section

		buffer[in]=item;
		in=(in+1)%BUFFER_SIZE;
		print_buffer();
		//end crit section

		//release lock
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
		return 0;
}

int remove_item(buffer_item* item){
		//wait for something to remove
		sem_wait(&full);
		//aquire lock
		pthread_mutex_lock(&mutex);
		//start crit section

		*item = buffer[out];
		buffer[out]=-1;
		out=(out+1)%BUFFER_SIZE;
		print_buffer();
		//end crit section

		//release lock
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		return 0;
}