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
#include "Backend_common.h"

static const float k_IDENTITY_MAXTRIX[16] = 
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

/*
=====================
BakeTextureMatrixIntoTexgen
=====================
*/
void crBackend::BakeTextureMatrixIntoTexgen( idPlane lightProject[3], const float *textureMatrix ) 
{
	float	genMatrix[16];
	float	final[16];

	genMatrix[0] = lightProject[0][0];
	genMatrix[4] = lightProject[0][1];
	genMatrix[8] = lightProject[0][2];
	genMatrix[12] = lightProject[0][3];

	genMatrix[1] = lightProject[1][0];
	genMatrix[5] = lightProject[1][1];
	genMatrix[9] = lightProject[1][2];
	genMatrix[13] = lightProject[1][3];

	genMatrix[2] = 0;
	genMatrix[6] = 0;
	genMatrix[10] = 0;
	genMatrix[14] = 0;

	genMatrix[3] = lightProject[2][0];
	genMatrix[7] = lightProject[2][1];
	genMatrix[11] = lightProject[2][2];
	genMatrix[15] = lightProject[2][3];

	crTransform::GlMultMatrix( genMatrix, lightTextureMatrix, final );

	lightProject[0][0] = final[0];
	lightProject[0][1] = final[4];
	lightProject[0][2] = final[8];
	lightProject[0][3] = final[12];

	lightProject[1][0] = final[1];
	lightProject[1][1] = final[5];
	lightProject[1][2] = final[9];
	lightProject[1][3] = final[13];
}

/*
================
RB_PrepareStageTexturing
================
*/
void crBackend::PrepareStageTexturing( const shaderStage_t *pStage,  const drawSurf_t *surf ) 
{
	static int32_t texgen[4] = { 1, 1, 1, 1 }; // all one to enable texgen

	// set privatePolygonOffset if necessary
	if ( pStage->privatePolygonOffset ) // todo: set this direct on shader 
		vkCmdSetDepthBias( m_commandBuffers[frameID], r_offsetFactor.GetFloat(), 0.0f, r_offsetUnits.GetFloat() * pStage->privatePolygonOffset );

	// set the texture matrix if needed
	if ( pStage->texture.hasMatrix ) 
		LoadShaderTextureMatrix( surf->shaderRegisters, &pStage->texture );

	switch ( pStage->texture.texgen )
	{
	case TG_DIFFUSE_CUBE:
	{
	} break;
	case TG_SKYBOX_CUBE:
	{
		//TODO: set texture matrixes 
	} break;
	case TG_WOBBLESKY_CUBE:
	{
		//TODO: set texture matrixes 
	}
	case TG_SCREEN:
	case TG_SCREEN2:
	{
		float plane[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
		m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgen );

		crRenderMatrix textureMatrix = surf->space->modelViewMatrix * viewDef->projectionMatrix;
		
		// S face plane
		plane[0] = textureMatrix[0];
		plane[1] = textureMatrix[4];
		plane[2] = textureMatrix[8];
		plane[3] = textureMatrix[12];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, plane ); // glTexGenfv( GL_S, GL_OBJECT_PLANE, plane );

		// T face plane
		plane[0] = textureMatrix[1];
		plane[1] = textureMatrix[5];
		plane[2] = textureMatrix[9];
		plane[3] = textureMatrix[13];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_T, 1, plane ); // glTexGenfv( GL_T, GL_OBJECT_PLANE, plane );

		// Q face plane 
		plane[0] = textureMatrix[3];
		plane[1] = textureMatrix[7];
		plane[2] = textureMatrix[11];
		plane[3] = textureMatrix[15];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_Q, 1, plane ); // qglTexGenfv( GL_Q, GL_OBJECT_PLANE, plane );
	} break;
	case TG_GLASSWARP:
	{
		float plane[4]{ 1.0f, 1.0f, 1.0f, 1.0f }; // all one to enable texgen
		crRenderMatrix textureMatrix;
	
		// glEnable( GL_TEXTURE_GEN_S );
		// glEnable( GL_TEXTURE_GEN_T );
		// glEnable( GL_TEXTURE_GEN_Q ); 
		m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgen );
	
		SelectTexture( 2 );
		globalImages->scratchImage->Bind();

		SelectTexture( 1 );
		globalImages->scratchImage2->Bind();

		textureMatrix = surf->space->modelViewMatrix * viewDef->projectionMatrix; 
		plane[0] = textureMatrix[0];
		plane[1] = textureMatrix[4];
		plane[2] = textureMatrix[8];
		plane[3] = textureMatrix[12];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, plane ); //glTexGenfv( GL_S, GL_OBJECT_PLANE, plane );

		plane[0] = textureMatrix[1];
		plane[1] = textureMatrix[5];
		plane[2] = textureMatrix[9];
		plane[3] = textureMatrix[13];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_T, 1, plane ); //glTexGenfv( GL_T, GL_OBJECT_PLANE, plane );

		plane[0] = textureMatrix[3];
		plane[1] = textureMatrix[7];
		plane[2] = textureMatrix[11];
		plane[3] = textureMatrix[15];
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, plane ); //glTexGenfv( GL_Q, GL_OBJECT_PLANE, plane );
		
		SelectTexture( 0 );
	} break;
	case TG_REFLECT_CUBE:
	{
		// see if there is also a bump map specified
		const shaderStage_t *bumpStage = surf->material->GetBumpStage();
		if ( bumpStage ) 
		{
			// per-pixel reflection mapping with bump mapping
			SelectTexture( 1 );
			bumpStage->texture.image->Bind();
			SelectTexture( 0 );
			// Program env 5, 6, 7, 8 have been set in RB_SetProgramEnvironmentSpace
		} 
	} 
	default:
		break;
	}
}

