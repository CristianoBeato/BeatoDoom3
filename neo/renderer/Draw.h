/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

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

#ifndef __DRAW_H__
#define __DRAW_H__

typedef enum 
{
	RC_NOP,
	RC_DRAW_VIEW,
	RC_SET_BUFFER,
	RC_COPY_RENDER,
	RC_SWAP_BUFFERS		// can't just assume swap at end of list because
						// of forced list submission before syncs
} renderCommand_t;

typedef struct 
{
	renderCommand_t		commandId, *next;
} emptyCommand_t;

typedef struct 
{
	renderCommand_t		commandId, *next;
	GLenum	buffer;
	int		frameCount;
} setBufferCommand_t;

typedef struct 
{
	renderCommand_t				commandId, *next;
	crAutoPointer<viewDef_t> 	viewDef;
} drawSurfsCommand_t;

typedef struct 
{
	renderCommand_t		commandId, *next;
	int		x, y, imageWidth, imageHeight;
	idImage	*image;
	int		cubeFace;					// when copying to a cubeMap
} copyRenderCommand_t;


// a request for frame memory will never fail
// (until malloc fails), but it may force the
// allocation of a new memory block that will
// be discontinuous with the existing memory
typedef struct frameMemoryBlock_s 
{
	struct frameMemoryBlock_s *next;
	int		size;
	int		used;
	int		poop;			// so that base is 16 byte aligned
	byte	base[4];	// dynamically allocated as [size]
} frameMemoryBlock_t;

// all of the information needed by the back end must be
// contained in a frameData_t.  This entire structure is
// duplicated so the front and back end can run in parallel
// on an SMP machine (OBSOLETE: this capability has been removed)
typedef struct 
{
	size_t				memoryHighwater;	// max used on any frame

    // one or more blocks of memory for all frame
	// temporary allocations
	frameMemoryBlock_t* memory;

	// alloc will point somewhere into the memory chain
	frameMemoryBlock_t* alloc;

	srfTriangles_t *	firstDeferredFreeTriSurf;
	srfTriangles_t *	lastDeferredFreeTriSurf;


	// the currently building command list 
	// commands can be inserted at the front if needed, as for required
	// dynamically generated textures
	emptyCommand_t	*cmdHead, *cmdTail;		// may be of other command type based on commandId
} frameData_t;

class crDraw
{    
public:
    crDraw( void );
    ~crDraw( void );

    void    			InitFrameData( void ); 
    void    			ShutdownFrameData( void ); 
    int     			CountFrameData( void );
    void    			ToggleSmpFrame( void );
    void*   			FrameAlloc( size_t bytes );
    void*   			ClearedFrameAlloc( size_t bytes );
    void    			FrameFree( void *data );
    void*   			StaticAlloc( size_t bytes );		// just malloc with error checking
    void*   			ClearedStaticAlloc( size_t bytes );	// with memset
    void    			StaticFree( void *data );    
    void    			ClearCommandChain( void );
    void    			AddDrawViewCmd( crAutoPointer<viewDef_t> parms );
    void*   			GetCommandBuffer( size_t bytes );
    void    			IssueRenderCommands( void );

    ID_INLINE size_t  	GetMemoryHighwater( void ) const { return frameData ? frameData->memoryHighwater : 0; }
	ID_INLINE intptr_t	GetStaticAllocCount( void ) const { return staticAllocCount; }
	ID_INLINE void		SetStaticAllocCountZero( void );

private:
	intptr_t		staticAllocCount;	// running total of bytes allocated
    frameData_t*	frameData;
};

#endif //!__DRAW_H__