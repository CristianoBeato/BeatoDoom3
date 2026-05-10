#include "tr_local.h"
#include "qgl.h"
#include <SDL3/SDL_video.h>

PFNGLGETERRORPROC					glGetError = nullptr;

PFNGLGETINTEGERVPROC                glGetIntegerv = nullptr;
PFNGLGETFLOATVPROC                  glGetFloatv = nullptr;
PFNGLGETSTRINGPROC					glGetString = nullptr;

PFNGLFINISHPROC 					glFinish = nullptr;
PFNGLFLUSHPROC						glFlush = nullptr;

PFNGLISENABLEDPROC					glIsEnabled = nullptr;
PFNGLENABLEPROC						glEnable = nullptr;
PFNGLDISABLEPROC 					glDisable = nullptr;

PFNGLCLEARPROC						glClear = nullptr;
PFNGLCLEARCOLORPROC 				glClearColor = nullptr;

PFNGLCLEARDEPTHPROC 				glClearDepth = nullptr;
PFNGLDEPTHFUNCPROC 					glDepthFunc = nullptr;
PFNGLDEPTHMASKPROC 					glDepthMask = nullptr;
PFNGLDEPTHRANGEPROC					glDepthRange = nullptr;

PFNGLSTENCILFUNCPROC                glStencilFunc =  nullptr;
PFNGLSTENCILOPPROC                  glStencilOp =  nullptr;
PFNGLCLEARSTENCILPROC 				glClearStencil =  nullptr;
PFNGLSTENCILMASKPROC				glStencilMask =  nullptr;
PFNGLSTENCILOPSEPARATEPROC			glStencilOpSeparate =  nullptr;
PFNGLSTENCILFUNCSEPARATEPROC		glStencilFuncSeparate = nullptr;
PFNGLSTENCILMASKSEPARATEPROC		glStencilMaskSeparate = nullptr;

PFNGLPIXELSTOREIPROC                glPixelStorei = nullptr;

PFNGLBLENDFUNCPROC                  glBlendFunc = nullptr;

PFNGLVIEWPORTPROC					glViewport = nullptr;
PFNGLSCISSORPROC					glScissor = nullptr;

PFNGLCULLFACEPROC					glCullFace = nullptr;

PFNGLPOLYGONMODEPROC                glPolygonMode = nullptr;
PFNGLPOLYGONOFFSETPROC              glPolygonOffset = nullptr;

PFNGLCOLORMASKPROC                  glColorMask = nullptr;

PFNGLDRAWBUFFERPROC                 glDrawBuffer =  nullptr;
PFNGLREADBUFFERPROC                 glReadBuffer =  nullptr;
PFNGLREADPIXELSPROC                 glReadPixels =  nullptr;

PFNGLDRAWARRAYSPROC                 glDrawArrays = nullptr;
PFNGLDRAWELEMENTSPROC               glDrawElements = nullptr;

PFNGLPOINTSIZEPROC                  glPointSize = nullptr;
PFNGLLINEWIDTHPROC                  glLineWidth = nullptr;

// vertex array 
PFNGLISVERTEXARRAYPROC              glIsVertexArray = nullptr;
PFNGLCREATEVERTEXARRAYSPROC         glCreateVertexArrays = nullptr;
PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC            glBindVertexArray = nullptr;             
PFNGLENABLEVERTEXARRAYATTRIBPROC    glEnableVertexArrayAttrib = nullptr;
PFNGLDISABLEVERTEXARRAYATTRIBPROC   glDisableVertexArrayAttrib = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC   glVertexArrayAttribBinding = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC    glVertexArrayAttribFormat = nullptr;
PFNGLVERTEXARRAYELEMENTBUFFERPROC   glVertexArrayElementBuffer = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC    glVertexArrayVertexBuffer = nullptr; 

// shader 
PFNGLISSHADERPROC                   glIsShader = nullptr;
PFNGLCREATESHADERPROC               glCreateShader = nullptr;
PFNGLDELETESHADERPROC               glDeleteShader = nullptr;
PFNGLSHADERSOURCEPROC               glShaderSource = nullptr;
PFNGLSHADERBINARYPROC               glShaderBinary = nullptr;
PFNGLCOMPILESHADERPROC              glCompileShader = nullptr;
PFNGLSPECIALIZESHADERPROC           glSpecializeShader = nullptr;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog = nullptr;
PFNGLGETSHADERIVPROC                glGetShaderiv = nullptr;

