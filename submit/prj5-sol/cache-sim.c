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
	first->next = NULL;
	first->prev = NULL;
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

//Replace most recent line in a set with a new line (pushing the other to be second)
void setMostRecentLine(Set *s, LineNode *line){
        if(line->prev == NULL){
                return;
        }
        if(line->next == NULL){
                s->last = line->prev;
        }

        //
        line->prev->next = line->next;
        line->next = s->first;
        line->prev = NULL;
        s->first = line;
}

//Returns most recent line used
static inline LineNode *getMostRecetLine(Set *s){
        return s->first;
}

//Returns least recent line used
static inline LineNode *getLeastRecentLine(Set *s){
        return s->last;
}


//Returns the tag associated with a line
static inline unsigned long getTagBits(Line line){
        return (line & ((~0lu) >> 1));
}

//Sets the tags of a given line
static inline void setTagBits(Line *line, unsigned long tag){
        tag = tag & ((~0lu) >> 1);
        *line = *line & (~((~0lu) >> 1));
        *line = *line + tag;
}

//Gets wether the data in this line is valid
static inline int getValidBit(Line line){
        return (line & (~((~0lu) >> 1))) ? 1 : 0;
}

//Sets wether the data in this line is valid 
static inline void setValidBit(Line *line, unsigned int bit){
        *line = *line & ((~0lu)>>1);
        *line = *line | ((unsigned long)bit << (sizeof(long) * 8 - 1 ));
}

//finds line specified by tag
LineNode *findLine(Set *s, unsigned long tag){
        LineNode *p = s->first ;
        while(p != NULL){
                if(getTagBits(p->line) == tag) return p;
                p = p->next;
        }
        return NULL;
}

//Finds the first empty line in the set
LineNode *findEmpty(Set *s){
        LineNode *p = s->first ;
        while(p != NULL){
                if(!getValidBit(p->line)) return p;
                p = p->next;
        }
        return NULL;
}

int power(int base, int exp){
        int ret = 1;
        for(int i = 0; i < exp; i++){
                ret *= base;
        }
        return ret;
}

typedef struct CacheSimImpl {
        Set **sets;
        CacheParams params;
} CacheSim;




/** Create and return a new cache-simulation structure for a
 *  cache for main memory withe the specified cache parameters params.
 *  No guarantee that *params is valid after this call.
 */
CacheSim *
new_cache_sim(const CacheParams *params)
{
        CacheSim *cache  = malloc(sizeof(CacheSim));
        if(cache == NULL){
                printf("Malloc experienced an error");
                return NULL;
        }
        cache->params = *params;
        int numSets = (int) power(2, params->nSetBits);
        int numLines = params->nLinesPerSet;

        Set **sets = malloc( sizeof(Set *) * numSets);
        for(int i = 0; i < numSets ; i++){
                sets[i] = newSet(numLines);
        };
        cache->sets = sets;
        return cache;  
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
        Set **sets = cache->sets;
        int numSets = (int) power(2, cache->params.nSetBits );
        for(int i = 0; i < numSets ; i ++){
                freeSet(sets[i]);
        }
        free(sets);
        free(cache);
}

/** Return result for requesting addr from cache */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr addr)
{
  CacheResult result = { 0, 0 };
  //TODO
  return result;
}
