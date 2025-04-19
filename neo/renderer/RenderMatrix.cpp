
#include "precompiled.h"
#include "renderer/renderer_common.h"
#include "RenderMatrix.h"

#include <xmmintrin.h>

// row major 
//{ 00, 01, 02, 03 }		{ [0][0], [0][1], [0][2], [0][3] }
//{ 04, 05, 06, 07 }   =   	{ [1][0], [1][1], [1][2], [1][3] }
//{ 08, 09, 10, 11 }  		{ [2][0], [2][1], [2][2], [2][3] }
//{ 12, 13, 14, 15 }		{ [3][0], [3][1], [3][2], [3][3] }

// colum major
//{ 00, 04, 08, 12 }		{ [0][0], [0][1], [0][2], [0][3] }
//{ 01, 05, 09, 13 }   =	{ [1][0], [1][1], [1][2], [1][3] }
//{ 02, 06, 10, 14 }		{ [2][0], [2][1], [2][2], [2][3] }
//{ 03, 07, 11, 15 }		{ [3][0], [3][1], [3][2], [3][3] }

static inline __m128 _mm_madd_ps( __m128 a, __m128 b, __m128 c )
{
	return _mm_add_ps( _mm_mul_ps( (a), (b) ), (c) );
}

static inline __m128 _mm_splat_ps( __m128 x, int i )
{
	return reinterpret_cast<__m128>( _mm_shuffle_epi32( reinterpret_cast<__m128i>( x ), _MM_SHUFFLE( i, i, i, i ) ) );
}

crRenderMatrix::crRenderMatrix(void)
{
	Identity();	
}

crRenderMatrix::~crRenderMatrix( void )
{
	Zero();
}

void crRenderMatrix::Identity(void)
{
#if ID_USE_INSTRINSEC
	_mm_store_ps( mat[0], _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f) ); // 1 0 0 0
	_mm_store_ps( mat[1], _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f) ); // 0 1 0 0
	_mm_store_ps( mat[2], _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f) ); // 0 0 1 0
	_mm_store_ps( mat[3], _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f) ); // 0 0 0 1
#else
	mat[0][0] = 1.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[0][3] = 0.0f;

	mat[1][0] = 0.0f;
	mat[1][1] = 1.0f;
	mat[1][2] = 0.0f;
	mat[1][3] = 0.0f;

	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = 1.0f;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 1.0f;
#endif
}

void crRenderMatrix::Zero(void)
{
#if ID_USE_INSTRINSEC
	_mm_store_ps( mat[0], _mm_setzero_ps() );
	_mm_store_ps( mat[1], _mm_setzero_ps() );
	_mm_store_ps( mat[1], _mm_setzero_ps() );
	_mm_store_ps( mat[2], _mm_setzero_ps() );
#else
	memset( &mat[0], 0x00, sizeof(float) * 16 );
#endif
}

//idVec3 crRenderMatrix::LocalPointToGlobal(const idVec3 &in)
//{
//	idVec3 out = idVec3();
//    return out;
//}

//idVec3 crRenderMatrix::GlobalPointToLocal(const idVec3 &in)
//{
//	idVec3 out = idVec3();
//    return out;
//}

idVec3 crRenderMatrix::LocalVectorToGlobal( const idVec3 &in ) const
{
	idVec4 out = idVec4();

#if ID_USE_INSTRINSEC
	idVec4 iv = idVec4( in.x, in.y, in.z, 0.0f );

#else
	out[0] = in[0] * mat[0][0] + in[1] * mat[0][1] + in[2] * mat[0][2];
	out[1] = in[0] * mat[1][0] + in[1] * mat[1][1] + in[2] * mat[1][2];
	out[2] = in[0] * mat[2][0] + in[1] * mat[2][1] + in[2] * mat[2][2];
#endif 

return out.ToVec3();
}


