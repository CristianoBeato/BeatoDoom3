/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
Copyright (C) 2016-2024 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

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

#include "renderer/renderer_common.h"
#include "Transform.h"

// BEATO Begin:
// TODO: move to a common header 
#include <smmintrin.h> // SSSE 3 header  
#include <immintrin.h> // for FMA 
// BEATO End

void crTransform::AxisToModelMatrix( const idMat3 &axis, const idVec3 &origin, float modelMatrix[16] ) 
{
#if ID_USE_INSTRINSEC
	__m128 row1 = _mm_set_ps(0.0f, axis[0][2], axis[0][1], axis[0][0]);
	__m128 row2 = _mm_set_ps(0.0f, axis[1][2], axis[1][1], axis[1][0]);
	__m128 row3 = _mm_set_ps(0.0f, axis[2][2], axis[2][1], axis[2][0]);
	__m128 row4 = _mm_set_ps(1.0f, origin[2], origin[1], origin[0]);

	_mm_storeu_ps(&modelMatrix[0], row1);
	_mm_storeu_ps(&modelMatrix[4], row2);
	_mm_storeu_ps(&modelMatrix[8], row3);
	_mm_storeu_ps(&modelMatrix[12], row4);
#else
	modelMatrix[0] = axis[0][0];
	modelMatrix[4] = axis[1][0];
	modelMatrix[8] = axis[2][0];
	modelMatrix[12] = origin[0];

	modelMatrix[1] = axis[0][1];
	modelMatrix[5] = axis[1][1];
	modelMatrix[9] = axis[2][1];
	modelMatrix[13] = origin[1];

	modelMatrix[2] = axis[0][2];
	modelMatrix[6] = axis[1][2];
	modelMatrix[10] = axis[2][2];
	modelMatrix[14] = origin[2];

	modelMatrix[3] = 0;
	modelMatrix[7] = 0;
	modelMatrix[11] = 0;
	modelMatrix[15] = 1;
#endif
}


// FIXME: these assume no skewing or scaling transforms
void crTransform::LocalPointToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out ) 
{
#if ID_USE_INSTRINSEC
	__m128 matCol1 = _mm_loadu_ps(&modelMatrix[0]);
	__m128 matCol2 = _mm_loadu_ps(&modelMatrix[4]);
	__m128 matCol3 = _mm_loadu_ps(&modelMatrix[8]);
	__m128 matCol4 = _mm_loadu_ps(&modelMatrix[12]);

	__m128 inVec = _mm_set_ps(1.0f, in[2], in[1], in[0]);

#if 1
	__m128 res = _mm_add_ps( _mm_add_ps(_mm_mul_ps(matCol1, _mm_set1_ps(in[0])), _mm_mul_ps(matCol2, _mm_set1_ps(in[1]))),
		_mm_add_ps(_mm_mul_ps(matCol3, _mm_set1_ps(in[2])), matCol4) );
#else
	__m128 res = _mm_fmadd_ps( _mm_set1_ps(in[0]), matCol1, _mm_fmadd_ps(_mm_set1_ps(in[1]), matCol2, _mm_fmadd_ps(_mm_set1_ps(in[2]), matCol3, matCol4)));
#endif 

	out[0] = res[0];
	out[1] = res[1];
	out[2] = res[2];
#else
	out[0] = in[0] * modelMatrix[0] + in[1] * modelMatrix[4] + in[2] * modelMatrix[8] + modelMatrix[12];
	out[1] = in[0] * modelMatrix[1] + in[1] * modelMatrix[5] + in[2] * modelMatrix[9] + modelMatrix[13];
	out[2] = in[0] * modelMatrix[2] + in[1] * modelMatrix[6] + in[2] * modelMatrix[10] + modelMatrix[14];
#endif
}

