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

// vertex attributes 
layout(location = 0) in vec3 attrb_position;
layout(location = 1) in vec2 attrb_texcoord;
layout(location = 2) in vec3 attrb_normal;
layout(location = 3) in vec4 attrb_color;
layout(location = 4) in vec3 attrb_binormal;
layout(location = 5) in vec3 attrb_tangent;

// vertex transform block 
struct vetexTransform
{
  vec4 rpLocalViewOrigin; 
  vec4 rpColorModulate;   //
  vec4 rpColorAdd;        //
  vec4 rpTextureMatrixS;
  vec4 rpTextureMatrixT;
  mat4 rpModelMatrix;
  mat4 rpViewMatrix;
  mat4 rpProjectionMatrix;
};

// vertex shader storage buffer 
layout( std430, binding = 0 ) buffer vertexStorageBlock
{
  vetexTransform vertUnifom[];
};

// vertex shader to fragment output variables
out vs_output
{
  vec4 vcolor;
  vec2 vtexcoord;
} result;

void main(void)
{
  //
  mat4 mvp = vertUnifom[gl_DrawID].rpModelMatrix * vertUnifom[gl_DrawID].rpViewMatrix * vertUnifom[gl_DrawID].rpProjectionMatrix;

  gl_Position = mvp * vec4( attrb_position, 1.0 );  

  vec4 vertex_texcoord4 = vec4( attrb_texcoord, 1.0, 1.0 );

  result.vtexcoord = vec2( dot( vertUnifom[gl_DrawID].rpTextureMatrixS, vertex_texcoord4 ), dot( vertUnifom[gl_DrawID].rpTextureMatrixT, vertex_texcoord4 ) );
  result.vcolor = ( attrb_color / 255.0 ) * vertUnifom[gl_DrawID].rpColorModulate + vertUnifom[gl_DrawID].rpColorAdd;
}
