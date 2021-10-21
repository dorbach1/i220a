#include "int-set.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
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



static int unlinkNodeAfter(Node *p){
	if(!p->succ) return 0;
	Node *next = p->succ->succ;
	free(p->succ); 
	p->succ = next; 
	return 1; 
}

static Node *linkNodeAfter(Node *p0, int value){
	Node *newNode = malloc(sizeof(Node));
	if(!newNode) return NULL; 
	newNode->value = value;
	newNode->succ = p0->succ;
	p0->succ = newNode;
	return newNode; 
}

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
	Header *header = (Header *) intSet;
	for(Node *p = header->dummy.succ; p != NULL; p = p->succ){
		if(p->value == element) return 1; 
	}
	return 0; 
}

/** Change intSet by adding element to it.  Returns # of elements
 *  in intSet after addition.  Returns < 0 on error with errno
 *  set.
 */
int addIntSet(void *intSet, int element){
	Header *header = (Header *) intSet;
	Node *p = &header->dummy;
	while(p->succ != NULL && p->succ->value < element){
		p = p->succ; 
	}
	if(p->succ != NULL && p->succ->value == element){
		return header->nElements; 
	}

	if(!linkNodeAfter(p, element)) return -1;
	return ++header->nElements;
	
	return 0; 
}

/** Change intSet by adding all elements in array elements[nElements] to
 *  it.  Returns # of elements in intSet after addition.  Returns
 *  < 0 on error with errno set.
 */
int addMultipleIntSet(void *intSet, const int elements[], int nElements){
	int i;
	for(i = 0 ; i < nElements; i ++){
		if(addIntSet(intSet, elements[i]) < 0)  return -1; 
	} 
	return ((Header * )intSet)->nElements;
	
}

/** Set intSetA to the union of intSetA and intSetB.  Return # of
 *  elements in the updated intSetA.  Returns < 0 on error.
 */
int unionIntSet(void *intSetA, void *intSetB){
	Header *aHeader = (Header *) intSetA;
	Header *bHeader = (Header *) intSetB;
	
	Node *aP = &(aHeader->dummy);
	Node *bP = bHeader->dummy.succ;
	
	while(bP != NULL && aP != NULL  && aP->succ != NULL){
		printf("ap current value %d bp current value %d \n", aP->value, bP->value);
		int bV = bP->value;
		int aV = aP->succ->value; 
		if( bV < aV){
			aP = linkNodeAfter(aP, bV);
			if(aP == NULL) return -1;
			++(aHeader->nElements);
			bP = bP->succ; 
		}
		else if(bV == aV){
			bP = bP->succ;
			aP = aP->succ;  
		}
		else{
			aP = aP->succ;
		}	
	}
	while(bP != NULL && aP!=NULL){
		aP = linkNodeAfter(aP, bP->value);
		if(aP == NULL) return -1;
		++(aHeader->nElements);
		bP = bP->succ; 
	}
	return aHeader->nElements; 
}

/** Set intSetA to the intersection of intSetA and intSetB.  Return #
 *  of elements in the updated intSetA.  Returns < 0 on error.
 */
int intersectionIntSet(void *intSetA, void *intSetB){
	Header *aHeader = (Header *) intSetA;
	Header *bHeader = (Header *) intSetB;

	Node *aP = &aHeader->dummy;
	Node *bP = bHeader->dummy.succ;


	//if aP is large and bp is small we will have to clear out the rest of aP
	// {1 5 7 9 11 113}  {2, 3, 7}
	//if bP is large and aP is small everything works, maybe clear out aP
	while(bP != NULL &&  aP->succ != NULL){
		printf("Current a %d, Current b %d, num a %d \n", aP->succ->value, bP->value, aHeader->nElements);
		if(bP->value < aP->succ->value){
			bP = bP->succ;
		}
		else if(bP->value == aP->succ->value){
			bP = bP->succ;
			aP = aP->succ;
		}
		else {
			unlinkNodeAfter(aP);
			aHeader->nElements -= 1; 
		} 
	}
	while(aP->succ != NULL){
		unlinkNodeAfter(aP);
		aHeader->nElements -= 1; 
	}
	
	return aHeader->nElements;  
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
	Header *header = (Header *) intSet;
	return header->dummy.succ;  
}

/** Return current element for intSetIterator. */
int intSetIteratorElement(const void *intSetIterator){
	Node *node = (Node *) intSetIterator;
	return node->value; 
}

/** Step intSetIterator and return stepped iterator.  Return
 *  NULL if no more iterations are possible.
 */
const void *stepIntSetIterator(const void *intSetIterator){
	Node *node = (Node *) intSetIterator;
	return node->succ;
}

