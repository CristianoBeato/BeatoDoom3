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

#include <iostream>
#include <sstream>
#include <SDL3/SDL_video.h>
#include <GL/glext.h>
#include <GL/gl.h>

#include "renderer/renderer_common.h"
#include "qgl.h"

#include "glContext.h"
#include "glCommandQueue.h"

/*
=================
R_CheckExtension
=================
*/
static bool R_CheckExtension( const char *name )
{
	if (!SDL_GL_ExtensionSupported( name ))
	{
		common->Printf( "X..%s not found\n", name );
		return false;
	}

	common->Printf( "...using %s\n", name );
	return true;
}

// OpenGL debug output messages
static void APIENTRY R_DebugOutput( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam );

void crGLContext::StartUp(void)
{
    m_queueLock = new crMutex();
    InitLibrary();
    LoadOpenGLFunctions();
    CreateContext();

	CheckPortableExtensions();

	// 
#if defined( _DEBUG ) || defined( DEBUG )
	InitOpenGLDebugOutput();
#endif

    StartWorkerThread("OpenGLDipatch");
}

void crGLContext::ShutDown(void)
{
    StopThread( true );
    DestroyContext();
    ShutDownLibrary();
    if( m_queueLock != nullptr )
    {
        delete m_queueLock;
        m_queueLock = nullptr;
    }
}

void crGLContext::SwapBuffer(void)
{
    // check swap interval
	if (r_swapInterval.IsModified())
	{
		r_swapInterval.ClearModified();
		SDL_GL_SetSwapInterval( r_swapInterval.GetInteger() );
	}

	SDL_GL_SwapWindow( m_window );
}

void crGLContext::SubmitQueue(  crAutoPointer<glCommandQueue> queue )
{
    crScopeLock lock( m_queueLock );
}

void crGLContext::InitLibrary(void)
{
    const char* libname = r_glDriver.GetString();  

    // try load the custom lib
    if( !SDL_GL_LoadLibrary( libname ) )
    {
        // if custom fail, try load defalt 
        if( !SDL_GL_LoadLibrary( nullptr ) )
            throw idException();
    }
}

void crGLContext::ShutDownLibrary(void)
{
    SDL_GL_UnloadLibrary();
}

void crGLContext::CreateContext(void)
{
	m_window = static_cast<SDL_Window*>( Sys_videoWindowHandler() );
	assert( m_window != nullptr );
	
    // defalt parameters
	int colorbits = 8;
	int alphabits = 8;
	int depthbits = 24;
	int stencilbits = 8;
	Uint32 samplesCount = 4;

	auto gWindow = static_cast<SDL_Window*>( Sys_videoWindowHandler() );

	common->Printf( "Initializing OpenGL subsystem\n" );

	// OpenGL 4.5 core context
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 6 );
    // Use OpenGL
#if 1
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
	// SDL_GL_SetAttribute( SDL_GL_STEREO, parms.stereo ? 1 : 0 );

	// Set the buffers size
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, colorbits );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, alphabits );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, depthbits );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencilbits );

	//
	m_renderContext = SDL_GL_CreateContext( gWindow );
	if (m_renderContext == nullptr )
        throw idException( SDL_GetError() );
}

void crGLContext::DestroyContext(void)
{
    if ( m_commandContext != nullptr )
    {
        SDL_GL_DestroyContext( m_commandContext );
        m_commandContext = nullptr;
    }

    if ( m_renderContext != nullptr )
    {
        SDL_GL_DestroyContext( m_renderContext );
        m_renderContext = nullptr;
    }
}

