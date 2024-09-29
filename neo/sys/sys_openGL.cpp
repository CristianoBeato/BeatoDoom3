/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2012 dhewg (dhewm3)
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

#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_mouse.h>

#include "renderer/qgl.h"
#include "renderer/tr_local.h"
#include "sys_main.h"

static struct btRenderContext_s
{
	SDL_GLContext			renderContext = nullptr;
}renderContext;

extern idCVar r_inhibitFragmentProgram;
static void R_CheckPortableExtensions( void );
static bool R_CheckExtension( const char *name );
static const bool CheckExtension( const char* extName, const bool required = false );

int Sys_GetVideoRam( void ) 
{
	return 0;
}

void Sys_StartUpOpenGLAPI(void)
{
	// TODO: Cvar to load specific OpenGL lib
	SDL_GL_LoadLibrary( nullptr );
	LoadOpenGLFunctions();
}

//
void Sys_ShutDownOpenGLAPI( void )
{
	SDL_GL_UnloadLibrary();
}

/*
=====================
Sys_SwapBuffersGL
=====================
*/
void Sys_SwapBuffersGL( void )
{
	// Quick and dirt, fix that
	static SDL_Window* window;
	if ( !window )
		window = (SDL_Window*)Sys_videoWindowHandler();
	
	if (r_swapInterval.IsModified())
	{
		r_swapInterval.ClearModified();
		SDL_GL_SetSwapInterval( r_swapInterval.GetInteger() );
	}

	SDL_GL_SwapWindow( window );
}

const bool   Sys_InitOpenGLContext( glimpParms_t parms )
{
	// defalt parameters
	int colorbits = 8;
	int alphabits = 8;
	int depthbits = 24;
	int stencilbits = 8;
	Uint32 samplesCount = 4;

	auto gWindow = static_cast<SDL_Window*>( Sys_videoWindowHandler() );
	assert( gWindow != nullptr );

	common->Printf( "Initializing OpenGL subsystem\n" );

	if (parms.fullScreen == 1)
	{
		// TODO:
		Sys_videoFullScreen( 1 );
	}
	else
	{
		int x, y;
		SDL_GetGlobalMouseState( &x, &y );

		int numDisplays = SDL_GetNumVideoDisplays();

		//Resize window to final size
		Sys_videoWindowSize( numDisplays, parms.width, parms.height );
	}

	// OpenGL 4.5 core context
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
#if 0
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#else
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
#endif

	// Set Debug output 
#if defined( _DEBUG ) || defined( DEBUG )
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
#endif //_DEBUG

	//
	// enable sRGB render buffers
	SDL_GL_SetAttribute( SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1 );

	//
	// Enable Hardware acelerate rendering on some platforms
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

	// We do our own triple buffer renderer
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 0 );

	//
	// Enable MSAA
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, (samplesCount > 0) ? 1 : 0 );
	SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, samplesCount );

	//
	// Enable Stereo rendering ( is this needed for current time ? ) 
	SDL_GL_SetAttribute( SDL_GL_STEREO, parms.stereo ? 1 : 0 );

	// Set the buffers size
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, alphabits );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, depthbits );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencilbits );

	//
	renderContext.renderContext = SDL_GL_CreateContext( gWindow );
	if (renderContext.renderContext == nullptr)
	{
		const char* SDLErr = SDL_GetError();
		return false;
	}

	R_CheckPortableExtensions();

	// 
#if defined( _DEBUG ) || defined( DEBUG )
	InitOpenGLDebugOutput();
#endif
}

void Sys_ShutDownOpenGLContext( void )
{
	common->Printf( "Shutting down OpenGL subsystem\n" );	
	if (renderContext.renderContext != nullptr)
	{
		SDL_GL_DeleteContext( renderContext.renderContext );
		renderContext.renderContext = nullptr;
	}
}

/*
===================
GLimp_ActivateContext
===================
*/
//void		GLimp_ActivateContext( void )
//{
//	auto gWindow = static_cast<SDL_Window*>( Sys_videoWindowHandler() );
//	assert( renderContext.renderContext != nullptr && gWindow != nullptr );
//
//	auto currentGLContext = SDL_GL_GetCurrentContext();
//	if (currentGLContext != renderContext.renderContext) // TIP: use a memcmp ?
//		SDL_GL_MakeCurrent( gWindow, renderContext.renderContext );
//}

/*
===================
GLimp_DeactivateContext
===================
*/
//void		GLimp_DeactivateContext( void )
//{
//	auto gWindow = static_cast<SDL_Window*>( Sys_videoWindowHandler() );
//	assert( gWindow != nullptr );
//
//	glFinish();
//	SDL_GL_MakeCurrent( gWindow, nullptr );
//}

