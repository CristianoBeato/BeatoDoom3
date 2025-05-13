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

#ifndef __GL_TEXTURE_H__
#define __GL_TEXTURE_H__

// helper structure form image format
struct glInternalFormat_t
{
    glInternalFormat_t( void ) : format( 0 ){}
    glInternalFormat_t( const GLenum _format ) : format( _format ) {}

    GLenum    GetInternalFormat( void ) const { return format; }
    GLenum    GetFormat( void ) const;
//    uint32_t    GetElementSize( void ) const;
    GLenum    GetDataType( void ) const;

private:
    GLenum format;
};

/// @brief  texture image
/// @note   this class is used to create a texture image object that store and control texture image loading, binding, etc.
class crGLTexture : public crTexture
{
public:
    crGLTexture( void );
    ~crGLTexture( void);

    virtual bool    Create( const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t layers, const uint32_t mips, const GLenum format, const uint32_t type );
    virtual void    Destroy( void );
    virtual void    CopyBufferToImage( const crBuffer* buffer, const uint32_t rowLength, const bufferTextureRect_t * imageMap, const uint32_t count );
    virtual void    MakeResident( const crTextureSampler* sampler );
    virtual void    Unmakeresident( void );

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

private:
    GLuint              m_texture;          // texture object
    GLenum              m_target;           // texture target
    GLuint64            m_bindingHandler;   // texture binding handler  
    glInternalFormat_t  m_format;
};

/// @brief texture sampler
/// @note this class is used to create a texture sampler object and control texture image sampling, filterin, repeating, etc.
class crGLTextureSampler : public crTextureSampler
{
public:
    crGLTextureSampler( void );
    ~crGLTextureSampler( void );

    /// @brief create the sampler
    /// @param minFilter the minification filter
    /// @param magFilter the magnification filter
    /// @param wrapS the wrap mode for the S axis
    /// @param wrapT the wrap mode for the T axis
    /// @param anisotropicLevel the anisotropic level
    /// @return true if the sampler was created successfully
    virtual bool    Create( const uint32_t minFilter, const uint32_t magFilter, const uint32_t wrapS, const uint32_t wrapT, const float anisotropicLevel, const float LODBias );
    
    /// @brief destroy the sampler
    virtual void    Destroy( void );

    /// @brief get the sampler handle
    GLuint      GetHandler( void ) const { return m_sampler; }

private:
    GLuint      m_sampler;
};

#endif // !__TEXTURE_H__