/*
================
crBackend::FinishStageTexturing
================
*/
void crBackend::FinishStageTexturing( const shaderStage_t *pStage, const drawSurf_t *surf ) 
{
	static int32_t texgen[4] { 0, 0, 0, 0 };
	// unset privatePolygonOffset if necessary
	if ( pStage->privatePolygonOffset && !surf->material->TestMaterialFlag(MF_POLYGONOFFSET) ) 
	{

	}

	switch ( pStage->texture.texgen )
	{
	case TG_DIFFUSE_CUBE:
	{
	} break;
	case TG_WOBBLESKY_CUBE:
	{
	} break;

	case TG_SCREEN:
	case TG_SCREEN2:
	{
		// glDisable( GL_TEXTURE_GEN_S );
		// glDisable( GL_TEXTURE_GEN_T );
		// glDisable( GL_TEXTURE_GEN_Q ); 
		m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgen );
	}
	case TG_GLASSWARP:
	{
		// glDisable( GL_TEXTURE_GEN_S );
		// glDisable( GL_TEXTURE_GEN_T );
		// glDisable( GL_TEXTURE_GEN_Q ); 
		m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgen );

		SelectTexture( 2 );
		globalImages->BindNull();

		SelectTexture( 1 );
		if ( pStage->texture.hasMatrix ) 
			LoadShaderTextureMatrix( surf->shaderRegisters, &pStage->texture );
		
		globalImages->BindNull();
		SelectTexture( 0 );
	} break;
	case TG_REFLECT_CUBE:
	{
		// see if there is also a bump map specified
		const shaderStage_t *bumpStage = surf->material->GetBumpStage();
		if ( bumpStage ) 
		{
			// per-pixel reflection mapping with bump mapping
			SelectTexture( 1 );
			globalImages->BindNull();
			SelectTexture( 0 );
		}
	} break;
	default:
		break;
	}

	if ( pStage->texture.hasMatrix ) 
	{
		// glMatrixMode( GL_TEXTURE );
		// glLoadIdentity();
		// glMatrixMode( GL_MODELVIEW );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXTUREMATRIX_S, 2, k_IDENTITY_MAXTRIX );
	}
}

/*
=============================================================================================

FILL DEPTH BUFFER

=============================================================================================
*/

/*
==================
RB_T_FillDepthBuffer
==================
*/
void crBackend::FillDepthBuffer( const drawSurf_t *surf ) 
{
	int			stage;
	const idMaterial	*shader;
	const shaderStage_t *pStage;
	const float	*regs;
	float		color[4];
	const srfTriangles_t	*tri;

	tri = surf->geo;
	shader = surf->material;

	// update the clip plane if needed
	if ( viewDef->numClipPlanes && surf->space != currentSpace ) 
	{
		SelectTexture( 1 );
		
		idPlane	plane = surf->space->modelMatrix.GlobalPlaneToLocal( viewDef->clipPlanes[0] );

		//crTransform::GlobalPlaneToLocal( surf->space->modelMatrix, viewDef->clipPlanes[0], plane );
		surf->space->modelMatrix.GlobalPlaneToLocal( plane );
		
		plane[3] += 0.5;	// the notch is in the middle
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, plane.ToFloatPtr() ); // glTexGenfv( GL_S, GL_OBJECT_PLANE, plane.ToFloatPtr() );
		SelectTexture( 0 );
	}

	if ( !shader->IsDrawn() ) 
		return;

	// some deforms may disable themselves by setting numIndexes = 0
	if ( !tri->numIndexes ) 
		return;

	// translucent surfaces don't put anything in the depth buffer and don't
	// test against it, which makes them fail the mirror clip plane operation
	if ( shader->Coverage() == MC_TRANSLUCENT )
		return;

	if ( !tri->ambientCache ) 
	{
		common->Printf( "RB_T_FillDepthBuffer: !tri->ambientCache\n" );
		return;
	}

	// get the expressions for conditionals / color / texcoords
	regs = surf->shaderRegisters;

	// if all stages of a material have been conditioned off, don't do anything
	for ( stage = 0; stage < shader->GetNumStages() ; stage++ ) 
	{		
		pStage = shader->GetStage(stage);
		// check the stage enable condition
		if ( regs[ pStage->conditionRegister ] != 0 )
			break;
	}

	if ( stage == shader->GetNumStages() ) 
		return;

	// set polygon offset if necessary
	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) // todo: set this direct on shader 
		vkCmdSetDepthBias( m_commandBuffers[frameID], r_offsetFactor.GetFloat(), 0.0f, r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );

	// subviews will just down-modulate the color buffer by overbright
	if ( shader->GetSort() == SS_SUBVIEW ) 
	{
		color[0] = color[1] = color[2] = ( 1.0 / overBright );
		color[3] = 1;
	} 
	else 
	{
		// others just draw black
		color[0] = color[1] = color[2] = 0;
		color[3] = 1;
	}

	bool drawSolid = false;

	if ( shader->Coverage() == MC_OPAQUE ) 
		drawSolid = true;
	
	// we may have multiple alpha tested stages
	if ( shader->Coverage() == MC_PERFORATED ) 
	{
		// if the only alpha tested stages are condition register omitted,
		// draw a normal opaque surface
		bool	didDraw = false;

		// perforated surfaces may have multiple alpha tested stages
		for ( stage = 0; stage < shader->GetNumStages() ; stage++ ) 
		{		
			pStage = shader->GetStage(stage);

			if ( !pStage->hasAlphaTest )
				continue;

			// check the stage enable condition
			if ( regs[ pStage->conditionRegister ] == 0 ) 
				continue;

			// if we at least tried to draw an alpha tested stage,
			// we won't draw the opaque surface
			didDraw = true;

			// set the alpha modulate
			color[3] = regs[ pStage->color.registers[3] ];

			// skip the entire stage if alpha would be black
			if ( color[3] <= 0 )
				continue;

			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR, 1, color );
			
			// TODO: shader unifrorm alphaRef "if( color.a <= alphaRef ) discard"
			float alphaf[4] = { regs[ pStage->alphaTestRegister ], 0.0f, 0.0f, 0.0f };
			m_vertexUniformBlock->ProgramParameter4fv( FRAGMENT_RENDERPARM_ALPHA_TEST, 1, alphaf ); //glAlphaFunc( GL_GREATER, regs[ pStage->alphaTestRegister ] );

			// BEATO Begin: bind the shader stage pipeline
			SetPipeline( pStage->pipeline );
			// BEATO End

			// bind the texture
			pStage->texture.image->Bind();

			// set texture matrix and texGens
			PrepareStageTexturing( pStage, surf );

			// draw it
			DrawElementsWithCounters( tri );

			FinishStageTexturing( pStage, surf );
		}

		if ( !didDraw ) 
			drawSolid = true;
	}

	// draw the entire surface solid
	if ( drawSolid ) 
	{
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR, 1, color );
		globalImages->whiteImage->Bind();

		// draw it
		DrawElementsWithCounters( tri );
	}


	// reset polygon offset
	//if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) 
	//	continue;

	// reset blending
	// if ( shader->GetSort() == SS_SUBVIEW )
	// 	GL_State( GLS_DEPTHFUNC_LESS );
}

