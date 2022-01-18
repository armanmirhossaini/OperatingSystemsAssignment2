#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_SIZE 12
int end;
int p,q;
char buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_cond_t empty,full;

typedef struct str_thdata{
    int thread_no;
    int partial_count;
} thdata;

void producer_func(FILE *fp){

    char s1;
	    
    s1 = fgetc(fp);
    while(1){
        pthread_mutex_lock(&mutex);
        buffer[p] = s1;
        p = (p+1)%BUFFER_SIZE;
	if((p-1)%BUFFER_SIZE == q){
		pthread_cond_signal(&empty);
	}
	if(p == q){
		
		pthread_cond_wait(&full, &mutex);
	}
	pthread_mutex_unlock(&mutex);
	
	s1=fgetc(fp);
        if(feof(fp)){
            pthread_mutex_lock(&mutex);
            end = 1;
            pthread_cond_signal(&empty);
            pthread_mutex_unlock(&mutex);
	    break;
	}
    }
}

void consumer_func(void *ptr){
    while(1){
        pthread_mutex_lock(&mutex);
        if(p == q){
            if(end) break;
            else{
		pthread_cond_wait(&empty, &mutex);
		if(p == q && end) break;
	    }
        }

	printf("%c",buffer[q]);
        q = (q+1)%BUFFER_SIZE;
	if(p == (q-1)%BUFFER_SIZE){
		pthread_cond_signal(&full);
	}
	pthread_mutex_unlock(&mutex);
     }
     pthread_exit(NULL);
 }


int main(int argc, char *argv[]){
    pthread_t producer, consumer;  
    FILE *fp;

    p = 0;
    q = 0;
    

	if((fp=fopen("messages.txt", "r"))==NULL){
		printf("ERROR: can't open string.txt!\n");
		return -1;
	}
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&empty, NULL);
    pthread_cond_init (&full, NULL);

    end = 0;


    pthread_create (&producer, NULL, (void *) &producer_func, (FILE *) fp);
    pthread_create (&consumer, NULL, (void *) &consumer_func, NULL);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);
    
	return 1;
}
