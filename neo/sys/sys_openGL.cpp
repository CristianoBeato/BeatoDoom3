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

#include <SDL_mouse.h>
#include <SDL_video.h>

#include "common/window.h"
#include "renderer/tr_local.h"
#include "renderer/qgl.h"
#include "sys_main.h"

// TODO: add a debug level Cvar
// Debug Level Flags 
#define DEBUG_CONTEX_VERBOSE 0
#define DEBUG_CONTEX_IGNORE_NON_SIGNIFICANT 1
#define DEBUG_CONTEX_ERROR_ONLY 2

struct btDiplayModes_s
{
	int index = 0;
	int w = 0;
	int h = 0;
};

struct btDisplayInfo_s
{
	int	index = 0;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	idStr name = "";
	idList <btDiplayModes_s> modes;
};

// TODO: move to a better place 
static struct btRenderContext_s
{
	SDL_GLContext			renderContext = nullptr;
	bool					gammaOrigError = false;
	bool					gammaOrigSet = false;
	unsigned short			gammaOrigRed[256];
	unsigned short			gammaOrigGreen[256];
	unsigned short			gammaOrigBlue[256];
	idList<btDisplayInfo_s>	displays;
}renderContext;

/*

*/

//
//ARB_debug_output
static PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = nullptr;
static PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl = nullptr;

PFNGLACTIVESTENCILFACEEXTPROC	glActiveStencilFaceEXT = nullptr;

PFNGLSTENCILOPSEPARATEPROC		glStencilOpSeparate = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC	glStencilFuncSeparate = nullptr;

// ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DPROC	glCompressedTexImage2D = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC	glGetCompressedTexImage = nullptr;

/*

*/
extern idCVar r_inhibitFragmentProgram;
static void R_CheckPortableExtensions( void );
static bool R_CheckExtension( char *name );
static void GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] );
static void GLimp_RestoreGamma( void );
static const bool CheckExtension( const char* extName, const bool required = false );
static void GLAPIENTRY DebuOutput( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
static void	LoadGLExtPointers( void );
static void InitDebugOutput( int inflags );

template<typename t_>
ID_INLINE GLboolean glGetProc( t_ &proc, const GLchar* procName )
{
	proc = static_cast<t_>(SDL_GL_GetProcAddress( procName ));
	return (proc != nullptr) ? GL_TRUE : GL_FALSE;
}

#define GET_EXT( ext, req ) CheckExtension(#ext, req)
#define GET_GLPROC(proc) { SDL_assert( glGetProc( proc, #proc ) == GL_TRUE ); }


/*
====================================================
OPEN GL DEBUG CONTEXT
Print openGL messages in runtime
====================================================
*/
enum
{
	CTXD_OUTPUT_API = 0,
	CTXD_OUTPUT_WSY,
	CTXD_OUTPUT_SHC,
	CTXD_OUTPUT_THP,
	CTXD_OUTPUT_APP,
	CTXD_OUTPUT_OTH,
	CTXD_OUTPUT_ERR,
	CTXD_OUTPUT_DPB,
	CTXD_OUTPUT_UDB,
	CTXD_OUTPUT_PTB,
	CTXD_OUTPUT_PFC,
	CTXD_OUTPUT_MKR,
	CTXD_OUTPUT_PSG,
	CTXD_OUTPUT_PPG,
	CTXD_OUTPUT_HIG,
	CTXD_OUTPUT_MED,
	CTXD_OUTPUT_LOW,
	CTXD_OUTPUT_NTF,
	CTXD_OUTPUT_UKN
};

static const char k_returns[CTXD_OUTPUT_UKN][24] =
{
	"API",
	"WINDOW SYSTEM",
	"SHADER COMPILER",
	"THIRD PARTY",
	"APPLICATION",
	"OTHER",
	"ERROR",
	"DEPRECATE BEHAVIOUR",
	"UNDEFINED BEHAVIOUR",
	"PORTABILITY",
	"PERFOMANCE",
	"MARKER",
	"PUSH GROUP",
	"POP GROUP",
	"HIGH",
	"MEDIUM",
	"LOW"
	"NOTIFICATION"
	"UNKNOW"
};

void GLAPIENTRY DebuOutput( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
	unsigned char outType = CTXD_OUTPUT_UKN;
	unsigned char outSource = CTXD_OUTPUT_UKN;
	unsigned char outSeverity = CTXD_OUTPUT_UKN;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		outType = CTXD_OUTPUT_ERR;
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		outType = CTXD_OUTPUT_DPB;
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		outType = CTXD_OUTPUT_UDB;
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		outType = CTXD_OUTPUT_PTB;
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		outType = CTXD_OUTPUT_PFC;
		break;
	case GL_DEBUG_TYPE_MARKER:
		outType = CTXD_OUTPUT_MKR;
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		outType = CTXD_OUTPUT_PSG;
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		outType = CTXD_OUTPUT_PPG;
		break;
	case GL_DEBUG_TYPE_OTHER:
		outType = CTXD_OUTPUT_OTH;
		break;
	default:
		break;
	}

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		outSource = CTXD_OUTPUT_API;
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		outSource = CTXD_OUTPUT_WSY;
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		outSource = CTXD_OUTPUT_SHC;
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		outSource = CTXD_OUTPUT_THP;
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		outSource = CTXD_OUTPUT_APP;
		break;
	case GL_DEBUG_SOURCE_OTHER:
		outSource = CTXD_OUTPUT_OTH;
		break;
	default:
		break;
	};

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		outSeverity = CTXD_OUTPUT_HIG;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		outSeverity = CTXD_OUTPUT_MED;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		outSeverity = CTXD_OUTPUT_LOW;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		outSeverity = CTXD_OUTPUT_NTF;
		break;
	default:
		break;
	}

	if (severity == GL_DEBUG_SEVERITY_HIGH)
		Sys_Error("id %i\nSource: %s\nType : %s\nSeverity : %s\n %s\n", 
		id, k_returns[outSource], k_returns[outType], k_returns[outSeverity], message );
	else
		Sys_Printf("OpenGL DebugOut: id %i\nSource: %s\nType: %s\nSeverity: %s\n %s\n", 
			id, k_returns[outSource], k_returns[outType], k_returns[outSeverity], message );

	// "pause" current thread
	if (severity == GL_DEBUG_SEVERITY_HIGH) SDL_TriggerBreakpoint();
}