/*
=====================
crBackend::STD_FillDepthBuffer

If we are rendering a subview with a near clip plane, use a second texture
to force the alpha test to fail when behind that clip plane
=====================
*/
void crBackend::STD_FillDepthBuffer( drawSurf_t **drawSurfs, int numDrawSurfs ) 
{
	// if we are just doing 2D rendering, no need to fill the depth buffer
	if ( !viewDef->viewEntitys ) 
		return;

	RB_LogComment( "---------- RB_STD_FillDepthBuffer ----------\n" );

// BEATO Begin:
	Framebuffer( FRAMEBUFFER_DEPTH_PASS );
// BEATO End

	// enable the second texture for mirror plane clipping if needed
	if ( viewDef->numClipPlanes ) 
	{
		SelectTexture( 1 );
		globalImages->alphaNotchImage->Bind();
	}

	// the first texture will be used for alpha tested surfaces
	SelectTexture( 0 );
	
	RenderDrawSurfListWithFunction( drawSurfs, numDrawSurfs, std::bind(&crBackend::FillDepthBuffer, this, std::placeholders::_1) );

	if ( viewDef->numClipPlanes ) 
	{
		SelectTexture( 1 );
		globalImages->BindNull();
		SelectTexture( 0 );
	}

}

/*
=============================================================================================

SHADER PASSES

=============================================================================================
*/

/*
==================
RB_SetProgramEnvironment

Sets variables that can be used by all vertex programs
==================
*/
void crBackend::SetProgramEnvironment( void ) 
{
	float	parm[4]{};
	int		pot = 0;

#if 0
	// screen power of two correction factor, one pixel in so we don't get a bilerp
	// of an uncopied pixel
	int	 w = backEnd.viewDef->viewport.x2 - backEnd.viewDef->viewport.x1 + 1;
	pot = globalImages->currentRenderImage->uploadWidth;
	if ( w == pot ) {
		parm[0] = 1.0;
	} else {
		parm[0] = (float)(w-1) / pot;
	}

	int	 h = backEnd.viewDef->viewport.y2 - backEnd.viewDef->viewport.y1 + 1;
	pot = globalImages->currentRenderImage->uploadHeight;
	if ( h == pot ) {
		parm[1] = 1.0;
	} else {
		parm[1] = (float)(h-1) / pot;
	}

	parm[2] = 0;
	parm[3] = 1;
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 0, parm );
#else
	// screen power of two correction factor, assuming the copy to _currentRender
	// also copied an extra row and column for the bilerp
	int	 w = viewDef->viewport.x2 - viewDef->viewport.x1 + 1;
	pot = globalImages->currentRenderImage->uploadWidth;
	parm[0] = (float)w / pot;

	int	 h = viewDef->viewport.y2 - viewDef->viewport.y1 + 1;
	pot = globalImages->currentRenderImage->uploadHeight;
	parm[1] = (float)h / pot;

	parm[2] = 0;
	parm[3] = 1;
	m_fragmentUniformBlock->ProgramParameter4fv( FRAGMENT_RENDERPARM_SCREENCORRECTIONFACTOR, 1, parm );
	//qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 0, parm );
	//qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 0, parm );
#endif

	// window coord to 0.0 to 1.0 conversion
	parm[0] = 1.0 / w;
	parm[1] = 1.0 / h;
	parm[2] = 0;
	parm[3] = 1;
	m_fragmentUniformBlock->ProgramParameter4fv( FRAGMENT_RENDERPARM_WINDOWCOORD, 1, parm ); //qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 1, parm );

	//
	// set eye position in global space
	//
	parm[0] = viewDef->renderView.vieworg[0];
	parm[1] = viewDef->renderView.vieworg[1];
	parm[2] = viewDef->renderView.vieworg[2];
	parm[3] = 1.0;
	m_fragmentUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_GLOBALEYEPOS, 1, parm ); //qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 1, parm );
}

/*
==================
RB_SetProgramEnvironmentSpace

Sets variables related to the current space that can be used by all vertex programs
==================
*/
void crBackend::SetProgramEnvironmentSpace( void ) 
{
	float	parm[4]{};
	const struct viewEntity_s *space = currentSpace;

	// set eye position in local space
	idVec3 localeye = space->modelMatrix.GlobalPointToLocal( viewDef->renderView.vieworg );
	parm[0] = localeye[0];
	parm[1] = localeye[1];
	parm[2] = localeye[2];
	parm[3] = 1.0f;
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_VIEWORIGIN, 1, parm ); // glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 5, parm );

	// we need the model matrix without it being combined with the view matrix
	// so we can transform local vectors to global coordinates
	parm[0] = space->modelMatrix[0];
	parm[1] = space->modelMatrix[4];
	parm[2] = space->modelMatrix[8];
	parm[3] = space->modelMatrix[12];
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_MODELMATRIX_X, 1, parm ); // glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 6, parm );
	parm[0] = space->modelMatrix[1];
	parm[1] = space->modelMatrix[5];
	parm[2] = space->modelMatrix[9];
	parm[3] = space->modelMatrix[13];
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_MODELMATRIX_Y, 1, parm ); // glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 7, parm );
	parm[0] = space->modelMatrix[2];
	parm[1] = space->modelMatrix[6];
	parm[2] = space->modelMatrix[10];
	parm[3] = space->modelMatrix[14];
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_MODELMATRIX_Z, 1, parm ); // glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, 8, parm );
}

