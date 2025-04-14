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

/*
===========================================================================
crFence
===========================================================================
*/
class crFence 
{
public:
    virtual ~crFence( void ) = default;

    /// @brief create the fence
    virtual void Create( void ) = 0;
    
    /// @brief wait for fence to be signaled 
    virtual void wait( void ) = 0;
    
    /// @brief reset fence if signaled
    virtual void reset( void ) = 0;
    
    /// @brief check if the fence is already signaled  
    /// @return true on signaled 
    virtual bool isSignaled( void ) = 0;
};

/*
===========================================================================
crBuffer
===========================================================================
*/
class crBuffer
{
public:
    crBuffer( void );
    virtual ~crBuffer( void ) = default;

    /// @brief create buffer object and reserve size 
    /// @param size of the buffer 
    /// @return true on sucess 
    virtual bool        Create( const size_t size, const bool write = true, const bool read = false ) = 0;
    
    /// @brief destroy object and release memory
    /// @param  
    virtual void        Destroy( void ) = 0;

    /// @brief Upload buffer data
    /// @param data pointer to by copy
    /// @param offset of the data to copy 
    /// @param size of the memory to be copied
    virtual void        Upload( const void* data, const uintptr_t offset, const size_t size ) const;

    /// @brief copy data from buffer to pointer 
    /// @param data location to copy 
    /// @param offset of the buffer to copy
    /// @param size size to copy 
    void        Download( void* data, const uintptr_t offset, const size_t size ) const;

    /// @brief buffer raw pointer
    /// @return 
    void*       GetMap( void ) const { return m_map; }

    /// @brief buffer total size 
    /// @return size of the buffer 
    size_t      GetSize( void ) const { return m_size; }

protected:
    size_t  m_size;
    void*   m_map;
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


// ===========================================================================

//vertex shader storage estructure:
//struct vetexTransform
//{
//  vec4 rpLocalViewOrigin; 
//  vec4 rpColorModulate;   //
//  vec4 rpColorAdd;        //
//  vec4 rpClipBounds;
//  mat4 rpTextureMatrix;
//  mat4 rpModelMatrix;
//  mat4 rpViewMatrix;
//  mat4 rpProjectionMatrix;
//};

// fragment shader storage structure 
//struct fragmentTransfom
//{
//    uint32_t    sampler[8]; // we can acess the max of 8 textures from the sampler buffer 
//    vec4        rpCurrentRenderSize;
//    vec4        rpDiffuseColor;
//    vec4        rpSpecularColor;
//    vec4        shaderParm0;
//    vec4        shaderParm1;
//    vec4        shaderParm2;
//    vec4        shaderParm3;
//};

//interaction shader pass transform
//struct lightTransform
//{
//    vec4 rpLocalLightOrigin;
//    vec4 rpLightProjectionS;
//    vec4 rpLightProjectionT;
//    vec4 rpLightProjectionQ;
//    vec4 rpLightFallOff;
//
//};

// ATTENTION: Must match the definition in the shader.
// 0 is for the texture sampler array 
static const uint32_t   SHADER_BUFFER_BINDING_VERTEX_BLOCK = 1;     // layout( std430, binding = 1 ) buffer vertexStorageBlock
static const uint32_t   SHADER_BUFFER_BINDING_FRAGMENT_BLOCK = 2;   // layout( std430, binding = 2 ) buffer fragmentStorageBlock
static const uint32_t   SHADER_BUFFER_BINDING_LIGHT_BLOCK = 3;      // layout( std430, binding = 3 ) buffer lightStorageBlock

static const size_t     SHADER_VERTEX_BLOCK_SIZE;
static const size_t     SHADER_FRAGMENT_BLOCK_SIZE;
static const size_t     SHADER_LIGHT_BLOCK_SIZE;

static const size_t     FRAME_UNIFORM_VERTEX_SIZE;       // ~1,25 mb
static const size_t     FRAME_UNIFORM_FRAGMENT_SIZE;   // ~576 kb
static const size_t     FRAME_UNIFORM_LIGHT_SIZE;         // ~320 kbb
static const size_t     FRAME_TEXTURE_HANDLE_SIZE;     // ~256 kb

static const size_t     UNIFORMS_BUFFER_VERTEX_SIZE;
static const size_t     UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE;
static const size_t     UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE;
static const size_t     TEXTURE_BUFFER_HANDLES_SIZE;

// ===========================================================================

enum
{
    // VERTEX SHADER UNIFORMS
    VERTEX_UNIFORM_LOCATION_LOCAL_VIEW_ORIGIN = 0,    // vec4 rpLocalViewOrigin;
    VERTEX_UNIFORM_LOCATION_COLOR_MODULATE,           // vec4 rpColorModulate;
    VERTEX_UNIFORM_LOCATION_COLOR_ADD,                // vec4 rpColorAdd;
    VERTEX_UNIFORM_LOCATION_TEXTURE_MATRIX,           // vec4 rpTextureMatrix;
    VERTEX_UNIFORM_LOCATION_CLIP_BOUDS,               // vec4 rpClipBounds;
    VERTEX_UNIFORM_LOCATION_MODEL_MATRIX,             // mat4 rpModelMatrix;
    VERTEX_UNIFORM_LOCATION_VIEW_MATRIX,              // mat4 rpViewMatrix;
    VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX,        // mat4 rpProjectionMatrix;
    
