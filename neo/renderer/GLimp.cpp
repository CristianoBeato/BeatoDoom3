
#include "idlib/precompiled.h"
#include "tr_local.h"
#include "GLimp.hpp"

#include <SDL3/SDL_video.h>

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

    /// try create the render context
    GLimp.context = SDL_GL_CreateContext( GLimp.window );

    if( !GLimp_SpawnRenderThread( RenderThreadEntryPoint ) )
        return false;

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