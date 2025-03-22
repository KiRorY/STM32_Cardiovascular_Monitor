#ifndef CONSTANT_H_
#define CONSTANT_H_

typedef int    BOOL;

#define TRUE   1
#define FALSE  0


#define DATALEN       255


#define ROUND_TO_UINT16(x)   ((uint16_t)(x)+0.5)>(x)? ((uint16_t)(x)):((uint16_t)(x)+1) //将浮点数x四舍五入为uint16_t

#endif /*CONSTANT_H_*/
