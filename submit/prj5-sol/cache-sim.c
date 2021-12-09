#include "cache-sim.h"

#include "memalloc.h"
#include <stdio.h>
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


//Extracts set from a a address
unsigned long extractSet(CacheParams params, MemAddr address){
        unsigned long byte_bits  = params.nLineBits;
        unsigned long set_bits = params.nSetBits;
        unsigned long tag_bits = (sizeof(unsigned long) * 8) - byte_bits  - set_bits; //address may be smaller then a long
        unsigned long set = address & ((~0lu) << byte_bits );
        set = set &  ( (~0lu) >> ( (sizeof(unsigned long) * 8) - params.nMemAddrBits  ) );
        set = set & ((~0lu) >> tag_bits);
        set = set >> byte_bits ;
        return set;
}


//Extracts tag from an adresss
unsigned long extractTag(CacheParams params, MemAddr address){
        unsigned long byte_bits  = params.nLineBits;
        unsigned long set_bits = params.nSetBits;
        unsigned long tag_bits = (sizeof(unsigned long) * 8) - byte_bits  - set_bits;
        unsigned long tag = address & ~((~0lu) >> tag_bits);
        tag = tag &  ( (~0lu) >> ( (sizeof(unsigned long) * 8) - params.nMemAddrBits  ) );
        tag = tag >> (set_bits + byte_bits);
        return tag;
}

//Fills in line with new data and returns the previous address that was stored there
MemAddr fillLine(CacheParams params, unsigned long setIndex, unsigned long newTag, LineNode *line){
        unsigned long byte_bits  = params.nLineBits;
        unsigned long set_bits = params.nSetBits;
        MemAddr add = getTagBits(line->line);
        add = add << (byte_bits + set_bits);
        add = add | (setIndex << byte_bits);
        setTagBits(&line->line, newTag);
        setValidBit(&line->line, 1);
        return add;
}

LineNode *pickReplacement(CacheParams params, Set *s){
        Replacement strat = params.replacement;
        if(strat == LRU_R){
                return s->last;
        } else if(strat == MRU_R){
                return s->first;
        } else {
                int setSize = params.nLinesPerSet;
                int index = rand() % setSize;
                LineNode *p = s->first;
                for(int i =0; i < index && p != NULL; i++){
                        p = p->next;
                }
                return p;
        }


}





/** Return result for requesting addr from cache */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr addr)
{
	CacheResult result = { 0, 0 };
	unsigned long setI = extractSet(cache->params , addr);
	unsigned long tag = extractTag(cache->params, addr);
	Set *set = cache->sets[setI]; 
	LineNode *reqLine = findLine(set, tag);
	if(reqLine != NULL){
		setMostRecentLine(set, reqLine); 
		result.status = CACHE_HIT;
		return result;
	}

	LineNode *nextEmpty = findEmpty(set);
	if(nextEmpty != NULL){
		fillLine(cache->params , setI, tag, nextEmpty);
		setMostRecentLine(set , nextEmpty);
		result.status = CACHE_MISS_WITHOUT_REPLACE;
		return result;
	}
	LineNode *toReplace = pickReplacement(cache->params, set);
	MemAddr replaced = fillLine(cache->params, setI, tag, toReplace);
	result.status = CACHE_MISS_WITH_REPLACE;
	result.replaceAddr = replaced;
	return result;

	
	
	return result;
}




