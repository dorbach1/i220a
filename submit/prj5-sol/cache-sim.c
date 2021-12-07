#include "cache-sim.h"

#include "memalloc.h"

#include <stdbool.h>
#include <stddef.h>

typedef unsigned long Line;

typedef struct LineNodeStruct{
        Line line;
        struct LineNodeStruct   *next;
        struct LineNodeStruct   *prev;
} LineNode;

typedef struct SetStruct {
        LineNode *first;
        LineNode *last;
} Set;

//Allocates memory for a set with a single line in it (auxillary)
Set * newEmptySet(){
        Set* s = (Set *) malloc(sizeof(Set));
        if(!s){
                printf("Error allocating mem\n");
                return NULL;
        }
        LineNode *first = (LineNode *)malloc(sizeof(LineNode));
        if(!first){
                printf("Error allocating mem\n");
                return NULL;
        }
        first->line = 0;
        s->first = first;
        s->last = first; //This will remain the last line in the set untill it is hit 
        return s;
}

//Add new node to a set (auxillary)
int addNewNode(Set* s){
        LineNode *temp = (LineNode *) malloc(sizeof(LineNode));
        if(!temp){
                printf("Error allocating mem\n");
                return 1;
        }
        temp->line = 0;
        temp->next = s->first;
        s->first->prev = temp;
        s->first = temp;
        return 0;
}

//Free up memory allocated for a set
void freeSet(Set* set){

        LineNode  *p1 = set->first;
        LineNode *p2 = p1->next;
        while(p2 != NULL){
                free(p1);
                p1 = p2;
                p2 = p2->next;
        }
        free(p1);
        free(set);
}

//Initialize net set with numLines number of lines 
Set * newSet(int numLines){
        Set *s = newEmptySet();
        for(int i = 0; i < numLines - 1; i ++){
                addNewNode(s);
        }
        return s;
}




/** Create and return a new cache-simulation structure for a
 *  cache for main memory withe the specified cache parameters params.
 *  No guarantee that *params is valid after this call.
 */
CacheSim *
new_cache_sim(const CacheParams *params)
{
  //TODO
  return NULL;
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
  //TODO
}

/** Return result for requesting addr from cache */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr addr)
{
  CacheResult result = { 0, 0 };
  //TODO
  return result;
}
