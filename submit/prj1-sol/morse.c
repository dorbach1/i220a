#include "morse.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  char c;
  const char *code;
} TextMorse;

//<https://en.wikipedia.org/wiki/Morse_code#/media/File:International_Morse_Code.svg>
static const TextMorse charCodes[] = {
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "_._." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', "-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },

  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },

  { '\0', ".-.-." }, //AR Prosign indicating End-of-message
                     //<https://en.wikipedia.org/wiki/Prosigns_for_Morse_code>
};


/** Return NUL-terminated code string for char c. Returns NULL if
 *  there is no code for c.
 */
static const char *
charToCode(Byte c) {
  for (int i = 0; i < sizeof(charCodes)/sizeof(charCodes[0]); i++) {
    if (charCodes[i].c == c) return charCodes[i].code;
  }
  return NULL;
}


/** Return char for code. Returns < 0 if code is invalid.
 */
static int
codeToChar(const char *code) {
  for (int i = 0; i < sizeof(charCodes)/sizeof(charCodes[0]); i++) {
    if (strcmp(charCodes[i].code, code) == 0) return charCodes[i].c;
  }
  return -1;
}

/** Given an array of Bytes, a bit index is the offset of a bit
 *  in the array (with MSB having offset 0).
 *
 *  Given a bytes[] array and some bitOffset, and assuming that
 *  BITS_PER_BYTE is 8, then (bitOffset >> 3) represents the index of
 *  the byte within bytes[] and (bitOffset & 0x7) gives the bit-index
 *  within the byte (MSB represented by bit-index 0) and .
 *
 *  For example, given array a[] = {0xB1, 0xC7} which is
 *  { 0b1011_0001, 0b1100_0111 } we have the following:
 *
 *     Bit-Offset   Value
 *        0           1
 *        1           0
 *        2           1
 *        3           1
 *        4           0
 *        5           0
 *        6           0
 *        7           1
 *        8           1
 *        9           1
 *       10           0
 *       11           0
 *       12           0
 *       13           1
 *       14           1
 *       15           1
 *
 */


/** Return mask for a Byte with bit at bitIndex set to 1, all other
 *  bits set to 0.  Note that bitIndex == 0 represents the MSB,
 *  bitIndex == 1 represents the next significant bit and so on.
 */
static inline unsigned
byteBitMask(unsigned bitIndex)
{
	unsigned char mask = ~0;
	mask >>= 1;
	mask = ~mask;
	mask >>= bitIndex; 
	return mask; 
}

/** Given a power-of-2 powerOf2, return log2(powerOf2) */
static inline unsigned
getLog2PowerOf2(unsigned powerOf2)
{
	unsigned int counter = 0;
	for(int i = 1; (i != powerOf2); i <<= 1 ){
		counter ++; 
	} 
	return counter;
}

/** Given a bitOffset return the bitIndex part of the bitOffset. */
static inline unsigned
getBitIndex(unsigned bitOffset)
{
 	unsigned int bitsToMask = getLog2PowerOf2(BITS_PER_BYTE);
	unsigned int mask = ~(( ~0 ) << bitsToMask);
	return (bitOffset & mask);
}

/** Given a bitOffset return the byte offset part of the bitOffset */
static inline unsigned
getOffset(unsigned bitOffset)
{
	unsigned int bitsToPush = getLog2PowerOf2(BITS_PER_BYTE);
	return bitOffset >> bitsToPush;
}

/** Return bit at offset bitOffset in array[]; i.e., return
 *  (bits(array))[bitOffset]
 */
static inline int
getBitAtOffset(const Byte array[], unsigned bitOffset)
{
	Byte byte = array[getOffset(bitOffset)];
	unsigned char  mask  = ~0;
	mask >>= 1;
	mask = ~mask;
	mask >>= getBitIndex(bitOffset);
	if(byte & mask) return 1;
	else return 0;  
}

/** Set bit selected by bitOffset in array to bit. */
static inline void
setBitAtOffset(Byte array[], unsigned bitOffset, unsigned bit)
{
 	Byte byte = array[getOffset(bitOffset)];
	unsigned char mask = ~0;
	mask >>= 1;
	mask = ~mask;
	mask >>= getBitIndex(bitOffset);
	if(bit){
		byte |= mask;
			
	} else {
		byte &= ~mask; 
	}
	array[getOffset(bitOffset)] = byte;
 	return;
}

/** Set count bits in array[] starting at bitOffset to bit.  Return
 *  bit-offset one beyond last bit set.
 */
static inline unsigned
setBitsAtOffset(Byte array[], unsigned bitOffset, unsigned bit, unsigned count)
{
	unsigned int i;
	for(i = 0; i < count; i ++){
		setBitAtOffset(array, bitOffset + i, bit);		
	}
	return bitOffset + i;
}

int writeDash(Byte array[], unsigned *bitOffset){
	*bitOffset = setBitsAtOffset(array, *bitOffset,  1, 3);
	return 0; 
}

int writeDot(Byte array[], unsigned *bitOffset){
	*bitOffset = setBitsAtOffset(array, *bitOffset, 1, 1);
	return 0; 
}

int writeSingleSpace(Byte array[], unsigned *bitOffset){
	*bitOffset = setBitsAtOffset(array, *bitOffset, 0, 1); 
	return 0;
}

int writeCharSpace(Byte array[], unsigned *bitOffset){
	*bitOffset = setBitsAtOffset(array, *bitOffset, 0, 2);
	return 0;
}


int writeWordSpace(Byte array[], unsigned *bitOffset){
	*bitOffset = setBitsAtOffset(array, *bitOffset, 0, 4);
	return 0;  
}


