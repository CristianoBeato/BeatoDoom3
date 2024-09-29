

#ifndef __INTRIN_H__
#define __INTRIN_H__

#include <pmmintrin.h>

extern __always_inline void _mm_fldcw( unsigned short controlWord )
{
#if defined( __GNUC__ )
	// Load Control Word to FPU
	__asm__ __volatile__ (  "fldcw %0" : : "m" (controlWord) );
#endif

}

extern __always_inline void _mm_fninit( void )
{
#if defined( __GNUC__ )
	// Reset the FPU (clear the FPU stack)
	__asm__ __volatile__ ( "fninit" );
#endif
}

extern __always_inline unsigned short _mm_fstenv( void )
{
	unsigned short tagWord = 0x0000;
#if defined( __GNUC__ )
	__asm__ __volatile__ ( "fstenv %[tagWord]\n\t" : [tagWord] "=m" (tagWord) ); 
#endif
	return tagWord;
}

extern __always_inline unsigned int _mm_fnstenv( void )
{
	unsigned int state = 0x000000;
#if defined( __GNUC__ )
	__asm__ __volatile__( "fnstenv (%0)" : "=m" ( state ) ); 
#endif
	return state; 
}

extern __always_inline unsigned short _mm_fnstcw( void )
{
	unsigned short controlWord = 0x0000;
#if defined( __GNUC__ )
	// Stores the status word in the operand (status)
    __asm__ __volatile__ ( "fnstcw %0" : "=m" ( controlWord ) );
#endif
	return controlWord;
}

extern __always_inline unsigned short _mm_fstsw( void )
{
	unsigned short state = 0;
#if defined( __GNUC__ )
	// Stores the status word in the operand (status)
    __asm__ __volatile__ ( "fstsw %0" : "=m" ( state ) );
#endif
	return state;
}

// The code below assumes that a cache line is 64 bytes.
extern __always_inline void Prefetch( const void * ptr, intptr_t offset ) 
{
	_mm_prefetch( static_cast<const char*>( ptr ) + offset, _MM_HINT_NTA );
}

extern __always_inline void ZeroCacheLine( void * ptr, intptr_t offset ) 
{
	__m128i zero = _mm_setzero_si128();
	char * bytePtr = ( (char *) ptr ) + offset;
	_mm_store_si128( (__m128i *) ( bytePtr + 0 * 16 ), zero );
	_mm_store_si128( (__m128i *) ( bytePtr + 1 * 16 ), zero );
	_mm_store_si128( (__m128i *) ( bytePtr + 2 * 16 ), zero );
	_mm_store_si128( (__m128i *) ( bytePtr + 3 * 16 ), zero );
}

extern __always_inline void FlushCacheLine( const void * ptr, intptr_t offset ) 
{
	_mm_clflush( static_cast< const char* >( ptr ) + offset );
}

#endif //!__INTRIN_H__