ID_INLINE GLint glGetInteger( const GLenum pname )
{
	GLint params = 0;
	glGetIntegerv( pname, &params );
	return params;
}

ID_INLINE GLboolean glGetState( const GLenum flag )
{
	return glIsEnabled( flag ) ? GL_TRUE : GL_FALSE;
}

ID_INLINE GLboolean glSetState( const GLenum flag, const GLboolean enable )
{
	GLboolean former = glGetState( flag );
	if (former != enable)
	{
		if (enable)
			glEnable( flag );
		else
			glDisable( flag );
	}
	return former;
}

ID_INLINE GLenum glGetActiveTexture( void )
{
	GLint unit = 0;
	glGetIntegerv( GL_ACTIVE_TEXTURE, &unit );
	return unit;
}

static ID_INLINE const GLuint glGetBuferBinding( const GLenum target )
{
	GLenum binding = GL_INVALID_ENUM;
	switch (target)
	{
	case GL_ARRAY_BUFFER:
		binding = GL_ARRAY_BUFFER_BINDING; break;
	case GL_ATOMIC_COUNTER_BUFFER:
		binding = GL_ATOMIC_COUNTER_BUFFER_BINDING; break;
	case GL_COPY_READ_BUFFER:
		binding = GL_COPY_READ_BUFFER_BINDING; break;
	case GL_COPY_WRITE_BUFFER:
		binding = GL_COPY_WRITE_BUFFER_BINDING; break;
	case GL_DISPATCH_INDIRECT_BUFFER:
		binding = GL_DISPATCH_INDIRECT_BUFFER_BINDING; break;
	case GL_DRAW_INDIRECT_BUFFER:
		binding = GL_DRAW_INDIRECT_BUFFER_BINDING; break;
	case GL_ELEMENT_ARRAY_BUFFER:
		binding = GL_ELEMENT_ARRAY_BUFFER_BINDING; break;
	case GL_PIXEL_PACK_BUFFER:
		binding = GL_PIXEL_PACK_BUFFER_BINDING; break;
	case GL_PIXEL_UNPACK_BUFFER:
		binding = GL_PIXEL_UNPACK_BUFFER_BINDING; break;
	case GL_QUERY_BUFFER:
		binding = GL_QUERY_BUFFER_BINDING; break;
	case GL_SHADER_STORAGE_BUFFER:
		binding = GL_SHADER_STORAGE_BUFFER_BINDING; break;
	case GL_TEXTURE_BUFFER:
		binding = GL_TEXTURE_BUFFER_BINDING; break;
	case GL_TRANSFORM_FEEDBACK_BUFFER:
		binding = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING; break;
	case GL_UNIFORM_BUFFER:
		binding = GL_UNIFORM_BUFFER_BINDING; break;
	default:
		assert( false );
	}

	return glGetInteger( binding );
}

