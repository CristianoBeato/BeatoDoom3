#include "idlib/precompiled.h"
#include "qgl.h"

#include <SDL2/SDL_video.h>

//===========================================================================

PFNGLGETERRORPROC					    glGetError = nullptr;
PFNGLGETSTRINGPROC					    glGetString = nullptr;
PFNGLGETFLOATVPROC					    glGetFloatv = nullptr;
PFNGLGETINTEGERVPROC					glGetIntegerv = nullptr;
PFNGLENABLEPROC						    glEnable = nullptr;
PFNGLDISABLEPROC						glDisable = nullptr;
PFNGLISENABLEDPROC                      glIsEnabled = nullptr;
PFNGLCLEARPROC						    glClear = nullptr;
PFNGLFLUSHPROC						    glFlush = nullptr;
PFNGLFINISHPROC						    glFinish = nullptr;
PFNGLVIEWPORTPROC					    glViewport = nullptr;

PFNGLCLEARCOLORPROC					    glClearColor = nullptr;
PFNGLCOLORMASKPROC					    glColorMask = nullptr;

// blend
PFNGLBLENDFUNCPROC					    glBlendFunc = nullptr;

// depth
PFNGLCLEARDEPTHPROC					    glClearDepth = nullptr;
PFNGLDEPTHMASKPROC					    glDepthMask = nullptr;
PFNGLDEPTHFUNCPROC					    glDepthFunc = nullptr;
PFNGLDEPTHRANGEPROC					    glDepthRange = nullptr;

// stencil
PFNGLSCISSORPROC                        glScissor = nullptr;
PFNGLSTENCILOPPROC					    glStencilOp = nullptr;
PFNGLSTENCILFUNCPROC					glStencilFunc = nullptr;
PFNGLSTENCILMASKPROC					glStencilMask = nullptr;
PFNGLCLEARSTENCILPROC				    glClearStencil = nullptr;
PFNGLSTENCILOPSEPARATEPROC			    glStencilOpSeparate = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC			glStencilFuncSeparate = nullptr;

// poligon 
PFNGLCULLFACEPROC					    glCullFace = nullptr;
PFNGLPOLYGONMODEPROC					glPolygonMode = nullptr;
PFNGLPOLYGONOFFSETPROC				    glPolygonOffset = nullptr;
PFNGLLINEWIDTHPROC					    glLineWidth = nullptr;
PFNGLPOINTSIZEPROC					    glPointSize = nullptr;

// draw buffers 
PFNGLDRAWBUFFERPROC					    glDrawBuffer = nullptr;
PFNGLREADBUFFERPROC					    glReadBuffer = nullptr;
PFNGLREADPIXELSPROC					    glReadPixels = nullptr;
PFNGLPIXELSTOREIPROC					glPixelStorei = nullptr;

// ARB_vertex_buffer_object
PFNGLBINDBUFFERPROC					    glBindBuffer = nullptr;
PFNGLDELETEBUFFERSPROC	 			    glDeleteBuffers = nullptr;
PFNGLGENBUFFERSPROC	 				    glGenBuffers = nullptr;
PFNGLISBUFFERPROC	 				    glIsBuffer = nullptr;
PFNGLBUFFERDATAPROC	 				    glBufferData = nullptr;
PFNGLBUFFERSUBDATAPROC	 			    glBufferSubData = nullptr;
PFNGLGETBUFFERSUBDATAPROC	 		    glGetBufferSubData = nullptr;
PFNGLMAPBUFFERPROC	 				    glMapBuffer = nullptr;
PFNGLUNMAPBUFFERPROC 				    glUnmapBuffer = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC 		    glGetBufferParameteriv = nullptr;
PFNGLGETBUFFERPOINTERVPROC 			    glGetBufferPointerv = nullptr;
PFNGLCOPYBUFFERSUBDATAPROC			    glCopyBufferSubData = nullptr;

