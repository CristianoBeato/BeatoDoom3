/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

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

#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "renderer/backend/Backend_apiwrapper.h"
#include "qgl.h"
#include "glPipeline.h"

crGLPipeline::crGLPipeline( void ) : 
    m_programPipeline( 0 ),
    m_vertexArrayObject( 0 )
{
    m_shaderStages[0] = 0;
    m_shaderStages[1] = 0;
    m_shaderStages[2] = 0;
    m_shaderStages[3] = 0;
    m_shaderStages[4] = 0;
    m_shaderStages[5] = 0;
}

crGLPipeline::~crGLPipeline( void )
{
}

void crGLPipeline::Create(const shaderProgram_t *program, const vertexAttribute_t *attributes, const size_t numAttributes)
{
    CreateShaderProgram( program );
    CreateVertexPipeline( attributes, numAttributes );
}

void crGLPipeline::Destroy(void)
{
    DestroyVertexPipeline();
    DestroyShaderProgram();
}

void crGLPipeline::Begin(void)
{
    // bind vertex array
    glBindVertexArray( m_vertexArrayObject );

    // bind shder pipeline 
    glBindProgramPipeline( m_programPipeline );

    // color config
    glColorMask( m_colorMask[0], m_colorMask[1], m_colorMask[2], m_colorMask[3] );
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );

    // blending
    glSetState( GL_BLEND, m_blend );
    glBlendFunc( m_blendSRCFactor, m_blendDSTFactor );

    // depth config
    glClearDepth( m_clearDepth );
    glSetState( GL_DEPTH_TEST, m_depthTest );
    glDepthFunc( m_depthFunc );
    glDepthMask( m_depthMask );

    // Clear the color, depth and stencil buffers
    glClearStencil( m_clearStencil );
    glSetState( GL_STENCIL_TEST, m_stencil );
    glSetState( GL_SCISSOR_TEST, m_scissor );

    // poligon
    glSetState( GL_CULL_FACE, m_faceCulling );
    glCullFace( m_cullFace );
	glPolygonMode ( m_poligonModeFace, m_poligonMode );
    
#if CR_USE_CLIP_AS_SCISSOR
	glSetState( GL_CLIP_DISTANCE0, m_clipping );
	glSetState( GL_CLIP_DISTANCE1, m_clipping );
	glSetState( GL_CLIP_DISTANCE2, m_clipping );
	glSetState( GL_CLIP_DISTANCE3, m_clipping );
#endif
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    
}

void crGLPipeline::End(void)
{
    // release program pipeline
    glBindProgramPipeline( 0 );

    // release vertex array
    glBindVertexArray( 0 );

    // release framebuffer
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); 
}

void crGLPipeline::AttachVertexBuffer(crBuffer *buffer, uintptr_t offset, const size_t size)
{
    auto glBuffer = dynamic_cast<crGLBuffer*>( buffer );
    glVertexArrayVertexBuffer( m_vertexArrayObject, 0, glBuffer->GetHandler(), offset, sizeof(idDrawVert) );
}

void crGLPipeline::AttachIndexBuffer(crBuffer *buffer, uintptr_t offset, const size_t size)
{
    auto glBuffer = dynamic_cast<crGLBuffer*>( buffer );
    glVertexArrayElementBuffer( m_vertexArrayObject, glBuffer->GetHandler() );
}

void crGLPipeline::SetViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void crGLPipeline::SetScissor(int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
}

void crGLPipeline::ClearColor(const float red, const float green, const float blue, const float alpha)
{
    glClearColor( red, green, blue, alpha );
}

void crGLPipeline::CreateVertexPipeline(const vertexAttribute_t *attributes, const size_t numAttributes)
{
    glCreateVertexArrays( 1, &m_vertexArrayObject );
    glBindVertexArray( m_vertexArrayObject );
    for ( uint32_t i = 0; i < numAttributes; i++ )
    {
        glEnableVertexArrayAttrib( m_vertexArrayObject, attributes[i].location );
        glVertexArrayAttribBinding( m_vertexArrayObject, attributes[i].location, attributes[i].binding );
        glVertexArrayAttribFormat( m_vertexArrayObject, attributes[i].location, attributes[i].elements, attributes[i].format, attributes[i].format, attributes[i].offset );
    }
    glBindVertexArray( 0 );
}

