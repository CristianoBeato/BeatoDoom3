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

#include "precompiled.h"
#pragma hdrstop

#include "renderer_common.h"
#include "Draw.h"

static const size_t	MEMORY_BLOCK_SIZE = 0x100000;

crDrawFrameData::crDrawFrameData( void ) : 
	m_frame( 0 ),
	staticAllocCount( 0 ),
	memoryHighwater( 0 ),
	frameData( nullptr )
{
}

crDrawFrameData::~crDrawFrameData( void )
{
}

/*
=================
R_ViewStatistics
=================
*/
static void R_ViewStatistics( viewDefptr_t parms ) 
{
	// report statistics about this view
	if ( !r_showSurfaces.GetBool() ) 
		return;
	
	common->Printf( "view:%p surfs:%i\n", parms, parms->numDrawSurfs );
}

/*
=====================
crDrawFrameData::InitFrameData
=====================
*/
void crDrawFrameData::InitFrameData( void ) 
{
	size_t size = 0;

	ShutdownFrameData();
	size = MEMORY_BLOCK_SIZE;
	
	for ( uint32_t i = 0; i < SMP_FRAMES; i++)
	{
		frameData_t*		frame = nullptr;
		frameMemoryBlock_t*	block = nullptr;

		frame = &smpFrameData[i];

		block = static_cast<frameMemoryBlock_t*>( Mem_Alloc( size + sizeof( *block ) ) );
		if ( !block ) 
			common->FatalError( "R_InitFrameData: Mem_Alloc() failed" );

		block->size = size;
		block->used = 0;
		block->next = nullptr;
		frame->memory = block;
	}
	
	memoryHighwater = 0;

	ToggleSmpFrame();
}

/*
=====================
crDrawFrameData::ShutdownFrameData
=====================
*/
void crDrawFrameData::ShutdownFrameData( void ) 
{
	frameData_t* frame = nullptr;
	frameMemoryBlock_t* block = nullptr;

	// free any current data
	frame = frameData;
	if ( !frame )
		return;

	tr.frontend->FreeDeferredTriSurfs( frame );

	frameMemoryBlock_t *nextBlock;
	for ( block = frame->memory ; block ; block = nextBlock ) 
	{
		nextBlock = block->next;
		Mem_Free( block );
	}
	Mem_Free( frame );
	frameData = nullptr;
}

void crDrawFrameData::ToggleSmpFrame(void)
{
	// clear frame-temporary data
	frameData_t*		frame = nullptr; 
	frameMemoryBlock_t*	block = nullptr;

	// update the highwater mark
	CountFrameData();

// BEATO BEgin:
	m_frame = ( m_frame + 1 ) % SMP_FRAMES;

	// update the frame data 
	frameData = &smpFrameData[m_frame];
// BEATO End

	frame = frameData;

	// reset the memory allocation to the first block
	frame->alloc = frame->memory;

	// clear all the blocks
	for ( block = frame->memory ; block ; block = block->next ) 
    {
		block->used = 0;
	}
}

/*
================
crDrawFrameData::CountFrameData
================
*/
int crDrawFrameData::CountFrameData( void ) 
{
	int count = 0;
	frameData_t*		frame = nullptr;
	frameMemoryBlock_t*	block = nullptr;

	frame = frameData;
	for ( block = frame->memory ; block ; block=block->next ) 
	{
		count += block->used;
		if ( block == frame->alloc ) 
			break;
	}

	// note if this is a new highwater mark
	if ( count > memoryHighwater ) 
		memoryHighwater = count;

	return count;
}

/*
=================
crDrawFrameData::StaticAlloc
=================
*/
void* crDrawFrameData::StaticAlloc( size_t bytes ) 
{
	void	*buf;

	tr.frontend->GetPerformanceCounters().c_alloc++;

	staticAllocCount += bytes;

    buf = Mem_Alloc( bytes );

	// don't exit on failure on zero length allocations since the old code didn't
	if ( !buf && ( bytes != 0 ) ) 
		common->FatalError( "tr.frameData->StaticAlloc failed on %i bytes", bytes );

	return buf;
}

