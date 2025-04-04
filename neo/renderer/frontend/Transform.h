/*
===========================================================================

Beato idTech 4 Source Code
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
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

namespace crTransform
{
    void    AxisToModelMatrix( const idMat3 &axis, const idVec3 &origin, float modelMatrix[16] );
    void    LocalPointToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    void    PointTimesMatrix( const float modelMatrix[16], const idVec4 &in, idVec4 &out );
    void    GlobalPointToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    void    LocalVectorToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    void    GlobalVectorToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    void    GlobalPlaneToLocal( const float modelMatrix[16], const idPlane &in, idPlane &out );
    void    LocalPlaneToGlobal( const float modelMatrix[16], const idPlane &in, idPlane &out );
    void    TransformEyeZToWin( float src_z, const float *projectionMatrix, float &dst_z );
    void    TransformModelToClip( const idVec3 &src, const float *modelMatrix, const float *projectionMatrix, idPlane &eye, idPlane &dst );
    void    TransformClipToDevice( const idPlane &clip, idVec3 &normalized );
    void    GlMultMatrix( const float *a, const float *b, float *out );
    void    TransposeGLMatrix( const float in[16], float out[16] );
};    

#endif //!__TRANSFORM_H__