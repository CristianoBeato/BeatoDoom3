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
#include "renderer/renderer_common.h"
#include "renderer/backend/Backend_common.h"
#include "ShaderStorage.hpp"

static const uint32_t k_MAX_VERTEX_UNIFORM_BLOCKS = 1024 * 32; // max 32k geometrys per frame
static const uint32_t k_MAX_FRAGMENT_UNIFORM_BLOCKS = 1024 * 32; // max 32k geometrys per frame
static const uint32_t k_MAX_LIGHT_UNIFORM_BLOCKS = 1000 * 12; // 16 klights

/*
===========================================================================
crUniformBlock
===========================================================================
*/

/*
=======================
crUniformBlock::Create
=======================
*/
void crUniformBlock::Create( const uint32_t in_blockElements, const uint32_t in_blockCount )
{
    auto renderer = tr.GetRenderDevice();
    static const VkMemoryPropertyFlags k_staginProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    static const VkMemoryPropertyFlags k_localProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    m_blockSize = sizeof( float ) * 4 * in_blockElements;
    m_bufferSize = m_blockSize * in_blockCount;

    // Vertex shader buffer object
    m_shaderStorageBuffer = crAutoPointer<crvkBufferStatic>::New();
    m_shaderStorageBuffer->Create( &renderer, m_bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, k_localProperties );
    
    // Staging vertex shader buffer object
    m_shaderStorageBufferStagin = crAutoPointer<crvkBuffer>::New();
    m_shaderStorageBufferStagin->Create( &renderer, m_bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, k_staginProperties );
    m_bufferMap = static_cast<uniformVec4_t*>( m_shaderStorageBufferStagin->Map( 0, m_bufferSize, CRVK_BUFFER_MAP_ACCESS_WRITE ) );

    m_region.head = 0;
    m_region.tail = 0;
}

/*
=======================
crUniformBlock::Destoy
=======================
*/
void crUniformBlock::Destoy( void )
{
    if( m_shaderStorageBuffer )
        m_shaderStorageBuffer->Destroy();

    if( m_shaderStorageBufferStagin )
    {
        m_shaderStorageBufferStagin->Unmap();
        m_shaderStorageBufferStagin->Destroy();
    }
}

/*
=======================
crUniformBlock::Flush
=======================
*/
void crUniformBlock::Flush( void )
{
    // copy content from staging buffer to the device buffer
    VkBufferCopy2 copy{};
    copy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    copy.pNext = nullptr;
    copy.size = m_region.head - m_region.tail;
    copy.dstOffset = m_region.tail;
    copy.srcOffset = m_region.tail;
    m_shaderStorageBuffer->CopyFromBuffer( m_shaderStorageBufferStagin->Handle(), &copy, 1 );
    m_region.tail = m_region.head;
}

/*
=======================
crUniformBlock::Submit
=======================
*/
void crUniformBlock::Submit( void )
{
    std::memcpy( &m_bufferMap[m_region.head], m_block, m_blockSize );

    if( ( m_region.head + m_blockSize ) <= m_bufferSize )
    {
        m_region.head += m_blockSize;
    }
    else
    {
        Flush();
        // back to the buffer start
        m_region.head = 0;
        m_region.tail = 0;
    }
}

/*
=======================
crUniformBlock::ProgramParameter4fv
=======================
*/
void crUniformBlock::ProgramParameter4fv( const renderParms_t in_renderParm, const uint32_t in_count, const float* in_parms )
{
    assert( in_renderParm < m_blockSize );
    std::memcpy( &m_block[in_renderParm], in_parms, sizeof( uniformVec4_t ) * in_count );
}

/*
=======================
crUniformBlock::ProgramParameter4iv
=======================
*/
void crUniformBlock::ProgramParameter4iv(const renderParms_t in_renderParm, const uint32_t in_count, const int32_t *in_parms )
{
    assert( in_renderParm < m_blockSize );
    std::memcpy( &m_block[in_renderParm], in_parms, sizeof( uniformVec4_t ) * in_count );
}