idVec3 crRenderMatrix::GlobalVectorToLocal(const idVec3 &in) const
{
	idVec4 inv = idVec4( in.x, in.y, in.z, 0.0f );
	idVec4 out = idVec4();

#if ID_USE_INSTRINSEC

	//
	__m128 i = _mm_load_ps( inv.ToFloatPtr() );

	// load matrix colums 
	__m128 col0 = _mm_load_ps( mat[0] );
	__m128 col1 = _mm_load_ps( mat[1] );
	__m128 col2 = _mm_load_ps( mat[2] );
	__m128 col3 = _mm_load_ps( mat[3] );

	// transpose to rows 
	__m128 row0 = _mm_unpacklo_ps( col0, col1 );
	__m128 row1 = _mm_unpackhi_ps( col0, col1 );
	__m128 row2 = _mm_unpacklo_ps( col2, col3 );
	__m128 row3 = _mm_unpackhi_ps( col2, col3 );

	//
	__m128 ir0 = _mm_splat_ps( i , 0 );
	__m128 ir1 = _mm_splat_ps( i , 1 );
	__m128 ir2 = _mm_splat_ps( i , 2 );
	__m128 ir3 = _mm_splat_ps( i , 3 );

	// perform multiplication and addiction
	__m128 sum0 = _mm_add_ps( _mm_mul_ps( ir0, row0 ), _mm_mul_ps( ir1, row1 ) );
	__m128 sum1 = _mm_add_ps( _mm_mul_ps( ir2, row2 ), _mm_mul_ps( ir3, row3 ) );
	__m128 res = _mm_add_ps( sum0, sum1 );

	// store vector 
	_mm_store_ps( out.ToFloatPtr(), res );
#else
	out[0] = inv[0] * mat[0][0] + inv[1] * mat[1][0] + inv[2] * mat[2][0] + inv[3] * mat[3][0];
	out[1] = inv[0] * mat[0][1] + inv[1] * mat[1][1] + inv[2] * mat[2][1] + inv[3] * mat[3][1];
	out[2] = inv[0] * mat[0][2] + inv[1] * mat[1][2] + inv[2] * mat[2][2] + inv[3] * mat[3][2];
	out[3] = inv[0] * mat[0][3] + inv[1] * mat[1][3] + inv[2] * mat[2][3] + inv[3] * mat[3][3];
#endif

    return out.ToVec3();
}

idPlane crRenderMatrix::GlobalPlaneToLocal( const idPlane &in ) const
{
	idPlane out = idPlane();
	idVec3 row1( mat[0][0], mat[0][2], mat[0][3] );
	idVec3 row2( mat[1][0], mat[1][2], mat[1][3] );
	idVec3 row3( mat[2][0], mat[2][2], mat[2][3] );

	out[0] = DotProduct( in, row1 );
	out[1] = DotProduct( in, row2 );
	out[2] = DotProduct( in, row3 );
	out[3] = in[3] + mat[0][3] * in[0] + mat[1][3] * in[1] + mat[2][3] * in[2];

	return out;
}

idPlane crRenderMatrix::LocalPlaneToGlobal( const idPlane &in ) const
{
	float	offset = 0.0f;
	idPlane out = idPlane();
	out.Normal() = LocalVectorToGlobal( in.Normal() );

	// offset = mat[12] * out[0] + mat[13] * out[1] + mat[14] * out[2];
	offset = mat[0][3] * out[0] + mat[1][3] * out[1] + mat[2][3] * out[2];
	out[3] = in[3] - offset;
	
    return out;
}

float crRenderMatrix::TransformEyeZToWin( const float srcz ) const
{
	float clip_z = 0.0f, clip_w = 0.0f, dst_z = 0.0f;

	// projection
	// clip_z = srcz * mat[ 2 + 2 * 4 ] + mat[ 2 + 3 * 4 ];
	// clip_w = srcz * mat[ 3 + 2 * 4 ] + mat[ 3 + 3 * 4 ];
	clip_z = srcz * mat[2][2] + mat[3][2];
	clip_w = srcz * mat[2][3] + mat[3][3];

	if ( clip_w <= 0.0f ) 
	{
		dst_z = 0.0f;					// clamp to near plane
	} 
	else 
	{
		dst_z = clip_z / clip_w;
		dst_z = dst_z * 0.5f + 0.5f;	// convert to window coords
	}

	return dst_z;
}

