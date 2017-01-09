#include "Lanton.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* We use siphash to calculate */
uint64_t siphash24(const char *in, unsigned long inlen, const char k[16]);
char hash_key[16] = { 0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf };

static uint64_t __default_hash_fun(const char *object, unsigned long len)
{
	return siphash24(object, len, hash_key);
}

static void __generate_population(Lanton_t *lanton)
{
	if(lanton->backend_num == 0)
	{
		return;
	}
	int64_t backend_id;
	for(uint32_t i = 0; i < lanton->backend_num; i++)
	{
		backend_id = lanton->backend_list[i];
		uint64_t offset = __default_hash_fun((const char *)&backend_id, sizeof(int64_t))
								% (lanton->lookuptb_size);
		uint64_t skip = (__default_hash_fun((const char *)&backend_id, sizeof(int64_t)))
								% (lanton->lookuptb_size - 1) + 1;

		for(uint32_t j = 0; j < lanton->lookuptb_size; j++)
		{
			lanton->permutation[j][i] = (offset + j * skip) % lanton->lookuptb_size;
		}
	}
}


static void __populate(Lanton_t *lanton)
{
	if (lanton->backend_num == 0)
	{
		return;
	}

	uint32_t i, j, n = 0;
	uint32_t next[MAX_BACKENDS_NUMBER];
	int32_t entry[MAX_LOOKUPTABLE_SIZE];

	memset(next, 0, sizeof(next));
	for(j = 0; j < lanton->lookuptb_size; j++)
	{
		entry[j] = -1;
	}

	while(1)
	{
		for(i = 0; i < lanton->backend_num; i++)
		{
			uint32_t c = lanton->permutation[i][next[i]];
			while(entry[c] >= 0)
			{
				next[i] = next[i] + 1;
				c = lanton->permutation[i][next[i]];
			}
			entry[c] = (int32_t)i;
			next[i] = next[i] + 1;
			n++;

			if(n == lanton->lookuptb_size)
			{
				for(uint32_t k = 0; k < lanton->lookuptb_size; k++)
				{
					lanton->lookup[k] = entry[k];
				}
				return;
			}
		}
	}
}


Lanton_t *
new_lanton(int64_t backends[], uint32_t backends_num, uint32_t lookup_table_size)
{
	assert(backends != NULL);
	assert(backends_num <= MAX_BACKENDS_NUMBER);
	assert(lookup_table_size <= MAX_LOOKUPTABLE_SIZE);

	Lanton_t *lanton = (Lanton_t *)calloc(1, sizeof(Lanton_t));
	if (lanton == NULL)
		return NULL;

	lanton->backend_num = backends_num;
	lanton->lookuptb_size = lookup_table_size;
	for(uint32_t i = 0; i < backends_num; i ++)
	{
		assert(backends[i] >= 0);
		lanton->backend_list[i] = backends[i];
	}

	__generate_population(lanton);
	__populate(lanton);
	return lanton;
}


void
destroy_Lanton(Lanton_t *lanton)
{
	assert(lanton != NULL);
	free(lanton);
}

int
add_backend(Lanton_t *lanton, int64_t backend_id)
{
	assert(lanton != NULL);
	assert(backend_id > 0);
	for(uint32_t i = 0; i < lanton->backend_num; i++)
	{
		if(lanton->backend_list[i] == backend_id)
		{
			//already exist
			return -1;
		}
	}

	lanton->backend_list[lanton->backend_num] = backend_id;
	lanton->backend_num++;
	assert(lanton->backend_num < MAX_BACKENDS_NUMBER);
	__generate_population(lanton);
	__populate(lanton);
	return 0;
}


int 
remove_backend(Lanton_t *lanton, int64_t backend_id)
{
	assert(lanton != NULL);
	assert(backend_id > 0);

	int not_found = 1;
	for (uint32_t i = 0; i < lanton->backend_num; i++)
	{
		if (lanton->backend_list[i] == backend_id)
		{
			not_found = 0;
		}
	}

	if (not_found)
	{
		return -1;
	}

	/* How to solve copy of array */
	uint32_t index = 0;
	for (index = 0; index < lanton->backend_num; index++)
	{
		if (lanton->backend_list[index] == backend_id)
		{
			break;
		}
	}
	//move following backends
	for (unsigned i = index; i < lanton->backend_num - 1; i++)
	{
		lanton->backend_list[i] = lanton->backend_list[i+1];
	}

	lanton->backend_num--;
	__generate_population(lanton);
	__populate(lanton);
	return 0;
}


int32_t
lookup_backend(Lanton_t *lanton, void *object, unsigned long obj_len)
{
	assert(lanton != NULL);
	if(lanton->backend_num == 0)
	{
		return -1;
	}

	uint32_t hashcode = __default_hash_fun((const char *)object, obj_len);
	return lanton->backend_list[lanton->lookup[hashcode % lanton->lookuptb_size]];
}
