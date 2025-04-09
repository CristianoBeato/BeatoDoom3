/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 2016 Johannes Ohlemacher (http://github.com/eXistence/fhDOOM)

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

#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_nonuniform_qualifier : enable

#define FRAGMENT // 
#include "shader_common.inc"

layout( set = 0, binding = 0 ) uniform sampler2D samplers[];

// fragment shader storage buffer 
layout( std430, binding = 2 ) buffer fragmentStorageBlock
{
    fragmentTransfom fragUnifom[];
};


layout( location = 0 ) in vs_output
{
  uint drawID;
  vec4 color;
  vec3 cubecoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec2 texcoord;
} frag;

vec3 reflect( vec3 I, vec3 N )
{
  return I - 2.0 * dot( N, I ) * N;  
}

// fragment color output 
layout( location = 0 ) out vec4 fragColor;

void main(void)
{ 
  uint normalID = fragUnifom[frag.drawID].samp[0];
  uint colorID = fragUnifom[frag.drawID].samp[1];

  vec3 localNormal = 2.0 * texture( samplers[normalID], frag.texcoord.st).agb - 1.0;  

  vec3 normal = normalize( frag.normal );
  vec3 tangent = normalize( frag.tangent );    
  vec3 binormal = normalize( frag.binormal );    
  mat3 TBN = mat3(tangent, binormal, normal);  

  vec3 r = reflect(frag.cubecoord, TBN * localNormal);
  fragColor = texture( samplers[colorID], toOpenGlCorrdinates( r ).xy ) * frag.color * fragUnifom[frag.drawID].rpDiffuseColor;
}