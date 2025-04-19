
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
#include "Backend_common.h"

void crBackend::CreatePipelines( void )
{
#if CR_USE_VULKAN


#endif
#if CR_USE_OPENGL
crAutoPointer<crGLPipeline> defaultPipeline = crAutoPointer<crGLPipeline>();
crAutoPointer<crGLPipeline> depthBufferPipeline = crAutoPointer<crGLPipeline>();
crAutoPointer<crGLPipeline> interectionPipeline = crAutoPointer<crGLPipeline>();
crAutoPointer<crGLPipeline> environmentPipeline = crAutoPointer<crGLPipeline>();
#endif

    // PIPE_DEFAULT
    defaultPipeline.New();
    m_pipelines[PIPE_DEFAULT] = defaultPipeline.DynamicCast<crPipeline>();
    // Color mask 1, 1, 1, 1
    
    // PIPE_DEPTH_BUFFER
    depthBufferPipeline.New();
    m_pipelines[PIPE_DEPTH_BUFFER] = depthBufferPipeline.DynamicCast<crPipeline>();
    // GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO | GLS_DEPTHFUNC_LESS

    // glEnable( GL_POLYGON_OFFSET_FILL );
    // glPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );

    // decal surfaces may enable polygon offset
	// glPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() );
	// GL_State( GLS_DEPTHFUNC_LESS );


    // Enable stencil test if we are going to be using it for shadows.
	// If we didn't do this, it would be legal behavior to get z fighting
	// from the ambient pass and the light passes.
	glEnable( GL_STENCIL_TEST );
	glStencilFunc( GL_ALWAYS, 1, 255 );


    // PIPE_INTERACTION
    interectionPipeline.New();
    m_pipelines[PIPE_INTERACTION] = interectionPipeline.DynamicCast<crPipeline>();
    // blend src GLS_SRCBLEND_ONE
    // blend dst GLS_DSTBLEND_ONE
    // dept mask GLS_DEPTHMASK
    // dept funtion GLS_DEPTHFUNC_LESS
    // stencil sunc( GL_ALWAYS, 128, 255 );
    
    // PIPE_ENVIRONMENT
    interectionPipeline.New();
    m_pipelines[PIPE_INTERACTION] = interectionPipeline.DynamicCast<crPipeline>();

    // PIPE_BUMPY_ENVIRONMENT
    
    // PIPE_STENCIL_SHADOW
    
    // PIPE_TEST
    
    // PIPE_AMBIENT
    
    // PIPE_GLASSWARP

    // PIPE_2D
    // GLS_DEPTHFUNC_ALWAYS
    // GLS_SRCBLEND_SRC_ALPHA
    // GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA
    // cull CT_TWO_SIDED
    // disable GL_DEPTH_TEST
    // disable GL_STENCIL_TEST

}

void crBackend::DestroyPipelines( void )
{
}