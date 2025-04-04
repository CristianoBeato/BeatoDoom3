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

#define	MEMORY_BLOCK_SIZE	0x100000

crDraw::crDraw( void ) : 
	staticAllocCount( 0 ),
	frameData( nullptr )
{
}

crDraw::~crDraw( void )
{
}

/*
=================
R_ViewStatistics
=================
*/
static void R_ViewStatistics( crAutoPointer<viewDef_t> parms ) 
{
	// report statistics about this view
	if ( !r_showSurfaces.GetBool() ) 
		return;
	
	common->Printf( "view:%p surfs:%i\n", parms, parms->numDrawSurfs );
}

/*
=====================
crDraw::InitFrameData
=====================
*/
void crDraw::InitFrameData( void ) 
{
	int size;
	frameData_t *frame;
	frameMemoryBlock_t *block;

	ShutdownFrameData();

	frameData = (frameData_t *)Mem_ClearedAlloc( sizeof( *frameData ));
	frame = frameData;
	size = MEMORY_BLOCK_SIZE;
	block = (frameMemoryBlock_t *)Mem_Alloc( size + sizeof( *block ) );
	if ( !block ) 
		common->FatalError( "R_InitFrameData: Mem_Alloc() failed" );
	

	block->size = size;
	block->used = 0;
	block->next = nullptr;
	frame->memory = block;
	frame->memoryHighwater = 0;

	ToggleSmpFrame();
}

/*
=====================
crDraw::ShutdownFrameData
=====================
*/
void crDraw::ShutdownFrameData( void ) 
{
	frameData_t *frame;
	frameMemoryBlock_t *block;

	// free any current data
	frame = frameData;
	if ( !frame )
		return;

	R_FreeDeferredTriSurfs( frame );

	frameMemoryBlock_t *nextBlock;
	for ( block = frame->memory ; block ; block = nextBlock ) {
		nextBlock = block->next;
		Mem_Free( block );
	}
	Mem_Free( frame );
	frameData = nullptr;
}

/*
================
crDraw::CountFrameData
================
*/
int crDraw::CountFrameData( void ) 
{
	frameData_t		*frame;
	frameMemoryBlock_t	*block;
	int				count;

	count = 0;
	frame = frameData;
	for ( block = frame->memory ; block ; block=block->next ) {
		count += block->used;
		if ( block == frame->alloc ) {
			break;
		}
	}

	// note if this is a new highwater mark
	if ( count > frame->memoryHighwater ) {
		frame->memoryHighwater = count;
	}

	return count;
}

/*
=================
crDraw::StaticAlloc
=================
*/
void* crDraw::StaticAlloc( size_t bytes ) 
{
	void	*buf;

	tr.pc.c_alloc++;

	staticAllocCount += bytes;

    buf = Mem_Alloc( bytes );

	// don't exit on failure on zero length allocations since the old code didn't
	if ( !buf && ( bytes != 0 ) ) 
		common->FatalError( "R_StaticAlloc failed on %i bytes", bytes );

	return buf;
}

/*
=================
crDraw::ClearedStaticAlloc
=================
*/
void* crDraw::ClearedStaticAlloc( size_t bytes ) 
{
	void	*buf;

	buf = StaticAlloc( bytes );
	SIMDProcessor->Memset( buf, 0, bytes );
	return buf;
}

/*
=================
R_StaticFree
=================
*/
void crDraw::StaticFree( void *data ) 
{
	tr.pc.c_free++;
    Mem_Free( data );
}

/*
================
crDraw::FrameAlloc

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
void* crDraw::FrameAlloc( size_t bytes ) 
{
	frameData_t		*frame;
	frameMemoryBlock_t	*block;
	void			*buf;
    
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
void *crDraw::ClearedFrameAlloc( size_t bytes ) 
{
	void	*r;

	r = FrameAlloc( bytes );
	SIMDProcessor->Memset( r, 0, bytes );
	return r;
}

/*
==================
crDraw::FrameFree

This does nothing at all, as the frame data is reused every frame
and can only be stack allocated.

The only reason for it's existance is so functions that can
use either static or frame memory can set function pointers
to both alloc and free.
==================
*/
void crDraw::FrameFree( void *data ) 
{
}

/*
====================
crDraw::ToggleSmpFrame
====================
*/
void crDraw::ToggleSmpFrame( void ) 
{
	if ( r_lockSurfaces.GetBool() ) 
		return;
	
	R_FreeDeferredTriSurfs( frameData );

	// clear frame-temporary data
	frameData_t		*frame;
	frameMemoryBlock_t	*block;

	// update the highwater mark
	CountFrameData();

	frame = frameData;

	// reset the memory allocation to the first block
	frame->alloc = frame->memory;

	// clear all the blocks
	for ( block = frame->memory ; block ; block = block->next ) 
    {
		block->used = 0;
	}

	ClearCommandChain();
}

/*
=============
crDraw::AddDrawViewCmd

This is the main 3D rendering command.  A single scene may
have multiple views if a mirror, portal, or dynamic texture is present.
=============
*/
void crDraw::AddDrawViewCmd( crAutoPointer<viewDef_t> parms ) 
{
	drawSurfsCommand_t	*cmd;

	cmd = (drawSurfsCommand_t *)GetCommandBuffer( sizeof( *cmd ) );
	cmd->commandId = RC_DRAW_VIEW;

	cmd->viewDef = parms;

	if ( parms->viewEntitys ) {
		// save the command for r_lockSurfaces debugging
		tr.lockSurfacesCmd = *cmd;
	}

	tr.pc.c_numViews++;

	R_ViewStatistics( parms );
}


/*
====================
R_IssueRenderCommands

Called by R_EndFrame each frame
====================
*/
void crDraw::IssueRenderCommands( void ) 
{
	if ( frameData->cmdHead->commandId == RC_NOP && !frameData->cmdHead->next ) {
		// nothing to issue
		return;
	}

	// r_skipBackEnd allows the entire time of the back end
	// to be removed from performance measurements, although
	// nothing will be drawn to the screen.  If the prints
	// are going to a file, or r_skipBackEnd is later disabled,
	// usefull data can be received.

	// r_skipRender is usually more usefull, because it will still
	// draw 2D graphics
	if ( !r_skipBackEnd.GetBool() ) 
    {
		RB_ExecuteBackEndCommands( frameData->cmdHead );
	}

	ClearCommandChain();
}

/*
============
crDraw::GetCommandBuffer

Returns memory for a command buffer (stretchPicCommand_t, 
drawSurfsCommand_t, etc) and links it to the end of the
current command chain.
============
*/
void* crDraw::GetCommandBuffer( size_t bytes ) 
{
	emptyCommand_t	*cmd;

	cmd = (emptyCommand_t *)FrameAlloc( bytes );
	cmd->next = nullptr;
	frameData->cmdTail->next = &cmd->commandId;
	frameData->cmdTail = cmd;

	return (void *)cmd;
}

/*
====================
crDraw::ClearCommandChain

Called after every buffer submission
and by R_ToggleSmpFrame
====================
*/
void crDraw::ClearCommandChain( void ) 
{
	// clear the command chain
	frameData->cmdHead = frameData->cmdTail = (emptyCommand_t *)FrameAlloc( sizeof( *frameData->cmdHead ) );
	frameData->cmdHead->commandId = RC_NOP;
	frameData->cmdHead->next = nullptr;
}

