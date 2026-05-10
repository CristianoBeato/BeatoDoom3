
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
extern GLExtension_t	GLimp_ExtensionPointer( const char *name );
extern bool 			GLimp_CheckExtension( const char *name );
extern void				GLimp_LoadExtension( void );

extern void R_CheckPortableExtensions( void );

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

ID_INLINE const GLuint glGetBuferBinding( const GLenum target )
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

ID_INLINE const GLuint glGetTextureBinding( const GLenum target )
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

#endif ///!__GL_IMP_HPP__