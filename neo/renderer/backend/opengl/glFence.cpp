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
#include "glFence.h"

crGLFence::crGLFence( void ) : m_fence( nullptr )
{
}

crGLFence::~crGLFence(void)
{
    if ( m_fence )
        glDeleteSync( m_fence );
}

void crGLFence::Create(void)
{
    m_fence = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
}

void crGLFence::Wait(void)
{
    if ( m_fence ) 
        glClientWaitSync( m_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0xFFFFFFFFFFFFFFFF );
}

void crGLFence::Reset(void)
{
    // clear sync
    if ( m_fence ) 
        glDeleteSync( m_fence );

    m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
}

bool crGLFence::IsSignaled(void) const
{
    GLint result = 0;
    glGetSynciv( m_fence, GL_SYNC_STATUS, sizeof(result), nullptr, &result );
    return result == GL_SIGNALED;
}
