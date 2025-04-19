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

  back end scene + lights rendering functions

*/


/*
=================
RB_DrawElementsImmediate

Draws with immediate mode commands, which is going to be very slow.
This should never happen if the vertex cache is operating properly.
=================
*/
void crBackend::DrawElementsImmediate( const srfTriangles_t *tri ) 
{
	// submit uniforms blocks
	m_uniforms->Submit();

	pc.c_drawElements++;
	pc.c_drawIndexes += tri->numIndexes;
	pc.c_drawVertexes += tri->numVerts;

	if ( tri->ambientSurface != nullptr  ) 
	{
		if ( tri->indexes == tri->ambientSurface->indexes ) 
			pc.c_drawRefIndexes += tri->numIndexes;
		
		if ( tri->verts == tri->ambientSurface->verts ) 
			pc.c_drawRefVertexes += tri->numVerts;
	}

	// send uniforms to buffer
	m_uniforms->Submit();

	// TODO: draw on this thread ? of flush commands 
	m_graphicQueue->SubmitDrawCommand( r_singleTriangle.GetBool() ? 3 : tri->numIndexes, vertexCache.Position( tri->indexCache ) );
	//glDrawElementsBaseVertex( GL_TRIANGLES, tri->numIndexes, GL_INDEX_TYPE, (int *)vertexCache.Position( tri->indexCache ), (GLint *)vertexCache.Position( tri->ambientCache ) );
}

/*
================
crBackend::DrawElementsWithCounters
================
*/
void crBackend::DrawElementsWithCounters( const srfTriangles_t *tri )
{
	pc.c_drawElements++;
	pc.c_drawIndexes += tri->numIndexes;
	pc.c_drawVertexes += tri->numVerts;

	if ( tri->ambientSurface != nullptr ) 
	{
		if ( tri->indexes == tri->ambientSurface->indexes )
			pc.c_drawRefIndexes += tri->numIndexes;

		if ( tri->verts == tri->ambientSurface->verts ) 
			pc.c_drawRefVertexes += tri->numVerts;
	}

	// Submit uniforms to draw
	m_uniforms->Submit();

	m_graphicQueue->SubmitDrawCommand( r_singleTriangle.GetBool() ? 3 : tri->numIndexes, vertexCache.Position( tri->indexCache ) );
	pc.c_vboIndexes += tri->numIndexes;
}

/*
================
crBackend::DrawShadowElementsWithCounters

May not use all the indexes in the surface if caps are skipped
================
*/
void crBackend::DrawShadowElementsWithCounters( const srfTriangles_t *tri, int numIndexes ) 
{
	pc.c_shadowElements++;
	pc.c_shadowIndexes += numIndexes;
	pc.c_shadowVertexes += tri->numVerts;

	// Submit uniforms 
	m_uniforms->Submit();

	assert( tri->indexCache );
	m_graphicQueue->SubmitDrawCommand( r_singleTriangle.GetBool() ? 3 : numIndexes, vertexCache.Position( tri->indexCache ) );
	pc.c_vboIndexes += numIndexes;
}

/*
===============
crBackend::RenderTriangleSurface

Sets texcoord and vertex pointers
===============
*/
void crBackend::RenderTriangleSurface( const srfTriangles_t *tri ) 
{
	if ( !tri->ambientCache )
	{
		DrawElementsImmediate( tri );
		return;
	}

	DrawElementsWithCounters( tri );
}

/*
===============
crBackend::EnterWeaponDepthHack
===============
*/
void crBackend::EnterWeaponDepthHack( void ) 
{
	glDepthRange( 0, 0.5 );

	float	matrix[16];

	memcpy( matrix, &viewDef->projectionMatrix, sizeof( matrix ) );

	matrix[14] *= 0.25;

	m_uniforms->SetUniform( matrix, VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX );
}