void crTransform::PointTimesMatrix( const float modelMatrix[16], const idVec4 &in, idVec4 &out ) 
{
#if 0 //ID_USE_INSTRINSEC
	__m128 matCol1 = _mm_loadu_ps( &modelMatrix[0] );
	__m128 matCol2 = _mm_loadu_ps( &modelMatrix[4] );
	__m128 matCol3 = _mm_loadu_ps( &modelMatrix[8] );
	__m128 matCol4 = _mm_loadu_ps( &modelMatrix[12] );

	__m128 inVec = _mm_set_ps(in[3], in[2], in[1], in[0]);


	// todo: option to FMA 
	__m128 res = _mm_fmadd_ps(_mm_set1_ps(in[0]), matCol1, _mm_fmadd_ps(_mm_set1_ps(in[1]), matCol2, _mm_fmadd_ps(_mm_set1_ps(in[2]), matCol3, _mm_mul_ps(_mm_set1_ps(in[3]), matCol4))));

	_mm_storeu_ps(&out[0], res);
#else
	out[0] = in[0] * modelMatrix[0] + in[1] * modelMatrix[4] + in[2] * modelMatrix[8] + modelMatrix[12];
	out[1] = in[0] * modelMatrix[1] + in[1] * modelMatrix[5] + in[2] * modelMatrix[9] + modelMatrix[13];
	out[2] = in[0] * modelMatrix[2] + in[1] * modelMatrix[6] + in[2] * modelMatrix[10] + modelMatrix[14];
	out[3] = in[0] * modelMatrix[3] + in[1] * modelMatrix[7] + in[2] * modelMatrix[11] + modelMatrix[15];
#endif
}

void crTransform::GlobalPointToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out ) 
{
#if ID_USE_INSTRINSEC
	__m128 matCol1 = _mm_loadu_ps(&modelMatrix[0]);
	__m128 matCol2 = _mm_loadu_ps(&modelMatrix[4]);
	__m128 matCol3 = _mm_loadu_ps(&modelMatrix[8]);
	__m128 matCol4 = _mm_loadu_ps(&modelMatrix[12]);

	__m128 inVec = _mm_set_ps(1.0f, in[2], in[1], in[0]);
	__m128 temp = _mm_sub_ps(inVec, matCol4);

	__m128 resX = _mm_mul_ps(temp, matCol1);
	__m128 resY = _mm_mul_ps(temp, matCol2);
	__m128 resZ = _mm_mul_ps(temp, matCol3);

	resX = _mm_hadd_ps(resX, resX);
	resX = _mm_hadd_ps(resX, resX);
	resY = _mm_hadd_ps(resY, resY);
	resY = _mm_hadd_ps(resY, resY);
	resZ = _mm_hadd_ps(resZ, resZ);
	resZ = _mm_hadd_ps(resZ, resZ);

	out[0] = _mm_cvtss_f32(resX);
	out[1] = _mm_cvtss_f32(resY);
	out[2] = _mm_cvtss_f32(resZ);
#else
	idVec3	temp;

	VectorSubtract( in, &modelMatrix[12], temp );

	out[0] = DotProduct( temp, &modelMatrix[0] );
	out[1] = DotProduct( temp, &modelMatrix[4] );
	out[2] = DotProduct( temp, &modelMatrix[8] );
#endif
}

void crTransform::LocalVectorToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out ) 
{
#if ID_USE_INSTRINSEC
	__m128 matCol1 = _mm_loadu_ps(&modelMatrix[0]);
	__m128 matCol2 = _mm_loadu_ps(&modelMatrix[4]);
	__m128 matCol3 = _mm_loadu_ps(&modelMatrix[8]);

#if 1
	__m128 res = _mm_add_ps( _mm_add_ps( _mm_mul_ps( _mm_set1_ps( in[0] ), matCol1 ) , _mm_mul_ps( _mm_set1_ps( in[1] ), matCol2 ) ), _mm_mul_ps( _mm_set1_ps( in[2] ), matCol3 ) );
#else
	__m128 res = _mm_fmadd_ps(_mm_set1_ps(in[0]), matCol1, _mm_fmadd_ps(_mm_set1_ps(in[1]), matCol2,  _mm_mul_ps(_mm_set1_ps(in[2]), matCol3)));
#endif

	out[0] = res[0];
	out[1] = res[1];
	out[2] = res[2];
#else
	out[0] = in[0] * modelMatrix[0] + in[1] * modelMatrix[4] + in[2] * modelMatrix[8];
	out[1] = in[0] * modelMatrix[1] + in[1] * modelMatrix[5] + in[2] * modelMatrix[9];
	out[2] = in[0] * modelMatrix[2] + in[1] * modelMatrix[6] + in[2] * modelMatrix[10];
#endif
}

void crTransform::GlobalVectorToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out ) 
{
	out[0] = DotProduct( in, &modelMatrix[0] );
	out[1] = DotProduct( in, &modelMatrix[4] );
	out[2] = DotProduct( in, &modelMatrix[8] );
}

void crTransform::GlobalPlaneToLocal( const float modelMatrix[16], const idPlane &in, idPlane &out ) 
{
	out[0] = DotProduct( in, &modelMatrix[0] );
	out[1] = DotProduct( in, &modelMatrix[4] );
	out[2] = DotProduct( in, &modelMatrix[8] );
	out[3] = in[3] + modelMatrix[12] * in[0] + modelMatrix[13] * in[1] + modelMatrix[14] * in[2];
}