/*
==================
crBackend::RenderShaderPasses

This is also called for the generated 2D rendering
==================
*/
void crBackend::RenderShaderPasses( const drawSurf_t *surf ) 
{
	int						stage = 0;
	const idMaterial*		shader = nullptr;
	const shaderStage_t*	pStage = nullptr;
	const float*			regs = nullptr;
	float					color[4] { 0.0f, 0.0f, 0.0f, 0.0f };
	const srfTriangles_t*	tri = nullptr;

	tri = surf->geo;
	shader = surf->material;

	if ( !shader->HasAmbient() ) 
		return;

	if ( shader->IsPortalSky() ) 
		return;
	
	// change the matrix if needed
	if ( surf->space != currentSpace ) 
	{
		// qglLoadMatrixf( surf->space->modelViewMatrix );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_MODELVIEWMATRIX_X, 4, &surf->space->modelViewMatrix );
		currentSpace = surf->space;
		SetProgramEnvironmentSpace();
	}

	// change the scissor if needed
	if ( r_useScissor.GetBool() && !currentScissor.Equals( surf->scissorRect ) ) 
		Scissor( surf->scissorRect );

	// some deforms may disable themselves by setting numIndexes = 0
	if ( !tri->numIndexes ) 
		return;

	if ( !tri->ambientCache ) 
	{
		common->Printf( "RB_T_RenderShaderPasses: !tri->ambientCache\n" );
		return;
	}

	// get the expressions for conditionals / color / texcoords
	regs = surf->shaderRegisters;

	// set face culling appropriately
	SetCull( shader->GetCullType() );

	// set polygon offset if necessary
	if ( shader->TestMaterialFlag( MF_POLYGONOFFSET ) )
	{
		// glEnable( GL_POLYGON_OFFSET_FILL );
		// glPolygonOffset( r_offsetFactor.GetFloat(), r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );
		vkCmdSetDepthBias( m_commandBuffers[frameID], r_offsetFactor.GetFloat(), 0.0f, r_offsetUnits.GetFloat() * shader->GetPolygonOffset() );
	}
	
	if ( surf->space->weaponDepthHack )
		EnterWeaponDepthHack();

	if ( surf->space->modelDepthHack != 0.0f ) 
		EnterModelDepthHack( surf->space->modelDepthHack );

	for ( stage = 0; stage < shader->GetNumStages() ; stage++ ) 
	{		
		pStage = shader->GetStage(stage);

		// check the enable condition
		if ( regs[ pStage->conditionRegister ] == 0 ) 
			continue;

		// skip the stages involved in lighting
		if ( pStage->lighting != SL_AMBIENT ) 
			continue;

		// skip if the stage is ( GL_ZERO, GL_ONE ), which is used for some alpha masks
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_ZERO | GLS_DSTBLEND_ONE ) ) 
			continue;

		// see if we are a new-style stage
		newShaderStage_t *newStage = pStage->newStage;
		if ( newStage ) 
		{
			//--------------------------
			//
			// new style stages
			//
			//--------------------------	
			if ( r_skipNewAmbient.GetBool() )
				continue;
		
			///
			//SeState( pStage->drawStateBits );
	
// TODO: get rid of megatextures 
#if 0
			// megaTextures bind a lot of images and set a lot of parameters
			if ( newStage->megaTexture ) 
			{
				newStage->megaTexture->SetMappingForSurface( tri );
				idVec3	localViewer;
				crTransform::GlobalPointToLocal( surf->space->modelMatrix, backEnd.viewDef->renderView.vieworg, localViewer );
				newStage->megaTexture->BindForViewOrigin( localViewer );
			}
#endif

// TODO: fix custom shader parms
#if 0 
			for ( int i = 0 ; i < newStage->numVertexParms ; i++ ) 
			{
				float	parm[4];
				parm[0] = regs[ newStage->vertexParms[i][0] ];
				parm[1] = regs[ newStage->vertexParms[i][1] ];
				parm[2] = regs[ newStage->vertexParms[i][2] ];
				parm[3] = regs[ newStage->vertexParms[i][3] ];
				glProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, i, parm );
			}
#endif 

			for ( int i = 0 ; i < newStage->numFragmentProgramImages ; i++ ) 
			{
				if ( newStage->fragmentProgramImages[i] ) 
				{
					SelectTexture( i );
					newStage->fragmentProgramImages[i]->Bind();
				}
			}

			// BEATO Begin: bind the shader stage pipeline
			//glBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, newStage->fragmentProgram );
			//glEnable( GL_FRAGMENT_PROGRAM_ARB );
			SetPipeline( pStage->pipeline );
			// BEATO End

			// draw it
			DrawElementsWithCounters( tri );

			for ( int i = 1 ; i < newStage->numFragmentProgramImages ; i++ ) 
			{
				if ( newStage->fragmentProgramImages[i] ) 
				{
					SelectTexture( i );
					globalImages->BindNull();
				}
			}

// BEATO Begin:
#if 0
			if ( newStage->megaTexture ) 
			{
				newStage->megaTexture->Unbind();
			}
#endif 
// BEATO End
			SelectTexture( 0 );
			continue;
		}

		//--------------------------
		//
		// old style stages
		//
		//--------------------------

		// set the color
		color[0] = regs[ pStage->color.registers[0] ];
		color[1] = regs[ pStage->color.registers[1] ];
		color[2] = regs[ pStage->color.registers[2] ];
		color[3] = regs[ pStage->color.registers[3] ];

		// skip the entire stage if an add would be black
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE ) && color[0] <= 0 && color[1] <= 0 && color[2] <= 0 )
			continue;

		// skip the entire stage if a blend would be completely transparent
		if ( ( pStage->drawStateBits & (GLS_SRCBLEND_BITS|GLS_DSTBLEND_BITS) ) == ( GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA ) && color[3] <= 0 ) 
			continue;

		// select the vertex color source
		if ( pStage->vertexColor == SVC_IGNORE ) 
		{
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR, 1, color );
		}
		else 
		{
			
			// BEATO BFG work arround
			SetVertexColorParms( pStage->vertexColor );
			// BEATO End

			// for vertex color and modulated color, we need to enable a second
			// texture stage
			if ( color[0] != 1 || color[1] != 1 || color[2] != 1 || color[3] != 1 ) 
			{
				SelectTexture( 1 );

				globalImages->whiteImage->Bind();
#if 0
				GL_TexEnv( GL_COMBINE_ARB );

				glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color );

				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_CONSTANT_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );
				glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1 );

				glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_CONSTANT_ARB );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA );
				glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA );
				glTexEnvi( GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1 );
#endif
				SelectTexture( 0 );
			}
		}

		// bind the texture
		BindVariableStageImage( &pStage->texture, regs );

		// set the state
		// BEATO Begin: bind the shader stage pipeline
		SetPipeline( pStage->pipeline ); //SetState( pStage->drawStateBits );
		// BEATO End

		PrepareStageTexturing( pStage, surf );

		// draw it
		DrawElementsWithCounters( tri );

		FinishStageTexturing( pStage, surf );
		
		SetVertexColorParms(  pStage->vertexColor );
	}

	// reset polygon offset
//	if ( shader->TestMaterialFlag(MF_POLYGONOFFSET) ) 
//		glDisable( GL_POLYGON_OFFSET_FILL );

	if ( surf->space->weaponDepthHack || surf->space->modelDepthHack != 0.0f ) 
		LeaveDepthHack();
}

static const float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
static const float one[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
static const float negOne[4] = { -1.0f, -1.0f, -1.0f, -1.0f };

/*
=====================
crBackend::SetVertexColorParms
=====================
*/
void crBackend::SetVertexColorParms(stageVertexColor_t svc)
{
	switch ( svc )
	{
		case SVC_IGNORE:
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_MODULATE, 1, zero );
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_ADD, 1, one );
			break;

		case SVC_MODULATE:
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_MODULATE, 1, one );
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_ADD, 1, zero );
			break;

		case SVC_INVERSE_MODULATE:
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_MODULATE, 1, negOne );
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR_ADD, 1, one );
			break;
	}
}

void crBackend::SetPipeline(const crAutoPointer<crvkPipeline> m_pipeline)
{
	assert( m_pipeline );
	m_currentPipeline = m_pipeline;
}

/*
 =====================
 crBackend::STD_DrawShaderPasses

 Draw non-light dependent passes
 =====================
 */