/*
===============
crBackend::EnterModelDepthHack
===============
*/
void crBackend::EnterModelDepthHack( float depth ) 
{
	glDepthRange( 0.0f, 1.0f );

	float	matrix[16];

	memcpy( matrix, &viewDef->projectionMatrix, sizeof( matrix ) );

	matrix[14] -= depth;

	m_uniforms->SetUniform( matrix, VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX );
}

/*
===============
crBackend::LeaveDepthHack
===============
*/
void crBackend::LeaveDepthHack( void ) 
{
	glDepthRange( 0.0f, 1.0f );
	m_uniforms->SetUniform( &viewDef->projectionMatrix, VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX );
}

/*
====================
crBackend::RenderDrawSurfListWithFunction

The triangle functions can check backEnd.currentSpace != surf->space
to see if they need to perform any new matrix setup.  The modelview
matrix will already have been loaded, and backEnd.currentSpace will
be updated after the triangle function completes.
====================
*/
void crBackend::RenderDrawSurfListWithFunction( drawSurf_t **drawSurfs, int numDrawSurfs, std::function<void( const drawSurf_t *)>triFunc_ ) 
{
	int	i = 0;
	const drawSurf_t* drawSurf = nullptr;

	currentSpace = nullptr;

	for (i = 0  ; i < numDrawSurfs ; i++ ) 
	{
		drawSurf = drawSurfs[i];

		// change the matrix if needed
		if ( drawSurf->space != currentSpace )
			m_uniforms->SetUniform( &drawSurf->space->modelViewMatrix, VERTEX_UNIFORM_LOCATION_VIEW_MATRIX ); 
		
		if ( drawSurf->space->weaponDepthHack ) 
			EnterWeaponDepthHack();
		
		if ( drawSurf->space->modelDepthHack != 0.0f )
			EnterModelDepthHack( drawSurf->space->modelDepthHack );

		// change the scissor if needed
		if ( r_useScissor.GetBool() && !currentScissor.Equals( drawSurf->scissorRect ) ) 
		{
			currentScissor = drawSurf->scissorRect;
			Scissor( viewDef->viewport.x1 + currentScissor.x1, viewDef->viewport.y1 + currentScissor.y1, currentScissor.x2 + 1 - currentScissor.x1, currentScissor.y2 + 1 - currentScissor.y1 );
		}

		// render it
		triFunc_( drawSurf );

		if ( drawSurf->space->weaponDepthHack || drawSurf->space->modelDepthHack != 0.0f )
			LeaveDepthHack();

		currentSpace = drawSurf->space;
	}
}

/*
======================
crBackend::RenderDrawSurfChainWithFunction
======================
*/
void crBackend::RenderDrawSurfChainWithFunction( const drawSurf_t *drawSurfs, void (*triFunc_)( const drawSurf_t *) ) 
{
	const drawSurf_t		*drawSurf;

	currentSpace = nullptr;

	for ( drawSurf = drawSurfs ; drawSurf ; drawSurf = drawSurf->nextOnLight ) 
	{
		// change the matrix if needed
		if ( drawSurf->space != currentSpace )
			m_uniforms->SetUniform( &drawSurf->space->modelViewMatrix, VERTEX_UNIFORM_LOCATION_VIEW_MATRIX );

		if ( drawSurf->space->weaponDepthHack ) 
			EnterWeaponDepthHack();

		if ( drawSurf->space->modelDepthHack ) 
			EnterModelDepthHack( drawSurf->space->modelDepthHack );

		// change the scissor if needed
		if ( r_useScissor.GetBool() && !currentScissor.Equals( drawSurf->scissorRect ) ) 
		{
			currentScissor = drawSurf->scissorRect;
			Scissor( viewDef->viewport.x1 + currentScissor.x1, viewDef->viewport.y1 + currentScissor.y1,currentScissor.x2 + 1 - currentScissor.x1,currentScissor.y2 + 1 - currentScissor.y1 );
		}

		// render it
		triFunc_( drawSurf );

		if ( drawSurf->space->weaponDepthHack || drawSurf->space->modelDepthHack != 0.0f ) 
			LeaveDepthHack();

		currentSpace = drawSurf->space;
	}
}