static ID_INLINE const GLuint glGetTextureBinding( const GLenum target )
{
	GLenum binding = GL_INVALID_ENUM;
	switch (target)
	{
	case GL_TEXTURE_1D:
		binding = GL_TEXTURE_BINDING_1D; break;
	case GL_TEXTURE_1D_ARRAY:
		binding = GL_TEXTURE_BINDING_1D_ARRAY; break;
	case GL_TEXTURE_2D:
		binding = GL_TEXTURE_BINDING_2D; break;
	case GL_TEXTURE_2D_ARRAY:
		binding = GL_TEXTURE_BINDING_2D_ARRAY; break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		binding = GL_TEXTURE_BINDING_2D_MULTISAMPLE; break;
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
		binding = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY; break;
	case GL_TEXTURE_3D:
		binding = GL_TEXTURE_BINDING_3D; break;
	case GL_TEXTURE_RECTANGLE:
		binding = GL_TEXTURE_BINDING_RECTANGLE; break;
	case GL_TEXTURE_CUBE_MAP:
		binding = GL_TEXTURE_BINDING_CUBE_MAP; break;
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		binding = GL_TEXTURE_BINDING_CUBE_MAP_ARRAY; break;
	default:
		assert( false );
	}

	return glGetInteger( binding );
}


//
void Sys_InitOpenGL( void )
{
	LoadGLExtPointers();
	ListDisplays();
}

//
void Sys_ShutDownOpenGL( void )
{
}

const bool CheckExtension( const char * extName, const bool required )
{
	bool avaidable = SDL_GL_ExtensionSupported( extName ) == SDL_TRUE;

	idLib::common->Printf( "%s, %s\n", extName, (avaidable) ? "Avaidable" : "Not Avaidabe" );
	if (required && (!avaidable))
		idLib::common->FatalError( " Required OpenGL extension, %s, is not avaidable, fail to initialize\n", required );

	return avaidable;
}

void LoadGLExtPointers( void )
{

	GET_GLPROC( glDepthBoundsEXT );
}