/*
=================
crDrawFrameData::ClearedStaticAlloc
=================
*/
void* crDrawFrameData::ClearedStaticAlloc( size_t bytes ) 
{
	void	*buf;

	buf = StaticAlloc( bytes );
	SIMDProcessor->Memset( buf, 0, bytes );
	return buf;
}

/*
=================
tr.frameData->StaticFree
=================
*/
void crDrawFrameData::StaticFree( void *data ) 
{
	tr.frontend->GetPerformanceCounters().c_free++;
    Mem_Free( data );
}

/*
================
crDrawFrameData::FrameAlloc

This data will be automatically freed when the
current frame's back end completes.

This should only be called by the front end.  The
back end shouldn't need to allocate memory.

If we passed smpFrame in, the back end could
alloc memory, because it will always be a
different frameData than the front end is using.

All temporary data, like dynamic tesselations
and local spaces are allocated here.

The memory will not move, but it may not be
contiguous with previous allocations even
from this frame.

The memory is NOT zero filled.
Should part of this be inlined in a macro?
================
*/
void* crDrawFrameData::FrameAlloc( size_t bytes ) 
{
	frameData_t*		frame = nullptr;
	frameMemoryBlock_t*	block = nullptr;
	void*				buf = nullptr;
    
	bytes = (bytes+16)&~15;
	
	// see if it can be satisfied in the current block
	frame = frameData;
	block = frame->alloc;

	if ( block->size - block->used >= bytes ) 
    {
		buf = block->base + block->used;
		block->used += bytes;
		return buf;
	}

	// advance to the next memory block if available
	block = block->next;

	// create a new block if we are at the end of
	// the chain
	if ( !block ) 
    {
		int		size;

		size = MEMORY_BLOCK_SIZE;
		block = (frameMemoryBlock_t *)Mem_Alloc( size + sizeof( *block ) );
		if ( !block ) 
			common->FatalError( "R_FrameAlloc: Mem_Alloc() failed" );
		
		block->size = size;
		block->used = 0;
		block->next = nullptr;
		frame->alloc->next = block;
	}

	// we could fix this if we needed to...
	if ( bytes > block->size )
    {
		common->FatalError( "R_FrameAlloc of %i exceeded MEMORY_BLOCK_SIZE",
			bytes );
	}

	frame->alloc = block;

	block->used = bytes;

	return block->base;
}

/*
==================
R_ClearedFrameAlloc
==================
*/
void *crDrawFrameData::ClearedFrameAlloc( size_t bytes ) 
{
	void	*r;

	r = FrameAlloc( bytes );
	SIMDProcessor->Memset( r, 0, bytes );
	return r;
}

/*
==================
crDrawFrameData::FrameFree

This does nothing at all, as the frame data is reused every frame
and can only be stack allocated.

The only reason for it's existance is so functions that can
use either static or frame memory can set function pointers
to both alloc and free.
==================
*/
void crDrawFrameData::FrameFree( void *data ) 
{
}

/*
==============
R_FreeStaticTriSurf

This will defer the free until the current frame has run through the back end.
==============
*/
void crDrawFrameData::FreeStaticTriSurf( srfTriangles_t *tri ) 
{
	frameData_t		*frame = nullptr;

	if ( !tri ) 
		return;

	if ( tri->nextDeferredFree ) 
	{
		common->Error( "R_FreeStaticTriSurf: freed a freed triangle" );
	}
	
	frame = frameData;

	if ( !frame ) 
	{
		// command line utility, or rendering in editor preview mode ( force )
		tr.frontend->ReallyFreeStaticTriSurf( tri );
	} 
	else 
	{
#ifdef ID_DEBUG_MEMORY
		R_CheckStaticTriSurfMemory( tri );
#endif
		tri->nextDeferredFree = nullptr;
		if ( frame->lastDeferredFreeTriSurf )
			frame->lastDeferredFreeTriSurf->nextDeferredFree = tri;
		else
			frame->firstDeferredFreeTriSurf = tri;
		
		frame->lastDeferredFreeTriSurf = tri;
	}
}

