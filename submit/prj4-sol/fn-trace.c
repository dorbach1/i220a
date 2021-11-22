#include "fn-trace.h"
#include "x86-64_lde.h"

#include "memalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum { INIT_SIZE = 2 };

struct FnsDataImpl {
  FnInfo *arr;
  int length;
  int size;
};

static inline bool is_ret(unsigned op);

/*Initialized empty FnsData structure*/ 
FnsData *initFnsData(){
	FnInfo *fnArr = (FnInfo *) mallocChk(20 * sizeof(FnInfo));
	FnsData *fnArray = (FnsData *) mallocChk(sizeof(FnsData));
	if(!fnArray) return NULL;
	fnArray->arr = fnArr;
	fnArray->length = 0;
	fnArray->size = 20; 
	return fnArray;   
}

//Add element to FnsData
void addFnInfo(FnsData *fnArray, FnInfo *fnInfo){
	if(fnArray->length == fnArray->size){
		FnInfo *fnArr = (FnInfo *) reallocChk(fnArray->arr, fnArray->size * 2 * sizeof(FnInfo));
		fnArray->arr = fnArr;
		fnArray->size = fnArray->size * 2;
	}

	//TODO: Figure out how to index with a pointer and finish method
	fnArray->arr[fnArray->length] = *fnInfo;
	fnArray->length++;
}


//Generate Function info

//Check if the function has been seen yet in FnData
int functionSeen(void); 

//Increment the amount of in calls in a funciton
static inline void incrementInCount(*FnInfo fn){
	fn->nInCalls ++;
};

static inline void incrementOutCalls(*FnInfo fn){
	fn->nOutCalls ++; 
};

//Recursive method
void recursiveFnData(FnsData * fnArray, const unsigned char * function);



















/** Return pointer to opaque data structure containing collection of
 *  FnInfo's for functions which are callable directly or indirectly
 *  from the function whose address is rootFn.
 */
const FnsData *
new_fns_data(void *rootFn)
{
  	//Initializing pointer to instructions, datastorage, and decoder
	Lde *decode = new_lde();
	const unsigned char *p = (const unsigned char *)rootFn;
	FnsData *fnArray = initFnsData();


	while(!is_ret(*p)){
		int length = get_op_length(decode, p);
		
		long l = (long) length; 
		long op_mask = ~( -(1l<<(l * 8)) );
  
		printf("*rootFn: %lx\n",(* (long *)p) &  op_mask ); //printing out the operation
        	printf("*rootFn length: %x\n", length);
        	printf("is ret? %d \n", is_ret(*p));

		p += length; 
	}

	return NULL;
}





/** Free all resources occupied by fnsData. fnsData must have been
 *  returned by new_fns_data().  It is not ok to use to fnsData after
 *  this call.
 */
void
free_fns_data(FnsData *fnsData)
{
  //TODO
}




/** Iterate over all FnInfo's in fnsData.  Make initial call with NULL
 *  lastFnInfo.  Keep calling with return value as lastFnInfo, until
 *  next_fn_info() returns NULL.  Values must be returned sorted in
 *  increasing order by fnInfoP->address.
 *
 *  Sample iteration:
 *
 *  for (FnInfo *fnInfoP = next_fn_info(fnsData, NULL); fnInfoP != NULL;
 *       fnInfoP = next_fn_info(fnsData, fnInfoP)) {
 *    //body of iteration
 *  }
 *
 */
const FnInfo *
next_fn_info(const FnsData *fnsData, const FnInfo *lastFnInfo)
{
  //TODO
  return NULL;
}






/** recognized opcodes for calls and returns */
enum {
  CALL_OP = 0xE8,    //used to identify an external call which is traced

  //used to recognize the end of a function
  RET_FAR_OP = 0xCB,
  RET_FAR_WITH_POP_OP = 0xCA,
  RET_NEAR_OP = 0xC3,
  RET_NEAR_WITH_POP_OP = 0xC2
};

static inline bool is_call(unsigned op) { return op == CALL_OP; }
static inline bool is_ret(unsigned op) {
  return
    op == RET_NEAR_OP || op == RET_NEAR_WITH_POP_OP ||
    op == RET_FAR_OP || op == RET_FAR_WITH_POP_OP;
}
