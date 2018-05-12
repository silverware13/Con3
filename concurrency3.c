/* Concurrency 3
 * CS444 Spring2018
 * ----------------
 * Name: Zachary Thomas
 * Email: thomasza@oregonstate.edu
 * Date: 5/11/2018
 * -------------------------------
 */

#include "mt19937ar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

//function prototype(s)
void spawn_threads(int insert, int search, int delete)
void* search_thread();
void* insert_thread();
void* delete_thread();
int random_range(int min_val, int max_val);

//create mutex lock(s)
pthread_mutex_t insert_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t search_lock  = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
	//first make sure the user entered numbers for
	//insert, search and delete threads
	if(argc < 4){
		printf("USEAGE: %s [number of inserter threads] [number of searcher threads] [number of deleter threads]\n", argv[0]);
		return 0;
	}		

	//make sure the user has entered digits
	if(!isdigit(*argv[1]) || !isdigit(*argv[2]) || !isdigit(*argv[3])){
		printf("Please enter arguments as unsigned integers.\n");
		return 0;
	}

	//convert arguments to int
	int insert, search, delete;
	insert = strtol(argv[1], NULL, 10);	
	search = strtol(argv[2], NULL, 10);
	delete = strtol(argv[3], NULL, 10);

	//seed random number generation
	init_genrand(time(NULL));
	srand(time(NULL));	

	//create threads and wait for their completion
	spawn_threads(insert, search, delete);
	
	//destroy mutex lock(s)	
	pthread_mutex_destroy(&insert_lock);
	pthread_mutex_destroy(&search_lock);
	
	return 0;
}

/* Function: spawn_threads
 * -----------------------
 * Spawns inserter, searcher, and deleter threads, then waits for threads to
 * finish execution and join. Since these threads will run forever, we expect
 * to block here indefinitely.
 */
void spawn_threads(int insert, int search, int delete)
{
	int some_threads;
	pthread_t thrd;
	
	if(!producers && !consumers)
		some_threads = false;
	else
		some_threads = true;

	printf("\nCreating %d inserter, %d searcher, and %d deleter threads.\n\n", insert, search, delete);
	
	while(insert || search || delete){
		if(insert){
			pthread_create(&thrd, NULL, insert_thread, NULL);
			producers--;
		}

		if(search){
			pthread_create(&thrd, NULL, search_thread, NULL);
			consumers--;
		}
		
		if(delete){
			pthread_create(&thrd, NULL, delete_thread, NULL);
			consumers--;
		}
	}

	//join thread (this should never finish)
	if(some_threads)
		pthread_join(thrd, NULL);
}
/* Function: search_thread
 * -------------------------
 * This function is called by a new search thread when it is created.
 *
 */
void* search_thread()
{
	int *select_ptr = (int*) thread_num; 
	int select = *select_ptr;
	char name[50];
	get_name(select, name);
	printf("%s sits down at the table.\n", name);
	sleep(1);

	while(true){
		think(name);
		get_forks(select, name);
		eat(name);
		put_forks(select, name);
	}

	free(thread_num);
}

/* Function: insert_thread
 * -------------------------
 * This function is called by a new insert thread when it is created.
 *
 */
void* insert_thread()
{
	int *select_ptr = (int*) thread_num; 
	int select = *select_ptr;
	char name[50];
	get_name(select, name);
	printf("%s sits down at the table.\n", name);
	sleep(1);

	while(true){
		think(name);
		get_forks(select, name);
		eat(name);
		put_forks(select, name);
	}

	free(thread_num);
}

/* Function: delete_thread
 * -------------------------
 * This function is called by a new delete thread when it is created.
 * 
 */
void* delete_thread()
{
	int *select_ptr = (int*) thread_num; 
	int select = *select_ptr;
	char name[50];
	get_name(select, name);
	printf("%s sits down at the table.\n", name);
	sleep(1);

	while(true){
		think(name);
		get_forks(select, name);
		eat(name);
		put_forks(select, name);
	}

	free(thread_num);
}

/* Function: random_range
 * ----------------------
 * This function finds a random number between a min and max value (inclusive).
 * The random value is created using rdrand x86 ASM on systems that support it,
 * and it uses Mersenne Twister on systems that do not support rdrand.
 *
 * min_val: The lowest possible random number.
 * max_val: The highest possible random number.
 *
 * returns: A random number in the given range. In the case that min_val is
 *          greater than max_val this function returns -1.
 */
int random_range(int min_val, int max_val)
{
	if(min_val > max_val)
		return -1;

	int output;
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;

	char vendor[13];
	
	eax = 0x01;

	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	if(ecx & 0x40000000){
		//use rdrand
	__asm__ __volatile__(
	                     "rdrand %0"
                             : "=r"(output)
	                     );
	} else {
		//use mt19937
		output = genrand_int32();
	}

	//get random number in the range requested 
	output = (abs(output) % (max_val + 1 - min_val)) + min_val;
	return output;
}
