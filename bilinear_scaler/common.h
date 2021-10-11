#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (__cplusplus) || defined (c_plusplus)
extern "C"{
#endif


#define FAIL							0
#define SUCCESS							1
#define TRUE							1
#define FALSE							0
#define MAX_RAW_BASE_FNAME_LENGHT		32
#define MIN_ROWS_TO_READ_IN_RAW_BUF		8

#define _assert(a)	do { if (!(a)) printf("!!! ASSERT: %s(),%d\n",__FUNCTION__, __LINE__); } while(0)


char* prepare_file_name(char *fin, char *oext, char *post);

#if defined (__cplusplus) || defined (c_plusplus)
}
#endif

#endif /* _COMMON_H_ */