/*
======================
crBackend::GetShaderTextureMatrix
======================
*/
void RB_GetShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture, float matrix[16] ) 
{
	matrix[0] = shaderRegisters[ texture->matrix[0][0] ];
	matrix[4] = shaderRegisters[ texture->matrix[0][1] ];
	matrix[8] = 0;
	matrix[12] = shaderRegisters[ texture->matrix[0][2] ];

	// we attempt to keep scrolls from generating incredibly large texture values, but
	// center rotations and center scales can still generate offsets that need to be > 1
	if ( matrix[12] < -40 || matrix[12] > 40 ) 
		matrix[12] -= (int)matrix[12];

	matrix[1] = shaderRegisters[ texture->matrix[1][0] ];
	matrix[5] = shaderRegisters[ texture->matrix[1][1] ];
	matrix[9] = 0;
	matrix[13] = shaderRegisters[ texture->matrix[1][2] ];
	if ( matrix[13] < -40 || matrix[13] > 40 ) 
		matrix[13] -= (int)matrix[13];

	matrix[2] = 0;
	matrix[6] = 0;
	matrix[10] = 1;
	matrix[14] = 0;

	matrix[3] = 0;
	matrix[7] = 0;
	matrix[11] = 0;
	matrix[15] = 1;
}

/*
======================
crBackend::LoadShaderTextureMatrix
======================
*/
void crBackend::LoadShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture ) 
{
	float	matrix[16];

	GetShaderTextureMatrix( shaderRegisters, texture, matrix );
	
	m_uniforms->SetUniform( matrix, VERTEX_UNIFORM_LOCATION_TEXTURE_MATRIX );}

/*
======================
RB_BindVariableStageImage

Handles generating a cinematic frame if needed
======================
*/
void crBackend::BindVariableStageImage( const textureStage_t *texture, const float *shaderRegisters ) 
{
	if ( texture->cinematic ) 
	{
		cinData_t	cin;

		if ( r_skipDynamicTextures.GetBool() ) 
		{
			globalImages->defaultImage->Bind();
			return;
		}

		// offset time by shaderParm[7] (FIXME: make the time offset a parameter of the shader?)
		// We make no attempt to optimize for multiple identical cinematics being in view, or
		// for cinematics going at a lower framerate than the renderer.
		cin = texture->cinematic->ImageForTime( (int)(1000 * ( viewDef->floatTime + viewDef->renderView.shaderParms[11] ) ) );

		if ( cin.image ) 
			globalImages->cinematicImage->UploadScratch( cin.image, cin.imageWidth, cin.imageHeight );
		else 
			globalImages->blackImage->Bind();
	} 
	else 
	{
		//FIXME: see why image is invalid
		if (texture->image) 
			texture->image->Bind();
	}
}

/*
======================
RB_BindStageTexture
======================
*/
void crBackend::BindStageTexture( const float *shaderRegisters, const textureStage_t *texture, const drawSurf_t *surf ) 
{
	// image
	BindVariableStageImage( texture, shaderRegisters );

#if 0
	// texgens
	if ( texture->texgen == TG_DIFFUSE_CUBE ) 
		glTexCoordPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ((idDrawVert *)vertexCache.Position( surf->geo->ambientCache ))->normal.ToFloatPtr() );
	
	if ( texture->texgen == TG_SKYBOX_CUBE || texture->texgen == TG_WOBBLESKY_CUBE ) 
		glTexCoordPointer( 3, GL_FLOAT, 0, vertexCache.Position( surf->dynamicTexCoords ) );
	
	if ( texture->texgen == TG_REFLECT_CUBE ) 
	{
		float	mat[16];
		crTransform::TransposeGLMatrix( backEnd.viewDef->worldSpace.modelViewMatrix, mat );
		// load texture matrix in the buffer 
		m_uniforms->SetUniform( mat, VERTEX_UNIFORM_LOCATION_TEXTURE_MATRIX );
	}
