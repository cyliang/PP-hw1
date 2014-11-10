#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef PSEUDO_RAND
#include <time.h>
#endif

#ifndef THREAD_COUNT
#include <sys/sysinfo.h>
#endif

/* number_of_tosses store 1/4 the number to toss */
long long int number_of_tosses;

void *compute_thread(void *arg) {
	long long int *result = (long long int *) arg;
	long long int in_circle = 0;
	long long int toss;

	/* Use thread-safe random number generator */
	struct drand48_data drand_buf;
#ifndef PSEUDO_RAND
	srand48_r(time(NULL) ^ (int) arg, &drand_buf);
#else
	srand48_r(*(int *) arg, &drand_buf);
#endif

	for(toss = 0; toss < number_of_tosses; toss++) {
		double x, y;
		/* Generate x & y that uniformly distribute in [0, 1)
		 * Because "x in [-1, 1]" and "x in [0, 1)" have the same distribution of "x * x" */
		drand48_r(&drand_buf, &x);
		drand48_r(&drand_buf, &y);

		if(x * x + y * y <= 1)
			in_circle++;
	}

	*result = in_circle;
	return NULL;
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s number_of_tosses\n", argv[0]);
		return 1;
	}
	
#ifdef THREAD_COUNT
	const int thread_count = THREAD_COUNT;
#else
	const int thread_count = get_nprocs();
#endif

	int	i;
	long long int number_in_circle[thread_count];
	long long int total_number_in_circle = 0;
	pthread_t thread_id[thread_count];

	number_of_tosses = atoll(argv[1]) / thread_count;

	for(i=0; i<thread_count; i++) {
#ifdef PSEUDO_RAND
		/* Used to give this thread a constant value 
		 * as the seed of the random number generator */
		number_in_circle[i] = i;
#endif
		pthread_create(
			&thread_id[i],
			NULL,
			&compute_thread,
			(void *) &number_in_circle[i]
		);
	}

	for(i=0; i<thread_count; i++) {
		pthread_join(thread_id[i], NULL);
		total_number_in_circle += number_in_circle[i];
	}

	printf("Estimated PI: %lf\n", 4 * total_number_in_circle / ((double) number_of_tosses * thread_count));
	
	return 0;
}