// program
PFNGLCREATEPROGRAMPROC              glCreateProgram = nullptr;
PFNGLDELETEPROGRAMPROC              glDeleteProgram = nullptr;
PFNGLISPROGRAMPROC                  glIsProgram = nullptr;
PFNGLPROGRAMPARAMETERIPROC          glProgramParameteri = nullptr;
PFNGLATTACHSHADERPROC               glAttachShader = nullptr;
PFNGLDETACHSHADERPROC               glDetachShader = nullptr;
PFNGLLINKPROGRAMPROC                glLinkProgram = nullptr;
PFNGLVALIDATEPROGRAMPROC            glValidateProgram = nullptr;
PFNGLGETPROGRAMIVPROC               glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog = nullptr;
PFNGLUSEPROGRAMPROC                 glUseProgram = nullptr;

// Uniforms
PFNGLUNIFORM1FVPROC			        glUniform1fv = nullptr;
PFNGLUNIFORM1IPROC                  glUniform1i = nullptr;
PFNGLUNIFORM1IVPROC                 glUniform1iv = nullptr;
PFNGLUNIFORM1UIVPROC                glUniform1uiv = nullptr;
PFNGLUNIFORM4FPROC			        glUniform4f = nullptr;
PFNGLUNIFORM4FVPROC			        glUniform4fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC	        glUniformMatrix4fv = nullptr;

// ARB_vertex_buffer_object
PFNGLBINDBUFFERPROC 				glBindBuffer = nullptr;
PFNGLISBUFFERPROC 					glIsBuffer = nullptr;
PFNGLCREATEBUFFERSPROC				glCreateBuffers = nullptr;
PFNGLDELETEBUFFERSPROC				glDeleteBuffers = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC			glNamedBufferStorage = nullptr;
PFNGLNAMEDBUFFERDATAPROC            glNamedBufferData = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC			glNamedBufferSubData = nullptr;
PFNGLGETNAMEDBUFFERSUBDATAPROC		glGetNamedBufferSubData = nullptr;
PFNGLGETNAMEDBUFFERPOINTERVPROC		glGetNamedBufferPointerv = nullptr;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC  glGetNamedBufferParameteriv = nullptr;

// textures
PFNGLBINDTEXTUREPROC				glBindTexture = nullptr;
PFNGLACTIVETEXTUREPROC				glActiveTexture = nullptr;
PFNGLGENTEXTURESPROC                glGenTextures = nullptr;
PFNGLDELETETEXTURESPROC				glDeleteTextures = nullptr;
PFNGLTEXIMAGE1DPROC					glTexImage1D = nullptr;
PFNGLTEXIMAGE2DPROC					glTexImage2D = nullptr;
PFNGLTEXIMAGE3DPROC					glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE1DPROC				glTexSubImage1D = nullptr;
PFNGLTEXSUBIMAGE2DPROC				glTexSubImage2D = nullptr;
PFNGLTEXSUBIMAGE3DPROC				glTexSubImage3D = nullptr;
PFNGLTEXPARAMETERFPROC				glTexParameterf = nullptr;
PFNGLTEXPARAMETERFVPROC             glTexParameterfv = nullptr;
PFNGLTEXPARAMETERIPROC				glTexParameteri = nullptr;
PFNGLTEXPARAMETERIVPROC				glTexParameteriv = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DPROC		glCompressedTexImage2D = nullptr;
PFNGLGETTEXIMAGEPROC                glGetTexImage = nullptr;
PFNGLGETCOMPRESSEDTEXIMAGEPROC      glGetCompressedTexImage = nullptr;
PFNGLCOPYTEXIMAGE1DPROC				glCopyTexImage1D = nullptr;
PFNGLCOPYTEXIMAGE2DPROC             glCopyTexImage2D = nullptr;
PFNGLCOPYTEXSUBIMAGE1DPROC          glCopyTexSubImage1D = nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC          glCopyTexSubImage2D = nullptr;

PFNGLDEBUGMESSAGECALLBACKPROC       glDebugMessageCallback = nullptr;
PFNGLDEBUGMESSAGECONTROLPROC        glDebugMessageControl = nullptr;

///================================================================================================
/// not in core

PFNGLDEPTHBOUNDSEXTPROC                 glDepthBoundsEXT = nullptr;

PFNGLCOLORTABLEEXTPROC                  glColorTableEXT = nullptr;

PFNGLVERTEXATTRIBPOINTERARBPROC	        glVertexAttribPointerARB = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC     glEnableVertexAttribArrayARB = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC    glDisableVertexAttribArrayARB = nullptr;
PFNGLPROGRAMSTRINGARBPROC			    glProgramStringARB = nullptr;
PFNGLBINDPROGRAMARBPROC				    glBindProgramARB = nullptr;
PFNGLGENPROGRAMSARBPROC				    glGenProgramsARB = nullptr;
PFNGLPROGRAMENVPARAMETER4FVARBPROC	    glProgramEnvParameter4fvARB = nullptr;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB = nullptr;

PFNGLCLIENTACTIVETEXTUREARBPROC	        glClientActiveTextureARB = nullptr;

