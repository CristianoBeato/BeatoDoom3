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

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/

/*
==================
RB_ARB2_DrawInteraction
==================
*/
void crBackend::DrawInteraction( const drawInteraction_t *din ) 
{
	// load all the program parameters

	// vertex
	m_uniforms->SetUniform( din->localViewOrigin.ToFloatPtr(),  VERTEX_UNIFORM_LOCATION_LOCAL_VIEW_ORIGIN ); // PP_VIEW_ORIGIN
	// m_uniforms->SetUniform(); // PP_BUMP_MATRIX_S
	// m_uniforms->SetUniform(); // PP_BUMP_MATRIX_T
	// m_uniforms->SetUniform(); // PP_DIFFUSE_MATRIX_S
	// m_uniforms->SetUniform(); // PP_DIFFUSE_MATRIX_T
	// m_uniforms->SetUniform(); // PP_SPECULAR_MATRIX_S
	// m_uniforms->SetUniform(); // PP_SPECULAR_MATRIX_T
	// todo : set the textures buffers
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_T, din->bumpMatrix[1].ToFloatPtr() );
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	// glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_T, din->specularMatrix[1].ToFloatPtr() );


	// light uniform 
	m_uniforms->SetUniform( din->localLightOrigin.ToFloatPtr(), LIGHT_UNIFORM_LOCATION_LOCAL_ORIGIN ); // PP_LIGHT_ORIGIN
	m_uniforms->SetUniform( din->lightProjection[0].ToFloatPtr(), LIGHT_UNIFORM_LOCATION_PROJECTION_S ); // PP_LIGHT_PROJECT_S
	m_uniforms->SetUniform( din->lightProjection[1].ToFloatPtr(), LIGHT_UNIFORM_LOCATION_PROJECTION_T ); // PP_LIGHT_PROJECT_T
	m_uniforms->SetUniform( din->lightProjection[2].ToFloatPtr(), LIGHT_UNIFORM_LOCATION_PROJECTION_Q ); // PP_LIGHT_PROJECT_Q
	m_uniforms->SetUniform( din->lightProjection[3].ToFloatPtr(), LIGHT_UNIFORM_LOCATION_FALLOFF ); // PP_LIGHT_FALLOFF_S
	
	
	// testing fragment based normal mapping
	//if ( r_testARBProgram.GetBool() ) 
	//{
	//	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 2, din->localLightOrigin.ToFloatPtr() );
	//	glProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->localViewOrigin.ToFloatPtr() );
	//}

	static const float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const float one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const float negOne[4] = { -1.0f, -1.0f, -1.0f, -1.0f };

	switch ( din->vertexColor ) 
	{
	case SVC_IGNORE:
	{
		m_uniforms->SetUniform( zero, VERTEX_UNIFORM_LOCATION_COLOR_MODULATE );
		m_uniforms->SetUniform( one, VERTEX_UNIFORM_LOCATION_COLOR_ADD );	
	} break;
	case SVC_MODULATE:
	{
		m_uniforms->SetUniform( one, VERTEX_UNIFORM_LOCATION_COLOR_MODULATE );
		m_uniforms->SetUniform( zero, VERTEX_UNIFORM_LOCATION_COLOR_ADD );	
	} break;
	case SVC_INVERSE_MODULATE:
	{
		m_uniforms->SetUniform( negOne, VERTEX_UNIFORM_LOCATION_COLOR_MODULATE );
		m_uniforms->SetUniform( one, VERTEX_UNIFORM_LOCATION_COLOR_ADD );	
	} break;
	}

	// set the constant colors
	m_uniforms->SetUniform( din->diffuseColor.ToFloatPtr(), FRAGMENT_UNIFORM_LOCATION_DIFUSE_COLOR );
	m_uniforms->SetUniform( din->specularColor.ToFloatPtr(), FRAGMENT_UNIFORM_LOCATION_SPECULAR_COLOR );	

	// set the textures

	// texture 1 will be the per-surface bump map
	SelectTexture( 1 );
	din->bumpImage->Bind();

	// texture 2 will be the light falloff texture
	SelectTexture( 2 );
	din->lightFalloffImage->Bind();

	// texture 3 will be the light projection texture
	SelectTexture( 3 );
	din->lightImage->Bind();

	// texture 4 is the per-surface diffuse map
	SelectTexture( 4 );
	din->diffuseImage->Bind();

	// texture 5 is the per-surface specular map
	SelectTexture( 5 );
	din->specularImage->Bind();

	// draw it
	DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_ARB2_CreateDrawInteractions

