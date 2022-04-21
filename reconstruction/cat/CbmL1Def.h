#ifndef CbmL1Def_h
#define CbmL1Def_h 1

// #define FAST_CODE // FAST_CODE = more unsafe

#include <assert.h>

#ifdef HAVE_SSE
  #include "vectors/P4_F32vec4.h"
//  #include "vectors/P4_F64vec2.h"

#else
  #include "vectors/PSEUDO_F32vec4.h"
  #error NoSseFound
#endif // HAVE_SSE

//#include "vectors/PSEUDO_F64vec1.h"



#ifdef FAST_CODE

#define L1_NO_ASSERT // use with asserts, etc.
#define L1_DYNAMIC_CAST static_cast

#else // FAST_CODE

#define L1_DYNAMIC_CAST dynamic_cast

#endif // FAST_CODE 

#if 1 && defined( __GNUC__ ) && __GNUC__ - 0 >= 3   // for speed up conditions
#define ISLIKELY(  x )    __builtin_expect( !!( x ),1 )
#define ISUNLIKELY(  x )  __builtin_expect( !!( x ),0 )
#else
#define ISLIKELY(  x )   (  x )
#define ISUNLIKELY(  x )  (  x )
#endif

#if defined( NDEBUG ) || defined( L1_NO_ASSERT )
#define L1_ASSERT(v, msg)
#define L1_assert(v)
#else
#define L1_ASSERT(v, msg) \
if (v) {} else { \
  std::cerr << __FILE__ << ":" << __LINE__ << " assertion failed: " \
            << #v << " = " << (v) << "\n" << msg << std::endl; \
  abort(); \
}
#define L1_assert(v) assert(v)
#endif







#endif // CbmL1Def_h
