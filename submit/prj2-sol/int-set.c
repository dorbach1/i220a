#include "int-set.h"
#include <stddef.h>
#include <stdlib.h>
/** Abstract data type for set of int's.  Note that sets do not allow
 *  duplicates.
 */
typedef struct NodeStruct{
	int value;
	struct NodeStruct *succ; 
} Node; 

typedef struct {
	int nElements;
	Node dummy;
} Header; 


/** Return a new empty int-set.  Returns NULL on error with errno set.
 */
//May need to be fixed
void *newIntSet(){
	return calloc(1, sizeof(Header));
};

/** Return # of elements in intSet */
int nElementsIntSet(void *intSet){
	Header *header = (Header *) intSet;
	return header->nElements; 
};

/** Return non-zero iff intSet contains element. */
int isInIntSet(void *intSet, int element){
	return 0; 
}

/** Change intSet by adding element to it.  Returns # of elements
 *  in intSet after addition.  Returns < 0 on error with errno
 *  set.
 */
int addIntSet(void *intSet, int element){
	return 0; 
}

/** Change intSet by adding all elements in array elements[nElements] to
 *  it.  Returns # of elements in intSet after addition.  Returns
 *  < 0 on error with errno set.
 */
int addMultipleIntSet(void *intSet, const int elements[], int nElements){
	return 0; 
}

/** Set intSetA to the union of intSetA and intSetB.  Return # of
 *  elements in the updated intSetA.  Returns < 0 on error.
 */
int unionIntSet(void *intSetA, void *intSetB){
	return 0; 
}

/** Set intSetA to the intersection of intSetA and intSetB.  Return #
 *  of elements in the updated intSetA.  Returns < 0 on error.
 */
int intersectionIntSet(void *intSetA, void *intSetB){
	return 0; 
}

/** Free all resources used by previously created intSet. */
void freeIntSet(void *intSet){
	Header *header = (Header *) intSet;
	Node *p1;
	for(Node *p = header->dummy.succ; p != NULL; p = p1){
		p1 = p->succ;
		free(p);
	}
	free(header);
	return; 
}

/** Return a new iterator for intSet.  Returns NULL if intSet
 *  is empty.
 */
const void *newIntSetIterator(const void *intSet){
	return NULL; 
}

/** Return current element for intSetIterator. */
int intSetIteratorElement(const void *intSetIterator){
	return 0; 
}

/** Step intSetIterator and return stepped iterator.  Return
 *  NULL if no more iterations are possible.
 */
const void *stepIntSetIterator(const void *intSetIterator){
	return NULL;
}

