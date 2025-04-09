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

#ifndef __UNIFORM_H__
#define __UNIFORM_H__

enum
{
    // VERTEX SHADER UNIFORMS
    VERTEX_UNIFORM_LOCATION_LOCAL_VIEW_ORIGIN = 0,    // vec4 rpLocalViewOrigin;
    VERTEX_UNIFORM_LOCATION_COLOR_MODULATE,           // vec4 rpColorModulate;
    VERTEX_UNIFORM_LOCATION_COLOR_ADD,                // vec4 rpColorAdd;
    VERTEX_UNIFORM_LOCATION_TEXTURE_MATRIX,           // vec4 rpTextureMatrix;
    VERTEX_UNIFORM_LOCATION_CLIP_BOUDS,               // vec4 rpClipBounds;
    VERTEX_UNIFORM_LOCATION_MODEL_MATRIX,             // mat4 rpModelMatrix;
    VERTEX_UNIFORM_LOCATION_VIEW_MATRIX,              // mat4 rpViewMatrix;
    VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX,        // mat4 rpProjectionMatrix;
    
    // FRAGMENT SHADER UNIFORMS
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS0,              // uint32_t    sampler[0];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS1,              // uint32_t    sampler[1];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS2,              // uint32_t    sampler[2];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS3,              // uint32_t    sampler[3];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS4,              // uint32_t    sampler[4];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS5,              // uint32_t    sampler[5];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS6,              // uint32_t    sampler[6];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS7,              // uint32_t    sampler[7];
    FRAGMENT_UNIFORM_LOCATION_CURRENT_RENDER_SIZE,    // vec4 rpCurrentRenderSize;
    FRAGMENT_UNIFORM_LOCATION_DIFUSE_COLOR,           // vec4 rpDiffuseColor;
    FRAGMENT_UNIFORM_LOCATION_SPECULAR_COLOR,         // vec4 rpSpecularColor;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM0,           // vec4 shaderParm0;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM1,           // vec4 shaderParm1;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM2,           // vec4 shaderParm2;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM3,           // vec4 shaderParm3;
    
    // LIGHT SHADER UNIFORMS
    LIGHT_UNIFORM_LOCATION_LOCAL_ORIGIN,              // vec4 rpLocalLightOrigin;
    LIGHT_UNIFORM_LOCATION_PROJECTION_S,              // vec4 rpLightProjectionS;
    LIGHT_UNIFORM_LOCATION_PROJECTION_T,              // vec4 rpLightProjectionT;
    LIGHT_UNIFORM_LOCATION_PROJECTION_Q,              // vec4 rpLightProjectionQ;
    LIGHT_UNIFORM_LOCATION_FALLOFF,                   // vec4 rpLightFallOff;
    MAX_UNIFORMS
};

class crUniform
{
public:
    crUniform( void );
    ~crUniform( void );
    void    StartUp( void );
    void    ShutDown( void );

    void    SetUniform( const void* uniform, const uint32_t location );
    void    Submit( void );
    void    Flush( void );

private:
    // current frame size uniforms  
    size_t                      m_vertexUniformSize;
    size_t                      m_fragmentUniformSize;
    size_t                      m_lightUniformSize;
    
    // buffer region offsets
    uintptr_t                   m_vertexUniformOffset;
    uintptr_t                   m_fragmentUniformOffset;
    uintptr_t                   m_lightUniformOffset;

    // temp uniform memmoty
    crPointer<byte*>            m_vertexUniform;
    crPointer<byte*>            m_fragmentUniform;
    crPointer<byte*>            m_lightUniform;

    // buffer handler 
    crAutoPointer<crBuffer>     m_vertexUniformSSBO;
    crAutoPointer<crBuffer>     m_fragmentUniformSSBO;
    crAutoPointer<crBuffer>     m_lightUniformSSBO;
};

#endif //__UNIFORM_H__