    // FRAGMENT SHADER UNIFORMS
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS0,              // uint32_t    sampler[0];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS1,              // uint32_t    sampler[1];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS2,              // uint32_t    sampler[2];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS3,              // uint32_t    sampler[3];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS4,              // uint32_t    sampler[4];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS5,              // uint32_t    sampler[5];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS6,              // uint32_t    sampler[6];
    FRAGMENT_UNIFORM_LOCATION_SAMPLERS7,              // uint32_t    sampler[7];
    FRAGMENT_UNIFORM_LOCATION_CURRENT_RENDER_SIZE,    // vec4 rpCurrentRenderSize;
    FRAGMENT_UNIFORM_LOCATION_DIFUSE_COLOR,           // vec4 rpDiffuseColor;
    FRAGMENT_UNIFORM_LOCATION_SPECULAR_COLOR,         // vec4 rpSpecularColor;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM0,           // vec4 shaderParm0;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM1,           // vec4 shaderParm1;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM2,           // vec4 shaderParm2;
    FRAGMENT_UNIFORM_LOCATION_SHADER_PARM3,           // vec4 shaderParm3;
    
    // LIGHT SHADER UNIFORMS
    LIGHT_UNIFORM_LOCATION_LOCAL_ORIGIN,              // vec4 rpLocalLightOrigin;
    LIGHT_UNIFORM_LOCATION_PROJECTION_S,              // vec4 rpLightProjectionS;
    LIGHT_UNIFORM_LOCATION_PROJECTION_T,              // vec4 rpLightProjectionT;
    LIGHT_UNIFORM_LOCATION_PROJECTION_Q,              // vec4 rpLightProjectionQ;
    LIGHT_UNIFORM_LOCATION_FALLOFF,                   // vec4 rpLightFallOff;
    MAX_UNIFORMS
};

/*
===========================================================================
crShaderStorage
===========================================================================
*/
class crShaderStorage
{
public:
    crShaderStorage( void );
    virtual ~crShaderStorage( void ) = default;

    /// @brief create buffer, allocate memory, create temp uniforms
    virtual void    StartUp( void ) = 0;

    /// @brief Release buffers and temporary uniforms 
    virtual void    ShutDown( void ) = 0;
   
    /// @brief load texture into the hanlde buffer, and store offset
    /// @param binding the texture binding location
    /// @param texture texture object handler 
    /// @param sampler texture sampling object handler 
    virtual void        BindTexture( const uint32_t binding, crAutoPointer<crTexture> texture, crAutoPointer<crTextureSampler> sampler ) = 0;
        
    ///@brief Begin frame, bind buffers    
    virtual void    Begin( void ) = 0;
   
    /// @brief swap buffer offsets 
    void    End( void );
   
    /// @brief copy our uniform to temporary memory  
    /// @param uniform uniform data 
    /// @param location uniform block location enum
    void    SetUniform( const void* uniform, const uint32_t location );
       
