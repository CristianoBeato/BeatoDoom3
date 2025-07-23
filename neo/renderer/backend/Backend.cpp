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

// BEATO Begin:
#include "Backend_common.h"

crBackend::crBackend( void )
{
}

crBackend::~crBackend( void )
{
}
// BEATO End

/*
======================
RB_SetDefaultGLState

This should initialize all GL state that any part of the entire program
may touch, including the editor.
======================
*/
void RB_SetDefaultGLState( void ) 
{
	int		i;

	RB_LogComment( "--- R_SetDefaultGLState ---\n" );

	glClearDepth( 1.0f );

	//
	// make sure our GL state vector is set correctly
	//
	memset( &backEnd.glState, 0, sizeof( backEnd.glState ) );
	backEnd.glState.forceGlState = true;

	glColorMask( 1, 1, 1, 1 );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_SCISSOR_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_LINE_STIPPLE );
	glDisable( GL_STENCIL_TEST );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_ALWAYS );
	
	glCullFace( GL_FRONT_AND_BACK );
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
//	glShadeModel( GL_SMOOTH ); // FIXED FUNCTION PIPELINE

	if ( r_useScissor.GetBool() ) 
	{
		glScissor( 0, 0, glConfig.vidWidth, glConfig.vidHeight );
	}

	for ( i = glConfig.maxTextureUnits - 1 ; i >= 0 ; i-- ) 
	{
		GL_SelectTexture( i );

		// object linear texgen is our default
		glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

		GL_TexEnv( GL_MODULATE );
		glDisable( GL_TEXTURE_2D );
		if ( glConfig.texture3DAvailable ) 
		{
			glDisable( GL_TEXTURE_3D );
		}
		
		if ( glConfig.cubeMapAvailable ) 
		{
			glDisable( GL_TEXTURE_CUBE_MAP );
		}
	}
}


/*
====================
RB_LogComment
====================
*/
void RB_LogComment( const char *comment, ... ) 
{
   va_list marker;

	if ( !tr.logFile ) 
	{
		return;
	}

	fprintf( tr.logFile, "// " );
	va_start( marker, comment );
	vfprintf( tr.logFile, comment, marker );
	va_end( marker );
}

//=============================================================================

/*
=============
crBackend::SetBuffer
=============
*/
void crBackend::SetBuffer( const void *data ) 
{
	// see which draw buffer we want to render the frame to
	const setBufferCommand_t	*cmd = static_cast<const setBufferCommand_t*>( data );

	frameCount = cmd->frameCount;

	// swap uniform buffer, and swap chain and prepare for a new frame
	m_uniforms->Begin( frameCount ); 
	m_swapChain->Begin( frameCount );

	// clear screen for debugging
	// automatically enable this with several other debug tools
	// that might leave unrendered portions of the screen
	if ( r_clear.GetFloat() || idStr::Length( r_clear.GetString() ) != 1 || r_lockSurfaces.GetBool() || r_singleArea.GetBool() || r_showOverDraw.GetBool() ) 
	{
		float c[3];
		if ( sscanf( r_clear.GetString(), "%f %f %f", &c[0], &c[1], &c[2] ) == 3 ) 
			m_currentPipeline->ClearColor( c[0], c[1], c[2], 1 );
		else if ( r_clear.GetInteger() == 2 ) 
			m_currentPipeline->ClearColor( 0.0f, 0.0f,  0.0f, 1.0f );
		else if ( r_showOverDraw.GetBool() ) 
			m_currentPipeline->ClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		else 
			m_currentPipeline->ClearColor( 0.4f, 0.0f, 0.25f, 1.0f );
		
		// clear current frame buffer 
		m_currentPipeline->Clear();
	}
}

#if 0
/*
===============
RB_ShowImages

Draw all the images to the screen, on top of whatever
was there.  This is used to test for texture thrashing.
===============
*/
static void RB_ShowImages( void )
 {
	int		i;
	idImage	*image;
	float	x, y, w, h;
	int		start, end;

	RB_SetGL2D();

	//glClearColor( 0.2, 0.2, 0.2, 1 );
	//glClear( GL_COLOR_BUFFER_BIT );

	glFinish();

	start = Sys_Milliseconds();

	for ( i = 0 ; i < globalImages->images.Num() ; i++ ) {
		image = globalImages->images[i];

		if ( image->texnum == idImage::TEXTURE_NOT_LOADED && image->partialImage == NULL ) {
			continue;
		}

		w = glConfig.vidWidth / 20;
		h = glConfig.vidHeight / 15;
		x = i % 20 * w;
		y = i / 20 * h;

		// show in proportional size in mode 2
		if ( r_showImages.GetInteger() == 2 ) {
			w *= image->uploadWidth / 512.0f;
			h *= image->uploadHeight / 512.0f;
		}

		image->Bind();
		glBegin (GL_QUADS);
		glTexCoord2f( 0, 0 );
		glVertex2f( x, y );
		glTexCoord2f( 1, 0 );
		glVertex2f( x + w, y );
		glTexCoord2f( 1, 1 );
		glVertex2f( x + w, y + h );
		glTexCoord2f( 0, 1 );
		glVertex2f( x, y + h );
		glEnd();
	}

	glFinish();

	end = Sys_Milliseconds();
	common->Printf( "%i msec to draw all images\n", end - start );
}
#endif

