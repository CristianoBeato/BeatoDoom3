/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2016 Johannes Ohlemacher (http://github.com/eXistence/fhDOOM)
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

#if defined( OPENGL ) 
#extension GL_ARB_separate_shader_objects : enable
#endif

#extension GL_GOOGLE_include_directive : enable

#define VERTEX
#include "shader_common.inc"

// vertex 
layout( location = 0 ) in vec3 attrb_position;
layout( location = 1 ) in vec2 attrb_texcoord;
layout( location = 2 ) in vec3 attrb_normal;
layout( location = 3 ) in vec4 attrb_color;
layout( location = 4 ) in vec3 attrb_binormal;
layout( location = 5 ) in vec3 attrb_tangent;


// vertex shader storage buffer 
layout( std430, binding = 1 ) buffer vertexStorageBlock
{
  vetexTransform vertUnifom[];
};

// vertex shader to fragment output variables
layout( location = 0 ) out vs_output
{
  vec4 vcolor;
} result;

void main(void)
{
  //
  mat4 mvp = vertUnifom[gl_DrawID].rpModelMatrix * vertUnifom[gl_DrawID].rpViewMatrix * vertUnifom[gl_DrawID].rpProjectionMatrix;
  
  //
  vec4 vert = mvp * vec4( attrb_position, 1.0);

  // primitive raster 
  gl_Position = vert;

  // our fake scissor rect
  vec4 scissor = vertUnifom[gl_DrawID].rpClipBounds;
  Scissor( vert, scissor );

  // gl_Position is in clip space (-w to w)
  // Then we scissor transform to this space ( assuming normalization )
  gl_ClipDistance[0] = vert.x - scissor.x;         // x >= scissor.x
  gl_ClipDistance[1] = scissor.z - vert.x;         // x <= scissor.z
  gl_ClipDistance[2] = vert.y - scissor.y;         // y >= scissor.y
  gl_ClipDistance[3] = scissor.w - vert.y;         // y <= scissor.w

  result.vcolor = ( attrb_color / 255.0 ) * vertUnifom[gl_DrawID].rpColorModulate + vertUnifom[gl_DrawID].rpColorAdd;
}