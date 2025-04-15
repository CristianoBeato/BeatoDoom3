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

#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "renderer/backend/Backend_apiwrapper.h"
#include "qgl.h"
#include "glSwapChain.h"

#include <SDL3/SDL_video.h>

crGLSwapChain::crGLSwapChain( void ) :
    crSwapChain(),
    m_window( nullptr) 
{
}

crGLSwapChain::~crGLSwapChain( void )
{
}

void crGLSwapChain::Create(const uint32_t width, const uint32_t height, const uint32_t vsync, const uint32_t samples )
{
    glCreateFramebuffers( SMP_FRAMES, m_framebuffers );
    glCreateRenderbuffers( SMP_FRAMES, m_renderbuffers );

    for ( uint32_t i = 0; i < SMP_FRAMES; i++)
    {
        if ( samples > 0 )
            glNamedRenderbufferStorageMultisample( m_renderbuffers[i], samples, GL_RGB8, width, height );
        else
            glNamedRenderbufferStorage( m_renderbuffers[i], GL_RGBA8, width, height );   
    }
    
}

void crGLSwapChain::Destroy(void)
{
    glDeleteRenderbuffers( SMP_FRAMES, m_renderbuffers );
    glDeleteFramebuffers( SMP_FRAMES, m_framebuffers );
}

void crGLSwapChain::Begin(void)
{
    // if GPU still rendering at this frame content, wait
    GLenum waitResult = glClientWaitSync( m_fences[m_currentFrame], 0, 100000000 );
    if ( waitResult == GL_TIMEOUT_EXPIRED || waitResult == GL_WAIT_FAILED )
        common->Error("Frame %i begin and GPU still in use", m_currentFrame );    

    glDeleteSync( m_fences[m_currentFrame] );

    // bind swapchain buffer and clear
    glBindFramebuffer( GL_FRAMEBUFFER, m_framebuffers[m_currentFrame] );
    glClear( GL_COLOR_BUFFER_BIT );
}

void crGLSwapChain::End(void)
{
    // clear back buffer
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    
    // copy frame buffer to the screen 
    glBlitNamedFramebuffer( m_framebuffers[m_currentFrame], 0, 0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR );

    // synch frame
    m_fences[m_currentFrame] = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

    // update frame count
    m_currentFrame = ( m_currentFrame + 1 ) % SMP_FRAMES;

}

void crGLSwapChain::SwapBuffers(void)
{
    // show to screen
    SDL_GL_SwapWindow( m_window );
}
