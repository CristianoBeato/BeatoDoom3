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

#if CR_USE_OPENGL
idCVar gl_useImageCopy( "gl_useImageCopy", "0", CVAR_RENDERER | CVAR_ARCHIVE | CVAR_BOOL, "use \"glCopyImageSubData\" instead of \"glCopyTextureSubImageXD\" for frame Buffer copy" );
#endif //CR_USE_OPENGL


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

static GLenum colorAttachaments[MAX_COLOR_ATTACHMENS] = 
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

void crGLFrameBuffer::Create( const uint32_t width, const uint32_t height, const uint32_t samples, const uint32_t layers, const uint32_t attachmentCount, crTexture** colorAttachament )
{
    uint32_t colorAttachCount = 0;

    // create frame buffer handler
    glCreateFramebuffers( 1, &m_framebuffer );

    for ( uint32_t i = 0; i < attachmentCount; i++)
    {
        auto attachment = dynamic_cast<crGLTexture*>( colorAttachament[i] ); 
        // the attachament is chosem by the texture type
        if ( attachment->GetFormat() == GL_DEPTH )
        {
            m_depthStencilAttachment = attachment->GetHandler();
            glNamedFramebufferTexture( m_framebuffer, GL_DEPTH_ATTACHMENT, m_depthStencilAttachment, 0 );
        }
        else if ( attachment->GetFormat() == GL_DEPTH_STENCIL )
        {
            m_depthStencilAttachment = attachment->GetHandler();
            glNamedFramebufferTexture( m_framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilAttachment, 0 );
        }
        else
        {
            m_attachments[colorAttachCount] =  attachment->GetHandler();
            glNamedFramebufferTexture( m_framebuffer, colorAttachaments[colorAttachCount], m_attachments[colorAttachCount], 0 );
            colorAttachCount++;
        }
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

void crGLFrameBuffer::CopyToImage(crTexture *textureDST, const int32_t x, const int32_t y, const uint32_t width, const uint32_t height, const uint32_t layers)
{
    uint32_t    imageWidth = width;
    uint32_t    imageHeight = height;
    uint32_t	potWidth = 0;
    uint32_t    potHeight = 0;

    crGLTexture* texture = dynamic_cast<crGLTexture*>( textureDST );

    if ( cvarSystem->GetCVarBool( "g_lowresFullscreenFX" ) ) 
    {
		imageWidth = 512;
		imageHeight = 512;
	}

	// if the size isn't a power of 2, the image must be increased in size

	potWidth = MakePowerOfTwo( imageWidth );
	potHeight = MakePowerOfTwo( imageHeight );

	// GetDownsize( imageWidth, imageHeight );
	// GetDownsize( potWidth, potHeight );

    if( gl_useImageCopy.GetBool() )
        glCopyImageSubData( m_attachments[0], GL_TEXTURE_2D, 0, x, y, 0, texture->GetHandler(), GL_TEXTURE_2D, 0, x, y, 0, potWidth, potHeight, 0 );
    else
    {
        // if not bind, bind to copy  
        GLint current = 0;
        glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &current );
        if ( current != m_framebuffer )
            glBindFramebuffer( GL_READ_FRAMEBUFFER, m_framebuffer );
        
        // copy frame buffer content 
        glCopyTextureSubImage2D( texture->GetHandler(), 0, x, y, x, y, width, height );  
        
        // restaure the frame buffer 
        if ( current != m_framebuffer )
            glBindFramebuffer( GL_READ_FRAMEBUFFER, current );
    }
}