#else
	// todo:
#endif

	// matrix
	if ( texture->hasMatrix ) 
		LoadShaderTextureMatrix( shaderRegisters, texture );
}

/*
======================
crFrontend::FinishStageTexture
======================
*/
void crBackend::FinishStageTexture( const textureStage_t *texture, const drawSurf_t *surf ) 
{
#if 0
	if ( texture->texgen == TG_REFLECT_CUBE ) 
	{
		glDisable( GL_TEXTURE_GEN_S );
		glDisable( GL_TEXTURE_GEN_T );
		glDisable( GL_TEXTURE_GEN_R );
		glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glDisableClientState( GL_NORMAL_ARRAY );

		glMatrixMode( GL_TEXTURE );
		glLoadIdentity();
		glMatrixMode( GL_MODELVIEW );
	}

	if ( texture->hasMatrix ) 
	{
		glMatrixMode( GL_TEXTURE );
		glLoadIdentity();
		glMatrixMode( GL_MODELVIEW );
	}
#else
	// TODO: set the texture matrix 
#endif
}



//=============================================================================================


/*
=================
crBackend::DetermineLightScale

Sets:
backEnd.lightScale
backEnd.overBright

Find out how much we are going to need to overscale the lighting, so we
can down modulate the pre-lighting passes.

We only look at light calculations, but an argument could be made that
we should also look at surface evaluations, which would let surfaces
overbright past 1.0
=================
*/
void crBackend::DetermineLightScale( void ) 
{
	int					i = 0, j = 0, numStages = 0;
	float				max;
	viewLight_t			*vLight = nullptr;
	const idMaterial	*shader = nullptr;
	const shaderStage_t	*stage = nullptr;

	// the light scale will be based on the largest color component of any surface
	// that will be drawn.
	// should we consider separating rgb scales?

	// if there are no lights, this will remain at 1.0, so GUI-only
	// rendering will not lose any bits of precision
	max = 1.0;

	for ( vLight = viewDef->viewLights ; vLight ; vLight = vLight->next ) 
	{
		// lights with no surfaces or shaderparms may still be present
		// for debug display
		if ( !vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions ) 
			continue;

		shader = vLight->lightShader;
		numStages = shader->GetNumStages();
		for ( i = 0 ; i < numStages ; i++ ) 
		{
			stage = shader->GetStage( i );
			for ( j = 0 ; j < 3 ; j++ ) 
			{
				float	v = r_lightScale.GetFloat() * vLight->shaderRegisters[ stage->color.registers[j] ];
				if ( v > max ) 
					max = v;
				
			}
		}
	}

	pc.maxLightValue = max;
	if ( max <= tr.backEndRendererMaxLight ) 
	{
		lightScale = r_lightScale.GetFloat();
		overBright = 1.0;
	} 
	else 
	{
		lightScale = r_lightScale.GetFloat() * tr.backEndRendererMaxLight / max;
		overBright = max / tr.backEndRendererMaxLight;
	}
}