bool GLimp_Init( glimpParms_t parms )
{
	// defalt parameters
	int colorbits = 8;
	int alphabits = 8;
	int depthbits = 24;
	int stencilbits = 8;
	Uint32 samplesCount = 4;

	auto gWindow = sysVars.window->GetHandler();
	assert( gWindow != nullptr );


	common->Printf( "Initializing OpenGL subsystem\n" );

	if (parms.fullScreen == 1)
	{
#if 0 // TODO:
		if (r_fullscreenDesktop.GetBool())
			sysVars.window->TogleFullScreen( true );
		else
			sysVars.window->TogleFullScreen( true, true );
#else
		sysVars.window->TogleFullScreen( true );
#endif
	}
	else
	{
		int x, y;
		SDL_GetGlobalMouseState( &x, &y );

		int numDisplays = SDL_GetNumVideoDisplays();

		//Resize window to final size
		SDL_Rect	windowBounds = {};

		sysVars.window.UpdateBounds( windowBounds );
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
#if _DEBUG
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
#endif //_DEBUG

	//
	// enable sRGB render buffers
	SDL_GL_SetAttribute( SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1 );

	//
	// Enable Hardware acelerate rendering on some platforms
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

	//
	// Double buferred
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

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


	renderContext.renderContext = SDL_GL_CreateContext( gWindow );
	if (renderContext.renderContext == nullptr)
	{
		const char* SDLErr = SDL_GetError();
		return false;
	}

	R_CheckPortableExtensions();

#if _DEBUG
	InitDebugOutput( DEBUG_CONTEX_IGNORE_NON_SIGNIFICANT );
#else
	InitDebugOutput( DEBUG_CONTEX_ERROR_ONLY );
#endif
}


bool GLimp_SetScreenParms( glimpParms_t parms )
{
	//
}

void GLimp_Shutdown( void )
{
	common->Printf( "Shutting down OpenGL subsystem\n" );
	GLimp_RestoreGamma();
	
	if (renderContext.renderContext != nullptr)
	{
		SDL_GL_DeleteContext( renderContext.renderContext );
		renderContext.renderContext = nullptr;
	}
}


/*
=================
GLimp_SetGamma
=================
*/
void GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] )
{
	auto gWindow = sysVars.window->GetHandler();
	assert( gWindow != nullptr );

	if (!renderContext.gammaOrigSet)
	{
		renderContext.gammaOrigSet = true;
		if (SDL_GetWindowGammaRamp( gWindow, renderContext.gammaOrigRed, renderContext.gammaOrigGreen, renderContext.gammaOrigBlue ) == -1)
		{
			renderContext.gammaOrigError = true;
			common->Warning( "Failed to get Gamma Ramp: %s\n", SDL_GetError() );
		}
	}

	if (SDL_SetWindowGammaRamp( gWindow, red, green, blue ))
		common->Warning( "Couldn't set gamma ramp: %s", SDL_GetError() );
}

/*
========================
GLimp_RestoreGamma
========================
*/
void GLimp_RestoreGamma( void )
{
	if (renderContext.gammaOrigError)
	{
		common->Warning( "Can't reset hardware gamma because getting the Gamma Ramp at startup failed!\n" );
		common->Warning( "You might have to restart the game for gamma/brightness in shaders to work properly.\n" );
		return;
	}

	auto gWindow = sysVars.window->GetHandler();
	assert( gWindow != nullptr );
	SDL_SetWindowGammaRamp( gWindow, renderContext.gammaOrigRed, renderContext.gammaOrigGreen, renderContext.gammaOrigBlue );
}

/*
=====================
GLimp_SwapBuffers
=====================
*/
void GLimp_SwapBuffers( void )
{
	auto gWindow = sysVars.window->GetHandler();
	assert( renderContext.renderContext != nullptr && gWindow != nullptr );
	if (r_swapInterval.IsModified())
	{
		r_swapInterval.ClearModified();
		SDL_GL_SetSwapInterval( r_swapInterval.GetInteger() );
	}

	SDL_GL_SwapWindow( gWindow );
}

/*
===================
GLimp_ActivateContext
===================
*/
void		GLimp_ActivateContext( void )
{
	auto gWindow = sysVars.window->GetHandler();
	assert( renderContext.renderContext != nullptr && gWindow != nullptr );

	auto currentGLContext = SDL_GL_GetCurrentContext();
	if (currentGLContext != renderContext.renderContext) // TIP: use a memcmp ?
		SDL_GL_MakeCurrent( gWindow, renderContext.renderContext );
}

/*
===================
GLimp_DeactivateContext
===================
*/
void		GLimp_DeactivateContext( void )
{
	auto gWindow = sysVars.window->GetHandler();
	assert( gWindow != nullptr );

	glFinish();
	SDL_GL_MakeCurrent( gWindow, nullptr );
}


bool GLimp_SpawnRenderThread( void( *function )(void) )
{
}

void *		GLimp_BackEndSleep( void )
{
}

void		GLimp_FrontEndSleep( void )
{
}

void		GLimp_WakeBackEnd( void *data )
{
}

