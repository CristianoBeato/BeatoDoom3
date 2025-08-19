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

#ifndef __BACKEND_API_WRAPER_H__
#define __BACKEND_API_WRAPER_H__

// ===========================================================================

//
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

enum
{
    // RED component 
    PIXEL_COLOR_R8I,        // int8_t
    PIXEL_COLOR_R8UI,       // uint8_t
    PIXEL_COLOR_R16I,       // int16_t
    PIXEL_COLOR_R16UI,      // uint16_t
    PIXEL_COLOR_R32I,       // int32_t
    PIXEL_COLOR_R32UI,      // uint32_t
    PIXEL_COLOR_R16F,       // float16 ( half float )
    PIXEL_COLOR_R32F,       // float 

    // RED GREEN Component 
    PIXEL_COLOR_RG8I,       // int8_t
    PIXEL_COLOR_RG8UI,      // uint8_t
    PIXEL_COLOR_RG16I,      // int16_t
    PIXEL_COLOR_RG16UI,     // uint16_t
    PIXEL_COLOR_RG32I,      // int32_t
    PIXEL_COLOR_RG32UI,     // uint32_t
    PIXEL_COLOR_RG16F,      // float16
    PIXEL_COLOR_RG32F,      // float

    // RED GREEN BLUE Component
    PIXEL_COLOR_RGB8I,       // int8_t
    PIXEL_COLOR_RGB8UI,      // uint8_t
    PIXEL_COLOR_RGB16I,      // int16_t
    PIXEL_COLOR_RGB16UI,     // uint16_t
    PIXEL_COLOR_RGB32I,      // int32_t
    PIXEL_COLOR_RGB32UI,     // uint32_t
    PIXEL_COLOR_RGB16F,      // float16
    PIXEL_COLOR_RGB32F,      // float

    // RED GREEN BLUE ALPHA Component
    PIXEL_COLOR_RGBA8I,       // int8_t
    PIXEL_COLOR_RGBA8UI,      // uint8_t
    PIXEL_COLOR_RGBA16I,      // int16_t
    PIXEL_COLOR_RGBA16UI,     // uint16_t
    PIXEL_COLOR_RGBA32I,      // int32_t
    PIXEL_COLOR_RGBA32UI,     // uint32_t
    PIXEL_COLOR_RGBA16F,      // float16
    PIXEL_COLOR_RGBA32F,      // float

    // TODO: srgb color 
    // TODO: compressed textures 

    // DEPTH COMPONENT 
    PIXEL_DEPTH_D16UI,
    PIXEL_DEPTH_D32F,

    // DEPTH STENCIL
    PIXEL_DEPTH_STENCIL_D24S8UI,
    PIXEL_DEPTH_STENCIL_D32F_S8UI
};

enum
{
    TEXTURE_TYPE_1D,
    TEXTURE_TYPE_1D_ARRAY,
    TEXTURE_TYPE_2D,
    TEXTURE_TYPE_2D_ARRAY,
    TEXTURE_TYPE_3D,
    TEXTURE_TYPE_CUBE,
    TEXTURE_TYPE_CUBE_ARRAY
};

typedef enum
{
    BUFFER_USAGE_NONE = -1,
    BUFFER_USAGE_VERTEX,        // vertex buffer 
    BUFFER_USAGE_ELEMENT,       // index buffer 
    BUFFER_USAGE_PIXEL,         // pixel storage buffer
    BUFFER_USAGE_SHADER         // shader storage buffer 
} bufferUsage_t;

enum
{
    SHADER_STAGE_VERTEX = 0,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_GEOMETRY,
    SHADER_STAGE_COMPUTE,
    SHADER_STAGE_TESS_CONTROL,
    SHADER_STAGE_TESS_EVALUATION,
    SHADER_STAGE_MAX
};

/*
===========================================================================
crTextureSampler
===========================================================================
*/
/// @brief texture sampler
/// @note this class is used to create a texture sampler object and control texture image sampling, filterin, repeating, etc.
class crTextureSampler
{
public:
    virtual ~crTextureSampler( void ) = default;

    /// @brief create the sampler
    /// @param minFilter the minification filter
    /// @param magFilter the magnification filter
    /// @param wrapS the wrap mode for the S axis
    /// @param wrapT the wrap mode for the T axis
    /// @param anisotropicLevel the anisotropic level
    /// @return true if the sampler was created successfully
    virtual bool    Create( const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel, const float LODBias ) = 0;
    
    /// @brief destroy the sampler
    virtual void    Destroy( void ) = 0;
};

/*
===========================================================================
crTexture
===========================================================================
*/

/// @brief  texture image
/// @note   this class is used to create a texture image object that store and control texture image loading, binding, etc.
class crTexture
{
public:
    crTexture( void );
    virtual ~crTexture( void);
  
    /// @brief create the texture image, and allocate the memory space
    /// @param width base width of the texture
    /// @param height base height of the texture
    /// @param depth base depth of the texture
    /// @param layers base layers of the texture
    /// @param mips num of mipmaps
    /// @param format texture color format
    /// @param type texture data type
    /// @return false on error 
    virtual bool    Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const uint32_t format, const uint32_t type ) = 0;

    /// @brief destroy the texture image, and realease the memory
    virtual void    Destroy( void ) = 0;

    /// @brief copy the image data from the transfer buffer 
    /// @param buffer origin buffer 
    /// @param rowLength 
    /// @param imageMap // image coordenates
    virtual void    CopyBufferToImage( const crBuffer* buffer, const uint32_t rowLength, const bufferTextureRect_t * imageMap, const uint32_t count ) = 0;

    /// @brief Return the binding index in the texture buffer binding 
    /// @return the texture index in the buffer or -1 if not bind 
    int32_t GetBindingIndex( void ) const { return m_bindindex; }

    /// @brief Set the texture binding index in the texture buffer 
    /// @param index the index 
    void    SetBinding( const int32_t index ) { m_bindindex = index; }

    /// @brief 
    /// @param sampler 
    virtual void    MakeResident( const crTextureSampler* sampler ) {};
    
    /// @brief 
    /// @param  
    virtual void    Unmakeresident( void ) {};

protected:
    uint32_t    m_width;        // texture width
    uint32_t    m_height;       // texture height
    uint32_t    m_depth;        // texture depth
    uint32_t    m_layers;       // texture layers
    uint32_t    m_mipcount;     // texture mipmap count
    uint32_t    m_samples;      // texture smple count
    int32_t     m_bindindex;    // texture binding index -1 if not in texture binding buffer 
};

struct shaderProgram_t
{
    uint32_t        stage = 0;
    uint32_t        count = 0;
    size_t*         sizes = 0;
    uint32_t**      sources = 0;
};

struct vertexAttribute_t
{
    uint32_t        binding = 0;    // buffer binding index
    uint32_t        location = 0;   // location in the shader
    uint32_t        elements = 0;   // elements per vertex
    uint32_t        format = 0;     // data format
    uint32_t        offset = 0;     // offset in the vertex
    uint32_t        normalized = 0; // normalized flag
};

#endif //__BACKEND_API_WRAPER_H__