/*
=================
crBackend::BeginDrawingView

Any mirrored or portaled views have already been drawn, so prepare
to actually render the visible surfaces for this view
=================
*/
void crBackend::BeginDrawingView( void ) 
{
	// set the modelview matrix for the viewer
	m_uniforms->SetUniform( &viewDef->projectionMatrix, VERTEX_UNIFORM_LOCATION_PROJECTION_MATRIX );
	
	// set the window clipping
	Viewport( tr.viewportOffset[0] + viewDef->viewport.x1, tr.viewportOffset[1] + viewDef->viewport.y1, viewDef->viewport.x2 + 1 - viewDef->viewport.x1, viewDef->viewport.y2 + 1 - viewDef->viewport.y1 );

	// the scissor may be smaller than the viewport for subviews
	Scissor( tr.viewportOffset[0] + viewDef->viewport.x1 + viewDef->scissor.x1, tr.viewportOffset[1] + viewDef->viewport.y1 + viewDef->scissor.y1, viewDef->scissor.x2 + 1 - viewDef->scissor.x1, viewDef->scissor.y2 + 1 - viewDef->scissor.y1 );
	currentScissor = viewDef->scissor;

	// ensures that depth writes are enabled for the depth clear
	// GL_State( GLS_DEFAULT );

	//// we don't have to clear the depth / stencil buffer for 2D rendering
	//if ( viewDef->viewEntitys ) 
	//{
	//	glStencilMask( 0xff );
	//	// some cards may have 7 bit stencil buffers, so don't assume this
	//	// should be 128
	//	glClearStencil( 1<<(glConfig.stencilBits-1) );
	//	glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	//	glEnable( GL_DEPTH_TEST );
	//} 
	//else 
	//{
	//	glDisable( GL_DEPTH_TEST );
	//	glDisable( GL_STENCIL_TEST );
	//}
//
	//glState.faceCulling = -1;		// force face culling to set next time
	//GL_Cull( CT_FRONT_SIDED );

}

/*
==================
R_SetDrawInteractions
==================
*/
static void R_SetDrawInteraction( const shaderStage_t *surfaceStage, const float *surfaceRegs, idImage **image, idVec4 matrix[2], float color[4] ) 
{
	*image = surfaceStage->texture.image;
	if ( surfaceStage->texture.hasMatrix ) 
	{
		matrix[0][0] = surfaceRegs[surfaceStage->texture.matrix[0][0]];
		matrix[0][1] = surfaceRegs[surfaceStage->texture.matrix[0][1]];
		matrix[0][2] = 0;
		matrix[0][3] = surfaceRegs[surfaceStage->texture.matrix[0][2]];

		matrix[1][0] = surfaceRegs[surfaceStage->texture.matrix[1][0]];
		matrix[1][1] = surfaceRegs[surfaceStage->texture.matrix[1][1]];
		matrix[1][2] = 0;
		matrix[1][3] = surfaceRegs[surfaceStage->texture.matrix[1][2]];

		// we attempt to keep scrolls from generating incredibly large texture values, but
		// center rotations and center scales can still generate offsets that need to be > 1
		if ( matrix[0][3] < -40 || matrix[0][3] > 40 ) 
			matrix[0][3] -= (int)matrix[0][3];
		
		if ( matrix[1][3] < -40 || matrix[1][3] > 40 ) 
			matrix[1][3] -= (int)matrix[1][3];
		
	} 
	else 
	{
		matrix[0][0] = 1;
		matrix[0][1] = 0;
		matrix[0][2] = 0;
		matrix[0][3] = 0;

		matrix[1][0] = 0;
		matrix[1][1] = 1;
		matrix[1][2] = 0;
		matrix[1][3] = 0;
	}

	if ( color ) 
	{
		for ( int i = 0 ; i < 4 ; i++ ) 
		{
			color[i] = surfaceRegs[surfaceStage->color.registers[i]];
			// clamp here, so card with greater range don't look different.
			// we could perform overbrighting like we do for lights, but
			// it doesn't currently look worth it.
			if ( color[i] < 0 )
				color[i] = 0;
			else if ( color[i] > 1.0 ) 
				color[i] = 1.0;
		}
	}
}