PFNGLPRIORITIZETEXTURESPROC             glPrioritizeTextures = nullptr;

PFNGLALPHAFUNCPROC                      glAlphaFunc = nullptr;

PFNGLPIXELZOOMPROC                      glPixelZoom = nullptr;

PFNGLENABLECLIENTSTATEPROC              glEnableClientState = nullptr;
PFNGLDISABLECLIENTSTATEPROC             glDisableClientState = nullptr;

PFNGLMATRIXMODEPROC                     glMatrixMode = nullptr;
PFNGLPUSHMATRIXPROC                     glPushMatrix = nullptr;
PFNGLPOPMATRIXPROC                      glPopMatrix = nullptr;
PFNGLLOADIDENTITYPROC                   glLoadIdentity = nullptr;
PFNGLLOADMATRIXFPROC                    glLoadMatrixf = nullptr;
PFNGLORTHOPROC                          glOrtho = nullptr;

PFNGLTEXENVIPROC                        glTexEnvi = nullptr;
PFNGLTEXENVFVPROC                       glTexEnvfv = nullptr;
PFNGLTEXGENFPROC                        glTexGenf = nullptr;
PFNGLTEXGENFVPROC                       glTexGenfv = nullptr;

PFNGLSHADEMODELPROC                     glShadeModel = nullptr;
PFNGLRASTERPOS2FPROC                    glRasterPos2f = nullptr;
PFNGLDRAWPIXELSPROC                     glDrawPixels = nullptr;
PFNGLPOPATTRIBPROC                      glPopAttrib = nullptr;
PFNGLPUSHATTRIBPROC                     glPushAttrib = nullptr;
PFNGLVERTEXPOINTERPROC                  glVertexPointer = nullptr;
PFNGLTEXCOORDPOINTERPROC                glTexCoordPointer = nullptr;
PFNGLNORMALPOINTERPROC                  glNormalPointer = nullptr;
PFNGLCOLORPOINTERPROC                   glColorPointer = nullptr;
PFNGLVERTEX2FPROC                       glVertex2f = nullptr;
PFNGLVERTEX2FVPROC                      glVertex2fv = nullptr;
PFNGLVERTEX3FPROC                       glVertex3f = nullptr;
PFNGLVERTEX3FVPROC                      glVertex3fv = nullptr;
PFNGLTEXCOORD2FPROC                     glTexCoord2f = nullptr;
PFNGLTEXCOORD2FVPROC                    glTexCoord2fv = nullptr;
PFNGLCOLOR3FPROC                        glColor3f = nullptr;
PFNGLCOLOR3FVPROC                       glColor3fv = nullptr;
PFNGLCOLOR3UBVPROC                      glColor3ubv = nullptr;
PFNGLCOLOR4FPROC                        glColor4f = nullptr;
PFNGLCOLOR4FVPROC                       glColor4fv = nullptr;
PFNGLCOLOR4UBVPROC                      glColor4ubv = nullptr;

PFNGLBEGINPROC                          glBegin = nullptr;
PFNGLENDPROC                            glEnd = nullptr;
PFNGLARRAYELEMENTPROC                   glArrayElement = nullptr;


template< typename _t>
void GetProcAndress( _t & in_address, const char* in_name )
{
    in_address = reinterpret_cast<_t>( SDL_GL_GetProcAddress( in_name ) );
}

#define PROC( X ) GetProcAndress( X, #X );

