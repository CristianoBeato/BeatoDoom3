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

#include "precompiled.h"
#pragma hdrstop

#include "renderer_common.h"
#include "VertexCache.h"

static const size_t VERTCACHE_INDEX_MEMORY =  1048576 * 128; // 128 mb for index 
static const size_t VERTCACHE_VERTEX_MEMORY = 1048576 * 768; // 768 mb for vertex 

idCVar idVertexCache::r_showVertexCache( "r_showVertexCache", "0", CVAR_INTEGER|CVAR_RENDERER, "" );
idCVar idVertexCache::r_vertexBufferMegs( "r_vertexBufferMegs", "32", CVAR_INTEGER|CVAR_RENDERER, "" );

idVertexCache							vertexCache;
static idBlockAlloc<vertCache_t,1024>	headerAllocator = idBlockAlloc<vertCache_t,1024>();

/*
==============
R_ListVertexCache_f
==============
*/
static void R_ListVertexCache_f( const idCmdArgs &args ) 
{
	vertexCache.List();
}

/*
==============
crCache::crCache
==============
*/
crCache::crCache( const size_t size, bool elements ) : m_allocedSize( size )
{
#if CR_USE_VULKAN
	crAutoPointer<crVKBuffer> buffer = crAutoPointer<crVKBuffer>();
	buffer.New();
	m_buffer = buffer.DynamicCast<crBuffer>();
#elif CR_USE_OPENGL
	crAutoPointer<crGLBuffer> buffer = crAutoPointer<crGLBuffer>();
	buffer.New();
	m_buffer = buffer.DynamicCast<crBuffer>();
#endif //CR_USE_OPENGL

	// Create the buffer
	m_buffer->Create( elements ? BUFFER_USAGE_ELEMENT : BUFFER_USAGE_VERTEX, size, true );
}

/*
==============
crCache::~crCache
==============
*/
crCache::~crCache( void )
{
	// release buffer object 
	m_buffer->Destroy();
}

/*
==============
crCache::Alloc
==============
*/
vertCache_t *crCache::Alloc(const size_t size)
{
	uintptr_t		endPos = 0;
	vertCache_t*	block = nullptr;

	// if we don't have any remaining unused headers, allocate some more
	if ( m_freeHeadersList == nullptr || m_freeHeadersList == m_freeHeadersList->next )
		AllocHeaders();

	// 
	endPos = m_memUsed.Add( size );
	if( endPos > m_allocedSize )
		idLib::Error( "Out of cache" );

	// new allocation 
	m_allocations.Add( 1 );

	
	// move it from the m_freeStaticVertexHeaders list to the staticHeaders list
	block = m_freeHeadersList->next;
	block->next->prev = block->prev;
	block->prev->next = block->next;
	
	//
	block->next = m_usedHeadersList->next;
	block->prev = m_usedHeadersList;
	block->next->prev = block;
	block->prev->next = block;
	
	block->size = size;
	block->offset = endPos - size;
	block->flags |= CACHE_TAG_USED;	// Set used tag 

    return nullptr;
}

/*
==============
crCache::Free
==============
*/
void crCache::Free( vertCache_t *block )
{
	block->flags |= CACHE_TAG_FREE;		// mark as free

	// unlink stick it back on the free list
	block->next->prev = block->prev;
	block->prev->next = block->next;

#if 1
	// stick it on the front of the free list so it will be reused immediately
	block->next = m_freeHeadersList->next;
	block->prev = m_usedHeadersList;
#else
	// stick it on the back of the free list so it won't be reused soon (just for debugging)
	block->next = m_usedHeadersList;
	block->prev = m_freeHeadersList->prev;
#endif

	block->next->prev = block;
	block->prev->next = block;
}

/*
==============
crCache::Upload
==============
*/
void crCache::Upload( const vertCache_t *cache, const void *data )
{
	m_buffer->Upload( data, cache->offset, cache->size );
}

/*
==============
crCache::Touch
==============
*/
void crCache::Touch( vertCache_t *block )
{
	// move to the head of the LRU list
	block->next->prev = block->prev;
	block->prev->next = block->next;

	block->next = m_usedHeadersList->next;
	block->prev = m_usedHeadersList;
	m_usedHeadersList->next->prev = block;
	m_usedHeadersList->next = block;

	// send to GPU
	m_buffer->Flush( block->offset, block->size );
}