/*
=================
RB_SubmittInteraction
=================
*/
static void RB_SubmittInteraction( drawInteraction_t *din, std::function<void(const drawInteraction_t *)> DrawInteraction ) 
{
	if ( !din->bumpImage ) 
		return;

	if ( !din->diffuseImage || r_skipDiffuse.GetBool() ) 
		din->diffuseImage = globalImages->blackImage;
	
	if ( !din->specularImage || r_skipSpecular.GetBool() || din->ambientLight ) 
		din->specularImage = globalImages->blackImage;
	
	if ( !din->bumpImage || r_skipBump.GetBool() ) 
		din->bumpImage = globalImages->flatNormalMap;

	// if we wouldn't draw anything, don't call the Draw function
	if ( ( ( 
		din->diffuseColor[0] > 0 || 
		din->diffuseColor[1] > 0 || 
		din->diffuseColor[2] > 0 ) && 
		din->diffuseImage != globalImages->blackImage ) ||
	 ( ( 
		din->specularColor[0] > 0 || 
		din->specularColor[1] > 0 || 
		din->specularColor[2] > 0 ) && 
		din->specularImage != globalImages->blackImage ) ) 
	{
		DrawInteraction( din );
	}
}

/*
=============
crBackend::CreateSingleDrawInteractions

This can be used by different draw_* backends to decompose a complex light / surface
interaction into primitive interactions
=============
*/
void crBackend::CreateSingleDrawInteractions( const drawSurf_t *surf, std::function<void(const drawInteraction_t *)> DrawInteraction ) 
{
	const idMaterial*					surfaceShader = surf->material;
	const float*						surfaceRegs = surf->shaderRegisters;
	const crAutoPointer<viewLight_t>	vLight = viewLight;
	const idMaterial*					lightShader = vLight->lightShader;
	const float*						lightRegs = vLight->shaderRegisters;
	drawInteraction_t					inter;

	if ( r_skipInteractions.GetBool() || !surf->geo || !surf->geo->ambientCache )
		return;

	if ( tr.logFile ) 
		RB_LogComment( "---------- RB_CreateSingleDrawInteractions %s on %s ----------\n", lightShader->GetName(), surfaceShader->GetName() );
	

	// change the matrix and light projection vectors if needed
	if ( surf->space != currentSpace ) 
	{
		currentSpace = surf->space;
		m_uniforms->SetUniform( &surf->space->modelViewMatrix, VERTEX_UNIFORM_LOCATION_VIEW_MATRIX );
	}

	// change the scissor if needed
	if ( r_useScissor.GetBool() && !currentScissor.Equals( surf->scissorRect ) ) 
	{
		currentScissor = surf->scissorRect;
		Scissor( viewDef->viewport.x1 + currentScissor.x1, viewDef->viewport.y1 + currentScissor.y1, currentScissor.x2 + 1 - currentScissor.x1, currentScissor.y2 + 1 - currentScissor.y1 );
	}

	// hack depth range if needed
	if ( surf->space->weaponDepthHack ) 
		EnterWeaponDepthHack();

	if ( surf->space->modelDepthHack )
		EnterModelDepthHack( surf->space->modelDepthHack );

	inter.surf = surf;
	inter.lightFalloffImage = vLight->falloffImage;

	crTransform::GlobalPointToLocal( surf->space->modelMatrix, vLight->globalLightOrigin, inter.localLightOrigin.ToVec3() );
	crTransform::GlobalPointToLocal( surf->space->modelMatrix, viewDef->renderView.vieworg, inter.localViewOrigin.ToVec3() );
	inter.localLightOrigin[3] = 0;
	inter.localViewOrigin[3] = 1;
	inter.ambientLight = lightShader->IsAmbientLight();

	// the base projections may be modified by texture matrix on light stages
	idPlane lightProject[4];
	for ( int i = 0 ; i < 4 ; i++ ) 
	{
		lightProject[i] = surf->space->modelMatrix.GlobalPlaneToLocal( viewLight->lightProject[i] );
		//crTransform::GlobalPlaneToLocal( surf->space->modelMatrix, viewLight->lightProject[i], lightProject[i] );
	}

	for ( int lightStageNum = 0 ; lightStageNum < lightShader->GetNumStages() ; lightStageNum++ ) 
	{
		const shaderStage_t	*lightStage = lightShader->GetStage( lightStageNum );

		// ignore stages that fail the condition
		if ( !lightRegs[ lightStage->conditionRegister ] )
			continue;
		
		inter.lightImage = lightStage->texture.image;

		memcpy( inter.lightProjection, lightProject, sizeof( inter.lightProjection ) );
		// now multiply the texgen by the light texture matrix
		if ( lightStage->texture.hasMatrix ) 
		{
			GetShaderTextureMatrix( lightRegs, &lightStage->texture, lightTextureMatrix );
			BakeTextureMatrixIntoTexgen( reinterpret_cast<class idPlane *>(inter.lightProjection), lightTextureMatrix );
		}

		inter.bumpImage = nullptr;
		inter.specularImage = nullptr;
		inter.diffuseImage = nullptr;
		inter.diffuseColor[0] = inter.diffuseColor[1] = inter.diffuseColor[2] = inter.diffuseColor[3] = 0;
		inter.specularColor[0] = inter.specularColor[1] = inter.specularColor[2] = inter.specularColor[3] = 0;

		float lightColor[4];

		// backEnd.lightScale is calculated so that lightColor[] will never exceed
		// tr.backEndRendererMaxLight
		lightColor[0] = lightScale * lightRegs[ lightStage->color.registers[0] ];
		lightColor[1] = lightScale * lightRegs[ lightStage->color.registers[1] ];
		lightColor[2] = lightScale * lightRegs[ lightStage->color.registers[2] ];
		lightColor[3] = lightRegs[ lightStage->color.registers[3] ];

		// go through the individual stages
		for ( int surfaceStageNum = 0 ; surfaceStageNum < surfaceShader->GetNumStages() ; surfaceStageNum++ ) 
		{
			const shaderStage_t	*surfaceStage = surfaceShader->GetStage( surfaceStageNum );

			switch( surfaceStage->lighting ) 
			{
				case SL_AMBIENT: 
					break; // ignore ambient stages while drawing interactions
				case SL_BUMP: 
				{
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
						break;
				
					// draw any previous interaction
					RB_SubmittInteraction( &inter, DrawInteraction );
					inter.diffuseImage = nullptr;
					inter.specularImage = nullptr;
					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.bumpImage, inter.bumpMatrix, nullptr );
					break;
				}
				case SL_DIFFUSE: 
				{
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
						break;
					
					if ( inter.diffuseImage ) 
						RB_SubmittInteraction( &inter, DrawInteraction );
					
					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.diffuseImage, inter.diffuseMatrix, inter.diffuseColor.ToFloatPtr() );
					
					inter.diffuseColor[0] *= lightColor[0];
					inter.diffuseColor[1] *= lightColor[1];
					inter.diffuseColor[2] *= lightColor[2];
					inter.diffuseColor[3] *= lightColor[3];
					inter.vertexColor = surfaceStage->vertexColor;
					break;
				}
				case SL_SPECULAR: 
				{
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
						break;
					
					if ( inter.specularImage ) 
						RB_SubmittInteraction( &inter, DrawInteraction );
					
					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.specularImage, inter.specularMatrix, inter.specularColor.ToFloatPtr() );
					
					inter.specularColor[0] *= lightColor[0];
					inter.specularColor[1] *= lightColor[1];
					inter.specularColor[2] *= lightColor[2];
					inter.specularColor[3] *= lightColor[3];
					inter.vertexColor = surfaceStage->vertexColor;
					break;
				}
			}
		}

		// draw the final interaction
		RB_SubmittInteraction( &inter, DrawInteraction );
	}

	// unhack depth range if needed
	if ( surf->space->weaponDepthHack || surf->space->modelDepthHack != 0.0f ) 
		LeaveDepthHack();
}

