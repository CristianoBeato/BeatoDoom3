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

#if OPENGL
# extension GL_ARB_bindless_texture : enable
# extension GL_ARB_shader_storage_buffer_object : enable
# extension GL_ARB_texture_storage : enable
#endif

# extension GL_EXT_nonuniform_qualifier : enable

// Uniform buffer para armazenar os identificadores de texturas
layout(std140, binding = 0) buffer TextureArray 
{
    uint textureHandles[]; // Array de handles de texturas
};

layout( set = 0, binding = 0 ) uniform sampler2D samplers[];

// fragment shader storage structure 
struct fragmentTransfom
{
    uint        samp0[8]; // we can acess the max of 8 textures from the sampler buffer 
    vec4        rpCurrentRenderSize;
    vec4        rpDiffuseColor;
    vec4        rpSpecularColor;
    vec4        shaderParm0;
    vec4        shaderParm1;
    vec4        shaderParm2;
    vec4        shaderParm3;
};

// fragment shader storage buffer 
layout( std430, binding = 2 ) buffer fragmentStorageBlock
{
    fragmentTransfom fragUnifom[];
};

// fragment color output 
layout( location = 0 ) out vec4 fragColor;

// vertex shader to fragment shader variables
layout( location = 0 ) in vs_output
{
  uint drawID;
  vec4 vcolor;
  vec2 vtexcoord;
} frag;

void main(void)
{
  // get the texture index from the uniform buffer
  uint texIndex = fragUnifom[frag.drawID].samp0[0]; 
  
  // get the texture handle from the texture array
  uint textureHandle = textureHandles[texIndex];

  // bind the texture using the handle
  vec4 textureColor = texture( samplers[textureHandle], frag.vtexcoord );
  fragColor = textureColor *  frag.vcolor * clamp( fragUnifom[frag.drawID].rpDiffuseColor, vec4(0,0,0,0), vec4(1,1,1,1));
}