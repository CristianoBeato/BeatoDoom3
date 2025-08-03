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

// vertex cache calls should only be made by the front end
#ifndef __VERTEX_CACHE_H__
#define __VERTEX_CACHE_H__

#include <atomic>

// BEATO Begin:
class crCache
{
public:
	crCache( void );
	~crCache( void );

	ID_INLINE void			Upload( const void* in_data, const size_t in_size );	
	ID_INLINE uintptr_t		Offset( void ) const { return m_offset; }
	ID_INLINE size_t		Size( void ) const { return m_size; }
	ID_INLINE void*			Ptr( void ) const { return m_data; }

private:
	size_t		m_size;
	uintptr_t	m_offset;
	void*		m_data;
	
protected:
	crCache*  	m_next;
    crCache*  	m_prev;

private:
	crCache( const crCache &ref  ) = delete;
	crCache operator = ( const crCache $ref ) = delete; 
};

void crCache::Upload( const void* in_data, const size_t in_size )
{
	std::memcpy( m_data, in_data, in_size < m_size ? in_size : m_size );
}

class crBuffer
{
public:
	crBuffer( void );
	~crBuffer( void );

	/// @brief Create buffer
	/// @param in_size the current buffer size 
	void    		Create( const size_t in_size );

    /// @brief Recreate the buffer whit a new size, and copy the content
    /// @param in_size 
    void    		Resize( const size_t in_size );

    /// @brief release the buffer 
    /// @param  
    void    		Destroy( void );
	crCache*   		Alloc( const size_t in_bytes );
    void            Free( crCache* in_block );
    const size_t	GetSize( void ) const { return m_size; }
    const size_t	GetFree( void ) const { return m_free; }
    const size_t	GetUsed( void ) const { return m_size - m_free; }

private:
	uint32_t				m_frame;	//
	size_t					m_size;		// full buffer size
	size_t					m_free; 	// availabe buffer size
	std::atomic<uintptr_t>	m_offset;	// current free offset location
	void*           		m_map;      // buffer map pointer
	crCache*   				m_freeList; // free data blocks 
    crCache*   				m_usedList; // used data blocks
};
// BEATO End

const int NUM_VERTEX_FRAMES = 2;

typedef enum bufferCacheType_t
{
	CACHE_TYPE_INDEX_STATIC = 0,
	CACHE_TYPE_INDEX_DYNAMIC,
	CACHE_TYPE_VERTEX_STATIC,
	CACHE_TYPE_VERTEX_DYNAMIC
} bufferCacheType_t;

typedef enum 
{
	TAG_FREE,
	TAG_USED,
	TAG_FIXED,		// for the temp buffers
	TAG_TEMP		// in frame temp area, not static area
} vertBlockTag_t;

class idVertexCache 
{
public:
	void			Init( void );
	void			Shutdown(void);

	// just for gfxinfo printing
	bool			IsFast( void );

	// called when vertex programs are enabled or disabled, because
	// the cached data is no longer valid
	void			PurgeAll( void );

	// Tries to allocate space for the given data in fast vertex
	// memory, and copies it over.
	// Alloc does NOT do a touch, which allows purging of things
	// created at level load time even if a frame hasn't passed yet.
	// These allocations can be purged, which will zero the pointer.
	crCache*		Alloc( const size_t in_bytes, const bufferCacheType_t in_type );

	// this block won't have to zero a buffer pointer when it is purged,
	// but it must still wait for the frames to pass, in case the GPU
	// is still referencing it
	void			Free( crCache* in_cache );	

	// notes that a buffer is used this frame, so it can't be purged
	// out from under the GPU
	void			Touch( crCache* in_cache );

	// updates the counter for determining which temp space to use
	// and which blocks can be purged
	// Also prints debugging info when enabled
	void			EndFrame( void );

	// listVertexCache calls this
	void			List( void );

private:
	bool			allocatingTempBuffer;	// force GL_STREAM_DRAW_ARB
	bool			tempOverflow;			// had to alloc a temp in static memory
	uint32_t		staticCountTotal;
	uint32_t		staticAllocTotal;		// for end of frame purging
	uint32_t		staticAllocThisFrame;	// debug counter
	uint32_t		staticCountThisFrame;
	uint32_t		dynamicAllocThisFrame;
	uint32_t		dynamicCountThisFrame;
	uint32_t		currentFrame;			// for purgable block tracking
	uint32_t		listNum;				// currentFrame % NUM_VERTEX_FRAMES, determines which tempBuffers to use											// staticHeaders.next is most recently used
	int				frameBytes;				// for each of NUM_VERTEX_FRAMES frames
	static idCVar	r_showVertexCache;
	static idCVar	r_vertexBufferMegs;

	void			InitMemoryBlocks( int size );
	void			ActuallyFree( crCache *block );

};

// todo: move to renderer (tr.) class
extern	idVertexCache	vertexCache;

#endif //__VERTEX_CACHE_H__