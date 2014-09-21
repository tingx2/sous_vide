#ifndef TYPES_H
#define TYPES_H

/**********************************************************************
  
  Type Definitions

**********************************************************************/

/* Basic Types */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

typedef enum
{
  false = 0,
  FALSE = false,
  true  = 1,
  TRUE  = true
} bool;
typedef bool boolean;

#define MAX_INT       0xFFFFFFFF
#define MAX_LONGINT   0xFFFFFFFFFFFFFFFF

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b

#endif // TYPES_H
