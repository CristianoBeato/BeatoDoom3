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
#include "glFramebuffer.h"

//// bind the frame buffer to draw
//glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBuffer );
//
//// get current bind to draw framebuffer
//GLint currentFrameBuffer = 0;
//glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &currentFrameBuffer );
//
//// if are not current frame buffer bind 
//if ( currentFrameBuffer != m_frameBuffer )
//    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBuffer );

static GLenum colorAttachaments[8] = 
{ 
    GL_COLOR_ATTACHMENT0, 
    GL_COLOR_ATTACHMENT1, 
    GL_COLOR_ATTACHMENT2, 
    GL_COLOR_ATTACHMENT3, 
    GL_COLOR_ATTACHMENT4, 
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7
};

crGLFrameBuffer::crGLFrameBuffer(void) : m_framebuffer( 0 )
{
}

crGLFrameBuffer::~crGLFrameBuffer(void)
{
    Delete();
}

void crGLFrameBuffer::Create( const frameBuffer_t* frameBuffer )
{
    uint32_t colorAttachCount = 0;

    // create frame buffer handler
    glCreateFramebuffers( 1, &m_framebuffer );

    for ( uint32_t i = 0; i < frameBuffer->attachmentCount; i++)
    {
        auto attachment = dynamic_cast<crGLTexture*>( frameBuffer->colorAttachament[i] ); 
        // the attachament is chosem by the texture type
        if ( attachment->GetFormat() == GL_DEPTH )
            glNamedFramebufferTexture( m_framebuffer, GL_DEPTH_ATTACHMENT, attachment->GetHandler(), 0 );
        else if ( attachment->GetFormat() == GL_DEPTH_STENCIL )
            glNamedFramebufferTexture( m_framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, attachment->GetHandler(), 0 );
        else
            glNamedFramebufferTexture( m_framebuffer, colorAttachaments[colorAttachCount++], attachment->GetHandler(), 0);
    }

    // bind the attachametn 
    glNamedFramebufferDrawBuffers( m_framebuffer, colorAttachCount, colorAttachaments );

    GLenum status = glCheckNamedFramebufferStatus( m_framebuffer, GL_FRAMEBUFFER );
    if (status != GL_FRAMEBUFFER_COMPLETE) 
        common->Error( "Incomplete frame buffer" );
}

void crGLFrameBuffer::Delete(void)
{
    if ( m_framebuffer != 0 )
    {
        glDeleteFramebuffers( 1, &m_framebuffer );
        m_framebuffer = 0;
    }
}