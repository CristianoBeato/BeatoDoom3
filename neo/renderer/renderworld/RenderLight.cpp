/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
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
#include "RenderLight.h"

//======================================================================

idRenderLightLocal::idRenderLightLocal() 
{
	memset( &parms, 0, sizeof( parms ) );
	memset( modelMatrix, 0, sizeof( modelMatrix ) );
	memset( shadowFrustums, 0, sizeof( shadowFrustums ) );
	memset( lightProject, 0, sizeof( lightProject ) );
	memset( frustum, 0, sizeof( frustum ) );
	memset( frustumWindings, 0, sizeof( frustumWindings ) );

	lightHasMoved			= false;
	world					= nullptr;
	index					= 0;
	areaNum					= 0;
	lastModifiedFrameNum	= 0;
	archived				= false;
	lightShader				= nullptr;
	falloffImage			= nullptr;
	globalLightOrigin		= vec3_zero;
	frustumTris				= nullptr;
	numShadowFrustums		= 0;
	viewCount				= 0;
	viewLight				= nullptr;
	references				= nullptr;
	foggedPortals			= nullptr;
	firstInteraction		= nullptr;
	lastInteraction			= nullptr;
}

void idRenderLightLocal::FreeRenderLight( void ) 
{
}

void idRenderLightLocal::UpdateRenderLight( const renderLight_t *re, bool forceUpdate )
{
}

void idRenderLightLocal::GetRenderLight( renderLight_t *re ) 
{
}

void idRenderLightLocal::ForceUpdate( void ) 
{
}

int idRenderLightLocal::GetIndex( void ) const
{
	return index;
}
