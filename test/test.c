#include "Lanton.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define CONSISTENT_HASH   //hash mode

#define MAX_THREAD_NUM 128
#define MAX_OBJECT_NUM 0xFFFFFFFFFFFFFFFF

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int64_t multi_queue[MAX_THREAD_NUM] = {0};

struct Five_tuple
{
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint16_t srcport;
	uint16_t dstport;
	uint8_t  protocol;
};


int32_t five_tuple_mod(void *object, unsigned long obj_len, int32_t thread_num)
{
	struct Five_tuple *five = (struct Five_tuple *)object;
	uint64_t hash_code = five->srcaddr + five->dstaddr 
		+ five->srcport + five->dstport + five->protocol;
	hash_code %= thread_num;
	return (int32_t)hash_code;
}


int32_t two_tuple_mod(void *object, unsigned long obj_len, int32_t thread_num)
{
	struct Five_tuple *five = (struct Five_tuple *)object;
	uint64_t hash_code = five->srcaddr + five->srcport; 
	hash_code %= thread_num;
	return (int32_t)hash_code;
}


void __scale_loads(Lanton_t *lanton, int32_t thread_num, FILE *fp)
{
	struct Five_tuple object;
    int64_t thread_id = -1;

	//init
	fseek(fp, 0, SEEK_SET);
	memset(multi_queue, 0, MAX_THREAD_NUM);

	while(fscanf(fp, "%u %u %d %d %d\n", &object.srcaddr, &object.dstaddr, 
		(int *)&object.srcport, (int *)&object.dstport, (int *)&object.protocol) != EOF)
	{
#if defined (CONSISTENT_HASH)
		thread_id = lookup_backend(lanton, (void *)&object, 13);
#elif defined (FIVE_TUPLE_HASH)
		thread_id = five_tuple_mod((void *)&object, 13, thread_num);
#elif defined (TWO_TUPLE_HASH)
		thread_id = two_tuple_mod((void *)&object, 13, thread_num);
#else
        printf("config a default hash func!!\n");
        break;
#endif
		assert(thread_id >= 0);
		multi_queue[thread_id] ++;
	}
}


static void __print_load(int64_t thread_num)
{
	int64_t min_load = INT64_MAX;
	int64_t max_load = INT64_MIN;
	for (int64_t i = 0; i < thread_num; i++)
	{
		printf("%lld\n", multi_queue[i]);
		min_load = min(min_load, multi_queue[i]);
		max_load = max(max_load, multi_queue[i]);
	}
	//printf("min_load = %lld, max_load = %lld\n", min_load, max_load);
}

int main(int argc, char* argv[])
{
	int64_t backends[MAX_THREAD_NUM];
	int64_t thread_num;
	char *file_path = NULL;
	FILE *fp = NULL;

	if (argc != 3)
	{
		printf("usage ./test [Thread_num] [INPUT_FILE]\n");
		exit(-1);
	}

	thread_num = atoll(argv[1]);
	file_path = argv[2];
	assert(thread_num >= 0 && thread_num <= MAX_THREAD_NUM);
	assert(file_path != NULL);

	if((fp = fopen(file_path, "r")) == NULL)
	{
		perror("fopen");
		exit(-1);
	}

	//backend 0 ~ MAX_THREAD_NUM-1
	for(int64_t i = 0; i < thread_num; i++)
	{
		backends[i] = i;
	}

	Lanton_t *lanton = new_lanton(backends, thread_num, DEFAULT_LOOKUPTABLE_SIZE);
	
	__scale_loads(lanton, thread_num, fp);
	__print_load(thread_num);

//	remove_backend(lanton, 5);
//
//	printf("after remove:\n");
//	__scale_loads(lanton, fp);
//	__print_load(thread_num);

	fclose(fp);
	return 0;
}
