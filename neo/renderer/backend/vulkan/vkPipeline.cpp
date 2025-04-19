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
#include "qvk.h"
#include "vkPipeline.h"

void crVKPipeline::DepthBias( const float factor, const float units )
{
    vkCmdSetDepthBias( m_commandBuffer, units, 0.0f, factor );
}

void crVKPipeline::SetViewport( const int x, const int y, const uint32_t width, const uint32_t height )
{
    VkViewport viewport{};
    viewport.x = static_cast<float>( x ); 
    viewport.y = static_cast<float>( y ); 
    viewport.width = static_cast<float>( width ); 
    viewport.height = static_cast<float>( height );
    viewport.minDepth = -1.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport( m_commandBuffer, 0, 1, &viewport );
}

void crVKPipeline::SetScissor( const int x, const int y, const uint32_t width, const uint32_t height )
{
    VkRect2D scissor{};
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor( m_commandBuffer, 0, 1, &scissor );
}