/*
==================
crBackend::ShowOverdraw
==================
*/
void crBackend::ShowOverdraw( void ) 
{
	int					i = 0;
	int					numDrawSurfs = 0;
	const idMaterial *	material = nullptr;
	drawSurf_t * *		drawSurfs = nullptr;
	const drawSurf_t *	surf = nullptr;
	viewLight_t *		vLight = nullptr;

	if ( r_showOverDraw.GetInteger() == 0 ) 
		return;

	material = declManager->FindMaterial( "textures/common/overdrawtest", false );
	if ( material == nullptr ) 
		return;

	drawSurfs = viewDef->drawSurfs;
	numDrawSurfs = viewDef->numDrawSurfs;

	int interactions = 0;
	for ( vLight = viewDef->viewLights; vLight; vLight = vLight->next ) 
	{
		for ( surf = vLight->localInteractions; surf; surf = surf->nextOnLight ) 
		{
			interactions++;
		}
		
		for ( surf = vLight->globalInteractions; surf; surf = surf->nextOnLight ) 
		{
			interactions++;
		}
	}

	drawSurf_t **newDrawSurfs = (drawSurf_t **)tr.drawQueue->StaticAlloc( numDrawSurfs + interactions * sizeof( newDrawSurfs[0] ) );

	for ( i = 0; i < numDrawSurfs; i++ ) 
	{
		surf = drawSurfs[i];
		if ( surf->material ) {
			const_cast<drawSurf_t *>(surf)->material = material;
		}
		newDrawSurfs[i] = const_cast<drawSurf_t *>(surf);
	}

	for ( vLight = viewDef->viewLights; vLight; vLight = vLight->next ) 
	{
		for ( surf = vLight->localInteractions; surf; surf = surf->nextOnLight ) 
		{
			const_cast<drawSurf_t *>(surf)->material = material;
			newDrawSurfs[i++] = const_cast<drawSurf_t *>(surf);
		}
		
		for ( surf = vLight->globalInteractions; surf; surf = surf->nextOnLight ) 
		{
			const_cast<drawSurf_t *>(surf)->material = material;
			newDrawSurfs[i++] = const_cast<drawSurf_t *>(surf);
		}
		
		vLight->localInteractions = nullptr;
		vLight->globalInteractions = nullptr;
	}

	switch( r_showOverDraw.GetInteger() ) 
	{
		case 1: // geometry overdraw
			viewDef->drawSurfs = newDrawSurfs;
			viewDef->numDrawSurfs = numDrawSurfs;
			break;
		case 2: // light interaction overdraw
			viewDef->drawSurfs = &newDrawSurfs[numDrawSurfs];
			viewDef->numDrawSurfs = interactions;
			break;
		case 3: // geometry + light interaction overdraw
			viewDef->drawSurfs = newDrawSurfs;
			viewDef->numDrawSurfs += interactions;
			break;
	}
}