int crBackend::STD_DrawShaderPasses( drawSurf_t **drawSurfs, int numDrawSurfs ) 
{
	int				i;

	// only obey skipAmbient if we are rendering a view
	if ( viewDef->viewEntitys && r_skipAmbient.GetBool() ) 
	{
		return numDrawSurfs;
	}

	RB_LogComment( "---------- RB_STD_DrawShaderPasses ----------\n" );

	// if we are about to draw the first surface that needs
	// the rendering in a texture, copy it over
	if ( drawSurfs[0]->material->GetSort() >= SS_POST_PROCESS ) {
		if ( r_skipPostProcess.GetBool() ) {
			return 0;
		}

		// only dump if in a 3d view
		if ( viewDef->viewEntitys ) 
		{
			globalImages->currentRenderImage->CopyFramebuffer( viewDef->viewport.x1,
				viewDef->viewport.y1,  viewDef->viewport.x2 -  viewDef->viewport.x1 + 1,
				viewDef->viewport.y2 -  viewDef->viewport.y1 + 1, true );
		}
		currentRenderCopied = true;
	}

	SelectTexture( 1 );
	globalImages->BindNull();

	SelectTexture( 0 );
	
	
	SetProgramEnvironment();

	// we don't use RB_RenderDrawSurfListWithFunction()
	// because we want to defer the matrix load because many
	// surfaces won't draw any ambient passes
	currentSpace = nullptr;
	for (i = 0  ; i < numDrawSurfs ; i++ ) 
	{
		if ( drawSurfs[i]->material->SuppressInSubview() ) 
			continue;

		if ( viewDef->isXraySubview && drawSurfs[i]->space->entityDef ) 
		{
			if ( drawSurfs[i]->space->entityDef->parms.xrayIndex != 2 ) 
				continue;
			
		}

		// we need to draw the post process shaders after we have drawn the fog lights
		if ( drawSurfs[i]->material->GetSort() >= SS_POST_PROCESS && !currentRenderCopied ) 
			break;
		

		RenderShaderPasses( drawSurfs[i] );
	}

	SetCull( CT_FRONT_SIDED );
	float color[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_COLOR, 1, color );
	// glColor3f( 1, 1, 1 );

	return i;
}



/*
==============================================================================

BACK END RENDERING OF STENCIL SHADOWS

==============================================================================
*/

#if 0
/*
=====================
RB_T_Shadow

the shadow volumes face INSIDE
=====================
*/
static void RB_T_Shadow( const drawSurf_t *surf ) 
{
	const srfTriangles_t* tri = nullptr;

	// set the light position if we are using a vertex program to project the rear surfaces
	if ( r_useShadowVertexProgram.GetBool()
		&& surf->space != backEnd.currentSpace ) {
		idVec4 localLight;

		crTransform::GlobalPointToLocal( surf->space->modelMatrix, backEnd.vLight->globalLightOrigin, localLight.ToVec3() );
		localLight.w = 0.0f;
		glProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_ORIGIN, localLight.ToFloatPtr() );
	}

	tri = surf->geo;

	if ( !tri->shadowCache ) {
		return;
	}

	glVertexPointer( 4, GL_FLOAT, sizeof( shadowCache_t ), vertexCache.Position(tri->shadowCache) );

	// we always draw the sil planes, but we may not need to draw the front or rear caps
	int	numIndexes;
	bool external = false;

	if ( !r_useExternalShadows.GetInteger() ) {
		numIndexes = tri->numIndexes;
	} else if ( r_useExternalShadows.GetInteger() == 2 ) { // force to no caps for testing
		numIndexes = tri->numShadowIndexesNoCaps;
	} else if ( !(surf->dsFlags & DSF_VIEW_INSIDE_SHADOW) ) { 
		// if we aren't inside the shadow projection, no caps are ever needed needed
		numIndexes = tri->numShadowIndexesNoCaps;
		external = true;
	} else if ( !backEnd.vLight->viewInsideLight && !(surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE) ) {
		// if we are inside the shadow projection, but outside the light, and drawing
		// a non-infinite shadow, we can skip some caps
		if ( backEnd.vLight->viewSeesShadowPlaneBits & surf->geo->shadowCapPlaneBits ) {
			// we can see through a rear cap, so we need to draw it, but we can skip the
			// caps on the actual surface
			numIndexes = tri->numShadowIndexesNoFrontCaps;
		} else {
			// we don't need to draw any caps
			numIndexes = tri->numShadowIndexesNoCaps;
		}
		external = true;
	} else {
		// must draw everything
		numIndexes = tri->numIndexes;
	}

	// set depth bounds
	if( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) {
		glDepthBoundsEXT( surf->scissorRect.zmin, surf->scissorRect.zmax );
	}

	// debug visualization
	if ( r_showShadows.GetInteger() ) {
		if ( r_showShadows.GetInteger() == 3 ) {
			if ( external ) {
				glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
			} else {
				// these are the surfaces that require the reverse
				glColor3f( 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright );
			}
		} else {
			// draw different color for turboshadows
			if ( surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE ) {
				if ( numIndexes == tri->numIndexes ) {
					glColor3f( 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright );
				} else {
					glColor3f( 1/backEnd.overBright, 0.4/backEnd.overBright, 0.1/backEnd.overBright );
				}
			} else {
				if ( numIndexes == tri->numIndexes ) {
					glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
				} else if ( numIndexes == tri->numShadowIndexesNoFrontCaps ) {
					glColor3f( 0.1/backEnd.overBright, 1/backEnd.overBright, 0.6/backEnd.overBright );
				} else {
					glColor3f( 0.6/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright );
				}
			}
		}

		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

// BEATO Begin:
		glSetState( GL_STENCIL_TEST, GL_FALSE ); //glDisable( GL_STENCIL_TEST );
		GL_Cull( CT_TWO_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		GL_Cull( CT_FRONT_SIDED );
		glSetState( GL_STENCIL_TEST, GL_TRUE ); //glEnable( GL_STENCIL_TEST );
// BEATO End

		return;
	}

	// patent-free work around
	if ( !external ) {
		// "preload" the stencil buffer with the number of volumes
		// that get clipped by the near or far clip plane
		glStencilOp( GL_KEEP, tr.stencilDecr, tr.stencilDecr );
		GL_Cull( CT_FRONT_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		glStencilOp( GL_KEEP, tr.stencilIncr, tr.stencilIncr );
		GL_Cull( CT_BACK_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
	}

	// traditional depth-pass stencil shadows
	glStencilOp( GL_KEEP, GL_KEEP, tr.stencilIncr );
	GL_Cull( CT_FRONT_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );

	glStencilOp( GL_KEEP, GL_KEEP, tr.stencilDecr );
	GL_Cull( CT_BACK_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );
}
#endif

#if 0
/*
=====================
RB_StencilShadowPass

Stencil test should already be enabled, and the stencil buffer should have
been set to 128 on any surfaces that might receive shadows
=====================
*/
void RB_StencilShadowPass( const drawSurf_t *drawSurfs ) 
{
	if ( !r_shadows.GetBool() )
		return;

	if ( !drawSurfs ) 
		return;

	RB_LogComment( "---------- RB_StencilShadowPass ----------\n" );

	globalImages->BindNull();
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// for visualizing the shadows
	if ( r_showShadows.GetInteger() ) 
	{
		if ( r_showShadows.GetInteger() == 2 ) 
		{
			// draw filled in
			GL_State( GLS_DEPTHMASK | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_LESS  );
		} 
		else 
		{
			// draw as lines, filling the depth buffer
			GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO | GLS_POLYMODE_LINE | GLS_DEPTHFUNC_ALWAYS  );
		}
	} 
	else 
	{
		// don't write to the color buffer, just the stencil buffer
		GL_State( GLS_DEPTHMASK | GLS_COLORMASK | GLS_ALPHAMASK | GLS_DEPTHFUNC_LESS );
	}

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() )
	{
		glPolygonOffset( r_shadowPolygonFactor.GetFloat(), -r_shadowPolygonOffset.GetFloat() );
		glSetState( GL_POLYGON_OFFSET_FILL, GL_TRUE ); //glEnable( GL_POLYGON_OFFSET_FILL );
	}

	glStencilFunc( GL_ALWAYS, 1, 255 );

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) 
	{
		glEnable( GL_DEPTH_BOUNDS_TEST_EXT );
	}

	RB_RenderDrawSurfChainWithFunction( drawSurfs, RB_T_Shadow );

	GL_Cull( CT_FRONT_SIDED );

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() ) 
	{
		glDisable( GL_POLYGON_OFFSET_FILL );
	}

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) 
	{
		glDisable( GL_DEPTH_BOUNDS_TEST_EXT );
	}

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glStencilFunc( GL_GEQUAL, 128, 255 );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
}
#endif 
/*
=============================================================================================

BLEND LIGHT PROJECTION

=============================================================================================
*/