/*
=============
crBackend::SwapBuffers
=============
*/
void crBackend::SwapBuffers( const void *data ) 
{
#if 0 // TODO:
	// texture swapping test
	if ( r_showImages.GetInteger() != 0 ) 
		RB_ShowImages();
#endif 

	// force a sync if requested
	if ( r_finish.GetBool() )
		m_swapChain->Flush();

    RB_LogComment( "***************** RB_SwapBuffers *****************\n\n\n" );

	// don't flip if drawing to front buffer
	if ( !r_frontBuffer.GetBool() )
		m_swapChain->SwapBuffers(); 
}

/*
=============
RB_CopyRender

Copy part of the current framebuffer to an image
=============
*/
void crBackend::CopyRender( const void *data ) 
{
	const copyRenderCommand_t	*cmd = static_cast<const copyRenderCommand_t *>( data );

	if ( r_skipCopyTexture.GetBool() )
		return;

    RB_LogComment( "***************** RB_CopyRender *****************\n" );

	m_currentFrameBuffer->CopyToImage( &cmd->image->GetTextureHandler(), cmd->x, cmd->y, cmd->imageWidth, cmd->imageHeight, 0 );
	c_copyFrameBuffer++;
}

/*
====================
ZeroPerformanceCounters
====================
*/
void crBackend::ZeroPerformanceCounters( void )
{
	pc = backEndCounters_t();
}

/*
====================
ExecuteBackEndCommands

This function will be called syncronously if running without
smp extensions, or asyncronously by another thread.
====================
*/
void crBackend::ExecuteBackEndCommands( const emptyCommand_t *cmds ) 
{
	static int backEndStartTime = 0; 
	static int backEndFinishTime = 0;

	// r_debugRenderToTexture
	int	c_draw3d = 0, c_draw2d = 0, c_setBuffers = 0, c_swapBuffers = 0, c_copyRenders = 0;

	if ( cmds->commandId == RC_NOP && !cmds->next ) 
		return;

	backEndStartTime = Sys_Milliseconds();

	// needed for editor rendering
	Pipeline( PIPE_DEFAULT );

	// upload any image loads that have completed
	globalImages->CompleteBackgroundImageLoads();

	for ( ; cmds ; cmds = (const emptyCommand_t *)cmds->next ) 
	{
		switch ( cmds->commandId ) 
		{
		case RC_NOP:
			break;
		case RC_DRAW_VIEW:
			DrawView( cmds );
			if ( ((const drawSurfsCommand_t *)cmds)->viewDef->viewEntitys ) 
				c_draw3d++;
			else 
				c_draw2d++;
			break;
		case RC_SET_BUFFER:
			SetBuffer( cmds );
			c_setBuffers++;
			break;
		case RC_SWAP_BUFFERS:
			SwapBuffers( cmds );
			c_swapBuffers++;
			break;
		case RC_COPY_RENDER:
			CopyRender( cmds );
			c_copyRenders++;
			break;
		default:
			common->Error( "RB_ExecuteBackEndCommands: bad commandId" );
			break;
		}
	}

	// stop rendering on this thread
	backEndFinishTime = Sys_Milliseconds();
	pc.msec = backEndFinishTime - backEndStartTime;

	if ( r_debugRenderToTexture.GetInteger() == 1 ) 
	{
		common->Printf( "3d: %i, 2d: %i, SetBuf: %i, SwpBuf: %i, CpyRenders: %i, CpyFrameBuf: %i\n", c_draw3d, c_draw2d, c_setBuffers, c_swapBuffers, c_copyRenders, backEnd.c_copyFrameBuffer );
		c_copyFrameBuffer = 0;
	}
}

void crBackend::Pipeline( const uint32_t pipelineID )
{
	if ( pipelineID > PIPE_INVALID && pipelineID < PIPE_COUNT )
		m_currentPipeline = m_pipelines[pipelineID];
	else
		m_currentPipeline = nullptr;
}

void crBackend::Framebuffer( const uint32_t framebufferID )
{
	if ( framebufferID > FRAMEBUFFER_INVALID && framebufferID < FRAMEBUFFER_COUNT )
		m_currentFrameBuffer = m_framebuffers[framebufferID];
	else
		m_currentFrameBuffer = nullptr;
}

void crBackend::Viewport( const int x, const int y, const int width, const int height )
{
	m_currentPipeline->SetViewport( x, y, width, height );
}

void crBackend::Scissor( const int x, const int y, const int width, const int height )
{
#if CR_USE_CLIP_AS_SCISSOR
	float scissor[4] = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
	m_uniforms->SetUniform( scissor, VERTEX_UNIFORM_LOCATION_CLIP_BOUDS );
#else
	m_currentPipeline->SetScissor( x, y, width, height );
#endif
}
