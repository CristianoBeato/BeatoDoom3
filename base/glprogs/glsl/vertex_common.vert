/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2025 Cristiano B. Santos (https://github.com/CristianoBeato/BeatoDoom3)

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#version 460 core

//######################################################
//
//  VERTEX SHADER UTILS
//
//######################################################

// our vertex output structure 
out gl_PerVertex
{
    vec4  gl_Position;
    float gl_ClipDistance[4];
};

struct vetexTransform
{
    vec4    rpColor;
    vec4    rpViewOrigin;
    vec4    rpGlobalEyePos;

    vec4    rpDiffuseModifier;
    vec4    rpSpecularModifier;

    vec4    rpVertexColorModulate;
    vec4    rpVertexColorAdd;

    //vec4  rpTextureMatrixS;
    //vec4  rpTextureMatrixT;
    //vec4  rpBumpMatrixS;
    //vec4  rpBumpMatrixT;
    //vec4  rpDiffuseMatrixS
    //vec4  rpDiffuseMatrixT
    //vec4  rpSpecularMatrixS;
    //vec4  rpSpecularMatrixT;
    mat4x2  rpTextureMatrix;
    mat4x2  rpBumpMatrix;
    mat4x2  rpDiffuseMatrix;
    mat4x2  rpSpecularMatrix;

    //vec4    rpMVPmatrixX;
    //vec4    rpMVPmatrixY;
    //vec4    rpMVPmatrixZ;
    //vec4    rpMVPmatrixW;
    mat4x4  rpMVPmatrix;

    //vec4    rpModelMatrixX;
    //vec4    rpModelMatrixY;
    //vec4    rpModelMatrixZ;
    //vec4    rpModelMatrixW;
    mat4x4  rpModelMatrix;

    //vec4    rpProjectionMatrixX;
    //vec4    rpProjectionMatrixY;
    //vec4    rpProjectionMatrixZ;
    //vec4    rpProjectionMatrixW;
    mat4x4  rpProjectionMatrix;

    //vec4    rpModelViewMatrixX;
    //vec4    rpModelViewMatrixY;
    //vec4    rpModelViewMatrixZ;
    //vec4    rpModelViewMatrixW;
    mat4x4  rpModelViewMatrix;

    //vec4    rpTexGen0S;
    //vec4    rpTexGen0T;
    //vec4    rpTexGen0Q;
    mat4x3  rpTexGen0;
    vec4    rpTexGen0Enabled;

    //vec4    rpTexGen1S;
    //vec4    rpTexGen1T;
    //vec4    rpTexGen1Q;
    mat4x3  rpTexGen1;
    vec4    rpTexGen1Enabled;

    //vec4    rpWobbleSkyX;
    //vec4    rpWobbleSkyY;
    //vec4    rpWobbleSkyZ;
    mat4x3  rpWobbleSky;
};