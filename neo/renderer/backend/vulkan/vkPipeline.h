
#ifndef __VK_PIPELINE_H__
#define __VK_PIPELINE_H__

class crVKPipeline : public crPipeline
{
public:
    crVKPipeline( void );
    ~crVKPipeline( void );

    virtual void    Create( const shaderProgram_t* program, const vertexAttribute_t* attributes, const size_t numAttributes );
    virtual void    Destroy( void );
    virtual void    Begin( void );
    virtual void    End( void );
    virtual void    AttachVertexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    virtual void    AttachIndexBuffer( crBuffer* buffer, uintptr_t offset, const size_t size );
    virtual void    AttachUniformBuffer( crBuffer* buffer, const uint32_t bindingID, uintptr_t offset, const size_t size );
    virtual void    DepthBias( const float factor, const float units );
    virtual void    SetViewport( const int x, const int y, const uint32_t width, const uint32_t height );
    virtual void    SetScissor( const int x, const int y, const uint32_t width, const uint32_t height );
    virtual void    ClearColor( const float red, const float green, const float blue, const float alpha );
    virtual void    Clear( void );

private:
    VkClearValue      m_clearColor;
    VkPipelineLayout  m_pipelineLayout;
    VkPipeline        m_pipeline;
    VkDescriptorSet   m_descriptorSet;
    VkDescriptorPool  m_descriptorPool;
    VkCommandBuffer   m_commandBuffer;
    VkRenderPass      m_renderPass;
    VkFramebuffer     m_framebuffer;
    VkCommandPool     m_commandPool;

    bool    CreateVertexPipeline( const vertexAttribute_t* attributes, const size_t numAttributes );
    void    DestroyVertexPipeline( void );
    bool    CreateShaderProgram( const shaderProgram_t* program );
    void    DestroyShaderProgram( void );
};

#endif //__VK_PIPELINE_H__