idVec4 crRenderMatrix::operator*( const idVec4 &in ) const
{
	idVec4 out;
#if ID_USE_INSTRINSEC
	
	// Load colums of the matrix 
	__m128 col0 = _mm_loadu_ps( mat[0] );
	__m128 col1 = _mm_loadu_ps( mat[1] );
	__m128 col2 = _mm_loadu_ps( mat[2] );
	__m128 col3 = _mm_loadu_ps( mat[3] );

	// load vector 
	__m128 inVec = _mm_load_ps( in.ToFloatPtr() );

	// 
	__m128 iR0 = _mm_splat_ps( inVec , 0 );
	__m128 iR1 = _mm_splat_ps( inVec , 1 );
	__m128 iR2 = _mm_splat_ps( inVec , 2 );
	__m128 iR3 = _mm_splat_ps( inVec , 3 );

	// multiply vector by colunms 
	__m128 mul0 = _mm_mul_ps( col0, iR0 );
	__m128 mul1 = _mm_mul_ps( col1, iR1 );
	__m128 mul2 = _mm_mul_ps( col2, iR2 );
	__m128 mul3 = _mm_mul_ps( col3, iR3 );

	// perform addiction 
	__m128 result = _mm_add_ps( _mm_add_ps( mul0, mul1 ), _mm_add_ps( mul2, mul3 ) );

	_mm_storeu_ps( out.ToFloatPtr(), result);
#else
	//  O		  MULA			  MULB			    MULC    
	out[0] = in[0] * mat[0][0] + in[1] * mat[0][1] + in[2] * mat[0][2] + in[3] * mat[0][3];
	out[1] = in[0] * mat[1][0] + in[1] * mat[1][1] + in[2] * mat[1][2] + in[3] * mat[1][3];
	out[2] = in[0] * mat[2][0] + in[1] * mat[2][1] + in[2] * mat[2][2] + in[3] * mat[2][3];
	out[3] = in[0] * mat[3][0] + in[1] * mat[3][1] + in[2] * mat[3][2] + in[3] * mat[3][3];
#endif
}