/*
=====================
T_BlendLight
=====================
*/
void crBackend::T_BlendLight( const drawSurf_t *surf )
{
	const srfTriangles_t *tri;

	tri = surf->geo;

	if ( currentSpace != surf->space ) 
	{
		idPlane	lightProject[4];
		int		i;

		for ( i = 0 ; i < 4 ; i++ ) 
		{
			//crTransform::GlobalPlaneToLocal( surf->space->modelMatrix, backEnd.vLight->lightProject[i], lightProject[i] );
			lightProject[i] = surf->space->modelMatrix.GlobalPlaneToLocal( viewLight->lightProject[i] );
		}

		SelectTexture( 0 );
		// glTexGenfv( GL_S, GL_OBJECT_PLANE, lightProject[0].ToFloatPtr() );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, lightProject[0].ToFloatPtr() );
		// glTexGenfv( GL_T, GL_OBJECT_PLANE, lightProject[1].ToFloatPtr() );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_T, 1, lightProject[1].ToFloatPtr() );
		// glTexGenfv( GL_Q, GL_OBJECT_PLANE, lightProject[2].ToFloatPtr() );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_Q, 1, lightProject[2].ToFloatPtr() );

		SelectTexture( 1 );
		// glTexGenfv( GL_S, GL_OBJECT_PLANE, lightProject[3].ToFloatPtr() );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_1_S, 1, lightProject[0].ToFloatPtr() );
	}

	// this gets used for both blend lights and shadow draws
	VkBuffer buffer = nullptr;
	VkDeviceSize size = 0;
	VkDeviceSize offset = 0;
	VkDeviceSize stride = sizeof( idDrawVert );
	if ( tri->ambientCache ) 
	{
		offset = tri->ambientCache->Offset();
		size = tri->ambientCache->Size();
	} 
	else if ( tri->shadowCache ) 
	{
		offset = tri->shadowCache->Offset();
		size = tri->shadowCache->Size();
	}
	
	vkCmdBindVertexBuffers2( m_commandBuffers[frameID], 0, 1, &buffer, &offset, &size, &stride );
	DrawElementsWithCounters( tri );
}

/*
=====================
crBackend::BlendLight

Dual texture together the falloff and projection texture with a blend
mode to the framebuffer, instead of interacting with the surface texture
=====================
*/
void crBackend::BlendLight( const drawSurf_t *drawSurfs,  const drawSurf_t *drawSurfs2 )
{
	const idMaterial	*lightShader = nullptr;
	const shaderStage_t	*stage = nullptr;
	int					i = 0;
	const float	*regs;

	if ( !drawSurfs ) 
		return;
	
	if ( r_skipBlendLights.GetBool() ) 
		return;
	
	RB_LogComment( "---------- RB_BlendLight ----------\n" );

	lightShader = viewLight->lightShader;
	regs = viewLight->shaderRegisters;

	// texture 1 will get the falloff texture
	SelectTexture( 1 ); 
	
	// glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	//glEnable( GL_TEXTURE_GEN_S );
	// TODO: glTexCoord2f( 0, 0.5 );
	viewLight->falloffImage->Bind();

	// texture 0 will get the projected texture
	SelectTexture( 0 );
	//glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	//glEnable( GL_TEXTURE_GEN_S );
	//glEnable( GL_TEXTURE_GEN_T );
	//glEnable( GL_TEXTURE_GEN_Q );
	float enable[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, enable );

	for ( i = 0 ; i < lightShader->GetNumStages() ; i++ ) 
	{
		stage = lightShader->GetStage(i);

		if ( !regs[ stage->conditionRegister ] ) 
			continue;

		// TODO: 
		// GL_State( GLS_DEPTHMASK | stage->drawStateBits | GLS_DEPTHFUNC_EQUAL );

		SelectTexture( 0 );
		stage->texture.image->Bind();

		if ( stage->texture.hasMatrix )
			LoadShaderTextureMatrix( regs, &stage->texture );

		// get the modulate values from the light, including alpha, unlike normal lights
		lightColor[0] = regs[ stage->color.registers[0] ];
		lightColor[1] = regs[ stage->color.registers[1] ];
		lightColor[2] = regs[ stage->color.registers[2] ];
		lightColor[3] = regs[ stage->color.registers[3] ];
		
		//glColor4fv( backEnd.lightColor );


		RenderDrawSurfChainWithFunction( drawSurfs, std::bind(&crBackend::T_BlendLight, this, std::placeholders::_1) );
		RenderDrawSurfChainWithFunction( drawSurfs2, std::bind(&crBackend::T_BlendLight, this, std::placeholders::_1) );

		if ( stage->texture.hasMatrix ) 
		{
			SelectTexture( 0 );
			// glMatrixMode( GL_TEXTURE );
			// glLoadIdentity();
			// glMatrixMode( GL_MODELVIEW );
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXTUREMATRIX_S, 2, k_IDENTITY_MAXTRIX );
			m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 3, k_IDENTITY_MAXTRIX );
		}
	}

	SelectTexture( 1 );
	//glDisable( GL_TEXTURE_GEN_S );
	globalImages->BindNull();

	SelectTexture( 0 );
	float texten[4] { 0.0f, 0.0f, 0.0f, 0.0f };
	// glDisable( GL_TEXTURE_GEN_S );
	// glDisable( GL_TEXTURE_GEN_T );
	// glDisable( GL_TEXTURE_GEN_Q );
	m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texten );
}