void crGLContext::CheckPortableExtensions(void)
{
//  m_features.textureLODBiasAvailable = true;//R_CheckExtension(); 
//  m_features.textureEnvAddAvailable = R_CheckExtension("GL_ARB_texture_env_add");
//  m_features.textureEnvCombineAvailable = R_CheckExtension("GL_ARB_texture_env_combine");
//  m_features.envDot3Available = R_CheckExtension("GL_ARB_texture_env_dot3"); 
//  m_features.sharedTexturePaletteAvailable = R_CheckExtension("GL_EXT_shared_texture_palette");
//  m_features.texture3DAvailable = R_CheckExtension("GL_EXT_texture3D");
    m_features.multitextureAvailable = R_CheckExtension( "GL_ARB_multitexture" );
    m_features.textureCompressionAvailable = R_CheckExtension( "GL_ARB_texture_compression_bptc" );
    m_features.anisotropicAvailable = R_CheckExtension( "GL_ARB_texture_filter_anisotropic" );
    m_features.cubeMapAvailable = R_CheckExtension( "GL_ARB_texture_cube_map" );
    m_features.cubeMapArrayAvailable = R_CheckExtension( "GL_ARB_texture_cube_map_array" );
    m_features.vertexBufferObjectAvailable = R_CheckExtension( "GL_ARB_vertex_buffer_object" ); 
    m_features.twoSidedStencilAvailable = R_CheckExtension( "GL_EXT_stencil_two_side" );
    m_features.textureNonPowerOfTwoAvailable = R_CheckExtension( "GL_ARB_texture_non_power_of_two" );
    m_features.depthClamp = R_CheckExtension( "GL_ARB_depth_clamp" );
    m_features.depthBoundsTestAvailable = R_CheckExtension( "GL_EXT_depth_bounds_test" );
    m_features.directStateAccessAvailable = R_CheckExtension( "GL_ARB_direct_state_access" );
    m_features.bufferStorageAvailable = R_CheckExtension( "GL_ARB_buffer_storage" );
    m_features.shaderStorageBufferObjectAvailable = R_CheckExtension( "GL_ARB_shader_storage_buffer_object" );
    m_features.mapBufferRangeAvailable = R_CheckExtension( "GL_ARB_map_buffer_range" );
    m_features.multiBindAvailable = R_CheckExtension( "GL_ARB_multi_bind" ); 
    m_features.drawElementsBaseVertexAvailable = R_CheckExtension( "GL_ARB_draw_elements_base_vertex" );
    m_features.drawIndirectAvailable = R_CheckExtension( "GL_ARB_draw_indirect" ); 
    m_features.textureStorageAvailable = R_CheckExtension( "GL_ARB_texture_storage" ); 
    m_features.textureStorageMultisampleAvailable = R_CheckExtension( "GL_ARB_texture_storage_multisample" ); 
    m_features.samplerObjectsAvailable = R_CheckExtension( "GL_ARB_sampler_objects" ); 
    m_features.separateShaderObjectsAvailable = R_CheckExtension("GL_ARB_separate_shader_objects");
    m_features.syncAvailable = R_CheckExtension("GL_ARB_sync"); 
    m_features.viewportArrayAvailable = R_CheckExtension("GL_ARB_viewport_array"); 
    m_features.debugOutputAvailable = R_CheckExtension("GL_ARB_debug_output");
}

void crGLContext::CheckProperties(void)
{
    GLfloat farray[2];

    glGetFloatv( GL_MAX_TEXTURE_LOD_BIAS, &m_properties.maxTextureLodBias );
    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY, &m_properties.maxTextureAnisostropy );
    glGetFloatv( GL_MAX_CLIP_DISTANCES, &m_properties.maxClipDistances );

    //
    glGetFloatv( GL_MAX_VIEWPORT_DIMS, farray );
    m_properties.maxViewportWidth = farray[0];
    m_properties.maxViewportheight = farray[1];

    glGetIntegerv( GL_MIN_MAP_BUFFER_ALIGNMENT, &m_properties.minMapBufferAlignment );
    glGetIntegerv( GL_MAJOR_VERSION, &m_properties.majorVersion );
    glGetIntegerv( GL_MINOR_VERSION, &m_properties.minorVersion );
    glGetIntegerv( GL_MAX_VERTEX_ATTRIB_BINDINGS, &m_properties.maxVetexAttribBindings );
    glGetIntegerv( GL_MAX_SAMPLES, &m_properties.maxSamples );
    glGetIntegerv( GL_MAX_IMAGE_SAMPLES, &m_properties.maxImageSamples );
    glGetIntegerv( GL_MAX_FRAMEBUFFER_SAMPLES, &m_properties.maxFrameBufferSamples );
    glGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS, &m_properties.maxArrayTextureLayers );
    glGetIntegerv( GL_MAX_FRAMEBUFFER_LAYERS, &m_properties.maxFrameBufferLayers );
    glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &m_properties.maxColorAttachments );
    glGetIntegerv( GL_MAX_DRAW_BUFFERS, &m_properties.maxDrawBuffers );
    glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &m_properties.maxRenderBufferSize );
    glGetIntegerv( GL_MAX_VIEWPORTS, &m_properties.maxViewports );
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &m_properties.maxTextureSize );
    glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &m_properties.maxUniformBlockSize );
    glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &m_properties.maxUniformBufferBindings );
    glGetIntegerv( GL_MAX_UNIFORM_LOCATIONS, &m_properties.maxUniformBufferLocations );
    glGetIntegerv( GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &m_properties.maxComputeSahderStorageBlocks );
    glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE, &m_properties.maxCubeMapTextureSize );
    glGetIntegerv( GL_MAX_COMBINED_IMAGE_UNIFORMS, &m_properties.maxCombinedImageUniforms );
    glGetIntegerv( GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &m_properties.maxCombinedVertexUniformComponents );
    glGetIntegerv( GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &m_properties.maxCombinedFragmentUniformComponents );
    glGetIntegerv( GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &m_properties.maxCombinedComputeUniformComponents );
    glGetIntegerv( GL_MAX_COLOR_TEXTURE_SAMPLES, &m_properties.maxColorTextureSamples );
    glGetIntegerv( GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &m_properties.maxAtomicCounterBufferBinding );
    glGetIntegerv( GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, &m_properties.maxAtomicCounterBufferSize );
    glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &m_properties.max3dTextureSize );
    glGetIntegerv( GL_MAX_COMBINED_UNIFORM_BLOCKS, &m_properties.maxCombinedUniformBlocks );
    glGetIntegerv( GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &m_properties.maxFragmentShaderStorageBlocks );
    glGetIntegerv( GL_MAX_DEPTH_TEXTURE_SAMPLES, &m_properties.maxDepthTextureSamples );
    glGetIntegerv( GL_MAX_TEXTURE_BUFFER_SIZE, &m_properties.maxTextureBufferSize );
    glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &m_properties.maxTextureImageUnits );

}

