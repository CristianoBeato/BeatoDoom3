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

#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "renderer/backend/Backend_apiwrapper.h"
#include "qvk.h"
#include "vkShaderStorage.h"

crVKShaderStorage::crVKShaderStorage(void)
{
}

crVKShaderStorage::~crVKShaderStorage(void)
{
}

void crVKShaderStorage::StartUp(void)
{
    CreateBuffer();
    CreateDescriptor();
    // reserve temp unifom 
    m_vertexUniform.Alloc( SHADER_VERTEX_BLOCK_SIZE );
    m_fragmentUniform.Alloc( SHADER_FRAGMENT_BLOCK_SIZE );
    m_lightUniform.Alloc( SHADER_LIGHT_BLOCK_SIZE );

}

void crVKShaderStorage::ShutDown(void)
{
    // release out copy space 
    if( m_vertexUniform ) 
        m_vertexUniform.Free();

    if( m_fragmentUniform ) 
        m_fragmentUniform.Free();
    
    if( m_lightUniform ) 
        m_lightUniform.Free();

    DestroyDescriptor();
    DestroyBuffers();
}

void crVKShaderStorage::Begin(void)
{
    auto device = tr.vulkan->GetDevice();

    // get buffers handlers
    auto vertexTransformBuffer = m_vertexUniformSSBO.DynamicCast<crVKBuffer>()->GetHandler();
    auto fragmentMaterialBuffer = m_fragmentUniformSSBO.DynamicCast<crVKBuffer>()->GetHandler();
    auto lightBuffer = m_lightUniformSSBO.DynamicCast<crVKBuffer>()->GetHandler();
    auto textureBuffer = m_textureHandlerSSBO.DynamicCast<crVKBuffer>()->GetHandler();

    VkDescriptorBufferInfo bufferInfos[4] = 
    {
        { vertexTransformBuffer,    m_frameOffsetTextureHandler,    FRAME_TEXTURE_HANDLE_SIZE },
        { vertexTransformBuffer,    m_frameOffsetVertex,            FRAME_UNIFORM_VERTEX_SIZE },
        { fragmentMaterialBuffer,   m_frameOffsetFragment,          FRAME_UNIFORM_FRAGMENT_SIZE },
        { lightBuffer,              m_frameOffsetLight,             FRAME_UNIFORM_LIGHT_SIZE }
    };
    
    VkWriteDescriptorSet writes[4] = {};
    
    for ( int i = 0; i < 4; ++i) 
    {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = m_descriptorSet;
        writes[i].dstBinding = i;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writes[i].pBufferInfo = &bufferInfos[i];
    }
    
    // update buffers offsets 
    vkUpdateDescriptorSets( device, 4, writes, 0, nullptr );

     // update the copy uniforms 
     m_unformOffsetVertex = m_frameOffsetVertex; 
     m_unformOffsetFragment = m_frameOffsetFragment; 
     m_unformOffsetLight = m_frameOffsetLight; 
     m_currentTextureIndex = m_frameOffsetTextureHandler != 0 ? ( m_frameOffsetTextureHandler / sizeof( GLuint64 ) ) : 0;
}

void crVKShaderStorage::CreateBuffer(void)
{
    crAutoPointer<crVKBuffer> vertexUniformSSBO;
    crAutoPointer<crVKBuffer> fragmentUniformSSBO;
    crAutoPointer<crVKBuffer> lightUniformSSBO;
    crAutoPointer<crVKBuffer> textureHandlerSSBO;

    vertexUniformSSBO.New();
    fragmentUniformSSBO.New();
    lightUniformSSBO.New();
    textureHandlerSSBO.New();

    // Create buffers 
    vertexUniformSSBO->Create( UNIFORMS_BUFFER_VERTEX_SIZE ); // Create vertex uniform buffer storage 
    fragmentUniformSSBO->Create( UNIFORMS_BUFFER_FRAGMENT_UNIFORMS_SIZE ); // Create fragment uniform buffer storage 
    lightUniformSSBO->Create( UNIFORMS_BUFFER_LIGHT_UNIFORMS_SIZE ); // Create light uniform buffer stogare
    textureHandlerSSBO->Create( TEXTURE_BUFFER_HANDLES_SIZE ); // Create texture binding buffer 

    // reference the buffer 
    m_vertexUniformSSBO = vertexUniformSSBO.DynamicCast<crBuffer>();
    m_fragmentUniformSSBO = m_fragmentUniformSSBO.DynamicCast<crBuffer>();
    m_lightUniformSSBO = m_lightUniformSSBO.DynamicCast<crBuffer>();
    m_textureHandlerSSBO = m_textureHandlerSSBO.DynamicCast<crBuffer>();
}

void crVKShaderStorage::DestroyBuffers(void)
{
    // release our buffer
    if( m_lightUniformSSBO )
    { 
        m_lightUniformSSBO->Destroy();
        m_lightUniformSSBO.Delete();
    }

    if( m_fragmentUniformSSBO ) 
    {
        m_fragmentUniformSSBO->Destroy();
        m_fragmentUniformSSBO.Delete();
    }

    if( m_vertexUniformSSBO ) 
    {
        m_vertexUniformSSBO->Destroy();
        m_vertexUniformSSBO.Delete();
    }

    if ( m_textureHandlerSSBO )
    {
        m_textureHandlerSSBO->Destroy();
        m_textureHandlerSSBO.Delete();
    }
}

void crVKShaderStorage::CreateDescriptor(void)
{
    VkResult res = VK_SUCCESS;
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    VkDescriptorSetLayoutBinding bindings[4] = {};
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    // Texture storage
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    // Vertex Transform
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // Fragment Material
    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Light Buffer (usado nos dois shaders)
    bindings[3].binding = 3;
    bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[3].descriptorCount = 1;
    bindings[3].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 4;
    layoutInfo.pBindings = bindings;

    res = vkCreateDescriptorSetLayout( device, &layoutInfo, &allocator, &m_descriptorSetLayout );
    if( res != VK_SUCCESS );
    {
        common->Error( "vkCreateDescriptorSetLayout" );
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = 4; // 4 bindings 

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 5; // quantos sets você quer alocar do pool

    res = vkCreateDescriptorPool( device, &poolInfo, &allocator, &m_descriptorPool); 
    if ( res != VK_SUCCESS)
    {
        common->Error("vkCreateDescriptorPool");
    }
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    res = vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSet );
    if( res != VK_SUCCESS )
    {
        common->Error("vkAllocateDescriptorSets");
    }
}

void crVKShaderStorage::DestroyDescriptor(void)
{
    auto device = tr.vulkan->GetDevice();
    auto allocator = tr.vulkan->GetAllocator();

    if ( m_descriptorSet != VK_NULL_HANDLE  )
    {
        vkFreeDescriptorSets( device, m_descriptorPool, 1, &m_descriptorSet );
        m_descriptorSet = VK_NULL_HANDLE;
    }

    if ( m_descriptorPool != VK_NULL_HANDLE )
    {
        vkDestroyDescriptorPool( device, m_descriptorPool, &allocator );
        m_descriptorPool = VK_NULL_HANDLE;
    }

    if( m_descriptorSetLayout != VK_NULL_HANDLE )
    {
        vkDestroyDescriptorSetLayout( device, m_descriptorSetLayout, &allocator );
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }
}