void		GLimp_EnableLogging( bool enable )
{
}


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
		glMultiTexCoord2fARB = (void(APIENTRY *)(GLenum, GLfloat, GLfloat))GLimp_ExtensionPointer( "glMultiTexCoord2fARB" );
		glMultiTexCoord2fvARB = (void(APIENTRY *)(GLenum, GLfloat *))GLimp_ExtensionPointer( "glMultiTexCoord2fvARB" );
		glActiveTextureARB = (void(APIENTRY *)(GLenum))GLimp_ExtensionPointer( "glActiveTextureARB" );
		glClientActiveTextureARB = (void(APIENTRY *)(GLenum))GLimp_ExtensionPointer( "glClientActiveTextureARB" );
		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, (GLint *)&glConfig.maxTextureUnits );
		if (glConfig.maxTextureUnits > MAX_MULTITEXTURE_UNITS)
			glConfig.maxTextureUnits = MAX_MULTITEXTURE_UNITS;
		
		if (glConfig.maxTextureUnits < 2)
			glConfig.multitextureAvailable = false;	// shouldn't ever happen

		glGetIntegerv( GL_MAX_TEXTURE_COORDS_ARB, (GLint *)&glConfig.maxTextureCoords );
		glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS_ARB, (GLint *)&glConfig.maxTextureImageUnits );
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
	if (R_CheckExtension( "GL_ARB_texture_compression" ) && R_CheckExtension( "GL_EXT_texture_compression_s3tc" ))
	{
		glConfig.textureCompressionAvailable = true;
		qglCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)GLimp_ExtensionPointer( "glCompressedTexImage2DARB" );
		qglGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)GLimp_ExtensionPointer( "glGetCompressedTexImageARB" );
	}
	else
	{
		glConfig.textureCompressionAvailable = false;
	}

	// GL_EXT_texture_filter_anisotropic
	glConfig.anisotropicAvailable = R_CheckExtension( "GL_EXT_texture_filter_anisotropic" );
	if (glConfig.anisotropicAvailable)
	{
		qglGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glConfig.maxTextureAnisotropy );
		common->Printf( "   maxTextureAnisotropy: %f\n", glConfig.maxTextureAnisotropy );
	}
	else
	{
		glConfig.maxTextureAnisotropy = 1;
	}

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
	if (glConfig.sharedTexturePaletteAvailable)
	{
		glColorTableEXT = (void (APIENTRY *) (int, int, int, int, int, const void *)) GLimp_ExtensionPointer( "glColorTableEXT" );
	}

	// GL_EXT_texture3D (not currently used for anything)
	glConfig.texture3DAvailable = R_CheckExtension( "GL_EXT_texture3D" );
	if (glConfig.texture3DAvailable)
	{
		glTexImage3D =
			(void (APIENTRY *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))
			GLimp_ExtensionPointer( "glTexImage3D" );
	}

	// EXT_stencil_wrap
	// This isn't very important, but some pathological case might cause a clamp error and give a shadow bug.
	// Nvidia also believes that future hardware may be able to run faster with this enabled to avoid the
	// serialization of clamping.
	if (R_CheckExtension( "GL_EXT_stencil_wrap" ))
	{
		tr.stencilIncr = GL_INCR_WRAP_EXT;
		tr.stencilDecr = GL_DECR_WRAP_EXT;
	}
	else
	{
		tr.stencilIncr = GL_INCR;
		tr.stencilDecr = GL_DECR;
	}

	// GL_NV_register_combiners
	glConfig.registerCombinersAvailable = R_CheckExtension( "GL_NV_register_combiners" );
	if (glConfig.registerCombinersAvailable)
	{
		glCombinerParameterfvNV = (void (APIENTRY *)(GLenum pname, const GLfloat *params))
			GLimp_ExtensionPointer( "glCombinerParameterfvNV" );
		glCombinerParameterivNV = (void (APIENTRY *)(GLenum pname, const GLint *params))
			GLimp_ExtensionPointer( "glCombinerParameterivNV" );
		glCombinerParameterfNV = (void (APIENTRY *)(GLenum pname, const GLfloat param))
			GLimp_ExtensionPointer( "glCombinerParameterfNV" );
		glCombinerParameteriNV = (void (APIENTRY *)(GLenum pname, const GLint param))
			GLimp_ExtensionPointer( "glCombinerParameteriNV" );
		glCombinerInputNV = (void (APIENTRY *)(GLenum stage, GLenum portion, GLenum variable, GLenum input,
			GLenum mapping, GLenum componentUsage))
			GLimp_ExtensionPointer( "glCombinerInputNV" );
		glCombinerOutputNV = (void (APIENTRY *)(GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput,
			GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct,
			GLboolean cdDotProduct, GLboolean muxSum))
			GLimp_ExtensionPointer( "glCombinerOutputNV" );
		glFinalCombinerInputNV = (void (APIENTRY *)(GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage))
			GLimp_ExtensionPointer( "glFinalCombinerInputNV" );
	}

	// GL_EXT_stencil_two_side
	glConfig.twoSidedStencilAvailable = R_CheckExtension( "GL_EXT_stencil_two_side" );
	if (glConfig.twoSidedStencilAvailable)
	{
		glActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)GLimp_ExtensionPointer( "glActiveStencilFaceEXT" );
	}
	else
	{
		glConfig.atiTwoSidedStencilAvailable = R_CheckExtension( "GL_ATI_separate_stencil" );
		if (glConfig.atiTwoSidedStencilAvailable)
		{
			glStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)GLimp_ExtensionPointer( "glStencilFuncSeparateATI" );
			glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)GLimp_ExtensionPointer( "glStencilOpSeparateATI" );
		}
	}

	// GL_ATI_fragment_shader
	glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_fragment_shader" );
	if (!glConfig.atiFragmentShaderAvailable)
	{
		// only on OSX: ATI_fragment_shader is faked through ATI_text_fragment_shader (macosx_glimp.cpp)
		glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_text_fragment_shader" );
	}
	if (glConfig.atiFragmentShaderAvailable)
	{
		glGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)GLimp_ExtensionPointer( "glGenFragmentShadersATI" );
		glBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glBindFragmentShaderATI" );
		glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glDeleteFragmentShaderATI" );
		glBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glBeginFragmentShaderATI" );
		glEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glEndFragmentShaderATI" );
		glPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)GLimp_ExtensionPointer( "glPassTexCoordATI" );
		glSampleMapATI = (PFNGLSAMPLEMAPATIPROC)GLimp_ExtensionPointer( "glSampleMapATI" );
		glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp1ATI" );
		glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp2ATI" );
		glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp3ATI" );
		glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp1ATI" );
		glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp2ATI" );
		glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp3ATI" );
		glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)GLimp_ExtensionPointer( "glSetFragmentShaderConstantATI" );
	}

	// ARB_vertex_buffer_object
	glConfig.ARBVertexBufferObjectAvailable = R_CheckExtension( "GL_ARB_vertex_buffer_object" );
	if (glConfig.ARBVertexBufferObjectAvailable)
	{
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)GLimp_ExtensionPointer( "glBindBufferARB" );
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)GLimp_ExtensionPointer( "glDeleteBuffersARB" );
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)GLimp_ExtensionPointer( "glGenBuffersARB" );
		glIsBufferARB = (PFNGLISBUFFERARBPROC)GLimp_ExtensionPointer( "glIsBufferARB" );
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)GLimp_ExtensionPointer( "glBufferDataARB" );
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)GLimp_ExtensionPointer( "glBufferSubDataARB" );
		glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)GLimp_ExtensionPointer( "glGetBufferSubDataARB" );
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)GLimp_ExtensionPointer( "glMapBufferARB" );
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)GLimp_ExtensionPointer( "glUnmapBufferARB" );
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)GLimp_ExtensionPointer( "glGetBufferParameterivARB" );
		glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)GLimp_ExtensionPointer( "glGetBufferPointervARB" );
	}

	// ARB_vertex_program
	glConfig.ARBVertexProgramAvailable = R_CheckExtension( "GL_ARB_vertex_program" );
	if (glConfig.ARBVertexProgramAvailable)
	{
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)GLimp_ExtensionPointer( "glVertexAttribPointerARB" );
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)GLimp_ExtensionPointer( "glEnableVertexAttribArrayARB" );
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)GLimp_ExtensionPointer( "glDisableVertexAttribArrayARB" );
		glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)GLimp_ExtensionPointer( "glProgramStringARB" );
		glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)GLimp_ExtensionPointer( "glBindProgramARB" );
		glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)GLimp_ExtensionPointer( "glGenProgramsARB" );
		glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramEnvParameter4fvARB" );
		glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramLocalParameter4fvARB" );
	}

	// ARB_fragment_program
	if (r_inhibitFragmentProgram.GetBool())
	{
		glConfig.ARBFragmentProgramAvailable = false;
	}
	else
	{
		glConfig.ARBFragmentProgramAvailable = R_CheckExtension( "GL_ARB_fragment_program" );
		if (glConfig.ARBFragmentProgramAvailable)
		{
			// these are the same as ARB_vertex_program
			GET_GLPROC( glProgramStringARB );
			GET_GLPROC( glBindProgramARB );
			GET_GLPROC( glProgramEnvParameter4fvARB );
			GET_GLPROC( glProgramLocalParameter4fvARB );
		}
	}

	// check for minimum set
	if (!glConfig.multitextureAvailable || !glConfig.textureEnvCombineAvailable || !glConfig.cubeMapAvailable
		|| !glConfig.envDot3Available)
	{
		common->Error( common->GetLanguageDict()->GetString( "#str_06780" ) );
	}

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
bool R_CheckExtension( char *name )
{
	if (!SDL_GL_ExtensionSupported( name ))
	{
		common->Printf( "X..%s not found\n", name );
		return false;
	}

	common->Printf( "...using %s\n", name );
	return true;
}