// GL_ARB_direct_state_access GL_ARB_buffer_storage  GL_ARB_uniform_buffer_object  GL_ARB_map_buffer_range
PFNGLCREATEBUFFERSPROC				    glCreateBuffers = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC			    glNamedBufferStorage = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC			    glNamedBufferSubData = nullptr;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC		    glCopyNamedBufferSubData = nullptr;
PFNGLCLEARNAMEDBUFFERDATAPROC		    glClearNamedBufferData = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC			glMapNamedBufferRange = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC			    glUnmapNamedBuffer = nullptr;

// Draw Command
PFNGLDRAWELEMENTSPROC				    glDrawElements = nullptr;

// Textures
PFNGLACTIVETEXTUREPROC				    glActiveTexture = nullptr;
PFNGLGENTEXTURESPROC					glGenTextures = nullptr;
PFNGLDELETETEXTURESPROC				    glDeleteTextures = nullptr;
PFNGLBINDTEXTUREPROC					glBindTexture = nullptr;
PFNGLTEXIMAGE2DPROC					    glTexImage2D = nullptr;
PFNGLTEXIMAGE3DPROC					    glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE2DPROC				    glTexSubImage2D = nullptr;
PFNGLTEXSUBIMAGE3DPROC				    glTexSubImage3D = nullptr;
PFNGLTEXPARAMETERIPROC				    glTexParameteri = nullptr;
PFNGLTEXPARAMETERFPROC				    glTexParameterf = nullptr;
PFNGLTEXPARAMETERFVPROC				    glTexParameterfv = nullptr;
PFNGLGETTEXIMAGEPROC					glGetTexImage = nullptr;
PFNGLCOPYTEXIMAGE2DPROC				    glCopyTexImage2D = nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC			    glCopyTexSubImage2D = nullptr;
PFNGLCOPYTEXSUBIMAGE3DPROC			    glCopyTexSubImage3D = nullptr;

// ARB_texture_compression
PFNGLCOMPRESSEDTEXIMAGE2DPROC		    glCompressedTexImage2D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC		    glCompressedTexImage3D = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC		    glGetCompressedTexImage = nullptr;

// GL_EXT_depth_bounds_test
PFNGLDEPTHBOUNDSEXTPROC                 glDepthBoundsEXT = nullptr;

// EXT_stencil_two_side
PFNGLACTIVESTENCILFACEEXTPROC		    glActiveStencilFaceEXT = nullptr;

// GL_ARB_debug_output
static PFNGLDEBUGMESSAGECONTROLPROC     glDebugMessageControl = nullptr;
static PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback = nullptr;


//===========================================================================
// DEPRECATE TO REMOVE and WORK ARROUND
//===========================================================================

// ARB_vertex_program / ARB_fragment_program
PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC	    glDisableVertexAttribArray = nullptr;
PFNGLPROGRAMSTRINGARBPROC			    glProgramStringARB = nullptr;
PFNGLBINDPROGRAMARBPROC				    glBindProgramARB = nullptr;
PFNGLGENPROGRAMSARBPROC				    glGenProgramsARB = nullptr;
PFNGLPROGRAMENVPARAMETER4FVARBPROC	    glProgramEnvParameter4fvARB = nullptr;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB = nullptr;

PFNGLMULTITEXCOORD2FARBPROC			    glMultiTexCoord2fARB = nullptr;
PFNGLMULTITEXCOORD2FVARBPROC			glMultiTexCoord2fvARB = nullptr;
PFNGLCLIENTACTIVETEXTUREARBPROC		    glClientActiveTextureARB = nullptr;