int writeChar(Byte array[], Byte letter , unsigned *bitOffset){
	const char *code = charToCode(letter);
	for(int i = 0; code[i] != '\0'; i++ ){
		if(code[i] == '.'){
			writeDot(array, bitOffset);
		}
		else{
			writeDash(array, bitOffset);
		}
		writeSingleSpace(array, bitOffset); 
	}
	if(letter != '\0') writeCharSpace(array, bitOffset); 
	return 0; 
}
/** Convert text[nText] into a binary encoding of morse code in
 *  morse[].  It is assumed that array morse[] is initially all zero
 *  and is large enough to represent the morse code for all characters
 *  in text[].  The result in morse[] should be terminated by the
 *  morse prosign AR.  Any sequence of non-alphanumeric characters in
 *  text[] should be treated as a *single* inter-word space.  Leading
 *  non alphanumeric characters in text are ignored.
 *
 *  Returns count of number of bytes used within morse[].
 */
int
textToMorse(const Byte text[], unsigned nText, Byte morse[])
{
	unsigned int bitOffset = 0; 
	int skipNextSpace = 1;
	

	//Looping through each char
	for(int i = 0; i < nText; i++){
		if(isalnum(text[i])){
			writeChar(morse, text[i], &bitOffset);
			skipNextSpace = 0; 
		} else if(!skipNextSpace){
			writeWordSpace(morse, &bitOffset);
			skipNextSpace = 1; 	 
		}
	}
	

	writeChar(morse, '\0', &bitOffset); 
	return (getBitIndex(bitOffset) == 0 ? getOffset(bitOffset): getOffset(bitOffset) + 1);
}

/** Return count of run of identical bits starting at bitOffset
 *  in bytes[nBytes].
 */
static inline unsigned
runLength(const Byte bytes[], unsigned nBytes, unsigned bitOffset)
{
	int i = 0; 
	int cont = 1;
	while(cont){
		if(getBitAtOffset(bytes, bitOffset + i) == getBitAtOffset(bytes, bitOffset)){
			i++;
		} 
		else{
			cont = 0;
		}
		if(getOffset(bitOffset) >= nBytes){
			
			 cont = 0;

		}
	
	}
	return i;
}

/** Convert AR-prosign terminated binary Morse encoding in
 *  morse[nMorse] into text in text[].  It is assumed that array
 *  text[] is large enough to represent the decoding of the code in
 *  morse[].  Leading zero bits in morse[] are ignored. Encodings
 *  representing word separators are output as a space ' ' character.
 *
 *  Returns count of number of bytes used within text[], < 0 on error.
 */

void printCharCode(char code[],int  nCode){
	for(int i = 0; i < nCode; i ++){
		printf("[%c] ", code[i]);
	}
	printf("\n");
}

void printBits(const Byte morse[] ,int nMorse){
	for(int i = 0; getOffset(i) < nMorse && getBitIndex(i) < BITS_PER_BYTE; i ++){
		printf("%d", getBitAtOffset(morse, i)); 
	}
	printf("\n");
}


int
morseToText(const Byte morse[], unsigned nMorse, Byte text[])
{
	//printBits(morse, nMorse);	
	//Skipping Over Leading 0s 
	unsigned int bitOffset = 0; 
	if(!getBitAtOffset(morse, bitOffset)){ 
		unsigned int skip = runLength(morse, nMorse, bitOffset);
		bitOffset += skip; 
	}
	
 	//Initializing Array to hold the morsecode for  a charachter
	int MAX_LENGTH = 5; 
	char nextCode[5] = { 0 };
	int codeIndex = 0; 
	
	//Initializing Pointer to be used to write chars to the text array 
	int textIndex = 0; 

	//while we are not past the last byte in the morse array 
	while(getOffset(bitOffset) < nMorse){ 
		//If Current Bit is a 1
		if(getBitAtOffset(morse, bitOffset)){
			int numOnes = runLength(morse, nMorse, bitOffset);
			//printf("There are %d ones at Bit  %d\n",numOnes, bitOffset);
			//printf("codeIndex is %d, at Bit %d \n", codeIndex, bitOffset);
			if(numOnes == 1) nextCode[codeIndex] = '.';
			else if(numOnes == 3) nextCode[codeIndex] = '-';
			else return -1;
			codeIndex ++;
			bitOffset += numOnes; 
			//printCharCode(nextCode, MAX_LENGTH); 
		} 
		//Else if Bit is a zero 
		else {
			int numSpaces = runLength(morse, nMorse, bitOffset);
			char sym; 
			switch(numSpaces){
				case 7: 
					sym = codeToChar(nextCode);
					text[textIndex] = sym;
					text[textIndex + 1] = ' ';
					textIndex += 2 ;
					for(codeIndex = codeIndex; codeIndex >= 0; codeIndex --){
						nextCode[codeIndex] = 0; 
					}
					codeIndex ++; 
					bitOffset += numSpaces; 
					break; 
				case 3:
                                        //printCharCode(nextCode , MAX_LENGTH);
                                        //printf("Sym : %c \n" , sym);
					//Write Char and increment textIndex
					sym = codeToChar(nextCode);
					text[textIndex] = sym;
					textIndex ++;
					//Reset Array and Array Pointer to get Ready for new Char to come in 
					for(codeIndex = codeIndex; codeIndex >= 0; codeIndex -- ){
						nextCode[codeIndex] = 0; 
					}
					codeIndex++;
					bitOffset += numSpaces;
					break; 
				case 1 :
					//For all spaces we need to skip over them after 
					bitOffset += numSpaces; 
					break;
				default:
					return  textIndex; 
			}
		}
	}

	return ( textIndex ) ;
}