=============
*/
void crBackend::CreateDrawInteractions( const drawSurf_t *surf ) 
{
	if ( !surf )
		return;

	// texture 0 is the normalization cube map for the vector towards the light
	SelectTexture( 0 );
	if ( viewLight->lightShader->IsAmbientLight() ) 
		globalImages->ambientNormalMap->Bind();
	else 
		globalImages->normalCubeMapImage->Bind();

	// texture 6 is the specular lookup table
	SelectTexture( 6 );
	if ( r_testARBProgram.GetBool() ) 
		globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
	else 
		globalImages->specularTableImage->Bind();

	for ( ; surf ; surf=surf->nextOnLight ) 
	{
		// perform setup here that will not change over multiple interaction passes

		// this may cause DrawInteraction to be exacuted multiple
		// times with different colors and images if the surface or light have multiple layers
		CreateSingleDrawInteractions( surf, std::bind( &crBackend::DrawInteraction, this, std::placeholders::_1 ) );	
	}


	// disable features
	SelectTexture( 6 );
	globalImages->BindNull();

	SelectTexture( 5 );
	globalImages->BindNull();

	SelectTexture( 4 );
	globalImages->BindNull();

	SelectTexture( 3 );
	globalImages->BindNull();

	SelectTexture( 2 );
	globalImages->BindNull();

	SelectTexture( 1 );
	globalImages->BindNull();

	SelectTexture( 0 );
}


/*
==================
RB_ARB2_DrawInteractions
==================
*/
void crBackend::DrawInteractions( void ) 
{
	crAutoPointer<viewLight_t> 	vLight = nullptr;
	const idMaterial*			lightShader = nullptr;

	SelectTexture( 0 );
	
	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = viewDef->viewLights ; vLight ; vLight = vLight->next ) 
	{
		this->viewLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) 
			continue;
		
		if ( vLight->lightShader->IsBlendLight() ) 
			continue;

		if ( !vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions ) 
			continue;

		lightShader = vLight->lightShader;

// TODO: remove stencil shadow 		
//		// clear the stencil buffer if needed
//		if ( vLight->globalShadows || vLight->localShadows ) 
//		{
//			currentScissor = vLight->scissorRect;
//			if ( r_useScissor.GetBool() ) 
//			{
//				Scissor( 
//					viewDef->viewport.x1 + currentScissor.x1, 
//					viewDef->viewport.y1 + currentScissor.y1,
//					currentScissor.x2 + 1 - currentScissor.x1,
//					currentScissor.y2 + 1 - currentScissor.y1 );
//			}
//		} 
//		else 
//		{
//			// no shadows, so no need to read or write the stencil buffer
//			// we might in theory want to use GL_ALWAYS instead of disabling
//			// completely, to satisfy the invarience rules
//			glStencilFunc( GL_ALWAYS, 128, 255 );
//		}

//		if ( r_useShadowVertexProgram.GetBool() ) 
//		{
//			glEnable( GL_VERTEX_PROGRAM_ARB );
//			glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_STENCIL_SHADOW );
//			RB_StencilShadowPass( vLight->globalShadows );
//			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );
//			glEnable( GL_VERTEX_PROGRAM_ARB );
//			glBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_STENCIL_SHADOW );
//			RB_StencilShadowPass( vLight->localShadows );
//			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
//			glDisable( GL_VERTEX_PROGRAM_ARB );	// if there weren't any globalInteractions, it would have stayed on
//		} 
//		else 
//		{
//			RB_StencilShadowPass( vLight->globalShadows );
//			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );
//			RB_StencilShadowPass( vLight->localShadows );
//			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
//		}

	
		// translucent surfaces never get stencil shadowed
		if ( r_skipTranslucent.GetBool() )
			continue;

		m_currentPipeline[PIPE_INTERACTION];

		CreateDrawInteractions( vLight->translucentInteractions );
	}

	SelectTexture( 0 );
}
