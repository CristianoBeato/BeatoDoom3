
#ifndef __GL_IMP_HPP__
#define __GL_IMP_HPP__

/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

typedef struct 
{
	bool		fullScreen;
	bool		stereo;
	uint32_t    multiSamples;
	uint32_t	width;
	uint32_t    height;
	uint32_t    displayHz;
} glimpParms_t;

extern bool		GLimp_Init( const glimpParms_t &parms );

// If the desired mode can't be set satisfactorily, false will be returned.
// The renderer will then reset the glimpParms to "safe mode" of 640x480
// fullscreen and try again.  If that also fails, the error will be fatal.
extern bool		GLimp_SetScreenParms( const glimpParms_t &parms );

// will set up gl up with the new parms

extern void		GLimp_Shutdown( void );
// Destroys the rendering context, closes the window, resets the resolution,
// and resets the gamma ramps.

extern void		GLimp_SwapBuffers( void );
// Calls the system specific swapbuffers routine, and may also perform
// other system specific cvar checks that happen every frame.
// This will not be called if 'r_drawBuffer GL_FRONT'

extern void		GLimp_SetGamma( unsigned short red[256], unsigned short green[256], unsigned short blue[256] );
// Sets the hardware gamma ramps for gamma and brightness adjustment.
// These are now taken as 16 bit values, so we can take full advantage
// of dacs with >8 bits of precision

extern bool		GLimp_SpawnRenderThread( int (*function)( void* ) );
// Returns false if the system only has a single processor

extern void *	GLimp_BackEndSleep( void );
extern void		GLimp_FrontEndSleep( void );
extern void		GLimp_WakeBackEnd( void *data );
extern void     GLimp_WakeFrontEnd( void );
// these functions implement the dual processor syncronization

extern void		GLimp_ActivateContext( void );
extern void		GLimp_DeactivateContext( void );
// These are used for managing SMP handoffs of the OpenGL context
// between threads, and as a performance tunining aid.  Setting
// 'r_skipRenderContext 1' will call GLimp_DeactivateContext() before
// the 3D rendering code, and GLimp_ActivateContext() afterwards.  On
// most OpenGL implementations, this will result in all OpenGL calls
// being immediate returns, which lets us guage how much time is
// being spent inside OpenGL.

void		GLimp_EnableLogging( bool enable );

typedef void (*GLExtension_t)(void);
extern GLExtension_t GLimp_ExtensionPointer( const char *name );

#endif ///!__GL_IMP_HPP__