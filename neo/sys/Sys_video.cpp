
#include "idlib/precompiled.h"
#include "sys_public.h"

#include <SDL3/SDL_video.h>

static struct video_t
{
    uint32_t    width;
    uint32_t    height;
    SDL_Window* window;
} video;

static bool Sys_CreateWindow( void )
{
    video.window = SDL_CreateWindow( GAME_NAME, 640, 480, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL );
    if( video.window == nullptr )
    {
        idLib::Error( SDL_GetError() );
        return false;
    }

    return true;
}

static void Sys_DestroyWindow( void )
{
	if (video.window != nullptr )
    {
        SDL_DestroyWindow( video.window );
        video.window = nullptr;
    }
}

bool Sys_InitVideo( void )
{
    SDL_GL_LoadLibrary( nullptr );

    Sys_DestroyWindow();
    Sys_CreateWindow();
}

void Sys_ShutdownVideo(void)
{
    Sys_DestroyWindow();
}

/*
==============
Sys_ShowWindow
==============
*/
void Sys_ShowWindow( const bool show )
{
	assert( video.window != nullptr );
	if (show)
        SDL_ShowWindow( video.window );
	else
        SDL_HideWindow( video.window );
}

/*
==============
Sys_IsWindowVisible
==============
*/
bool Sys_IsWindowVisible( void )
{
    assert( video.window != nullptr );
    auto flags = SDL_GetWindowFlags( video.window );
	return !( flags & SDL_WINDOW_HIDDEN );
}
