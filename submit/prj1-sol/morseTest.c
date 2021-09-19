#include "morse.h"
#include <stdio.h>


int main(int argc, char** argv){

unsigned int offset = 0;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 1;
printf("%d - %d\n" , offset ,  getOffset(offset));
offset = 8;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 11;
printf("%d - %d\n" , offset ,  getOffset(offset));
offset = 1;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 15;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 16;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 0;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 21;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 24;
printf("%d - %d\n" , offset ,  getOffset(offset));

offset = 258;
printf("%d - %d\n" , offset ,  getOffset(offset));


offset = 23;
printf("%d - %d\n" , offset ,  getOffset(offset));
return 0; 
}