//========================================================================

static idPlane	fogPlanes[4];

/*
=====================
RB_T_BasicFog

=====================
*/
void crBackend::T_BasicFog( const drawSurf_t *surf ) 
{
	if ( currentSpace != surf->space ) 
	{
		idPlane	local;

		SelectTexture( 0 );

		local = surf->space->modelMatrix.GlobalPlaneToLocal( fogPlanes[0] );
		local[3] += 0.5;
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_S, 1, local.ToFloatPtr() );//glTexGenfv( GL_S, GL_OBJECT_PLANE, local.ToFloatPtr() );

		local[0] = local[1] = local[2] = 0; local[3] = 0.5;
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_0_T, 1, local.ToFloatPtr() ); // glTexGenfv( GL_T, GL_OBJECT_PLANE, local.ToFloatPtr() );

		SelectTexture( 1 );

		// GL_S is constant per viewer
		local = surf->space->modelMatrix.GlobalPlaneToLocal( fogPlanes[2] );
		local[3] += FOG_ENTER;
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_1_T, 1, local.ToFloatPtr() ); // glTexGenfv( GL_T, GL_OBJECT_PLANE, local.ToFloatPtr() );

		local = surf->space->modelMatrix.GlobalPlaneToLocal( fogPlanes[3] );
		m_vertexUniformBlock->ProgramParameter4fv( VERTEX_RENDERPARM_TEXGEN_1_S, 1, local.ToFloatPtr() ); // glTexGenfv( GL_S, GL_OBJECT_PLANE, local.ToFloatPtr() );
	}

	RenderTriangleSurface( surf );
}



/*
==================
RB_FogPass
==================
*/
void crBackend::FogPass( const drawSurf_t *drawSurfs,  const drawSurf_t *drawSurfs2 ) 
{
	drawSurf_t				ds;
	int32_t texgens[4] { 0, 0, 0, 0 };
	const srfTriangles_t* 	frustumTris = nullptr;
	const idMaterial*		lightShader = nullptr;
	const shaderStage_t*	stage = nullptr;
	const float*			regs = nullptr;

	RB_LogComment( "---------- RB_FogPass ----------\n" );

	// create a surface for the light frustom triangles, which are oriented drawn side out
	frustumTris = viewLight->frustumTris;

	// if we ran out of vertex cache memory, skip it
	if ( !frustumTris->ambientCache ) 
		return;

	memset( &ds, 0, sizeof( ds ) );
	ds.space = &viewDef->worldSpace;
	ds.geo = frustumTris;
	ds.scissorRect = viewDef->scissor;

	// find the current color and density of the fog
	lightShader = viewLight->lightShader;
	regs = viewLight->shaderRegisters;
	// assume fog shaders have only a single stage
	stage = lightShader->GetStage(0);

	lightColor[0] = regs[ stage->color.registers[0] ];
	lightColor[1] = regs[ stage->color.registers[1] ];
	lightColor[2] = regs[ stage->color.registers[2] ];
	lightColor[3] = regs[ stage->color.registers[3] ];
	m_vertexUniformBlock->ProgramParameter4fv( LIGHT_RENDERPARM_LIGHTCOLOR, 1, lightColor ); // glColor3fv( backEnd.lightColor );

	// calculate the falloff planes
	float a = 0.0f;

	// if they left the default value on, set a fog distance of 500
	if ( lightColor[3] <= 1.0 ) 
		a = -0.5f / DEFAULT_FOG_DISTANCE;
	else 
		// otherwise, distance = alpha color
		a = -0.5f / lightColor[3];

	//GL_State( GLS_DEPTHMASK | GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_EQUAL );

	// texture 0 is the falloff image
	SelectTexture( 0 );
	globalImages->fogImage->Bind();
	
	//GL_Bind( tr.whiteImage );
	texgens[0] = 1; // glEnable( GL_TEXTURE_GEN_S );
	texgens[1] = 1; // glEnable( GL_TEXTURE_GEN_T );
	m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_1_ENABLED, 1, texgens );
	// glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// TODO:
	// glTexCoord2f( 0.5f, 0.5f );		// make sure Q is set

	fogPlanes[0][0] = a * viewDef->worldSpace.modelViewMatrix[2];
	fogPlanes[0][1] = a * viewDef->worldSpace.modelViewMatrix[6];
	fogPlanes[0][2] = a * viewDef->worldSpace.modelViewMatrix[10];
	fogPlanes[0][3] = a * viewDef->worldSpace.modelViewMatrix[14];

	fogPlanes[1][0] = a * viewDef->worldSpace.modelViewMatrix[0];
	fogPlanes[1][1] = a * viewDef->worldSpace.modelViewMatrix[4];
	fogPlanes[1][2] = a * viewDef->worldSpace.modelViewMatrix[8];
	fogPlanes[1][3] = a * viewDef->worldSpace.modelViewMatrix[12];


	// texture 1 is the entering plane fade correction
	SelectTexture( 1 );
	globalImages->fogEnterImage->Bind();
	//glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	texgens[0] = 1; //glEnable( GL_TEXTURE_GEN_S );
	texgens[1] = 1; //glEnable( GL_TEXTURE_GEN_T );
	m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgens );

	// T will get a texgen for the fade plane, which is always the "top" plane on unrotated lights
	fogPlanes[2][0] = 0.001f * viewLight->fogPlane[0];
	fogPlanes[2][1] = 0.001f * viewLight->fogPlane[1];
	fogPlanes[2][2] = 0.001f * viewLight->fogPlane[2];
	fogPlanes[2][3] = 0.001f * viewLight->fogPlane[3];

	// S is based on the view origin
	float s = viewDef->renderView.vieworg * fogPlanes[2].Normal() + fogPlanes[2][3];

	fogPlanes[3][0] = 0;
	fogPlanes[3][1] = 0;
	fogPlanes[3][2] = 0;
	fogPlanes[3][3] = FOG_ENTER + s;

	// TODO:
	// glTexCoord2f( FOG_ENTER + s, FOG_ENTER );

	// draw it
	RenderDrawSurfChainWithFunction( drawSurfs, std::bind( &crBackend::T_BasicFog, this, std::placeholders::_1 ) );
	RenderDrawSurfChainWithFunction( drawSurfs2, std::bind( &crBackend::T_BasicFog, this, std::placeholders::_1 ) );

	// the light frustum bounding planes aren't in the depth buffer, so use depthfunc_less instead
	// of depthfunc_equal
	//GL_State( GLS_DEPTHMASK | GLS_SRCBLEND_SRC_ALPHA | GLS_DSTBLEND_ONE_MINUS_SRC_ALPHA | GLS_DEPTHFUNC_LESS );
	//GL_Cull( CT_BACK_SIDED );
	RenderDrawSurfChainWithFunction( &ds, std::bind( &crBackend::T_BasicFog, this, std::placeholders::_1) );
	//GL_Cull( CT_FRONT_SIDED );


	SelectTexture( 1 );
	globalImages->BindNull();
	SelectTexture( 0 );

	texgens[0]= 0; // glDisable( GL_TEXTURE_GEN_S );
	texgens[1]= 0; // glDisable( GL_TEXTURE_GEN_T );
	m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_1_ENABLED, 1, texgens );
	m_vertexUniformBlock->ProgramParameter4iv( VERTEX_RENDERPARM_TEXGEN_0_ENABLED, 1, texgens );
}

