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

#ifndef __VK_TEXTURE_H__
#define __VK_TEXTURE_H__

class crVKTexture : public crTexture
{
public:
    crVKTexture( void );
    ~crVKTexture( void);

    virtual bool    Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const GLenum format, const uint32_t type ) override;
    virtual void    Destroy( void ) override;
    virtual void    CopyBufferToImage( const crBuffer* buffer, const uint32_t rowLength, const bufferTextureRect_t * imageMap, const uint32_t count ) override;
    virtual void    MakeResident( const crGLTextureSampler* sampler );
    virtual void    Unmakeresident( void );

    /// @brief get the texture object handle
    VkImage      GetHandler( void ) const { return m_texture; }

private:
    VkImage         m_texture;
    VkImageView     m_view;
    VkDeviceMemory  m_memory;
};

class crVKTextureSampler : public crTextureSampler
{
public:
    crVKTextureSampler( void );
    ~crVKTextureSampler( void );
    virtual bool    Create( const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel, const float LODBias ) override;
    virtual void    Destroy( void ) override;
    VkSampler       GetHandler( void ) const { return m_sampler; }

private:
    VkSampler   m_sampler;
};

#endif //!__VK_TEXTURE_H__