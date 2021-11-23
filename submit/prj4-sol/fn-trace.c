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
static inline bool is_call(unsigned op);
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
FnInfo *addFnInfo(FnsData *fnArray, FnInfo fnInfo){
	if(fnArray->length == fnArray->size){
		FnInfo *fnArr = (FnInfo *) reallocChk(fnArray->arr, fnArray->size * 2 * sizeof(FnInfo));
		fnArray->arr = fnArr;
		fnArray->size = fnArray->size * 2;
	}

	//TODO: Figure out how to index with a pointer and finish method
	fnArray->arr[fnArray->length] = fnInfo;
	//printf("%p \n", fnInfo.address);
	fnArray->length++;
	return &(fnArray->arr[fnArray->length -1]);
}


//Generate Function info and add it to the FnData structure specified by fnArray
FnInfo *newFnEntry(FnsData *fnArray,unsigned char *address){
	FnInfo fnEntry = {address , 0, 1, 0};
	return addFnInfo(fnArray, fnEntry);
}

//Check if the function has been seen yet in FnData
//Return 0 if not seen or 1 if seen
FnInfo *functionSeen(FnsData *fnArray, unsigned char *address){
	for(int i = 0; i < fnArray->length ; i ++){
		if(address == fnArray->arr[i].address) return &(fnArray->arr[i]); 
	}
	return NULL; 
} 

//Increment the amount of in calls in a funciton
static inline void incrementInCalls(FnInfo *fn){
	fn->nInCalls ++;
}

static inline void incrementOutCalls(FnInfo *fn){
	fn->nOutCalls ++; 
}

static inline void incrementLength(FnInfo *fn, int length){
	fn->length += length; 	
}

//Recursive method
void recursiveFnData(FnsData * fnArray, unsigned char * function, Lde *decoder){
	FnInfo *fn = functionSeen(fnArray, function);
	if(fn != NULL){
		incrementInCalls(fn);
		return; 		
	}
	//printf("Current function: %p \n", function);
	unsigned char *p = function; 
	fn = newFnEntry(fnArray, function);
	while(!is_ret(*p)){
		int length = get_op_length(decoder,(const unsigned char *) p);
		incrementLength(fn, length); 
		if(is_call(*p)){
			incrementOutCalls(fn); 
			int offset = (int)(*(p + 1)) | ( ((int) *(p + 2)) << 8) |
				    ( ((int)*(p+3)) << 16) | (((int)*(p+4))<< 24);	
			unsigned char *nextFunc = p + length + offset; 
			recursiveFnData(fnArray, nextFunc, decoder);  
		}
		p += length; 
	}
	incrementLength(fn, get_op_length(decoder, p)); 
	return;
	
}


void printFnInfo(FnInfo fnInfo){
	printf("%p, %d, %d" , fnInfo.address, fnInfo.nInCalls, fnInfo.nOutCalls);
}


void printFnData(FnsData  fnArray){
	for(int i = 0 ; i < fnArray.length ; i ++){
		printFnInfo(fnArray.arr[i]);
		printf("\n");
	}

}















/** Return pointer to opaque data structure containing collection of
 *  FnInfo's for functions which are callable directly or indirectly
 *  from the function whose address is rootFn.
 */
const FnsData *
new_fns_data(void *rootFn)
{
  	//Initializing pointer to instructions, datastorage, and decoder
	Lde *decode = new_lde();
	unsigned char *p = (unsigned char *)rootFn;
	FnsData *fnArray = initFnsData(); 
	recursiveFnData(fnArray, p, decode);
	printFnData(*fnArray);
	return fnArray; 
}





/** Free all resources occupied by fnsData. fnsData must have been
 *  returned by new_fns_data().  It is not ok to use to fnsData after
 *  this call.
 */
void
free_fns_data(FnsData *fnsData)
{
  free(fnsData->arr);
  free(fnsData); 
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
	if(lastFnInfo == NULL) return fnsData->arr; 
	FnInfo *base = fnsData->arr;
	long difference = (long) (lastFnInfo - base);
	if(difference >= sizeof(FnInfo) * fnsData->length || difference < 0) return NULL;
	FnInfo *ret =  sizeof(FnInfo) + lastFnInfo;
	return ret; 
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