void crGLContext::InitDebugOutput(void)
{
    if ( !m_features.debugOutputAvailable )
        return;

        GLint flags = 0; 
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable( GL_DEBUG_OUTPUT );
            glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );    // to block the current thread
    
            // TODO: add a debug level Cvar
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            
            glDebugMessageCallback( R_DebugOutput, nullptr );
        }
}

void APIENTRY R_DebugOutput( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message, const void *userParam )
{
    std::stringstream glLog;

    // ignore non-significant error/warning codes
    if( id == 131169 || id == 131185 || id == 131218 || id == 131204 )
        return;

    glLog << "---------------" << std::endl;
    glLog << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        glLog << "Source: API" << std::endl;
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        glLog << "Source: Window System" << std::endl;
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        glLog << "Source: Shader Compiler" << std::endl;
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        glLog << "Source: Third Party" << std::endl;
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        glLog << "Source: Application" << std::endl;
        break;
    case GL_DEBUG_SOURCE_OTHER:
        glLog << "Source: Other" << std::endl;
        break;
    } 

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        glLog << "Type: Error" << std::endl; 
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        glLog << "Type: Deprecated Behaviour" << std::endl; 
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        glLog << "Type: Undefined Behaviour" << std::endl;
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        glLog << "Type: Portability" << std::endl; 
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        glLog << "Type: Performance" << std::endl;
        break;
    case GL_DEBUG_TYPE_MARKER:
        glLog << "Type: Marker" << std::endl;
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        glLog << "Type: Push Group" << std::endl;
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        glLog << "Type: Pop Group" << std::endl;
        break;
    case GL_DEBUG_TYPE_OTHER:
        glLog << "Type: Other" << std::endl;
        break;
    };

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        glLog << "Severity: high" << std::endl;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        glLog << "Severity: medium" << std::endl;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        glLog << "Severity: low" << std::endl;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        glLog << "Severity: notification"<< std::endl; 
        break;
    };

    std::cerr << glLog.rdbuf();
}