void crTransform::LocalPlaneToGlobal( const float modelMatrix[16], const idPlane &in, idPlane &out ) 
{
	float	offset;

	LocalVectorToGlobal( modelMatrix, in.Normal(), out.Normal() );

	offset = modelMatrix[12] * out[0] + modelMatrix[13] * out[1] + modelMatrix[14] * out[2];
	out[3] = in[3] - offset;
}

// transform Z in eye coordinates to window coordinates
void crTransform::TransformEyeZToWin( float src_z, const float *projectionMatrix, float &dst_z ) 
{
	float clip_z, clip_w;

	// projection
	clip_z = src_z * projectionMatrix[ 2 + 2 * 4 ] + projectionMatrix[ 2 + 3 * 4 ];
	clip_w = src_z * projectionMatrix[ 3 + 2 * 4 ] + projectionMatrix[ 3 + 3 * 4 ];

	if ( clip_w <= 0.0f ) {
		dst_z = 0.0f;					// clamp to near plane
	} else {
		dst_z = clip_z / clip_w;
		dst_z = dst_z * 0.5f + 0.5f;	// convert to window coords
	}
}

/*
==========================
crTransform::TransformModelToClip
==========================
*/
void crTransform::TransformModelToClip( const idVec3 &src, const float *modelMatrix, const float *projectionMatrix, idPlane &eye, idPlane &dst ) 
{
	int i;

	for ( i = 0 ; i < 4 ; i++ ) 
	{
		eye[i] = 
			src[0] * modelMatrix[ i + 0 * 4 ] +
			src[1] * modelMatrix[ i + 1 * 4 ] +
			src[2] * modelMatrix[ i + 2 * 4 ] +
			1 * modelMatrix[ i + 3 * 4 ];
	}

	for ( i = 0 ; i < 4 ; i++ ) {
		dst[i] = 
			eye[0] * projectionMatrix[ i + 0 * 4 ] +
			eye[1] * projectionMatrix[ i + 1 * 4 ] +
			eye[2] * projectionMatrix[ i + 2 * 4 ] +
			eye[3] * projectionMatrix[ i + 3 * 4 ];
	}
}

/*
==========================
crTransform::TransformClipToDevice

Clip to normalized device coordinates
==========================
*/
void crTransform::TransformClipToDevice( const idPlane &clip, idVec3 &normalized ) 
{
	normalized[0] = clip[0] / clip[3];
	normalized[1] = clip[1] / clip[3];
	normalized[2] = clip[2] / clip[3];
}

