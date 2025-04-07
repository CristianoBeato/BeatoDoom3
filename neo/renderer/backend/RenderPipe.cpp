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
#include "RenderPipe.h"

crRenderPipe::crRenderPipe( void )
{
#if CR_USE_VULKAN
#else
    m_shaderStages[0] = 0;
    m_shaderStages[1] = 0;
    m_shaderStages[2] = 0;
    m_shaderStages[3] = 0;
    m_shaderStages[4] = 0;
    m_shaderStages[5] = 0;
    m_programPipeline = 0;
    m_vertexArrayObject = 0;
#endif
}

crRenderPipe::~crRenderPipe( void )
{
}

void crRenderPipe::Create(const shaderProgram_t *program, const frameBuffer_t *frameBuffer, const vertexAttribute_t *attributes, const size_t numAttributes)
{
    CreateShaderProgram( program );
    CreateVertexPipeline( attributes, numAttributes );
    CreateFrameBuffer( frameBuffer );
}

void crRenderPipe::Destroy(void)
{
    DestroyFrameBuffer();
    DestroyVertexPipeline();
    DestroyShaderProgram();
}

void crRenderPipe::Begin(void)
{
#if CR_USE_VULKAN
#elif CR_USE_OPENGL
    
    // sed
    glViewportArrayv( 0, m_numViewports, m_viewports );

    // bind the frame buffer to draw
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBuffer );

    // bind vertex array
    glBindVertexArray( m_vertexArrayObject );

    // bind shder pipeline 
    glBindProgramPipeline( m_programPipeline );
#endif
}

void crRenderPipe::End(void)
{


    // release program pipeline
    glBindProgramPipeline( 0 );

    // release vertex array
    glBindVertexArray( 0 );

    // release framebuffer
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ); 
}

void crRenderPipe::AttachVertexBuffer(crBuffer *buffer, uintptr_t offset, const size_t size)
{
    
}

void crRenderPipe::SetViewport(int x, int y, int width, int height)
{
#if CR_USE_VULKAN
    // Vulkan specific code to set the viewport
#elif CR_USE_OPENGL
    glViewport(x, y, width, height);
#endif // CR_USE_OPENGL
}

void crRenderPipe::SetScissor(int x, int y, int width, int height)
{
#if CR_USE_VULKAN
    // Vulkan specific code to set the scissor
#elif CR_USE_OPENGL
    glScissor(x, y, width, height);
#endif // CR_USE_OPENGL
}

void crRenderPipe::Clear(void)
{
#if CR_USE_VULKAN
    // Vulkan specific code to clear the pipeline state and buffers
#elif CR_USE_OPENGL
    // get current bind to draw framebuffer
    GLint currentFrameBuffer = 0;
    glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &currentFrameBuffer );

    // if are not current frame buffer bind 
    if ( currentFrameBuffer != m_frameBuffer )
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBuffer );

    // Clear the color, depth and stencil buffers
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepth( 1.0f );
    glClearStencil( 0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // clear viewport and scissors
    m_numViewports = 0;
    m_numScissors = 0;
    memset(m_viewports, 0, sizeof(m_viewports));
    memset(m_scissors, 0, sizeof(m_scissors));
#endif // CR_USE_OPENGL
}

bool crRenderPipe::CreateFrameBuffer( const frameBuffer_t *frameBuffer )
{
#if CR_USE_VULKAN
    // Vulkan specific code to create the framebuffer
#elif CR_USE_OPENGL
    GLenum* drawBuffers = nullptr;
    
    // create frame buffer handler
    glCreateFramebuffers( 1, &m_frameBuffer );

    drawBuffers = (GLenum*)Mem_Alloc( sizeof(GLenum) * frameBuffer->attachamentCount );

    uint32_t colorAttac = 0;
    for ( uint32_t i = 0; i < frameBuffer->attachamentCount; i++)
    {
        auto format = frameBuffer->colorAttachament[i]->GetFormat();
        
        // the attachament is chosem by the texture type
        if ( format == GL_DEPTH )
            glNamedFramebufferTexture( m_frameBuffer, GL_DEPTH_ATTACHMENT, frameBuffer->colorAttachament[i]->GetHandler(), 0 );
        else if ( format == GL_DEPTH_STENCIL )
            glNamedFramebufferTexture( m_frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, frameBuffer->colorAttachament[i]->GetHandler(), 0 );
        else
        {
            GLenum attachament = GL_COLOR_ATTACHMENT0 + colorAttac++;
            glNamedFramebufferTexture( m_frameBuffer, attachament, frameBuffer->colorAttachament[i]->GetHandler(), 0);
            drawBuffers[i] = attachament;
        }
    }

    // bind the attachametn 
    glNamedFramebufferDrawBuffers( m_frameBuffer, colorAttac, drawBuffers );

    Mem_Free( drawBuffers );

    GLenum status = glCheckNamedFramebufferStatus( m_frameBuffer, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) 
    {
        common->Error( "Incomplete frame buffer" );
        return false;
    }
#endif

    return true;
}

void crRenderPipe::DestroyFrameBuffer(void)
{
#if CR_USE_VULKAN
    // Vulkan specific code to destroy the framebuffer
#elif CR_USE_OPENGL
    if ( m_frameBuffer != 0 )
    {
        glDeleteFramebuffers( 1, &m_frameBuffer );
        m_frameBuffer = 0;
    }
#endif // CR_USE_OPENGL
}

bool crRenderPipe::CreateVertexPipeline(const vertexAttribute_t *attributes, const size_t numAttributes)
{
#if CR_USE_VULKAN
    // Vulkan specific code to create the vertex array
#elif CR_USE_OPENGL
    glCreateVertexArrays( 1, &m_vertexArrayObject );
    glBindVertexArray( m_vertexArrayObject );
    for ( uint32_t i = 0; i < numAttributes; i++ )
    {
        glEnableVertexArrayAttrib( m_vertexArrayObject, attributes[i].location );
        glVertexArrayAttribBinding( m_vertexArrayObject, attributes[i].location, attributes[i].binding );
        glVertexArrayAttribFormat( m_vertexArrayObject, attributes[i].location, attributes[i].elements, attributes[i].format, attributes[i].format, attributes[i].offset );
    }
    glBindVertexArray( 0 );
#endif // CR_USE_OPENGL
    return true;
}

void crRenderPipe::DestroyVertexPipeline(void)
{
#if CR_USE_VULKAN
    // Vulkan specific code to destroy the vertex array
#elif CR_USE_OPENGL
    if ( m_vertexArrayObject != 0 )
    {
        glDeleteVertexArrays( 1, &m_vertexArrayObject );
        m_vertexArrayObject = 0;
    }
#endif // CR_USE_OPENGL
}

bool crRenderPipe::CreateShaderProgram(const shaderProgram_t *program)
{
#if CR_USE_VULKAN
    // Vulkan specific code to create the shader program
#elif CR_USE_OPENGL
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
            common->Printf( "Shader compile error: %s\n", log );
            Mem_Free( log );
            glDeleteShader( shader );
            return false;
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
            common->Printf( "Shader link error: %s\n", log );
            Mem_Free( log );
            glDeleteProgram( m_shaderStages[stage] );
            return false;
        }

        // bind program to pipeline
        glUseProgramStages( m_programPipeline, shaderBinding, m_shaderStages[stage] );
    }
#endif // CR_USE_OPENGL
    return true;
}

void crRenderPipe::DestroyShaderProgram(void)
{
#if CR_USE_VULKAN
    // Vulkan specific code to destroy the shader program
#elif CR_USE_OPENGL
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
#endif // CR_USE_OPENGL
}