/*
=============
crBackend::DrawView
=============
*/
void crBackend::DrawView( const void *data ) 
{
	const drawSurfsCommand_t	*cmd = static_cast<const drawSurfsCommand_t *>( data );

	// copy current viewdef
	viewDef = cmd->viewDef;
	
	// we will need to do a new copyTexSubImage of the screen
	// when a SS_POST_PROCESS material is used
	currentRenderCopied = false;

	// if there aren't any drawsurfs, do nothing
	if ( !viewDef->numDrawSurfs ) 
		return;

	// skip render bypasses everything that has models, assuming
	// them to be 3D views, but leaves 2D rendering visible
	if ( r_skipRender.GetBool() && viewDef->viewEntitys ) 
		return;

// BEATO Begin:
#if 0
	// skip render context sets the wgl context to NULL,
	// which should factor out the API cost, under the assumption
	// that all gl calls just return if the context isn't valid
	if ( r_skipRenderContext.GetBool() && backEnd.viewDef->viewEntitys ) 
	{
		GLimp_DeactivateContext();
	}
#endif
// BEATO End

	pc.c_surfaces += viewDef->numDrawSurfs;

	ShowOverdraw();

	// render the scene, jumping to the hardware specific interaction renderers
	STD_DrawView();

	// restore the context for 2D drawing if we were stubbing it out
	if ( r_skipRenderContext.GetBool() && viewDef->viewEntitys )
		Pipeline( PIPE_DEFAULT );
}