/*
==========================
crTransform::myGlMultMatrix
==========================
*/
void crTransform::GlMultMatrix( const float a[16], const float b[16], float out[16] ) 
{
#if ID_USE_INSTRINSEC
	// load the matrix B rows 
	__m128 row1 = _mm_loadu_ps(&b[0]); 
	__m128 row2 = _mm_loadu_ps(&b[4]); 
	__m128 row3 = _mm_loadu_ps(&b[8]); 
	__m128 row4 = _mm_loadu_ps(&b[12]);

	for (int i = 0; i < 4; i++) 
	{
		// load the row of A
		__m128 a_row = _mm_loadu_ps(&a[i * 4]);
	
		// multiply the row of A with the columns of B
		__m128 col1 = _mm_set1_ps(a_row[0]);
		__m128 col2 = _mm_set1_ps(a_row[1]);
		__m128 col3 = _mm_set1_ps(a_row[2]);
		__m128 col4 = _mm_set1_ps(a_row[3]);
	
		// multiply and add the results
		__m128 res = _mm_add_ps(
			_mm_add_ps(_mm_mul_ps(col1, row1), _mm_mul_ps(col2, row2)),
			_mm_add_ps(_mm_mul_ps(col3, row3), _mm_mul_ps(col4, row4))
		);
	
		// store the result
		_mm_storeu_ps(&out[i * 4], res);
	}
#elif 1
	int		i, j;

	for ( i = 0 ; i < 4 ; i++ ) {
		for ( j = 0 ; j < 4 ; j++ ) {
			out[ i * 4 + j ] =
				a [ i * 4 + 0 ] * b [ 0 * 4 + j ]
				+ a [ i * 4 + 1 ] * b [ 1 * 4 + j ]
				+ a [ i * 4 + 2 ] * b [ 2 * 4 + j ]
				+ a [ i * 4 + 3 ] * b [ 3 * 4 + j ];
		}
	}

#else
	out[0*4+0] = a[0*4+0]*b[0*4+0] + a[0*4+1]*b[1*4+0] + a[0*4+2]*b[2*4+0] + a[0*4+3]*b[3*4+0];
	out[0*4+1] = a[0*4+0]*b[0*4+1] + a[0*4+1]*b[1*4+1] + a[0*4+2]*b[2*4+1] + a[0*4+3]*b[3*4+1];
	out[0*4+2] = a[0*4+0]*b[0*4+2] + a[0*4+1]*b[1*4+2] + a[0*4+2]*b[2*4+2] + a[0*4+3]*b[3*4+2];
	out[0*4+3] = a[0*4+0]*b[0*4+3] + a[0*4+1]*b[1*4+3] + a[0*4+2]*b[2*4+3] + a[0*4+3]*b[3*4+3];

	out[1*4+0] = a[1*4+0]*b[0*4+0] + a[1*4+1]*b[1*4+0] + a[1*4+2]*b[2*4+0] + a[1*4+3]*b[3*4+0];
	out[1*4+1] = a[1*4+0]*b[0*4+1] + a[1*4+1]*b[1*4+1] + a[1*4+2]*b[2*4+1] + a[1*4+3]*b[3*4+1];
	out[1*4+2] = a[1*4+0]*b[0*4+2] + a[1*4+1]*b[1*4+2] + a[1*4+2]*b[2*4+2] + a[1*4+3]*b[3*4+2];
	out[1*4+3] = a[1*4+0]*b[0*4+3] + a[1*4+1]*b[1*4+3] + a[1*4+2]*b[2*4+3] + a[1*4+3]*b[3*4+3];
	
	out[2*4+0] = a[2*4+0]*b[0*4+0] + a[2*4+1]*b[1*4+0] + a[2*4+2]*b[2*4+0] + a[2*4+3]*b[3*4+0];
	out[2*4+1] = a[2*4+0]*b[0*4+1] + a[2*4+1]*b[1*4+1] + a[2*4+2]*b[2*4+1] + a[2*4+3]*b[3*4+1];
	out[2*4+2] = a[2*4+0]*b[0*4+2] + a[2*4+1]*b[1*4+2] + a[2*4+2]*b[2*4+2] + a[2*4+3]*b[3*4+2];
	out[2*4+3] = a[2*4+0]*b[0*4+3] + a[2*4+1]*b[1*4+3] + a[2*4+2]*b[2*4+3] + a[2*4+3]*b[3*4+3];
	
	out[3*4+0] = a[3*4+0]*b[0*4+0] + a[3*4+1]*b[1*4+0] + a[3*4+2]*b[2*4+0] + a[3*4+3]*b[3*4+0];
	out[3*4+1] = a[3*4+0]*b[0*4+1] + a[3*4+1]*b[1*4+1] + a[3*4+2]*b[2*4+1] + a[3*4+3]*b[3*4+1];
	out[3*4+2] = a[3*4+0]*b[0*4+2] + a[3*4+1]*b[1*4+2] + a[3*4+2]*b[2*4+2] + a[3*4+3]*b[3*4+2];
	out[3*4+3] = a[3*4+0]*b[0*4+3] + a[3*4+1]*b[1*4+3] + a[3*4+2]*b[2*4+3] + a[3*4+3]*b[3*4+3];
#endif
}

/*
================
R_TransposeGLMatrix
================
*/
void crTransform::TransposeGLMatrix( const float in[16], float out[16] ) 
{
#if ID_USE_INSTRINSEC

	// load matrix rows into SSE registers
	__m128 row1 = _mm_loadu_ps(&in[0]); 
	__m128 row2 = _mm_loadu_ps(&in[4]); 
	__m128 row3 = _mm_loadu_ps(&in[8]); 
	__m128 row4 = _mm_loadu_ps(&in[12]);
	
	// perform the transpose
	_MM_TRANSPOSE4_PS(row1, row2, row3, row4);
	
	// store the transposed matrix back to memory
	_mm_storeu_ps(&out[0], row1);
	_mm_storeu_ps(&out[4], row2);
	_mm_storeu_ps(&out[8], row3);
	_mm_storeu_ps(&out[12], row4);

#else
	int		i, j;

	for ( i = 0 ; i < 4 ; i++ ) {
		for ( j = 0 ; j < 4 ; j++ ) {
			out[i*4+j] = in[j*4+i];
		}
	}
#endif
}