crRenderMatrix crRenderMatrix::operator*( const crRenderMatrix &in ) const
{
	crRenderMatrix out = crRenderMatrix();
#if ID_USE_INSTRINSEC
	__m128 t0;
	__m128 t1;
	__m128 t2;
	__m128 t3;

	// Load columns from A (A[col]) as 4 floats (a vector)
	__m128 a0 = _mm_load_ps( mat[0] );
	__m128 a1 = _mm_load_ps( mat[1] );
	__m128 a2 = _mm_load_ps( mat[2] );
	__m128 a3 = _mm_load_ps( mat[3] );
	
	// Load columns from B (B[col]) as 4 floats (a vector)
	__m128 b0 = _mm_load_ps( in.mat[0] );
	__m128 b1 = _mm_load_ps( in.mat[1] );
	__m128 b2 = _mm_load_ps( in.mat[2] );
	__m128 b3 = _mm_load_ps( in.mat[3] );

	t0 = _mm_mul_ps( _mm_splat_ps( a0, 0 ), b0 );
	t1 = _mm_mul_ps( _mm_splat_ps( a1, 0 ), b0 );
	t2 = _mm_mul_ps( _mm_splat_ps( a2, 0 ), b0 );
	t3 = _mm_mul_ps( _mm_splat_ps( a3, 0 ), b0 );

#if 0

	t0 = _mm_madd_ps( _mm_splat_ps( a0, 1 ), b1, t0 );
	t1 = _mm_madd_ps( _mm_splat_ps( a1, 1 ), b1, t1 );
	t2 = _mm_madd_ps( _mm_splat_ps( a2, 1 ), b1, t2 );
	t3 = _mm_madd_ps( _mm_splat_ps( a3, 1 ), b1, t3 );
	
	t0 = _mm_madd_ps( _mm_splat_ps( a0, 2 ), b2, t0 );
	t1 = _mm_madd_ps( _mm_splat_ps( a1, 2 ), b2, t1 );
	t2 = _mm_madd_ps( _mm_splat_ps( a2, 2 ), b2, t2 );
	t3 = _mm_madd_ps( _mm_splat_ps( a3, 2 ), b2, t3 );
	
	t0 = _mm_madd_ps( _mm_splat_ps( a0, 3 ), b3, t0 );
	t1 = _mm_madd_ps( _mm_splat_ps( a1, 3 ), b3, t1 );
	t2 = _mm_madd_ps( _mm_splat_ps( a2, 3 ), b3, t2 );
	t3 = _mm_madd_ps( _mm_splat_ps( a3, 3 ), b3, t3 );

#else

	t0 = _mm_fmadd_ps( _mm_splat_ps( a0, 1 ), b1, t0 );
	t1 = _mm_fmadd_ps( _mm_splat_ps( a1, 1 ), b1, t1 );
	t2 = _mm_fmadd_ps( _mm_splat_ps( a2, 1 ), b1, t2 );
	t3 = _mm_fmadd_ps( _mm_splat_ps( a3, 1 ), b1, t3 );

	t0 = _mm_fmadd_ps( _mm_splat_ps( a0, 2 ), b2, t0 );
	t1 = _mm_fmadd_ps( _mm_splat_ps( a1, 2 ), b2, t1 );
	t2 = _mm_fmadd_ps( _mm_splat_ps( a2, 2 ), b2, t2 );
	t3 = _mm_fmadd_ps( _mm_splat_ps( a3, 2 ), b2, t3 );

	t0 = _mm_fmadd_ps( _mm_splat_ps( a0, 3 ), b3, t0 );
	t1 = _mm_fmadd_ps( _mm_splat_ps( a1, 3 ), b3, t1 );
	t2 = _mm_fmadd_ps( _mm_splat_ps( a2, 3 ), b3, t2 );
	t3 = _mm_fmadd_ps( _mm_splat_ps( a3, 3 ), b3, t3 );

#endif

#else

    for ( int col = 0; col < 4; ++col ) 
	{
        for ( int row = 0; row < 4; ++row ) 
		{
            out.mat[col][row] = 0.0f;
            for (int k = 0; k < 4; ++k) 
			{
                out.mat[col][row] += mat[k][row] * in.mat[col][k];
            }
        }
    }

#endif
    return out;
}

crRenderMatrix crRenderMatrix::Transpose(void) const
{
	crRenderMatrix out;
#if ID_USE_INSTRINSEC

	// load matrix rows into SSE registers
	__m128 r0 = _mm_load_ps( mat[0] ); 
	__m128 r1 = _mm_load_ps( mat[1] ); 
	__m128 r2 = _mm_load_ps( mat[2] ); 
	__m128 r3 = _mm_load_ps( mat[3] );

	// perform the transpose
	__m128 t0 = _mm_unpacklo_ps( r0, r1 );
	__m128 t1 = _mm_unpackhi_ps( r0, r1 );
	__m128 t2 = _mm_unpacklo_ps( r2, r3 );
	__m128 t3 = _mm_unpackhi_ps( r2, r3 );

	// store the transposed matrix back to memory
	_mm_store_ps( out.mat[0], t0 );
	_mm_store_ps( out.mat[1], t1 );
	_mm_store_ps( out.mat[2], t2 );
	_mm_store_ps( out.mat[3], t3 );

#else

	for (int row = 0; row < 4; ++row) 
	{
		for (int col = 0; col < 4; ++col) 
		{
			// Swap the row and column indices to transpose
			out.mat[col][row] = mat[row][col];
		}
	}
	
#endif
	return out;
}