void *crRenderAllocator::Allocate(const size_t size)
{
    return tr.frameData->StaticAlloc( size );
}

void *crRenderAllocator::Reallocate( void *ptr, const size_t size )
{
	void* newPtr = Allocate( size );
	std::memcpy( newPtr, ptr, sizeof(ptr) );
	Deallocate( ptr ); 
    return newPtr;
}

void crRenderAllocator::Deallocate( void *ptr )
{
	tr.frameData->StaticFree( ptr );
}

/*
=============
crDrawCommandQueue::crDrawCommandQueue
=============
*/
crDrawCommandQueue::crDrawCommandQueue(void) : 
	cmdHead( nullptr ),
	cmdTail( nullptr )
{
}

/*
=============
crDrawCommandQueue::AddDrawViewCmd

This is the main 3D rendering command.  A single scene may
have multiple views if a mirror, portal, or dynamic texture is present.
=============
*/
void crDrawCommandQueue::AddDrawViewCmd( viewDefptr_t parms ) 
{
	drawSurfsCommand_t	*cmd = static_cast<drawSurfsCommand_t*>( GetCommandBuffer( sizeof( *cmd ) ) );
	cmd->commandId = RC_DRAW_VIEW;

	cmd->viewDef = parms;

	if ( parms->viewEntitys ) 
		tr.lockSurfacesCmd = *cmd; // save the command for r_lockSurfaces debugging

	tr.frontend->GetPerformanceCounters().c_numViews++;

	R_ViewStatistics( parms );
}

/*
============
crDrawCommandQueue::GetCommandBuffer

Returns memory for a command buffer (stretchPicCommand_t, 
drawSurfsCommand_t, etc) and links it to the end of the
current command chain.
============
*/
void* crDrawCommandQueue::GetCommandBuffer( size_t bytes ) 
{
	emptyCommand_t	*cmd = nullptr;

	cmd = static_cast<emptyCommand_t*>( tr.frameData->FrameAlloc( bytes ) );
	cmd->next = nullptr;
	cmdTail->next = &cmd->commandId;
	cmdTail = cmd;

	return reinterpret_cast<void*>( cmd );
}

/*
====================
crDrawCommandQueue::IssueRenderCommands

Called by R_EndFrame each frame
====================
*/
void crDrawCommandQueue::IssueRenderCommands(void)
{
	if ( cmdHead->commandId == RC_NOP && !cmdHead->next )
		return; // nothing to issue

	// r_skipBackEnd allows the entire time of the back end
	// to be removed from performance measurements, although
	// nothing will be drawn to the screen.  If the prints
	// are going to a file, or r_skipBackEnd is later disabled,
	// usefull data can be received.

	// r_skipRender is usually more usefull, because it will still
	// draw 2D graphics
	if ( !r_skipBackEnd.GetBool() )
		tr.backend->ExecuteBackEndCommands( cmdHead ); 
    
	ClearCommandChain();
}

/*
====================
crDrawCommandQueue::ToggleSmpFrame
====================
*/
void crDrawCommandQueue::ToggleSmpFrame( void ) 
{
	if ( r_lockSurfaces.GetBool() ) 
		return;
	
	tr.frameData->FreeDeferredTriSurfs();
	tr.frameData->ToggleSmpFrame();

	ClearCommandChain();
}

/*
====================
crDrawCommandQueue::ClearCommandChain

Called after every buffer submission
and by R_ToggleSmpFrame
====================
*/
void crDrawCommandQueue::ClearCommandChain( void ) 
{
	// clear the command chain
	cmdHead = cmdTail = static_cast<emptyCommand_t*>( tr.frameData->FrameAlloc( sizeof( emptyCommand_t ) ) );
	cmdHead->commandId = RC_NOP;
	cmdHead->next = nullptr;
}
