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

#ifndef __VK_SWAP_CHAIN_H__
#define __VK_SWAP_CHAIN_H__

class crVKSwapChain : public crSwapChain
{
public:
    crVKSwapChain( void );
    ~crVKSwapChain( void );
    virtual void            Create( const uint32_t width, const uint32_t height, const uint32_t vsync, const uint32_t samples ) override;
    virtual void            Destroy( void ) override;
    virtual void            Begin( void );
    virtual void            End( void );
    virtual void            SwapBuffers( void );

private:
    VkExtent2D                  m_swapChainExtent;
    VkSurfaceFormatKHR          m_surfaceFormat;
    VkSwapchainKHR              m_swapChain;
    VkRenderPass                m_renderPass;
    VkQueue                     m_presentQueue;
    VkSemaphore                 m_semaphore;
    VkFence                     m_fence;
    idList<VkImage>             m_images;      // SwapChain Image chain
    idList<VkImageView>         m_imagesView;
    idList<VkFramebuffer>       m_frameBuffers;
};

#endif //__VK_SWAP_CHAIN_H__