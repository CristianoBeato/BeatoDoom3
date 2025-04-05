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

#include "renderer/renderer_common.h"
#include "RenderEntity.h"

idRenderEntityLocal::idRenderEntityLocal( void ) 
{
	memset( &parms, 0, sizeof( parms ) );
	memset( modelMatrix, 0, sizeof( modelMatrix ) );

	world					= nullptr;
	index					= 0;
	lastModifiedFrameNum	= 0;
	archived				= false;
	dynamicModel			= nullptr;
	dynamicModelFrameCount	= 0;
	cachedDynamicModel		= nullptr;
	referenceBounds			= bounds_zero;
	viewCount				= 0;
	viewEntity				= nullptr;
	visibleCount			= 0;
	decals					= nullptr;
	overlay					= nullptr;
	entityRefs				= nullptr;
	firstInteraction		= nullptr;
	lastInteraction			= nullptr;
	needsPortalSky			= false;
}

void idRenderEntityLocal::FreeRenderEntity( void ) 
{
}

void idRenderEntityLocal::UpdateRenderEntity( const renderEntity_t *re, bool forceUpdate ) 
{
}

void idRenderEntityLocal::GetRenderEntity( renderEntity_t *re ) 
{
}

void idRenderEntityLocal::ForceUpdate( void ) 
{
}

int idRenderEntityLocal::GetIndex( void )  const
{
	return index;
}

void idRenderEntityLocal::ProjectOverlay( const idPlane localTextureAxis[2], const idMaterial *material ) 
{
}

void idRenderEntityLocal::RemoveDecals( void ) 
{
}
