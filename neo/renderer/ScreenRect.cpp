/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

This file is part of the Beato idTech 4  GPL Source Code (?Beato idTech 4  Source Code?).

Beato idTech 4  Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato idTech 4  Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato idTech 4  Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "renderer_common.h"
#include "ScreenRect.h"

#if ID_USE_INSTRINSEC
#include <smmintrin.h> //SSSE3
#endif //ID_USE_INSTRINSEC

/*
======================
idScreenRect::Clear
======================
*/
void idScreenRect::Clear() 
{
	x1 = y1 = 32000;
	x2 = y2 = -32000;
	zmin = 0.0f; zmax = 1.0f;
}

/*
======================
idScreenRect::AddPoint
======================
*/
void idScreenRect::AddPoint( float x, float y ) 
{
	int	ix = idMath::FtoiFast( x );
	int iy = idMath::FtoiFast( y );

	if ( ix < x1 ) 
		x1 = ix;

	if ( ix > x2 ) 
		x2 = ix;

	if ( iy < y1 ) 
		y1 = iy;
	
	if ( iy > y2 )
		y2 = iy;
}

/*
======================
idScreenRect::Expand
======================
*/
void idScreenRect::Expand( void ) 
{
	x1--;
	y1--;
	x2++;
	y2++;
}

/*
======================
idScreenRect::Intersect
======================
*/
void idScreenRect::Intersect( const idScreenRect &rect ) 
{
#if ID_USE_INSTRINSEC
	// Load the current values ​​of x1, x2, y1, y2 into a SIMD vector
	__m128i current = _mm_set_epi32(y2, y1, x2, x1);
	__m128i newRect = _mm_set_epi32(rect.y2, rect.y1, rect.x2, rect.x1);

	// Calculate the maximums for x1 and y1 (keep the largest values)
	__m128i maxVals = _mm_max_epi32(current, newRect);
	
	// Calculate the minimums for x2 and y2 (keep the smallest values)
	__m128i minVals = _mm_min_epi32(current, newRect);
	
    // Updates the values ​​of the original rect
	x1 = _mm_extract_epi32(maxVals, 0);
	x2 = _mm_extract_epi32(minVals, 1);
	y1 = _mm_extract_epi32(maxVals, 2);
	y2 = _mm_extract_epi32(minVals, 3);
#elif 1
	x1 = std::max( x1, rect.x1 );
	x2 = std::min( x2, rect.x2 );
	y1 = std::max( y1, rect.y1 );
	y2 = std::min( y2, rect.y2 );
#else
	if ( rect.x1 > x1 ) x1 = rect.x1; 
	if ( rect.x2 < x2 ) x2 = rect.x2;
	if ( rect.y1 > y1 ) y1 = rect.y1;
	if ( rect.y2 < y2 ) y2 = rect.y2;
#endif
}

/*
======================
idScreenRect::Union
======================
*/
void idScreenRect::Union( const idScreenRect &rect ) 
{
#if ID_USE_INSTRINSEC
    // Load the current values ​​of x1, x2, y1, y2 into a SIMD vector
    __m128i current = _mm_set_epi32(y2, y1, x2, x1);
    __m128i newRect = _mm_set_epi32(rect.y2, rect.y1, rect.x2, rect.x1);

	// Calculate the minimums for x1 and y1 (new smaller values ​​are kept)
	__m128i minVals = _mm_min_epi32(current, newRect);

	// Calculate the maximums for x2 and y2 (new larger values ​​are kept)
	__m128i maxVals = _mm_max_epi32(current, newRect);

    // Updates the values ​​of the original rect
    x1 = _mm_extract_epi32( minVals, 0);
    x2 = _mm_extract_epi32( maxVals, 1);
    y1 = _mm_extract_epi32( minVals, 2);
    y2 = _mm_extract_epi32( maxVals, 3);
#elif 1
	x1 = std::min( x1, rect.x1 );
	x2 = std::max( x2, rect.x2 );
	y1 = std::min( y1, rect.y1 );
	y2 = std::max( y2, rect.y2 );
#else
	if ( rect.x1 < x1 ) x1 = rect.x1;
	if ( rect.x2 > x2 ) x2 = rect.x2;
	if ( rect.y1 < y1 ) y1 = rect.y1;
	if ( rect.y2 > y2 ) y2 = rect.y2;
#endif
}

/*
======================
idScreenRect::Equals
======================
*/
bool idScreenRect::Equals( const idScreenRect &rect ) const 
{
#if ID_USE_INSTRINSEC
	// Load the values ​​of the two rectangles into SIMD vectors
	__m128i rect1 = _mm_set_epi32(y2, y1, x2, x1);
	__m128i rect2 = _mm_set_epi32(rect.y2, rect.y1, rect.x2, rect.x1);

	// Compares values ​​(returns 0xFFFFFFFF for equal and 0x00000000 for different)
	__m128i cmp = _mm_cmpeq_epi32(rect1, rect2);

	// Creates a bitwise mask (if all are the same, it will be 0xFFFFFFFF)
	return (_mm_movemask_epi8(cmp) == 0xFFFF);
#else
	return ( x1 == rect.x1 && x2 == rect.x2 && y1 == rect.y1 && y2 == rect.y2 );
#endif //
}

/*
======================
idScreenRect::IsEmpty
======================
*/
bool idScreenRect::IsEmpty( void ) const 
{
	return ( x1 > x2 || y1 > y2 );
}
