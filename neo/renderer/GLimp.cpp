
#include "idlib/precompiled.h"
#include "tr_local.h"
#include "GLimp.hpp"

#include <SDL3/SDL_video.h>

#define DEBUG_CONTEX_VERBOSE 0
#define DEBUG_CONTEX_IGNORE_NON_SIGNIFICANT 1
#define DEBUG_CONTEX_ERROR_ONLY 2

static idCVar r_inhibitFragmentProgram( "r_inhibitFragmentProgram", "0", CVAR_RENDERER | CVAR_BOOL, "ignore the fragment program extension" );

static struct GLimp_t
{
    int32_t         writeIndex;
    int32_t         readIndex;
    SDL_GLContext   context;
    SDL_Window*     window;
    SDL_Thread*     renderer;
    btMutex*        renderLock;
    btSemaphore*    renderCmd;
    btSemaphore*    renderDone;
    void*           frameData[SMP_FRAMES];
} GLimp;

static int RenderThreadEntryPoint( void* data )
{
    renderSystem->RenderThread();
    return 0;
}


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

static void APIENTRY DebuOutput( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
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

/*
=================
InitDebugOutput
=================
*/
static void InitDebugOutput( int inflags )
{
	// Disable if unsuported 
	if (!glConfig.debugOutputAvailable)
		return;

	GLint flags = glGetInteger( GL_CONTEXT_FLAGS );
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// Habilita a saida de depura��o do contexto 
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

bool    GLimp_Init( const glimpParms_t &parms )
{
    ///
    /// Utilize Most moder OpenGL 4.5 
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

    ///
    /// Context profile We current need compatibilty, for fixed pipelide debug properties
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

    ///
    /// Enable OpenGL Debug ouput extension
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );

    ///
    /// Enable MSAA multisampling
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, parms.multiSamples ? 1 : 0 );
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, parms.multiSamples );

    ///
    /// Enable stereo 3D ( current not used )
    SDL_GL_SetAttribute( SDL_GL_STEREO, parms.stereo ? 1 : 0 );

    ///
    /// Enable double bufering
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    ///
    /// Out buffer color depth ( GL_RGBA8 )
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    ///
    /// Out Depth Stencil buffer depth ( GL_DEPTH_STENCIL_24_8 ) 
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    SDL_GL_SetSwapInterval( r_swapInterval.GetInteger() );

    /// try create the render context
    GLimp.context = SDL_GL_CreateContext( GLimp.window );

    if( !GLimp_SpawnRenderThread( RenderThreadEntryPoint ) )
        return false;

	InitDebugOutput( 1 );

    return GLimp.context != nullptr;
}

bool    GLimp_SetScreenParms( glimpParms_t parms )
{
}

void    GLimp_Shutdown( void )
{
    if( GLimp.context != nullptr )
    {
        SDL_GL_DestroyContext( GLimp.context );
        GLimp.context = nullptr;
    }
}

void    GLimp_SwapBuffers( void )
{
    if( GLimp.window )
    {   
        if (r_swapInterval.IsModified())
    	{
	    	r_swapInterval.ClearModified();
		    SDL_GL_SetSwapInterval( r_swapInterval.GetInteger() );
	    }

        if( !SDL_GL_SwapWindow( GLimp.window ) )
            idLib::Error( SDL_GetError() );
    }
    else
    {
        idLib::Error( "No Render Window Set");
    }
}

void    GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] )
{

}

bool    GLimp_SpawnRenderThread( int (*function)( void* ) )
{
    GLimp.renderLock = new btMutex();
    GLimp.renderCmd = new btSemaphore( 0 );
    GLimp.renderDone = new btSemaphore( SMP_FRAMES );
    GLimp.renderer = SDL_CreateThread( function, "RenderThread", nullptr );
}

void*   GLimp_BackEndSleep( void )
{
    /// Wait for a new frame
    GLimp.renderCmd->Wait();

    btScopeLock lock( GLimp.renderLock );
    void* frameData = GLimp.frameData[GLimp.writeIndex];
    GLimp.writeIndex = ( GLimp.writeIndex + 1 ) % SMP_FRAMES;
    return frameData;
}

void    GLimp_FrontEndSleep( void )
{
    /// wait for available frames
    GLimp.renderDone->Wait();
}

void	GLimp_WakeBackEnd( void *data )
{
    btScopeLock lock( GLimp.renderLock );
    GLimp.frameData[GLimp.writeIndex] = data;
    GLimp.writeIndex = ( GLimp.writeIndex + 1 ) % SMP_FRAMES;

    /// Wake render thread
    GLimp.renderCmd->Trigger();
}

void    GLimp_WakeFrontEnd(void)
{
    GLimp.renderDone->Trigger();
}

void    GLimp_ActivateContext( void )
{
    if ( GLimp.context != nullptr && GLimp.window != nullptr )
    {
        SDL_GL_MakeCurrent( GLimp.window, GLimp.context );
    }
}

void    GLimp_DeactivateContext( void )
{
    if ( GLimp.context != nullptr && GLimp.window != nullptr )
    {
        SDL_GL_MakeCurrent( GLimp.window, nullptr );
    }
}

void GLimp_EnableLogging( bool enable )
{
}

GLExtension_t GLimp_ExtensionPointer( const char *name )
{
    return reinterpret_cast<GLExtension_t>( SDL_GL_GetProcAddress( name ) );
}