void    GLimp_LoadExtension( void  )
{
    PROC( glGetError );

    PROC( glGetIntegerv );
    PROC( glGetFloatv );
    PROC( glGetString );

    PROC( glFinish );
    PROC( glFlush );

    PROC( glIsEnabled );
    PROC( glEnable );
    PROC( glDisable );

    PROC( glClear );
    PROC( glClearColor );

    PROC( glClearDepth );
    PROC( glDepthFunc );
    PROC( glDepthMask );
    PROC( glDepthRange );

    PROC( glStencilFunc );
    PROC( glStencilOp );
    PROC( glClearStencil );
    PROC( glStencilMask );
    PROC( glStencilOpSeparate );
    PROC( glStencilFuncSeparate );
    PROC( glStencilMaskSeparate );

    PROC( glPixelStorei );

    PROC( glBlendFunc );

    PROC( glViewport );
    PROC( glScissor );

    PROC( glCullFace );

    PROC( glPolygonMode );
    PROC( glPolygonOffset );

    PROC( glColorMask );


    PROC( glDrawBuffer );
    PROC( glReadBuffer );
    PROC( glReadPixels );

    PROC( glDrawArrays );
    PROC( glDrawElements );

    PROC( glPointSize );
    PROC( glLineWidth );

    // vertex array 
    PROC( glIsVertexArray );
    PROC( glCreateVertexArrays );
    PROC( glDeleteVertexArrays );
    PROC( glBindVertexArray );             
    PROC( glEnableVertexArrayAttrib );
    PROC( glDisableVertexArrayAttrib );
    PROC( glVertexArrayAttribBinding );
    PROC( glVertexArrayAttribFormat );
    PROC( glVertexArrayElementBuffer );
    PROC( glVertexArrayVertexBuffer ); 

    // shader 
    PROC( glIsShader );
    PROC( glCreateShader );
    PROC( glDeleteShader );
    PROC( glShaderSource );
    PROC( glShaderBinary );
    PROC( glCompileShader );
    PROC( glSpecializeShader );
    PROC( glGetShaderInfoLog );
    PROC( glGetShaderiv );

// program
    PROC( glCreateProgram );
    PROC( glDeleteProgram );
    PROC( glIsProgram );
    PROC( glProgramParameteri );
    PROC( glAttachShader );
    PROC( glDetachShader );
    PROC( glLinkProgram );
    PROC( glValidateProgram );
    PROC( glGetProgramiv );
    PROC( glGetProgramInfoLog );
    PROC( glUseProgram );

// Uniforms
    PROC( glUniform1fv );
    PROC( glUniform1i );
    PROC( glUniform1iv );
    PROC( glUniform1uiv );
    PROC( glUniform4f );
    PROC( glUniform4fv );
    PROC( glUniformMatrix4fv );

// ARB_vertex_buffer_object
    PROC( glBindBuffer );
    PROC( glIsBuffer );
    PROC( glCreateBuffers );
    PROC( glDeleteBuffers );
    PROC( glNamedBufferStorage );
    PROC( glNamedBufferData );
    PROC( glNamedBufferSubData );
    PROC( glGetNamedBufferSubData );
    PROC( glGetNamedBufferPointerv );
    PROC( glGetNamedBufferParameteriv );

// textures
    PROC( glBindTexture );
    PROC( glActiveTexture );
    PROC( glGenTextures );
    PROC( glDeleteTextures );
    PROC( glTexImage1D );
    PROC( glTexImage2D );
    PROC( glTexImage3D );
    PROC( glTexSubImage1D );
    PROC( glTexSubImage2D );
    PROC( glTexSubImage3D );
    PROC( glTexParameterf );
    PROC( glTexParameterfv );
    PROC( glTexParameteri );
    PROC( glTexParameteriv );
    PROC( glCompressedTexImage2D );
    PROC( glGetTexImage );
    PROC( glGetCompressedTexImage );
    PROC( glCopyTexImage1D );
    PROC( glCopyTexImage2D );
    PROC( glCopyTexSubImage1D );
    PROC( glCopyTexSubImage2D );

    PROC( glDebugMessageCallback );
    PROC( glDebugMessageControl );

///================================================================================================
/// not in core

    PROC( glDepthBoundsEXT );

    PROC( glColorTableEXT );

    PROC( glVertexAttribPointerARB );
    PROC( glEnableVertexAttribArrayARB );
    PROC( glDisableVertexAttribArrayARB );
    PROC( glProgramStringARB );
    PROC( glBindProgramARB );
    PROC( glGenProgramsARB );
    PROC( glProgramEnvParameter4fvARB );
    PROC( glProgramLocalParameter4fvARB );

    PROC( glClientActiveTextureARB );

    PROC( glPrioritizeTextures );

    PROC( glAlphaFunc );

    PROC( glPixelZoom );

    PROC( glEnableClientState );
    PROC( glDisableClientState );

    PROC( glMatrixMode );
    PROC( glPushMatrix );
    PROC( glPopMatrix );
    PROC( glLoadIdentity );
    PROC( glLoadMatrixf );
    PROC( glOrtho );

    PROC( glTexEnvi );
    PROC( glTexEnvfv );
    PROC( glTexGenf );
    PROC( glTexGenfv );

    PROC( glShadeModel );
    PROC( glRasterPos2f );
    PROC( glDrawPixels );
    PROC( glPopAttrib );
    PROC( glPushAttrib );
    PROC( glVertexPointer );
    PROC( glTexCoordPointer );
    PROC( glNormalPointer );
    PROC( glColorPointer );
    PROC( glVertex2f );
    PROC( glVertex2fv );
    PROC( glVertex3f );
    PROC( glVertex3fv );
    PROC( glTexCoord2f );
    PROC( glTexCoord2fv );
    PROC( glColor3f );
    PROC( glColor3fv );
    PROC( glColor3ubv );
    PROC( glColor4f );
    PROC( glColor4fv );
    PROC( glColor4ubv );

    PROC( glBegin );
    PROC( glEnd );
    PROC( glArrayElement );
}