/*
==============
crCache::Purge
==============
*/
void crCache::Purge(void)
{
	while( m_usedHeadersList->next != m_usedHeadersList ) 
	{
		Free( m_usedHeadersList->next );
	}
}

/*
==============
crCache::AllocHeaders
==============
*/
void crCache::AllocHeaders(void)
{
	vertCache_t*	block = nullptr;

	// allocate new blocks on free list
	for ( int i = 0; i < 1024; i++ ) 
	{
		block = headerAllocator.Alloc();
		block->next = m_freeHeadersList->next;
		block->prev = m_freeHeadersList;
		block->next->prev = block;
		block->prev->next = block;			
	}	
}

/*
==============
idVertexCache::ActuallyFree
==============
*/
void idVertexCache::ActuallyFree( vertCache_t *block ) 
{
	if (!block) 
		common->Error( "idVertexCache Free: NULL pointer" );

	// temp blocks are in a shared space that won't be freed
	if ( ( block->flags & CACHE_TAG_TEMP ) == CACHE_TAG_TEMP ) 
	{
		staticAllocTotal -= block->size;
		staticCountTotal--;
	}

	if ( ( block->flags & CACHE_TAG_ELEMENT) == CACHE_TAG_ELEMENT )
		m_staticElement->Free( block );
	else
		m_staticVertex->Free( block );
}

/*
==============
idVertexCache::Position

this will be a real pointer with virtual memory,
but it will be an int offset cast to a pointer with
ARB_vertex_buffer_object

The ARB_vertex_buffer_object will be bound
==============
*/
uintptr_t idVertexCache::Position( vertCache_t *buffer ) 
{
	if ( !buffer || ( buffer->flags & CACHE_TAG_FREE ) == CACHE_TAG_FREE ) 
		common->FatalError( "idVertexCache::Position: bad vertCache_t" );

	// the ARB vertex object just uses an offset
	if ( r_showVertexCache.GetInteger() == 2 ) 
	{
		if ( ( buffer->flags == CACHE_TAG_TEMP ) == CACHE_TAG_TEMP )
			common->Printf( "GL_ARRAY_BUFFER_ARB = %i + %i (%i bytes)\n", 0, buffer->offset, buffer->size ); 
		else
			common->Printf( "GL_ARRAY_BUFFER_ARB = %i (%i bytes)\n", 0, buffer->size ); 
	}
			
	return buffer->offset;
}

//================================================================================

/*
===========
idVertexCache::Init
===========
*/
void idVertexCache::Init( void ) 
{
// BEATO Begin:
	m_staticElement = crAutoPointer<crCache>::New( VERTCACHE_INDEX_MEMORY, true );
	m_staticVertex = crAutoPointer<crCache>::New( VERTCACHE_INDEX_MEMORY, false );
//	m_dynamicElement = crAutoPointer<crCache>::New( VERTCACHE_INDEX_MEMORY, true );
//	m_dynamicVertex = crAutoPointer<crCache>::New( VERTCACHE_INDEX_MEMORY, false );
// BEATO End

	cmdSystem->AddCommand( "listVertexCache", R_ListVertexCache_f, CMD_FL_RENDERER, "lists vertex cache" );

	if ( r_vertexBufferMegs.GetInteger() < 8 ) 
		r_vertexBufferMegs.SetInteger( 8 );

	// initialize the cache memory blocks
	
	// set up the dynamic frame memory
	staticAllocTotal = 0;

	EndFrame();
}

/*
===========
idVertexCache::PurgeAll

Used when toggling vertex programs on or off, because
the cached data isn't valid
===========
*/
void idVertexCache::PurgeAll( void ) 
{
	m_staticElement->Purge();
	m_staticVertex->Purge();
}

/*
===========
idVertexCache::Shutdown
===========
*/
void idVertexCache::Shutdown( void ) 
{
	PurgeAll();
	headerAllocator.Shutdown();
}

/*
===========
idVertexCache::AllocElement
===========
*/
vertCache_t* idVertexCache::AllocElement( const size_t size, const void *data, const bool temp = false )
{
	vertCache_t	*block = nullptr;
	if ( size == 0 ) 
	{
		common->Error( "idVertexCache::AllocElement: size = %i\n", size );
		return nullptr;
	}

	if ( m_staticElement->Free() < size )
		common->FatalError("INSULFICIENT ELEMENT BUFFER, level up the avaidable buffer memory");

	//
	block = m_staticElement->Alloc( size );
	block->flags |= CACHE_TAG_ELEMENT;

	// save data for debugging
	staticAllocThisFrame += block->size;
	staticCountThisFrame++;
	staticCountTotal++;
	staticAllocTotal += block->size;

	// allocation doesn't imply used-for-drawing, because at level
	// load time lots of things may be created, but they aren't
	// referenced by the GPU yet, and can be purged if needed.
	block->frame = currentFrame - SMP_FRAMES;

	// copy the data
	m_staticElement->Upload( block, data );

    return block;
}