/*
=================
GLimp_CheckExtension
=================
*/
bool GLimp_CheckExtension( const char *name )
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
==================
R_CheckPortableExtensions

==================
*/
void R_CheckPortableExtensions( void )
{
	glConfig.glVersion = atof( glConfig.version_string );

	// GL_ARB_multitexture
	glConfig.multitextureAvailable = GLimp_CheckExtension( "GL_ARB_multitexture" );
	if (glConfig.multitextureAvailable)
	{
		glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&glConfig.maxTextureUnits );
		if (glConfig.maxTextureUnits > MAX_MULTITEXTURE_UNITS)
			glConfig.maxTextureUnits = MAX_MULTITEXTURE_UNITS;
		
		if (glConfig.maxTextureUnits < 2)
			glConfig.multitextureAvailable = false;	// shouldn't ever happen

		glGetIntegerv( GL_MAX_TEXTURE_COORDS_ARB, (GLint *)&glConfig.maxTextureCoords );
		glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, (GLint *)&glConfig.maxTextureImageUnits );
	}

	// GL_ARB_texture_env_combine
	glConfig.textureEnvCombineAvailable = GLimp_CheckExtension( "GL_ARB_texture_env_combine" );

	// GL_ARB_texture_cube_map
	glConfig.cubeMapAvailable = GLimp_CheckExtension( "GL_ARB_texture_cube_map" );

	// GL_ARB_texture_env_dot3
	glConfig.envDot3Available = GLimp_CheckExtension( "GL_ARB_texture_env_dot3" );

	// GL_ARB_texture_env_add
	glConfig.textureEnvAddAvailable = GLimp_CheckExtension( "GL_ARB_texture_env_add" );

	// GL_ARB_texture_non_power_of_two
	glConfig.textureNonPowerOfTwoAvailable = GLimp_CheckExtension( "GL_ARB_texture_non_power_of_two" );

	// GL_ARB_texture_compression + GL_S3_s3tc
	// DRI drivers may have GL_ARB_texture_compression but no GL_EXT_texture_compression_s3tc
	if (GLimp_CheckExtension( "GL_ARB_texture_compression" ) && GLimp_CheckExtension( "GL_EXT_texture_compression_s3tc" ))
		glConfig.textureCompressionAvailable = true;
	else
		glConfig.textureCompressionAvailable = false;

	// GL_EXT_texture_filter_anisotropic
	glConfig.anisotropicAvailable = GLimp_CheckExtension( "GL_EXT_texture_filter_anisotropic" );
	if (glConfig.anisotropicAvailable)
	{
		glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY, &glConfig.maxTextureAnisotropy );
		common->Printf( "   maxTextureAnisotropy: %f\n", glConfig.maxTextureAnisotropy );
	}
	else
	{
		glConfig.maxTextureAnisotropy = 1;
	}

	// GL_EXT_texture_lod_bias
	// The actual extension is broken as specificed, storing the state in the texture unit instead
	// of the texture object.  The behavior in GL 1.4 is the behavior we use.
	if (glConfig.glVersion >= 1.4 || GLimp_CheckExtension( "GL_EXT_texture_lod" ))
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
	glConfig.sharedTexturePaletteAvailable = GLimp_CheckExtension( "GL_EXT_shared_texture_palette" );
	
	// GL_EXT_texture3D (not currently used for anything)
	glConfig.texture3DAvailable = GLimp_CheckExtension( "GL_EXT_texture3D" );
	
	// EXT_stencil_wrap
	// This isn't very important, but some pathological case might cause a clamp error and give a shadow bug.
	// Nvidia also believes that future hardware may be able to run faster with this enabled to avoid the
	// serialization of clamping.
	//if (GLimp_CheckExtension( "GL_EXT_stencil_wrap" ))
	//{
		tr.stencilIncr = GL_INCR_WRAP;
		tr.stencilDecr = GL_DECR_WRAP;
	//}
	//else
	//{
	//	tr.stencilIncr = GL_INCR;
	//	tr.stencilDecr = GL_DECR;
	//}

	// ARB_vertex_buffer_object
	glConfig.ARBVertexBufferObjectAvailable = GLimp_CheckExtension( "GL_ARB_vertex_buffer_object" );
	
	// ARB_vertex_program
	glConfig.ARBVertexProgramAvailable = GLimp_CheckExtension( "GL_ARB_vertex_program" );
	
	// ARB_fragment_program
	if (r_inhibitFragmentProgram.GetBool())
		glConfig.ARBFragmentProgramAvailable = false;
	else
		glConfig.ARBFragmentProgramAvailable = GLimp_CheckExtension( "GL_ARB_fragment_program" );

	// check for minimum set
	if (!glConfig.multitextureAvailable || 
		!glConfig.textureEnvCombineAvailable || 
		!glConfig.cubeMapAvailable || 
		!glConfig.envDot3Available)
	{
		common->Error( common->GetLanguageDict()->GetString( "#str_06780" ) );
	}

	// GL_EXT_depth_bounds_test
	glConfig.depthBoundsTestAvailable = GLimp_CheckExtension( "EXT_depth_bounds_test" );

	// GL_ARB_debug_output
	glConfig.debugOutputAvailable = GLimp_CheckExtension( "ARB_debug_output" );
}