void crGLPipeline::DestroyVertexPipeline(void)
{

    if ( m_vertexArrayObject != 0 )
    {
        glDeleteVertexArrays( 1, &m_vertexArrayObject );
        m_vertexArrayObject = 0;
    }
}

void crGLPipeline::CreateShaderProgram(const shaderProgram_t *program)
{
    glGenProgramPipelines( 1, &m_programPipeline );
    glBindProgramPipeline( m_programPipeline );
    
    for ( uint32_t i = 0; i < program->count; i++)
    {
        GLint       status = GL_FALSE;
        GLint       length = 0;
        GLenum      shaderType = GL_NONE;
        GLbitfield  shaderBinding = GL_NONE;
        uint32_t    stage = program->stage;

        switch ( stage )
        {
        case SHADER_STAGE_VERTEX:
            shaderType = GL_VERTEX_SHADER;
            shaderBinding = GL_VERTEX_SHADER_BIT;
            break;
        case SHADER_STAGE_FRAGMENT:
            shaderType = GL_FRAGMENT_SHADER;
            shaderBinding = GL_FRAGMENT_SHADER_BIT;
            break;
        case SHADER_STAGE_GEOMETRY:
            shaderType = GL_GEOMETRY_SHADER;
            shaderBinding = GL_GEOMETRY_SHADER_BIT;
            break;
        case SHADER_STAGE_COMPUTE:
            shaderType = GL_COMPUTE_SHADER;
            shaderBinding = GL_COMPUTE_SHADER_BIT;
            break;
        case SHADER_STAGE_TESS_CONTROL:
            shaderType = GL_TESS_CONTROL_SHADER;
            shaderBinding = GL_TESS_CONTROL_SHADER_BIT;
            break;
        case SHADER_STAGE_TESS_EVALUATION:
            shaderType = GL_TESS_EVALUATION_SHADER;
            shaderBinding = GL_TESS_EVALUATION_SHADER_BIT;
            break;
        
        default:
            //TODO: error printing
            break;
        }

        // create shader stage 
        GLuint shader = glCreateShader( shaderType );
        
        // load the spirv binary shader
        glShaderBinary( 1 , &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, program->sources[i], program->sizes[i] );
        glSpecializeShader( m_shaderStages[stage], "main", 0, NULL, NULL );  
        
        // check shader status
        glGetShaderiv( m_shaderStages[i], GL_COMPILE_STATUS, &status );
        if ( status != GL_TRUE )
        {
            glGetShaderiv( m_shaderStages[i], GL_INFO_LOG_LENGTH, &length );
            char *log = (char*)Mem_Alloc( length );
            glGetShaderInfoLog( m_shaderStages[i], length, nullptr, log );
            common->Error( "Shader compile error: %s\n", log );
            Mem_Free( log );
            glDeleteShader( shader );
        }

        // create separeble program
        m_shaderStages[stage] = glCreateProgram();
        glProgramParameteri( m_shaderStages[stage], GL_PROGRAM_SEPARABLE, GL_TRUE );
        // glProgramParameteri( m_shaderStages[stage], GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );

        // attach shader to program
        glAttachShader( m_shaderStages[stage], shader );
        glLinkProgram( m_shaderStages[stage] );
        glDetachShader( m_shaderStages[stage], shader );
        glDeleteShader( shader );

        // check program status
        glGetProgramiv( m_shaderStages[stage], GL_LINK_STATUS, &status );
        if ( status != GL_TRUE )
        {
            glGetProgramiv( m_shaderStages[stage], GL_INFO_LOG_LENGTH, &length );
            char *log = (char*)Mem_Alloc( length );
            glGetProgramInfoLog( m_shaderStages[stage], length, nullptr, log );
            common->Error( "Shader link error: %s\n", log );
            Mem_Free( log );
            glDeleteProgram( m_shaderStages[stage] );
        }

        // bind program to pipeline
        glUseProgramStages( m_programPipeline, shaderBinding, m_shaderStages[stage] );
    }
}

void crGLPipeline::DestroyShaderProgram(void)
{
    // release the shader stages
    for ( uint32_t i = 0; i < SHADER_STAGE_MAX; i++)
    {
        if ( m_shaderStages[i] != 0 )
        {
            glDeleteProgram( m_shaderStages[i] );
            m_shaderStages[i] = 0;
        }
    }

    // release the pipeline
    if ( m_programPipeline != 0 )
    {
        glDeleteProgramPipelines( 1, &m_programPipeline );
        m_programPipeline = 0;
    }
}