/*
==================
crBackend::STD_FogAllLights
==================
*/
void crBackend::STD_FogAllLights( void )
{
	crAutoPointer<viewLight_t>	vLight;

	if ( r_skipFogLights.GetBool() || r_showOverDraw.GetInteger() != 0 
		 || viewDef->isXraySubview /* dont fog in xray mode*/
		 ) {
		return;
	}

	RB_LogComment( "---------- RB_STD_FogAllLights ----------\n" );

	// glDisable( GL_STENCIL_TEST );

	for ( vLight = viewDef->viewLights ; vLight ; vLight = vLight->next ) 
	{
		viewLight = vLight;

		if ( !vLight->lightShader->IsFogLight() && !vLight->lightShader->IsBlendLight() )
			continue;

#if 0 // _D3XP disabled that
		if ( r_ignore.GetInteger() ) {
			// we use the stencil buffer to guarantee that no pixels will be
			// double fogged, which happens in some areas that are thousands of
			// units from the origin
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) {
				glScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			glClear( GL_STENCIL_BUFFER_BIT );

			glEnable( GL_STENCIL_TEST );

			// only pass on the cleared stencil values
			glStencilFunc( GL_EQUAL, 128, 255 );

			// when we pass the stencil test and depth test and are going to draw,
			// increment the stencil buffer so we don't ever draw on that pixel again
			glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
		}
#endif

		if ( vLight->lightShader->IsFogLight() )
			FogPass( vLight->globalInteractions, vLight->localInteractions );
		else if ( vLight->lightShader->IsBlendLight() ) 
			BlendLight( vLight->globalInteractions, vLight->localInteractions );
		
		// glDisable( GL_STENCIL_TEST );
	}

	// glEnable( GL_STENCIL_TEST );
}

//=========================================================================================

/*
==================
crBackend::STD_LightScale

Perform extra blending passes to multiply the entire buffer by
a floating point value
==================
*/
void crBackend::STD_LightScale( void ) 
{
	float	v = 0.0f, f = 0.0f;

	if ( overBright == 1.0f ) 
		return;

	if ( r_skipLightScale.GetBool() )
		return;

	RB_LogComment( "---------- RB_STD_LightScale ----------\n" );

	// the scissor may be smaller than the viewport for subviews
	if ( r_useScissor.GetBool() ) 
		Scissor( viewDef->scissor );

	// full screen blends
	// glLoadIdentity();
	// glMatrixMode( GL_PROJECTION );
	// glPushMatrix();
	// glLoadIdentity(); 
    // glOrtho( 0, 1, 0, 1, -1, 1 );

	// GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_SRC_COLOR );
	// GL_Cull( CT_TWO_SIDED );	// so mirror views also get it
	globalImages->BindNull();
	// glDisable( GL_DEPTH_TEST );
	// glDisable( GL_STENCIL_TEST );

	v = 1;
	while ( idMath::Fabs( v - overBright ) > 0.01 ) // a little extra slop
	{	
		f = overBright / v;
		f /= 2;
		if ( f > 1 )
			f = 1;
		
		// glColor3f( f, f, f );
		v = v * f * 2;

		//glBegin( GL_QUADS );
		//glVertex2f( 0,0 );	
		//glVertex2f( 0,1 );
		//glVertex2f( 1,1 );	
		//glVertex2f( 1,0 );	
		//glEnd();
	}


	// glPopMatrix();
	// glEnable( GL_DEPTH_TEST );
	// glMatrixMode( GL_MODELVIEW );
	// GL_Cull( CT_FRONT_SIDED );
}

//=========================================================================================

/*
=============
crBackend::STD_DrawView
=============
*/
void crBackend::STD_DrawView( void ) 
{
	drawSurf_t	 **drawSurfs;
	int			numDrawSurfs;

	RB_LogComment( "---------- RB_STD_DrawView ----------\n" );

	drawSurfs = (drawSurf_t **)&viewDef->drawSurfs[0];
	numDrawSurfs = viewDef->numDrawSurfs;

	// clear the z buffer, set the projection matrix, etc
	BeginDrawingView();

	// decide how much overbrighting we are going to do
	DetermineLightScale();

	// fill the depth buffer and clear color buffer to black except on
	// subviews
	STD_FillDepthBuffer( drawSurfs, numDrawSurfs );

	// main light renderer
	DrawInteractions();
	
	// disable stencil shadow test
	vkCmdSetStencilCompareMask( m_commandBuffers[frameID], VK_STENCIL_FACE_FRONT_AND_BACK, 255 ); // glStencilFunc( GL_ALWAYS, 128, 255 );

	// uplight the entire screen to crutch up not having better blending range
	STD_LightScale();

	// now draw any non-light dependent shading passes
	int	processed = STD_DrawShaderPasses( drawSurfs, numDrawSurfs );

	// fob and blend lights
	STD_FogAllLights();

	// now draw any post-processing effects using _currentRender
	if ( processed < numDrawSurfs ) 
	{
		STD_DrawShaderPasses( drawSurfs+processed, numDrawSurfs-processed );
	}

	RB_RenderDebugTools( drawSurfs, numDrawSurfs );

}
