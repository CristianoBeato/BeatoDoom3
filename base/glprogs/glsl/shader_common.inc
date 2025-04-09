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

//######################################################
//
//  VERTEX SHADER UTILS
//
//######################################################
#ifdef VERTEX 

// our vertex output structure 
out gl_PerVertex
{
    vec4  gl_Position;
    float gl_ClipDistance[4];
};

// vertex transform block 
struct vetexTransform
{
  vec4 rpLocalViewOrigin; 
  vec4 rpColorModulate;   
  vec4 rpColorAdd;        
  vec4 rpClipBounds;
  mat4 rpTextureMatrix;
  mat4 rpModelMatrix;
  mat4 rpViewMatrix;
  mat4 rpProjectionMatrix;
};

// our "intance" clipping alternative
void Scissor( vec4 clipPos, vec4 scissor )
{
  // gl_Position is in clip space (-w to w)
  // Then we scissor transform to this space ( assuming normalization )
  gl_ClipDistance[0] = clipPos.x - scissor.x; // x >= scissor.x
  gl_ClipDistance[1] = scissor.z - clipPos.x; // x <= scissor.z
  gl_ClipDistance[2] = clipPos.y - scissor.y; // y >= scissor.y
  gl_ClipDistance[3] = scissor.w - clipPos.y; // y <= scissor.w
}

#endif

//######################################################
//
//  GEOMETRY SHADER UTILS
//
//######################################################
#ifdef GEOMETRY

#endif 

//######################################################
//
//  FRAGMENT SHADER UTILS
//
//######################################################
#ifdef FRAGMENT

// fragment shader storage structure 
struct fragmentTransfom
{
    // we can acess the max of 8 textures from the sampler buffer 
    uint        samp[8]; 
    vec4        rpCurrentRenderSize;
    vec4        rpDiffuseColor;
    vec4        rpSpecularColor;
    vec4        shaderParm0;
    vec4        shaderParm1;
    vec4        shaderParm2;
    vec4        shaderParm3;
};

vec3 toOpenGlCorrdinates( vec3 v )
{
	return vec3( -v.y, v.z, -v.x );
}

#endif