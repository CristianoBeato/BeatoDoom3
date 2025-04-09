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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

class crBuffer;

// helper structure form image format
struct internalFormat_t
{
    internalFormat_t( void ) : format( 0 ){}
    internalFormat_t( uint32_t _format ) : format( _format ) {}

    uint32_t    GetInternalFormat( void ) const { return format; }
    uint32_t    GetFormat( void ) const;
//    uint32_t    GetElementSize( void ) const;
    uint32_t    GetDataType( void ) const;

private:
    uint32_t format;
};

struct bufferTextureRect_t
{
    uint16_t    layerLevel = 0;
    uint16_t    mipLevel = 0;
    uint32_t    xOffset = 0;
    uint32_t    yOffset = 0;
    uint32_t    zOffset = 0;
    uint32_t    width = 0;
    uint32_t    height = 0;
    uint32_t    depth = 0;
    uintptr_t   bufferOffset = 0;
};

class crTextureSampler;

/// @brief  texture image
/// @note   this class is used to create a texture image object that store and control texture image loading, binding, etc.
class crTexture
{
public:
    crTexture( void );
    ~crTexture( void);
  
    /// @brief create the texture image, and allocate the memory space
    /// @param width base width of the texture
    /// @param height base height of the texture
    /// @param depth base depth of the texture
    /// @param layers base layers of the texture
    /// @param mips num of mipmaps
    /// @param format texture color format
    /// @param type texture data type
    /// @return false on error 
    bool    Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const GLenum format, const uint32_t type );

    /// @brief destroy the texture image, and realease the memory
    void    Destroy( void );

    /// @brief copy the image data from the transfer buffer 
    /// @param buffer origin buffer 
    /// @param rowLength 
    /// @param imageMap // image coordenates
    void    CopyBufferToImage( const crBuffer* buffer, const uint32_t rowLength, const bufferTextureRect_t * imageMap, const uint32_t count );

    /// @brief Return the binding index in the texture buffer binding 
    /// @return the texture index in the buffer or -1 if not bind 
    int32_t GetBindingIndex( void ) const { return m_bindindex; }

    /// @brief Set the texture binding index in the texture buffer 
    /// @param index the index 
    void    SetBinding( const int32_t index ) { m_bindindex = index; }

    /// @brief 
    /// @param sampler 
    void    MakeResident( const crTextureSampler* sampler );
    
    /// @brief 
    /// @param  
    void    Unmakeresident( void );


#if CR_USE_VULKAN
    /// @brief get the texture image handle
    VkImage     GetHandler( void ) const { return m_image; }
    /// @brief get the texture image view handle
    VkImageView GetImageView( void ) const { return m_imageView; }
#elif CR_USE_OPENGL
    /// @brief get the texture object handle
    GLuint      GetHandler( void ) const { return m_texture; }
    /// @brief  Get the 64bit handler for resident texture binding 
    GLuint64    GetBindingHandler( void ) const { return m_bindingHandler; }
    /// @brief get the texture target
    GLenum      GetTarget( void ) const { return m_target; }
    /// @brief get the texture format
    GLenum      GetFormat( void ) const { return m_format.GetFormat(); }
    /// @brief get the texture type
    GLenum      GetType( void ) const { return m_format.GetDataType(); }
    /// @brief get the texture internal format
    GLenum      GetInternalFormat( void ) const { return m_format.GetInternalFormat(); }
#endif // CR_USE_OPENGL

private:
    uint32_t    m_width;        // texture width
    uint32_t    m_height;       // texture height
    uint32_t    m_depth;        // texture depth
    uint32_t    m_layers;       // texture layers
    uint32_t    m_mipcount;     // texture mipmap count
    uint32_t    m_samples;      // texture smple count
    int32_t     m_bindindex;    // texture binding index -1 if not in texture binding buffer 

#if CR_USE_VULKAN
    VkImage     m_image;        // texture image
    VkDeviceMemory m_memory;    // texture memory
    VkImageView m_imageView;    // texture image view
#elif CR_USE_OPENGL
    GLuint              m_texture;          // texture object
    GLenum              m_target;           // texture target
    GLuint64            m_bindingHandler;   // texture binding handler  
    internalFormat_t    m_format;
#endif

};

/// @brief texture sampler
/// @note this class is used to create a texture sampler object and control texture image sampling, filterin, repeating, etc.
class crTextureSampler
{
public:
    crTextureSampler( void );
    ~crTextureSampler( void );

    /// @brief create the sampler
    /// @param minFilter the minification filter
    /// @param magFilter the magnification filter
    /// @param wrapS the wrap mode for the S axis
    /// @param wrapT the wrap mode for the T axis
    /// @param anisotropicLevel the anisotropic level
    /// @return true if the sampler was created successfully
    bool    Create( const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel );
    
    /// @brief destroy the sampler
    void    Destroy( void );

    /// @brief get the sampler handle
#if CR_USE_VULKAN
    VkSampler   GetHandler( void ) const { return m_sampler; }
#elif CR_USE_OPENGL
    GLuint      GetHandler( void ) const { return m_sampler; }
#endif //CR_USE_OPENGL

private:
#if CR_USE_VULKAN
    VkSampler   m_sampler;
#elif CR_USE_OPENGL
    GLuint      m_sampler;
#endif
};

#endif // !__TEXTURE_H__