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

static const uint32_t CACHE_TAG_FREE	= 1 << 1;
static const uint32_t CACHE_TAG_TEMP	= 1 << 4;
static const uint32_t CACHE_TAG_USED	= 1 << 2;
static const uint32_t CACHE_TAG_FIXED	= 1 << 3;
static const uint32_t CACHE_TAG_VERTEX	= 1 << 5;
static const uint32_t CACHE_TAG_ELEMENT	= 1 << 6;

typedef struct vertCache_s
{
	uint32_t 	flags = 0;
	uint32_t	frame = 0;
	size_t		size = 0;
	uintptr_t	offset = 0;

protected:
	friend class crCache;
	vertCache_s* next = nullptr;
	vertCache_s* prev = nullptr;
} vertCache_t;

class crCache
{
public:
	crCache( const size_t size, bool elements );
	~crCache( void );

	vertCache_t*	Alloc( const size_t size );
	void			Free( vertCache_t *block );
	void			Upload( const vertCache_t* cache, const void* data );
	void			Touch( vertCache_t *block );
	void			Purge( void );
	
	const size_t	Free( void ) const { return m_allocedSize - m_memUsed.GetValue(); }

private:
	size_t						m_allocedSize;		// cache total size 
	idSysInterlockedInteger		m_memUsed;			// pointer offset 
	idSysInterlockedInteger		m_allocations;		// allocation call counter 
	vertCache_t*				m_freeHeadersList;	// free element list
	vertCache_t*				m_usedHeadersList;	// used element list
	crAutoPointer<crBuffer>		m_buffer;			// buffer handler 

	void						AllocHeaders( void );
};

class idVertexCache 
{
public:
	void			Init( void );
	void			Shutdown( void );

// BEATO Begin:
	// allocate memory in the static element buffer, and copies it over.
	// Alloc does NOT do a touch, which allows purging of things
	// created at level load time even if a frame hasn't passed yet.
	// These allocations can be purged, which will zero the pointer.
	vertCache_t*	AllocElement( const size_t size, const void* data, const bool temp = false );

	// allocate memory in the static element buffer, and copies it over.
	// Alloc does NOT do a touch, which allows purging of things
	// created at level load time even if a frame hasn't passed yet.
	// These allocations can be purged, which will zero the pointer.
	vertCache_t*	AllocVertex( const size_t size, const void* data, const bool temp = false );
// BEATO End

	// called when vertex programs are enabled or disabled, because
	// the cached data is no longer valid
	void			PurgeAll( void );

	// This will be a real pointer with virtual memory,
	// but it will be an int offset cast to a pointer of ARB_vertex_buffer_object
	uintptr_t		Position( vertCache_t *buffer );

	// notes that a buffer is used this frame, so it can't be purged
	// out from under the GPU
	void			Touch( vertCache_t *buffer );

	// this block won't have to zero a buffer pointer when it is purged,
	// but it must still wait for the frames to pass, in case the GPU
	// is still referencing it
	void			Free( vertCache_t *buffer );	

	// updates the counter for determining which temp space to use
	// and which blocks can be purged
	// Also prints debugging info when enabled
	void			EndFrame( void );

	// listVertexCache calls this
	void			List( void );

private:
	void			InitMemoryBlocks( int size );
	void			ActuallyFree( vertCache_t *block );

	static idCVar	r_showVertexCache;
	static idCVar	r_vertexBufferMegs;

	bool			allocatingTempBuffer;	// force GL_STREAM_DRAW_ARB
	bool			tempOverflow;			// had to alloc a temp in static memory

	int				staticCountTotal;
	int				staticAllocTotal;		// for end of frame purging
	int				currentFrame;			// for purgable block tracking
	int				listNum;				// currentFrame % NUM_VERTEX_FRAMES, determines which tempBuffers to use

											// staticHeaders.next is most recently used
	int				frameBytes;				// for each of NUM_VERTEX_FRAMES frames

	size_t			staticAllocThisFrame;	// debug counter
	uint32_t		dynamicCountThisFrame;

// BEATO Begin:
	uintptr_t				m_dynamicElementsOffset;
	uintptr_t				m_dynaimcVertexOffset;
	crAutoPointer<crCache>	m_staticElement;		// static buffer
	crAutoPointer<crCache>	m_staticVertex;			// static buffer
	crAutoPointer<crBuffer>	m_dynamicElementBuffer;	//
	crAutoPointer<crBuffer>	m_dyanmicVertexBuffer;
// BEATO End
};

extern	idVertexCache	vertexCache;