/*
=================
R_VidRestart_f
=================
*/
void R_VidRestart_f( const idCmdArgs &args ) 
{
	int	err;

	// if OpenGL isn't started, do nothing
	if ( !glConfig.isInitialized ) {
		return;
	}

	bool full = true;
	bool forceWindow = false;
	for ( int i = 1 ; i < args.Argc() ; i++ ) {
		if ( idStr::Icmp( args.Argv( i ), "partial" ) == 0 ) {
			full = false;
			continue;
		}
		if ( idStr::Icmp( args.Argv( i ), "windowed" ) == 0 ) {
			forceWindow = true;
			continue;
		}
	}

	// this could take a while, so give them the cursor back ASAP
	Sys_GrabMouseCursor( false );

	// dump ambient caches
	renderModelManager->FreeModelVertexCaches();

	// free any current world interaction surfaces and vertex caches
	tr.frontend->FreeDerivedData();

	// make sure the defered frees are actually freed
	tr.drawCommand->ToggleSmpFrame();
	tr.drawCommand->ToggleSmpFrame();

	// free the vertex caches so they will be regenerated again
	vertexCache.PurgeAll();

	// sound and input are tied to the window we are about to destroy

	if ( full ) 
	{
		// free all of our texture numbers
		soundSystem->ShutdownHW();
		Sys_ShutdownInput();
		globalImages->PurgeAllImages();
	
		// free the context 
#if CR_USE_VULKAN
	Sys_ShutDownVulkanDevice();
#else
	Sys_ShutDownOpenGLContext();
#endif
		
		glConfig.isInitialized = false;

		// create the new context and vertex cache
		bool latch = cvarSystem->GetCVarBool( "r_fullscreen" );
		if ( forceWindow ) {
			cvarSystem->SetCVarBool( "r_fullscreen", false );
		}
		R_InitOpenGL();
		cvarSystem->SetCVarBool( "r_fullscreen", latch );

		// regenerate all images
		globalImages->ReloadAllImages();
	} 
	else 
	{
		glimpParms_t	parms;
		parms.width = glConfig.vidWidth;
		parms.height = glConfig.vidHeight;
		parms.fullScreen = ( forceWindow ) ? false : r_fullscreen.GetBool();
		parms.displayHz = r_displayRefresh.GetInteger();
		parms.multiSamples = r_multiSamples.GetInteger();
		parms.stereo = false;
		// TODO: update window 
//		GLimp_SetScreenParms( parms );
	}

	// make sure the regeneration doesn't use anything no longer valid
	int viewCount = tr.frontend->GetViewCount() + 1;
	tr.frontend->SetViewCount( viewCount );
	tr.frontend->SetViewDef( viewDefptr_t() );

	// regenerate all necessary interactions
	R_RegenerateWorld_f( idCmdArgs() );

	// check for problems
#if !CR_USE_VULKAN
	err = glGetError();
	if ( err != GL_NO_ERROR ) 
	{
		common->Printf( "glGetError() = 0x%x\n", err );
	}
#endif

	// start sound playing again
	soundSystem->SetMute( false );
}


/*
==================
R_InitOpenGL

This function is responsible for initializing a valid OpenGL subsystem
for rendering.  This is done by calling the system specific GLimp_Init,
which gives us a working OGL subsystem, then setting all necessary openGL
state, including images, vertex programs, and display lists.

Changes to the vertex cache size or smp state require a vid_restart.

If glConfig.isInitialized is false, no rendering can take place, but
all renderSystem functions will still operate properly, notably the material
and model information functions.
==================
*/
void R_InitOpenGL( void ) 
{
	GLint			temp;
#if CR_USE_VULKAN
	vkParms_t		parms;
#else
	glimpParms_t	parms;
#endif
	int				i;

	common->Printf( "----- R_InitOpenGL -----\n" );

	if ( glConfig.isInitialized ) {
		common->FatalError( "R_InitOpenGL called while active" );
	}

	// in case we had an error while doing a tiled rendering
	tr.viewportOffset[0] = 0;
	tr.viewportOffset[1] = 0;

	//
	// initialize OS specific portions of the renderSystem
	//
	for ( i = 0 ; i < 2 ; i++ ) 
	{
		// set the parameters we are trying
		R_GetModeInfo( &glConfig.vidWidth, &glConfig.vidHeight, r_mode.GetInteger() );

		parms.width = glConfig.vidWidth;
		parms.height = glConfig.vidHeight;
		parms.fullScreen = r_fullscreen.GetBool();
		parms.displayHz = r_displayRefresh.GetInteger();
		parms.multiSamples = r_multiSamples.GetInteger();
		parms.stereo = false;

#if 	CR_USE_VULKAN
		if ( Sys_InitVulkanDevice( parms ) ) 
			break; // it worked
#else
		if( Sys_InitOpenGLContext( parms ) )
			break;
#endif

		if ( i == 1 ) 
			common->FatalError( "Unable to initialize OpenGL" );

		// if we failed, set everything back to "safe mode"
		// and try again
		r_mode.SetInteger( 3 );
		r_fullscreen.SetInteger( 1 );
		r_displayRefresh.SetInteger( 0 );
		r_multiSamples.SetInteger( 0 );
	}

	// get our config strings
	glConfig.vendor_string = (const char *)glGetString(GL_VENDOR);
	glConfig.renderer_string = (const char *)glGetString(GL_RENDERER);
	glConfig.version_string = (const char *)glGetString(GL_VERSION);
	glConfig.extensions_string = (const char *)glGetString(GL_EXTENSIONS);

	// OpenGL driver constants
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &temp );
	glConfig.maxTextureSize = temp;

	// stubbed or broken drivers may have reported 0...
	if ( glConfig.maxTextureSize <= 0 ) 
		glConfig.maxTextureSize = 256;

	glConfig.isInitialized = true;
//BEATO Begin: extencion cehck is done at context creation
	
	cmdSystem->AddCommand( "reloadARBprograms", R_ReloadARBPrograms_f, CMD_FL_RENDERER, "reloads ARB programs" );
	R_ReloadARBPrograms_f( idCmdArgs() );


// BEATO End
}