/*
==============
crCache::Alloc
==============
*/
vertCache_t* idVertexCache::AllocVertex( const size_t size, const void *data, const bool temp = false )
{
	vertCache_t	*block = nullptr;
	if ( size == 0 ) 
	{
		common->Error( "idVertexCache::AllocVertex: size = %i\n", size );
		return false;
	}

	if ( m_staticElement->Free() < size )
		common->FatalError("INSULFICIENT VERTEX BUFFER, level up the avaidable buffer memory");

	// if we can't find anything, it will be NULL
	*buffer = nullptr;

	//
	block = m_staticVertex->Alloc( size );
	block->flags |= CACHE_TAG_VERTEX;

	// save data for debugging
	staticAllocThisFrame += block->size;
	staticCountThisFrame++;
	staticCountTotal++;
	staticAllocTotal += block->size;

	// this will be set to zero when it is purged
	*buffer = block;

	// allocation doesn't imply used-for-drawing, because at level
	// load time lots of things may be created, but they aren't
	// referenced by the GPU yet, and can be purged if needed.
	block->frame = currentFrame - SMP_FRAMES;

	// copy the data
	m_staticVertex->Upload( block, data );

    return true;
}

/*
===========
idVertexCache::Touch
===========
*/
void idVertexCache::Touch( vertCache_t *block ) 
{
	if ( !block ) 
		common->Error( "idVertexCache Touch: NULL pointer" );

	if ( ( block->flags & CACHE_TAG_FREE ) == CACHE_TAG_FREE ) 
		common->FatalError( "idVertexCache Touch: freed pointer" );

	if ( ( block->flags & CACHE_TAG_TEMP ) == CACHE_TAG_TEMP ) 
		common->FatalError( "idVertexCache Touch: temporary pointer" );

	block->frame = currentFrame;
	if ( ( block->flags & CACHE_TAG_ELEMENT ) == CACHE_TAG_ELEMENT )
		m_staticElement->Touch( block );
	else
		m_staticVertex->Touch( block );
	
}

/*
===========
idVertexCache::Free
===========
*/
void idVertexCache::Free( vertCache_t *block ) 
{
	if (!block) 
		return;

	if ( block->flags & CACHE_TAG_FREE ) 
		common->FatalError( "idVertexCache Free: freed pointer" );
	
	if ( block->flags & CACHE_TAG_TEMP ) 
		common->FatalError( "idVertexCache Free: temporary pointer" );

	if ( block->flags & CACHE_TAG_ELEMENT )
		m_staticElement->Free( block );
	else
		m_staticVertex->Free( block );
}

/*
===========
idVertexCache::AllocFrameTemp

A frame temp allocation must never be allowed to fail due to overflow.
We can't simply sync with the GPU and overwrite what we have, because
there may still be future references to dynamically created surfaces.
===========
*/
vertCache_t	*idVertexCache::AllocFrameTemp( void *data, size_t size ) 
{
	vertCache_t	*block = nullptr;

	if ( size <= 0 ) 
		common->Error( "idVertexCache::AllocFrameTemp: size = %i\n", size );

	if ( dynamicAllocThisFrame + size > frameBytes ) 
	{
		// if we don't have enough room in the temp block, allocate a static block,
		// but immediately free it so it will get freed at the next frame
		tempOverflow = true;
		Alloc( data, size, &block );
		Free( block);
		return block;
	}

	// this data is just going on the shared dynamic list

	// if we don't have any remaining unused headers, allocate some more
	if ( freeDynamicHeaders.next == &freeDynamicHeaders ) 
	{
		for ( int i = 0; i < EXPAND_HEADERS; i++ ) {
			block = headerAllocator.Alloc();
			block->next = freeDynamicHeaders.next;
			block->prev = &freeDynamicHeaders;
			block->next->prev = block;
			block->prev->next = block;
		}
	}

	// move it from the freeDynamicHeaders list to the dynamicHeaders list
	block = freeDynamicHeaders.next;
	block->next->prev = block->prev;
	block->prev->next = block->next;
	block->next = dynamicHeaders.next;
	block->prev = &dynamicHeaders;
	block->next->prev = block;
	block->prev->next = block;

	block->size = size;
	block->tag = TAG_TEMP;
	block->indexBuffer = false;
	block->offset = dynamicAllocThisFrame;
	dynamicAllocThisFrame += block->size;
	dynamicCountThisFrame++;
	block->user = nullptr;
	block->frameUsed = 0;

	if ( block->vbo ) 
	{
		glBindBuffer( GL_ARRAY_BUFFER, block->vbo );
		glBufferSubData( GL_ARRAY_BUFFER, block->offset, (GLsizeiptr)size, data );
	} 
	else 
		SIMDProcessor->Memcpy( (byte *)block->virtMem + block->offset, data, size );

	return block;
}