/*
==================
R_CheckPortableExtensions

==================
*/
void R_CheckPortableExtensions( void )
{
	glConfig.glVersion = atof( glConfig.version_string );

	// GL_ARB_multitexture
	glConfig.multitextureAvailable = R_CheckExtension( "GL_ARB_multitexture" );
	if (glConfig.multitextureAvailable)
	{
		glGetIntegerv( GL_MAX_TEXTURE_UNITS, (GLint *)&glConfig.maxTextureUnits );
		if (glConfig.maxTextureUnits > MAX_MULTITEXTURE_UNITS)
			glConfig.maxTextureUnits = MAX_MULTITEXTURE_UNITS;
		
		if (glConfig.maxTextureUnits < 2)
			glConfig.multitextureAvailable = false;	// shouldn't ever happen

		glGetIntegerv( GL_MAX_TEXTURE_COORDS, (GLint *)&glConfig.maxTextureCoords );
		glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&glConfig.maxTextureImageUnits );
	}

	// GL_ARB_texture_env_combine
	glConfig.textureEnvCombineAvailable = R_CheckExtension( "GL_ARB_texture_env_combine" );

	// GL_ARB_texture_cube_map
	glConfig.cubeMapAvailable = R_CheckExtension( "GL_ARB_texture_cube_map" );

	// GL_ARB_texture_env_dot3
	glConfig.envDot3Available = R_CheckExtension( "GL_ARB_texture_env_dot3" );

	// GL_ARB_texture_env_add
	glConfig.textureEnvAddAvailable = R_CheckExtension( "GL_ARB_texture_env_add" );

	// GL_ARB_texture_non_power_of_two
	glConfig.textureNonPowerOfTwoAvailable = R_CheckExtension( "GL_ARB_texture_non_power_of_two" );

	// GL_ARB_texture_compression + GL_S3_s3tc
	// DRI drivers may have GL_ARB_texture_compression but no GL_EXT_texture_compression_s3tc
	if (!R_CheckExtension( "GL_ARB_texture_compression" ) && !R_CheckExtension( "GL_EXT_texture_compression_s3tc" ))
		glConfig.textureCompressionAvailable = false;

	// GL_EXT_texture_filter_anisotropic
	glConfig.anisotropicAvailable = R_CheckExtension( "GL_EXT_texture_filter_anisotropic" );
	if (glConfig.anisotropicAvailable)
	{
		glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY, &glConfig.maxTextureAnisotropy );
		common->Printf( "   maxTextureAnisotropy: %f\n", glConfig.maxTextureAnisotropy );
	}
	else
		glConfig.maxTextureAnisotropy = 1;

	// GL_EXT_texture_lod_bias
	// The actual extension is broken as specificed, storing the state in the texture unit instead
	// of the texture object.  The behavior in GL 1.4 is the behavior we use.
	if (glConfig.glVersion >= 1.4 || R_CheckExtension( "GL_EXT_texture_lod" ))
	{
		common->Printf( "...using %s\n", "GL_1.4_texture_lod_bias" );
		glConfig.textureLODBiasAvailable = true;
	}
	else
	{
		common->Printf( "X..%s not found\n", "GL_1.4_texture_lod_bias" );
		glConfig.textureLODBiasAvailable = false;
	}

	// GL_EXT_shared_texture_palette
	glConfig.sharedTexturePaletteAvailable = R_CheckExtension( "GL_EXT_shared_texture_palette" );

	// GL_EXT_texture3D (not currently used for anything)
	glConfig.texture3DAvailable = R_CheckExtension( "GL_EXT_texture3D" );

	// EXT_stencil_wrap
	// This isn't very important, but some pathological case might cause a clamp error and give a shadow bug.
	// Nvidia also believes that future hardware may be able to run faster with this enabled to avoid the
	// serialization of clamping.
	if (R_CheckExtension( "GL_EXT_stencil_wrap" ))
	{
		tr.stencilIncr = GL_INCR_WRAP;
		tr.stencilDecr = GL_DECR_WRAP;
	}
	else
	{
		tr.stencilIncr = GL_INCR;
		tr.stencilDecr = GL_DECR;
	}

	// GL_EXT_stencil_two_side
	glConfig.twoSidedStencilAvailable = R_CheckExtension( "GL_EXT_stencil_two_side" );
	glConfig.atiTwoSidedStencilAvailable = R_CheckExtension( "GL_ATI_separate_stencil" );
	
	// GL_ATI_fragment_shader
	glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_fragment_shader" );
	if (!glConfig.atiFragmentShaderAvailable)
	{
		// only on OSX: ATI_fragment_shader is faked through ATI_text_fragment_shader (macosx_glimp.cpp)
		glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_text_fragment_shader" );
	}
	
	// ARB_vertex_buffer_object
	glConfig.ARBVertexBufferObjectAvailable = R_CheckExtension( "GL_ARB_vertex_buffer_object" );
	
	// ARB_vertex_program
	glConfig.ARBVertexProgramAvailable = R_CheckExtension( "GL_ARB_vertex_program" );
	
	// ARB_fragment_program
	if (r_inhibitFragmentProgram.GetBool())
		glConfig.ARBFragmentProgramAvailable = false;
	else
		glConfig.ARBFragmentProgramAvailable = R_CheckExtension( "GL_ARB_fragment_program" );

	// check for minimum set
	if (!glConfig.multitextureAvailable || !glConfig.textureEnvCombineAvailable || !glConfig.cubeMapAvailable || !glConfig.envDot3Available)
		common->Error( common->GetLanguageDict()->GetString( "#str_06780" ) );

	// GL_EXT_depth_bounds_test
	glConfig.depthBoundsTestAvailable = R_CheckExtension( "EXT_depth_bounds_test" );

	// GL_ARB_debug_output
	glConfig.debugOutputAvailable = R_CheckExtension( "ARB_debug_output" );
}

/*
=================
R_CheckExtension
=================
*/
bool R_CheckExtension( const char *name )
{
	if (!SDL_GL_ExtensionSupported( name ))
	{
		common->Printf( "X..%s not found\n", name );
		return false;
	}

	common->Printf( "...using %s\n", name );
	return true;
}
