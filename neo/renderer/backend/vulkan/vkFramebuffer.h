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

#ifndef __VK_FRAMEBUFFER_H__
#define __VK_FRAMEBUFFER_H__

class crVKFrameBuffer : public crFramebuffer
{
public:
    crVKFrameBuffer( void );
    ~crVKFrameBuffer( void );
    virtual void Create( const uint32_t width, const uint32_t height, const uint32_t samples, const uint32_t layers, const uint32_t attachmentCount, crTexture** colorAttachament ) ;
    virtual void Delete( void );
    
private:
    VkExtent2D       m_extent;
    VkRenderPass     m_renderPass;
    VkFramebuffer    m_framebuffer;
};

#endif //__VK_FRAMEBUFFER_H__