/*
===========
idVertexCache::EndFrame
===========
*/
void idVertexCache::EndFrame( void ) 
{
	// display debug information
	if ( r_showVertexCache.GetBool() ) 
	{
		int	staticUseCount = 0;
		int staticUseSize = 0;

		for ( vertCache_t *block = staticHeaders.next ; block != &staticHeaders ; block = block->next ) 
		{
			if ( block->frame == currentFrame ) 
			{
				staticUseCount++;
				staticUseSize += block->size;
			}
		}

		const char *frameOverflow = tempOverflow ? "(OVERFLOW)" : "";

		common->Printf( "vertex dynamic:%i=%ik%s, static alloc:%i=%ik used:%i=%ik total:%i=%ik\n",
			dynamicCountThisFrame, dynamicAllocThisFrame/1024, frameOverflow,
			staticCountThisFrame, staticAllocThisFrame/1024,
			staticUseCount, staticUseSize/1024,
			staticCountTotal, staticAllocTotal/1024 );
	}

#if 0
	// if our total static count is above our working memory limit, start purging things
	while ( staticAllocTotal > r_vertexBufferMegs.GetInteger() * 1024 * 1024 ) {
		// free the least recently used

	}
#endif

	currentFrame = tr.frameCount;
	listNum = currentFrame % SMP_FRAMES;
	staticAllocThisFrame = 0;
	staticCountThisFrame = 0;
	dynamicAllocThisFrame = 0;
	dynamicCountThisFrame = 0;
	tempOverflow = false;

	// free all the deferred free headers
	while( deferredFreeList.next != &deferredFreeList ) 
	{
		ActuallyFree( deferredFreeList.next );
	}

	// free all the frame temp headers
	vertCache_t	*block = dynamicHeaders.next;
	if ( block != &dynamicHeaders ) 
	{
		block->prev = &freeDynamicHeaders;
		dynamicHeaders.prev->next = freeDynamicHeaders.next;
		freeDynamicHeaders.next->prev = dynamicHeaders.prev;
		freeDynamicHeaders.next = block;

		dynamicHeaders.next = dynamicHeaders.prev = &dynamicHeaders;
	}
}

/*
=============
idVertexCache::List
=============
*/
void idVertexCache::List( void ) 
{
	int	numActive = 0;
	int	numDeferred = 0;
	int frameStatic = 0;
	int	totalStatic = 0;
	int	deferredSpace = 0;

	vertCache_t *block;
	for ( block = staticHeaders.next ; block != &staticHeaders ; block = block->next) {
		numActive++;

		totalStatic += block->size;
		if ( block->frameUsed == currentFrame ) {
			frameStatic += block->size;
		}
	}

	int	numFreeStaticHeaders = 0;
	for ( block = freeStaticHeaders.next ; block != &freeStaticHeaders ; block = block->next ) {
		numFreeStaticHeaders++;
	}

	int	numFreeDynamicHeaders = 0;
	for ( block = freeDynamicHeaders.next ; block != &freeDynamicHeaders ; block = block->next ) {
		numFreeDynamicHeaders++;
	}

	common->Printf( "%i megs working set\n", r_vertexBufferMegs.GetInteger() );
	common->Printf( "%i dynamic temp buffers of %ik\n", NUM_VERTEX_FRAMES, frameBytes / 1024 );
	common->Printf( "%5i active static headers\n", numActive );
	common->Printf( "%5i free static headers\n", numFreeStaticHeaders );
	common->Printf( "%5i free dynamic headers\n", numFreeDynamicHeaders );
}

