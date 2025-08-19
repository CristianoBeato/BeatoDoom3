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

#ifndef __SHADER_STORAGE_HPP__
#define __SHADER_STORAGE_HPP__

enum vertexAttribIndex_t : uint8_t
{
    CR_VERTEX_ATTRIB_INDEX_VERTEX = 0,
    CR_VERTEX_ATTRIB_INDEX_ST,
    CR_VERTEX_ATTRIB_INDEX_NORMAL,
    CR_VERTEX_ATTRIB_INDEX_TANGENT,
    CR_VERTEX_ATTRIB_INDEX_COLOR,
    CR_VERTEX_ATTRIB_INDEX_COLOR2
};

enum renderParms_t : uint8_t
{
    // vertex render parms
    VERTEX_RENDERPARM_COLOR,                        // rpColor 
	VERTEX_RENDERPARM_VIEWORIGIN,                   // rpViewOrigin
	VERTEX_RENDERPARM_GLOBALEYEPOS,                 // rpGlobalEyePos
    VERTEX_RENDERPARM_DIFFUSEMODIFIER,              // rpDiffuseModifier
	VERTEX_RENDERPARM_SPECULARMODIFIER,             // rpSpecularModifier
	VERTEX_RENDERPARM_COLOR_MODULATE,               // rpVertexColorModulate 
	VERTEX_RENDERPARM_COLOR_ADD,                    // rpVertexColorAdd
    VERTEX_RENDERPARM_TEXTUREMATRIX_S,              // rpTextureMatrixS
	VERTEX_RENDERPARM_TEXTUREMATRIX_T,              // rpTextureMatrixT
	VERTEX_RENDERPARM_BUMPMATRIX_S,                 // rpBumpMatrixS
	VERTEX_RENDERPARM_BUMPMATRIX_T,                 // rpBumpMatrixT
	VERTEX_RENDERPARM_DIFFUSEMATRIX_S,              // rpDiffuseMatrixS
	VERTEX_RENDERPARM_DIFFUSEMATRIX_T,              // rpDiffuseMatrixT
	VERTEX_RENDERPARM_SPECULARMATRIX_S,             // rpSpecularMatrixS
	VERTEX_RENDERPARM_SPECULARMATRIX_T,             // rpSpecularMatrixT
	VERTEX_RENDERPARM_MVPMATRIX_X,                  // rpMVPmatrixX
	VERTEX_RENDERPARM_MVPMATRIX_Y,                  // rpMVPmatrixY
	VERTEX_RENDERPARM_MVPMATRIX_Z,                  // rpMVPmatrixZ
	VERTEX_RENDERPARM_MVPMATRIX_W,                  // rpMVPmatrixW
	VERTEX_RENDERPARM_MODELMATRIX_X,                // rpModelMatrixX
	VERTEX_RENDERPARM_MODELMATRIX_Y,                // rpModelMatrixY
	VERTEX_RENDERPARM_MODELMATRIX_Z,                // rpModelMatrixZ
	VERTEX_RENDERPARM_MODELMATRIX_W,                // rpModelMatrixW
	VERTEX_RENDERPARM_PROJMATRIX_X,                 // rpProjectionMatrixX
	VERTEX_RENDERPARM_PROJMATRIX_Y,                 // rpProjectionMatrixY
	VERTEX_RENDERPARM_PROJMATRIX_Z,                 // rpProjectionMatrixZ
	VERTEX_RENDERPARM_PROJMATRIX_W,                 // rpProjectionMatrixW
	VERTEX_RENDERPARM_MODELVIEWMATRIX_X,            // rpModelViewMatrixX
	VERTEX_RENDERPARM_MODELVIEWMATRIX_Y,            // rpModelViewMatrixY
	VERTEX_RENDERPARM_MODELVIEWMATRIX_Z,            // rpModelViewMatrixZ
	VERTEX_RENDERPARM_MODELVIEWMATRIX_W,            // rpModelViewMatrixW
	VERTEX_RENDERPARM_TEXGEN_0_S,                   // rpTexGen0S
	VERTEX_RENDERPARM_TEXGEN_0_T,                   // rpTexGen0T
	VERTEX_RENDERPARM_TEXGEN_0_Q,                   // rpTexGen0Q
	VERTEX_RENDERPARM_TEXGEN_0_ENABLED,             // rpTexGen0Enabled
	VERTEX_RENDERPARM_TEXGEN_1_S,                   // rpTexGen1S
	VERTEX_RENDERPARM_TEXGEN_1_T,                   // rpTexGen1T
	VERTEX_RENDERPARM_TEXGEN_1_Q,                   // rpTexGen1Q
	VERTEX_RENDERPARM_TEXGEN_1_ENABLED,             // rpTexGen1Enabled
	VERTEX_RENDERPARM_WOBBLESKY_X,                  // rpWobbleSkyX
	VERTEX_RENDERPARM_WOBBLESKY_Y,                  // rpWobbleSkyY
	VERTEX_RENDERPARM_WOBBLESKY_Z,                  // rpWobbleSkyZ