    /// @brief flush ou temp uniform memry to the buffer  
    void    Submit( void );

protected:
    // current texture index
    uint32_t                    m_currentTextureIndex;          // current texture index
    uint32_t                    m_textureCount;                 // attached texture count
    uintptr_t                   m_unformOffsetVertex;           // vertex block position in frame 
    uintptr_t                   m_unformOffsetFragment;         // fragment block positio in frame
    uintptr_t                   m_unformOffsetLight;            // light block position in frame 
    uintptr_t                   m_frameOffsetVertex;            // vertex buffer region positio 
    uintptr_t                   m_frameOffsetFragment;          // fragment buffer region position
    uintptr_t                   m_frameOffsetLight;             // light buffer offset position 
    uintptr_t                   m_frameOffsetTextureHandler;    // texture handler buffer offset position 
    crPointer<byte*>            m_vertexUniform;                //
    crPointer<byte*>            m_fragmentUniform;              //
    crPointer<byte*>            m_lightUniform;                 //
    crAutoPointer<crBuffer>     m_vertexUniformSSBO;            // vertex shader storage buffer 
    crAutoPointer<crBuffer>     m_fragmentUniformSSBO;          // fragment  shader storage buffer
    crAutoPointer<crBuffer>     m_lightUniformSSBO;             // light shader storage buffer
    crAutoPointer<crBuffer>     m_textureHandlerSSBO;           // texture handler shader storage buffer
    crAutoPointer<crTexture>    m_bindTextures[32768];          // chage the number if the FRAME_TEXTURE_HANDLE_SIZE has changed 
};

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

class crPipeline
{
public:
    virtual ~crPipeline( void ) = default;

    /// @brief Create render pipeline objects
    /// @param program 
    /// @param frameBuffer 
    /// @param attributes 
    /// @param numAttributes 
    virtual void    Create( const shaderProgram_t* program, const vertexAttribute_t* attributes, const size_t numAttributes ) = 0;
    
    // release pipeline objects 
    virtual void    Destroy( void ) = 0;

    /// @brief Begin pileline, binding the needed states
    /// @param  
    virtual void    Begin( void ) = 0;

    /// @brief Finish and flush the pipeline
    /// @param  
    virtual void    End( void ) = 0;

    /// @brief attach a vertex buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    virtual void    AttachVertexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size ) = 0;
    
    /// @brief attach an index buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    virtual void AttachIndexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size ) = 0;
    
    /// @brief attach a uniform buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param bindingID the binding id of the buffer
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    /// @note the offset and size are in bytes
    virtual void    AttachUniformBuffer( crBuffer* buffer, const uint32_t bindingID, uintptr_t offset, const size_t size ) = 0;

    /// @brief append a new viewport to the list
    /// @param x position on the x axis
    /// @param y position on the y axis
    /// @param width viewport width
    /// @param height viewport height
    virtual void    SetViewport( int x, int y, int width, int height ) = 0;

    /// @brief append a new scissor to the list
    /// @param x position on the x axis
    /// @param y position on the y axis
    /// @param width scissor width
    /// @param height scissor height
    virtual void    SetScissor( int x, int y, int width, int height ) = 0; 

    /// @brief set the buffer clear color
    /// @param red 
    /// @param green 
    /// @param blue 
    /// @param alpha 
    virtual void ClearColor( const float red, const float green, const float blue, const float alpha ) = 0;

    /// @brief Clear the pipeline state and buffers
    virtual void    Clear( void ) = 0;
};

/*
===========================================================================
crCommandQueue
===========================================================================
*/
class crCommandQueue
{
public:
    virtual ~crCommandQueue( void ) = default;
    virtual void    Create( void ) = 0;
    virtual void    Destroy( void ) = 0;
    virtual void    Begin( void ) = 0;
    virtual void    End( void ) = 0;
};

/*
===========================================================================
crFramebuffer
===========================================================================
*/
struct frameBuffer_t
{
    uint32_t        width;
    uint32_t        height;
    uint32_t        samples;
    uint32_t        layers;
    uint32_t        attachmentCount;
    crTexture**     colorAttachament = nullptr;
};

class crFramebuffer
{
public:
    ~crFramebuffer( void ) = default;
    virtual void    Create( const frameBuffer_t* frameBufferCreateInfo ) = 0;
    virtual void    Destroy( void ) = 0;
};

/*
===========================================================================
crSwapChain
===========================================================================
*/
class crSwapChain
{
public:
    ~crSwapChain( void ) = default;
    virtual void            Create( void ) = 0;
    virtual void            Destroy( void ) = 0;
    virtual const uint32_t  GetImageCount( void ) const = 0;
};

#endif //__BACKEND_API_WRAPER_H__