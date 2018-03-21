/* <Lanton>
 Lanton is a C implement of Google's consistent hashing algorithm used in Maglev system
 It is under support of siphash(https://github.com/majek/csiphash)
 The code is released under the MIT License

 <MIT License>
 Copyright (c) 2013  Marek Majkowski <marek@popcount.org>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 </MIT License>
*/



#ifndef __LANTON_H__
#define __LANTON_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

#define MAX_BACKENDS_NUMBER			128
#define MAX_LOOKUPTABLE_SIZE		65537
#define DEFAULT_LOOKUPTABLE_SIZE	65537

typedef struct __Lanton_s {
	uint32_t        backend_num;		                                            /* number of backends */
	uint32_t        lookuptb_size;		                                            /* size of lookup table */
	uint32_t        permutation[MAX_LOOKUPTABLE_SIZE][MAX_BACKENDS_NUMBER];
	int32_t         lookup[MAX_LOOKUPTABLE_SIZE];                                   /* store index of backend in backend_list */
	int64_t         backend_list[MAX_BACKENDS_NUMBER];                              /* backend list, in our scenario, backend
                                                                                       may be a thread, so an ID types of int64_t 
                                                                                       is used to represent a certain backend,
                                                                                       it must be unique and not smaller than 0 */
}Lanton_t;


Lanton_t *new_lanton(int64_t backends[], uint32_t backends_num, uint32_t lookup_table_size);

void destroy_Lanton(Lanton_t *lanton);

int add_backend(Lanton_t *lanton, int64_t backend_id);

int remove_backend(Lanton_t *lanton, int64_t backend_id);

//int64_t lookup_backend(Lanton_t *lanton, void *object, unsigned long obj_len);

int64_t lookup_backend(Lanton_t *lanton, uint64_t hashcode);
#ifdef __cplusplus
}
#endif

#endif