    VERTEX_RENDERPARM_TOTAL,

    FRAGMENT_RENDERPARM_SCREENCORRECTIONFACTOR = 0, // rpScreenCorrectionFactor
	FRAGMENT_RENDERPARM_WINDOWCOORD,                // rpWindowCoord
	FRAGMENT_RENDERPARM_OVERBRIGHT,                 // rpOverbright
	FRAGMENT_RENDERPARM_ALPHA_TEST,                 // rpAlphaTest
    FRAGMENT_RENDERPARM_TOTAL,

    LIGHT_RENDERPARM_LOCALLIGHTORIGIN = 0,          // rpLocalLightOrigin
	LIGHT_RENDERPARM_LOCALVIEWORIGIN,               // rpLocalViewOrigin
    LIGHT_RENDERPARM_LIGHTCOLOR,                    // rpLightColor
	LIGHT_RENDERPARM_LIGHTPROJECTION_S,             // rpLightProjectionS
	LIGHT_RENDERPARM_LIGHTPROJECTION_T,             // rpLightProjectionT
	LIGHT_RENDERPARM_LIGHTPROJECTION_Q,             // rpLightProjectionQ
	LIGHT_RENDERPARM_LIGHTFALLOFF_S,                // rpLightFalloffS
    LIGHT_RENDERPARM_TOTAL,
};

// ATTENTION: Must match the definition in the shader.
// 0 is for the texture sampler array 
static const uint32_t   SHADER_BUFFER_BINDING_VERTEX_BLOCK = 1;     // layout( std430, binding = 1 ) buffer vertexStorageBlock
static const uint32_t   SHADER_BUFFER_BINDING_FRAGMENT_BLOCK = 2;   // layout( std430, binding = 2 ) buffer fragmentStorageBlock
static const uint32_t   SHADER_BUFFER_BINDING_LIGHT_BLOCK = 3;      // layout( std430, binding = 3 ) buffer lightStorageBlock

class crUniformBlock
{
public:
    crUniformBlock( void );
    ~crUniformBlock( void );

    void    Create( const uint32_t in_blockElements, const uint32_t in_blockCount );
    void    Destoy( void );
    void    Flush( void );
    void    Submit( void );
    void    ProgramParameter4fv( const renderParms_t in_renderParm, const uint32_t in_count, const float* in_parms );

private:
    struct uniformVec4_t
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    struct uniformBufferRegion_t
    {
        uintptr_t   head = 0;
        uintptr_t   tail = 0;
    };

    size_t                              m_blockSize;
    size_t                              m_bufferSize;
    uniformBufferRegion_t               m_region;
    uniformVec4_t*                      m_block;
    uniformVec4_t*                      m_bufferMap;
    crAutoPointer<crvkBufferStatic>     m_shaderStorageBuffer;           // vertex shader storage buffer 
    crAutoPointer<crvkBuffer>           m_shaderStorageBufferStagin;     // vertex shader storage buffer 
};

#endif //__SHADER_STORAGE_HPP__