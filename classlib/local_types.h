//
// local_types.h:
//
#ifndef __local_types_h
#define __local_types_h

#ifndef Boolean
#define Boolean int
#endif // Boolean

#ifndef True
#define True 1
#endif // True

#ifndef False
#define Fakse 0
#endif // False

#ifndef Number
#define Number(x) (sizeof(x)/sizeof(x[0]))
#endif // Number

#define Done True

#define fatal() fprintf( stderr, "aborted at %d in %s\n", __LINE__, __FILE__ );

#endif // __local_types_h
