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

#ifndef __RENDERPIPE_H__
#define __RENDERPIPE_H__

// BEATO IDEA:
// 1. Create a RenderPipe class that handles the rendering pipeline.
// 2. on future create fixed size tables for the viewport and scissor  

static const uint32_t MAX_VIEWPORTS = 128;
static const uint32_t MAX_SCISSORS = 128;

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
    uint32_t        stage;
    uint32_t        count;
    size_t*         sizes;
    uint32_t**      sources;
};

struct frameBuffer_t
{
    uint32_t        width;
    uint32_t        height;
    uint32_t        samples;
    uint32_t        colorFormat;
    uint32_t        depthFormat;
    uint32_t        attachamentCount;
    crTexture**     colorAttachament = nullptr;
    crTexture*      depthStencilAttachament = nullptr;
};

struct vertexAttribute_t
{
    uint32_t        binding;    // buffer binding index
    uint32_t        location;   // location in the shader
    uint32_t        elements;   // elements per vertex
    uint32_t        format;     // data format
    uint32_t        offset;     // offset in the vertex
    uint32_t        normalized; // normalized flag
};

class crBuffer;
class crPipeline
{
public:
crPipeline( void );
    ~crPipeline( void );

    /// @brief Create render pipeline objects
    /// @param program 
    /// @param frameBuffer 
    /// @param attributes 
    /// @param numAttributes 
    void        Create( const shaderProgram_t* program, const frameBuffer_t* frameBuffer, const vertexAttribute_t* attributes, const size_t numAttributes );
    
    // release pipeline objects 
    void        Destroy( void );

    /// @brief Begin pileline, binding the needed states
    /// @param  
    void        Begin( void );

    /// @brief Finish and flush the pipeline
    /// @param  
    void        End( void );

    /// @brief attach a vertex buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    void        AttachVertexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    
    /// @brief attach an index buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    void        AttachIndexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    
    /// @brief attach a uniform buffer to the pipeline
    /// @param buffer the buffer to attach
    /// @param bindingID the binding id of the buffer
    /// @param offset the offset in the buffer
    /// @param size the size of the buffer
    /// @note the offset and size are in bytes
    void        AttachUniformBuffer( crBuffer* buffer, const uint32_t bindingID, uintptr_t offset, const size_t size );

    /// @brief append a new viewport to the list
    /// @param x position on the x axis
    /// @param y position on the y axis
    /// @param width viewport width
    /// @param height viewport height
    void        SetViewport( int x, int y, int width, int height );

    /// @brief append a new scissor to the list
    /// @param x position on the x axis
    /// @param y position on the y axis
    /// @param width scissor width
    /// @param height scissor height
    void        SetScissor( int x, int y, int width, int height );

    /// @brief Get the number of viewports 
    /// @return thew viewport count 
    uint32_t    GetNumViewports( void ) const { return m_numViewports; }

    /// @brief Get the number of scissors
    /// @return the scissor count
    uint32_t    GetNumScissors( void ) const { return m_numScissors; }

    /// @brief Get the current viewport
    /// @return the current viewport index
    uint32_t    GetCurrentViewport( void ) const { return m_numViewports - 1; }

    /// @brief Get the current scissor
    /// @return the current scissor index
    uint32_t    GetCurrentScissor( void ) const { return m_numScissors - 1; }

    /// @brief Clear the pipeline state and buffers
    void        Clear( void );

private:
    uint32_t        m_numViewports;
    uint32_t        m_numScissors;
    float           m_viewports[MAX_VIEWPORTS][4];
    float           m_scissors[MAX_SCISSORS][4];

#if CR_USE_VULKAN
    VkPipelineLayout  m_pipelineLayout;
    VkPipeline        m_pipeline;
    VkDescriptorSet   m_descriptorSet;
    VkDescriptorPool  m_descriptorPool;
    VkCommandBuffer   m_commandBuffer;
    VkRenderPass      m_renderPass;
    VkFramebuffer     m_framebuffer;
    VkCommandPool     m_commandPool;
#elif CR_USE_OPENGL
    GLuint            m_programPipeline;
    GLuint            m_vertexArrayObject;
    GLuint            m_frameBuffer;
    GLuint            m_shaderStages[SHADER_STAGE_MAX];
#endif

    bool    CreateFrameBuffer( const frameBuffer_t * frameBuffer );
    void    DestroyFrameBuffer( void );
    bool    CreateVertexPipeline( const vertexAttribute_t* attributes, const size_t numAttributes );
    void    DestroyVertexPipeline( void );
    bool    CreateShaderProgram( const shaderProgram_t* program );
    void    DestroyShaderProgram( void );
};

#endif // __RENDERPIPE_H__