void (*glEnd)(void);
void (*glBegin)(GLenum mode);
void (*glArrayElement)(GLint i);
void (*glPopMatrix)(void);
void (*glPushMatrix)(void);
void (*glPopAttrib)(void);
void (*glPushAttrib)(GLbitfield mask);
void (*glColor3f)(GLfloat red, GLfloat green, GLfloat blue);
void (*glColor3fv)(const GLfloat *v);
void (*glColor3ubv)(const GLubyte *v);
void (*glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void (*glColor4fv)(const GLfloat *v);
void (*glColor4ubv)(const GLubyte *v);
void (*glVertex2f)(GLfloat x, GLfloat y);
void (*glVertex3f)(GLfloat x, GLfloat y, GLfloat z);
void (*glVertex3fv)(const GLfloat *v);
void (*glTexCoord2f)(GLfloat s, GLfloat t);
void (*glTexCoord2fv)(const GLfloat *v);
void (*glMatrixMode)(GLenum mode);
void (*glLoadMatrixf)(const GLfloat *m);
void (*glLoadIdentity)(void);
void (*glTexEnvi)(GLenum target, GLenum pname, GLint param);
void (*glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
void (*glTexGenfv)(GLenum coord, GLenum pname, const GLfloat *params);
void (*glTexGenf)(GLenum coord, GLenum pname, GLfloat param);
void (*glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void (*glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void (*glNormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
void (*glEnableClientState)(GLenum array);
void (*glDisableClientState)(GLenum array);
void (*glPrioritizeTextures)(GLsizei n, const GLuint *textures, const GLclampf *priorities);
void (*glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void (*glAlphaFunc)(GLenum func, GLclampf ref);
void (*glOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void (*glDrawPixels)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void (*glRasterPos2f)(GLfloat x, GLfloat y);
void (*glPixelZoom)(GLfloat xfactor, GLfloat yfactor);


//===========================================================================

template< typename _t >
inline bool GL_GetExtensionPointer( _t &func, const char* name )
{
    return ( func = reinterpret_cast<_t>( SDL_GL_GetProcAddress( name ) ) ) != nullptr;
}

// OpenGL debug output messages
static void APIENTRY DebugOutput( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam );

void	InitOpenGLDebugOutput( void )
{
    GLint flags = 0; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable( GL_DEBUG_OUTPUT );

        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );    // to block the current context

        // TODO: add a debug level Cvar
        glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        
        glDebugMessageCallback( DebugOutput, nullptr );
    }
}

void LoadOpenGLFunctions( void )
{
    assert( GL_GetExtensionPointer( glGetError, "glGetError") );
    assert( GL_GetExtensionPointer( glGetString, "glGetString") );
    assert( GL_GetExtensionPointer( glGetFloatv, "glGetFloatv") );
    assert( GL_GetExtensionPointer( glGetIntegerv, "glGetIntegerv") );
    assert( GL_GetExtensionPointer( glEnable, "glEnable") );
    assert( GL_GetExtensionPointer( glDisable, "glDisable") );
    assert( GL_GetExtensionPointer( glIsEnabled, "glIsEnabled") );
    assert( GL_GetExtensionPointer( glClear, "glClear") );
    assert( GL_GetExtensionPointer( glFlush, "glFlush") );
    assert( GL_GetExtensionPointer( glFinish, "glFinish") );
    assert( GL_GetExtensionPointer( glViewport, "glViewport") );

    assert( GL_GetExtensionPointer( glClearColor, "glClearColor" ) );
    assert( GL_GetExtensionPointer( glColorMask, "glColorMask" ) );

    assert( GL_GetExtensionPointer( glBlendFunc, "glBlendFunc" ) );

    assert( GL_GetExtensionPointer( glClearDepth, "glClearDepth" ) );
    assert( GL_GetExtensionPointer( glDepthMask, "glDepthMask" ) );
    assert( GL_GetExtensionPointer( glDepthFunc, "glDepthFunc" ) );
    assert( GL_GetExtensionPointer( glDepthRange, "glDepthRange" ) );

    assert( GL_GetExtensionPointer( glScissor, "glScissor" ) );
    assert( GL_GetExtensionPointer( glStencilOp, "glStencilOp" ) );
    assert( GL_GetExtensionPointer( glStencilFunc, "glStencilFunc" ) );
    assert( GL_GetExtensionPointer( glStencilMask, "glStencilMask" ) );
    assert( GL_GetExtensionPointer( glClearStencil, "glClearStencil" ) );
    assert( GL_GetExtensionPointer( glStencilOpSeparate, "glStencilOpSeparate" ) );
    assert( GL_GetExtensionPointer( glStencilFuncSeparate, "glStencilFuncSeparate" ) );

    assert( GL_GetExtensionPointer( glCullFace, "glCullFace" ) );
    assert( GL_GetExtensionPointer( glPolygonMode, "glPolygonMode" ) );
    assert( GL_GetExtensionPointer( glPolygonOffset, "glPolygonOffset" ) );
    assert( GL_GetExtensionPointer( glLineWidth, "glLineWidth" ) );
    assert( GL_GetExtensionPointer( glPointSize, "glPointSize" ) );

    assert( GL_GetExtensionPointer( glDrawBuffer, "glDrawBuffer" ) );
    assert( GL_GetExtensionPointer( glReadBuffer, "glReadBuffer" ) );
    assert( GL_GetExtensionPointer( glReadPixels, "glReadPixels" ) );
    assert( GL_GetExtensionPointer( glPixelStorei, "glPixelStorei" ) );

    assert( GL_GetExtensionPointer( glBindBuffer, "glBindBuffer") );
    assert( GL_GetExtensionPointer( glDeleteBuffers, "glDeleteBuffers") );
    assert( GL_GetExtensionPointer( glGenBuffers, "glGenBuffers") );
    assert( GL_GetExtensionPointer( glIsBuffer, "glIsBuffer") );
    assert( GL_GetExtensionPointer( glBufferData, "glBufferData") );
    assert( GL_GetExtensionPointer( glBufferSubData, "glBufferSubData") );
    assert( GL_GetExtensionPointer( glGetBufferSubData, "glGetBufferSubData") );
    assert( GL_GetExtensionPointer( glMapBuffer, "glMapBuffer") );
    assert( GL_GetExtensionPointer( glUnmapBuffer, "glUnmapBuffer") );
    assert( GL_GetExtensionPointer( glGetBufferParameteriv, "glGetBufferParameteriv") );
    assert( GL_GetExtensionPointer( glGetBufferPointerv, "glGetBufferPointerv") );

    assert( GL_GetExtensionPointer( glCreateBuffers, "glCreateBuffers" ) );
    assert( GL_GetExtensionPointer( glNamedBufferStorage, "glNamedBufferStorage" ) );
    assert( GL_GetExtensionPointer( glNamedBufferSubData, "glNamedBufferSubData" ) );
    assert( GL_GetExtensionPointer( glCopyNamedBufferSubData, "glCopyNamedBufferSubData" ) );
    assert( GL_GetExtensionPointer( glClearNamedBufferData, "glClearNamedBufferData" ) );
    assert( GL_GetExtensionPointer( glMapNamedBufferRange, "glMapNamedBufferRange" ) );
    assert( GL_GetExtensionPointer( glUnmapNamedBuffer, "glUnmapNamedBuffer" ) );

    assert( GL_GetExtensionPointer( glDrawElements, "glDrawElements" ) );

    assert( GL_GetExtensionPointer( glActiveTexture, "glActiveTexture" ) );
    assert( GL_GetExtensionPointer( glGenTextures, "glGenTextures" ) );
    assert( GL_GetExtensionPointer( glDeleteTextures, "glDeleteTextures" ) );
    assert( GL_GetExtensionPointer( glBindTexture, "glBindTexture" ) );
    assert( GL_GetExtensionPointer( glTexImage2D, "glTexImage2D" ) );
    assert( GL_GetExtensionPointer( glTexImage3D, "glTexImage3D" ) );
    assert( GL_GetExtensionPointer( glTexSubImage2D, "glTexSubImage2D" ) );
    assert( GL_GetExtensionPointer( glTexSubImage3D, "glTexSubImage3D" ) );
    assert( GL_GetExtensionPointer( glTexParameteri, "glTexParameteri" ) );
    assert( GL_GetExtensionPointer( glTexParameterf, "glTexParameterf" ) );
    assert( GL_GetExtensionPointer( glTexParameterfv, "glTexParameterfv" ) );
    assert( GL_GetExtensionPointer( glGetTexImage, "glGetTexImage" ) );
    assert( GL_GetExtensionPointer( glCopyTexImage2D, "glCopyTexImage2D" ) );
    assert( GL_GetExtensionPointer( glCopyTexSubImage2D, "glCopyTexSubImage2D" ) );
    assert( GL_GetExtensionPointer( glCopyTexSubImage3D, "glCopyTexSubImage3D" ) );

    assert( GL_GetExtensionPointer( glCompressedTexImage2D, "glCompressedTexImage2D" ) );
    assert( GL_GetExtensionPointer( glCompressedTexImage3D, "glCompressedTexImage3D" ) );
    assert( GL_GetExtensionPointer( glGetCompressedTexImage, "glGetCompressedTexImage" ) );

    //
    assert( GL_GetExtensionPointer( glDebugMessageControl, "glDebugMessageControl" ) );
    assert( GL_GetExtensionPointer( glDebugMessageCallback, "glDebugMessageCallback" ) );

    //
    assert( GL_GetExtensionPointer( glDepthBoundsEXT, "glDepthBoundsEXT" ) );
//  assert( GL_GetExtensionPointer( glActiveStencilFaceEXT, "glActiveStencilFaceEXT" ) );
    assert( GL_GetExtensionPointer( glVertexAttribPointer, "glVertexAttribPointer" ) );
    assert( GL_GetExtensionPointer( glDisableVertexAttribArray, "glDisableVertexAttribArray" ) );
    assert( GL_GetExtensionPointer( glProgramStringARB, "glProgramStringARB" ) );
    assert( GL_GetExtensionPointer( glBindProgramARB, "glBindProgramARB" ) );
    assert( GL_GetExtensionPointer( glGenProgramsARB, "glGenProgramsARB" ) );
    assert( GL_GetExtensionPointer( glProgramEnvParameter4fvARB, "glProgramEnvParameter4fvARB" ) );
    assert( GL_GetExtensionPointer( glProgramLocalParameter4fvARB, "glProgramLocalParameter4fvARB" ) );
    assert( GL_GetExtensionPointer( glMultiTexCoord2fARB, "glMultiTexCoord2fARB" ) );
    assert( GL_GetExtensionPointer( glMultiTexCoord2fvARB, "glMultiTexCoord2fvARB" ) );
    assert( GL_GetExtensionPointer( glClientActiveTextureARB, "glClientActiveTextureARB" ) );

    assert( GL_GetExtensionPointer( glEnableClientState, "glEnableClientState" ) );
    assert( GL_GetExtensionPointer( glDisableClientState, "glDisableClientState" ) );
    assert( GL_GetExtensionPointer( glPrioritizeTextures, "glPrioritizeTextures" ) );
    assert( GL_GetExtensionPointer( glColorPointer, "glColorPointer" ) );
    assert( GL_GetExtensionPointer( glAlphaFunc, "glAlphaFunc" ) );
    assert( GL_GetExtensionPointer( glOrtho, "glOrtho" ) );
    assert( GL_GetExtensionPointer( glDrawPixels, "glDrawPixels" ) );
    assert( GL_GetExtensionPointer( glRasterPos2f, "glRasterPos2f" ) );
    assert( GL_GetExtensionPointer( glPixelZoom, "glPixelZoom" ) );

    assert( GL_GetExtensionPointer( glEnd, "glEnd" ) );
    assert( GL_GetExtensionPointer( glBegin, "glBegin" ) );
    assert( GL_GetExtensionPointer( glArrayElement, "glArrayElement" ) );
    assert( GL_GetExtensionPointer( glPopMatrix, "glPopMatrix" ) );
    assert( GL_GetExtensionPointer( glPushMatrix, "glPushMatrix" ) );
    assert( GL_GetExtensionPointer( glPopAttrib, "glPopAttrib" ) );
    assert( GL_GetExtensionPointer( glPushAttrib, "glPushAttrib" ) );
    assert( GL_GetExtensionPointer( glColor3f, "glColor3f" ) );
    assert( GL_GetExtensionPointer( glColor3fv, "glColor3fv" ) );
    assert( GL_GetExtensionPointer( glColor3ubv, "glColor3ubv" ) );
    assert( GL_GetExtensionPointer( glColor4f, "glColor4f" ) );
    assert( GL_GetExtensionPointer( glColor4fv, "glColor4fv" ) );
    assert( GL_GetExtensionPointer( glColor4ubv, "glColor4ubv" ) );
    assert( GL_GetExtensionPointer( glVertex2f, "glVertex2f" ) );
    assert( GL_GetExtensionPointer( glVertex3f, "glVertex3f" ) );
    assert( GL_GetExtensionPointer( glVertex3fv, "glVertex3fv" ) );
    assert( GL_GetExtensionPointer( glTexCoord2f, "glTexCoord2f" ) );
    assert( GL_GetExtensionPointer( glTexCoord2fv, "glTexCoord2fv" ) );
    assert( GL_GetExtensionPointer( glMatrixMode, "glMatrixMode" ) );
    assert( GL_GetExtensionPointer( glLoadMatrixf, "glLoadMatrixf" ) );
    assert( GL_GetExtensionPointer( glLoadIdentity, "glLoadIdentity" ) );
    assert( GL_GetExtensionPointer( glTexEnvi, "glTexEnvi" ) );
    assert( GL_GetExtensionPointer( glTexEnvfv, "glTexEnvfv" ) );
    assert( GL_GetExtensionPointer( glTexGenfv, "glTexGenfv" ) );
    assert( GL_GetExtensionPointer( glTexGenf, "glTexGenf" ) );
    assert( GL_GetExtensionPointer( glVertexPointer, "glVertexPointer" ) );
    assert( GL_GetExtensionPointer( glTexCoordPointer, "glTexCoordPointer" ) );
    assert( GL_GetExtensionPointer( glNormalPointer, "glNormalPointer" ) );
    assert( GL_GetExtensionPointer( glEnableClientState, "glEnableClientState" ) );
    assert( GL_GetExtensionPointer( glDisableClientState, "glDisableClientState" ) );
    assert( GL_GetExtensionPointer( glPrioritizeTextures, "glPrioritizeTextures" ) );
    assert( GL_GetExtensionPointer( glColorPointer, "glColorPointer" ) );
    assert( GL_GetExtensionPointer( glAlphaFunc, "glAlphaFunc" ) );
    assert( GL_GetExtensionPointer( glOrtho, "glOrtho" ) );
    assert( GL_GetExtensionPointer( glDrawPixels, "glDrawPixels" ) );
    assert( GL_GetExtensionPointer( glRasterPos2f, "glRasterPos2f" ) );
    assert( GL_GetExtensionPointer( glPixelZoom, "glPixelZoom" ) );
}

#include <iostream>
#include <sstream>

static void APIENTRY DebugOutput( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message, const void *userParam )
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


GLint glGetInteger( const GLenum pname )
{
	GLint params = 0;
	glGetIntegerv( pname, &params );
	return params;
}

GLboolean glSetState( const GLenum flag, const GLboolean enable )
{
	GLboolean former = glIsEnabled( flag );
	if ( former != enable )
	{
		if (enable)
			glEnable( flag );
		else
			glDisable( flag );
	}
	return former;
}

GLenum glGetActiveTexture( void )
{
	GLint unit = 0;
	glGetIntegerv( GL_ACTIVE_TEXTURE, &unit );
	return unit;
}

const GLuint glGetBuferBinding( const GLenum target )
{
    GLint buffer = 0;
	switch (target)
	{
	case GL_ARRAY_BUFFER:
        glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &buffer );
		break;
	case GL_ATOMIC_COUNTER_BUFFER:
		glGetIntegerv( GL_ATOMIC_COUNTER_BUFFER_BINDING, &buffer );
		break;
	case GL_COPY_READ_BUFFER:
		glGetIntegerv( GL_COPY_READ_BUFFER_BINDING, &buffer );
		break;
	case GL_COPY_WRITE_BUFFER:
		glGetIntegerv( GL_COPY_WRITE_BUFFER_BINDING, &buffer );
		break;
	case GL_DISPATCH_INDIRECT_BUFFER:
		glGetIntegerv( GL_DISPATCH_INDIRECT_BUFFER_BINDING, &buffer );
		break;
	case GL_DRAW_INDIRECT_BUFFER:
		glGetIntegerv( GL_DRAW_INDIRECT_BUFFER_BINDING, &buffer );
		break;
	case GL_ELEMENT_ARRAY_BUFFER:
		glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer );
		break;
	case GL_PIXEL_PACK_BUFFER:
		glGetIntegerv( GL_PIXEL_PACK_BUFFER_BINDING, &buffer );
		break;
	case GL_PIXEL_UNPACK_BUFFER:
		glGetIntegerv( GL_PIXEL_UNPACK_BUFFER_BINDING, &buffer );
		break;
	case GL_QUERY_BUFFER:
		glGetIntegerv( GL_QUERY_BUFFER_BINDING, &buffer );
		break;
	case GL_SHADER_STORAGE_BUFFER:
		glGetIntegerv( GL_SHADER_STORAGE_BUFFER_BINDING, &buffer );
		break;
	case GL_TEXTURE_BUFFER:
		glGetIntegerv( GL_TEXTURE_BUFFER_BINDING, &buffer );
		break;
	case GL_TRANSFORM_FEEDBACK_BUFFER:
		glGetIntegerv( GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &buffer );
		break;
	case GL_UNIFORM_BUFFER:
		glGetIntegerv( GL_UNIFORM_BUFFER_BINDING, &buffer );
		break;
	default:
		assert( false );
	}

	return buffer;
}

const GLuint glGetTextureBinding( const GLenum target )
{
    GLint texture;
	switch (target)
	{
	case GL_TEXTURE_1D:
		glGetIntegerv( GL_TEXTURE_BINDING_1D, &texture );
        break;
	case GL_TEXTURE_1D_ARRAY:
		glGetIntegerv( GL_TEXTURE_BINDING_1D_ARRAY, &texture );
        break;
	case GL_TEXTURE_2D:
		glGetIntegerv( GL_TEXTURE_BINDING_2D, &texture );
        break;
	case GL_TEXTURE_2D_ARRAY:
		glGetIntegerv( GL_TEXTURE_BINDING_2D_ARRAY, &texture );
        break;
	case GL_TEXTURE_2D_MULTISAMPLE:
		glGetIntegerv( GL_TEXTURE_BINDING_2D_MULTISAMPLE, &texture );
        break;
	case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
		glGetIntegerv( GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &texture );
        break;
	case GL_TEXTURE_3D:
		glGetIntegerv( GL_TEXTURE_BINDING_3D, &texture );
        break;
	case GL_TEXTURE_RECTANGLE:
		glGetIntegerv( GL_TEXTURE_BINDING_RECTANGLE, &texture );
        break;
	case GL_TEXTURE_CUBE_MAP:
		glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP, &texture );
        break;
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		glGetIntegerv( GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &texture );
        break;
	default:
		assert( false );
	}

	return texture;
}

/*
==================
GL_CheckErrors
==================
*/
void GL_CheckErrors( void )
{
    int		err;
    char	s[64];
	int		i;

	// check for up to 10 errors pending
	for ( i = 0 ; i < 10 ; i++ ) 
    {
		err = glGetError();
		if ( err == GL_NO_ERROR ) 
        {
			return;
		}
		
        switch( err ) 
        {
			case GL_INVALID_ENUM:
				strcpy( s, "GL_INVALID_ENUM" );
				break;
			case GL_INVALID_VALUE:
				strcpy( s, "GL_INVALID_VALUE" );
				break;
			case GL_INVALID_OPERATION:
				strcpy( s, "GL_INVALID_OPERATION" );
				break;
			case GL_STACK_OVERFLOW:
				strcpy( s, "GL_STACK_OVERFLOW" );
				break;
			case GL_STACK_UNDERFLOW:
				strcpy( s, "GL_STACK_UNDERFLOW" );
				break;
			case GL_OUT_OF_MEMORY:
				strcpy( s, "GL_OUT_OF_MEMORY" );
				break;
			default:
				idStr::snPrintf( s, sizeof(s), "%i", err);
				break;
		}

		if ( !r_ignoreGLErrors.GetBool() ) 
        {
			common->Printf( "GL_CheckErrors: %s\n", s );
		}
	}
}