/*
=================
InitDebugOutput
=================
*/
void InitDebugOutput( int inflags )
{
	// Disable if unsuported 
	if (!glConfig.debugOutputAvailable)
		return;

	GLint flags = glGetInteger( GL_CONTEXT_FLAGS );
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// Habilita a saida de depuração do contexto 
		// Enable context debug output 
		glSetState( GL_DEBUG_OUTPUT, GL_TRUE );

		// Faz com que a saida occora na thread atual
		// Make the output in current thread 
		glSetState( GL_DEBUG_OUTPUT_SYNCHRONOUS, GL_TRUE );
		glDebugMessageCallback( DebuOutput, nullptr );

		//
		//
		if (inflags == DEBUG_CONTEX_VERBOSE)
			glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
		else if (inflags == DEBUG_CONTEX_IGNORE_NON_SIGNIFICANT)
			glDebugMessageControl( GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE );
		//else if ()
	}
}

void ListDisplays( void )
{
	SDL_Rect displayBounds = {0, 0, 0, 0};
	int	displayModeCount = 0;
	int displaysCount = SDL_GetNumVideoDisplays();
	if (displaysCount < 1)
	{
		Sys_Error( " Can't list Displays %s\n", SDL_GetError() );
		return;
	}

	for (Uint32 displayIndex = 0; displayIndex < displaysCount; displayIndex++)
	{
		btDisplayInfo_s displayInfo;
		displayInfo.index = displayIndex;
		displayInfo.name = SDL_GetDisplayName( displayIndex );
		if(SDL_GetDisplayUsableBounds( displayIndex, &displayBounds ) != 0)
		{
			common->Warning( " Can't get Display %i (%s) usable bounds\n ", displayIndex, displayInfo.name.c_str() );
			continue;
		}

		displayModeCount = SDL_GetNumDisplayModes( displayIndex );
		for ( Uint32 modeIndex = 0; modeIndex < displayModeCount; modeIndex++)
		{
			SDL_DisplayMode mode = {};
			btDiplayModes_s displayMode;

			if(SDL_GetDisplayMode( displayIndex, modeIndex, &mode ) != 0)
			{
				common->Warning( " Can't get Display %i (%s) mode i%\n ", displayIndex, displayInfo.name.c_str(), modeIndex );
				continue;
			}

			displayMode.w = mode.w;
			displayMode.h = mode.h;

			displayInfo.modes.Append( displayMode );
		}

		renderContext.displays.Append( displayInfo );
	}
}