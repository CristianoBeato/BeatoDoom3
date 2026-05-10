/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
/*
** gl.H
*/

#ifndef __gl_H__
#define __gl_H__

#include <GL/glcorearb.h>

extern PFNGLGETERRORPROC								glGetError;

extern PFNGLGETINTEGERVPROC                             glGetIntegerv;
extern PFNGLGETFLOATVPROC                               glGetFloatv;
extern PFNGLGETSTRINGPROC								glGetString;

extern PFNGLFINISHPROC 									glFinish;
extern PFNGLFLUSHPROC									glFlush;

extern PFNGLISENABLEDPROC								glIsEnabled;
extern PFNGLENABLEPROC									glEnable;
extern PFNGLDISABLEPROC 								glDisable;

extern PFNGLCLEARPROC									glClear;
extern PFNGLCLEARCOLORPROC 								glClearColor;

extern PFNGLCLEARDEPTHPROC 								glClearDepth;
extern PFNGLDEPTHFUNCPROC 								glDepthFunc;
extern PFNGLDEPTHMASKPROC 								glDepthMask;
extern PFNGLDEPTHRANGEPROC								glDepthRange;

extern PFNGLSTENCILFUNCPROC                             glStencilFunc;
extern PFNGLSTENCILOPPROC                               glStencilOp;
extern PFNGLCLEARSTENCILPROC 							glClearStencil;
extern PFNGLSTENCILMASKPROC						        glStencilMask;
extern PFNGLSTENCILOPSEPARATEPROC						glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC						glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC						glStencilMaskSeparate;

extern PFNGLPIXELSTOREIPROC                             glPixelStorei;

extern PFNGLBLENDFUNCPROC								glBlendFunc;

extern PFNGLVIEWPORTPROC								glViewport;
extern PFNGLSCISSORPROC									glScissor;

extern PFNGLCULLFACEPROC								glCullFace;

extern PFNGLPOLYGONMODEPROC                             glPolygonMode;
extern PFNGLPOLYGONOFFSETPROC                           glPolygonOffset;

extern PFNGLCOLORMASKPROC								glColorMask;

extern PFNGLDRAWBUFFERPROC                              glDrawBuffer;
extern PFNGLREADBUFFERPROC                              glReadBuffer;
extern PFNGLREADPIXELSPROC                              glReadPixels;

extern PFNGLDRAWARRAYSPROC                              glDrawArrays;
extern PFNGLDRAWELEMENTSPROC                            glDrawElements;

extern PFNGLPOINTSIZEPROC                               glPointSize;
extern PFNGLLINEWIDTHPROC                               glLineWidth;

// vertex array 
extern PFNGLISVERTEXARRAYPROC                           glIsVertexArray;
extern PFNGLCREATEVERTEXARRAYSPROC                      glCreateVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC                      glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC                         glBindVertexArray;             
extern PFNGLENABLEVERTEXARRAYATTRIBPROC                 glEnableVertexArrayAttrib;
extern PFNGLDISABLEVERTEXARRAYATTRIBPROC                glDisableVertexArrayAttrib;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC                glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYATTRIBFORMATPROC                 glVertexArrayAttribFormat;
extern PFNGLVERTEXARRAYELEMENTBUFFERPROC                glVertexArrayElementBuffer;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC                 glVertexArrayVertexBuffer; 

// shader 
extern PFNGLISSHADERPROC                                glIsShader;
extern PFNGLCREATESHADERPROC                            glCreateShader;
extern PFNGLDELETESHADERPROC                            glDeleteShader;
extern PFNGLSHADERSOURCEPROC                            glShaderSource;
extern PFNGLSHADERBINARYPROC                            glShaderBinary;
extern PFNGLCOMPILESHADERPROC                           glCompileShader;
extern PFNGLSPECIALIZESHADERPROC                        glSpecializeShader;
extern PFNGLGETSHADERINFOLOGPROC                        glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC                             glGetShaderiv;

// program
extern PFNGLCREATEPROGRAMPROC                           glCreateProgram;
extern PFNGLDELETEPROGRAMPROC                           glDeleteProgram;
extern PFNGLISPROGRAMPROC                               glIsProgram;
extern PFNGLPROGRAMPARAMETERIPROC                       glProgramParameteri;
extern PFNGLATTACHSHADERPROC                            glAttachShader;
extern PFNGLDETACHSHADERPROC                            glDetachShader;
extern PFNGLLINKPROGRAMPROC                             glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC                         glValidateProgram;
extern PFNGLGETPROGRAMIVPROC                            glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC                       glGetProgramInfoLog;
extern PFNGLUSEPROGRAMPROC                              glUseProgram;

// Uniforms
extern PFNGLUNIFORM1FVPROC								glUniform1fv;
extern PFNGLUNIFORM1IPROC                               glUniform1i;
extern PFNGLUNIFORM1IVPROC                              glUniform1iv;
extern PFNGLUNIFORM1UIVPROC                             glUniform1uiv;
extern PFNGLUNIFORM4FPROC								glUniform4f;
extern PFNGLUNIFORM4FVPROC								glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC						glUniformMatrix4fv;

// ARB_vertex_buffer_object
extern PFNGLBINDBUFFERPROC 								glBindBuffer;
extern PFNGLISBUFFERPROC 								glIsBuffer;
extern PFNGLCREATEBUFFERSPROC							glCreateBuffers;
extern PFNGLDELETEBUFFERSPROC							glDeleteBuffers;
extern PFNGLNAMEDBUFFERSTORAGEPROC						glNamedBufferStorage;
extern PFNGLNAMEDBUFFERDATAPROC                         glNamedBufferData;
extern PFNGLNAMEDBUFFERSUBDATAPROC						glNamedBufferSubData;
extern PFNGLGETNAMEDBUFFERSUBDATAPROC					glGetNamedBufferSubData;
extern PFNGLGETNAMEDBUFFERPOINTERVPROC					glGetNamedBufferPointerv;
extern PFNGLGETNAMEDBUFFERPARAMETERIVPROC				glGetNamedBufferParameteriv;

// textures
extern PFNGLBINDTEXTUREPROC								glBindTexture;
extern PFNGLACTIVETEXTUREPROC							glActiveTexture;
extern PFNGLGENTEXTURESPROC                             glGenTextures;
extern PFNGLDELETETEXTURESPROC							glDeleteTextures;
extern PFNGLTEXIMAGE1DPROC								glTexImage1D;
extern PFNGLTEXIMAGE2DPROC								glTexImage2D;
extern PFNGLTEXIMAGE3DPROC								glTexImage3D;
extern PFNGLTEXSUBIMAGE1DPROC							glTexSubImage1D;
extern PFNGLTEXSUBIMAGE2DPROC							glTexSubImage2D;
extern PFNGLTEXSUBIMAGE3DPROC							glTexSubImage3D;
extern PFNGLTEXPARAMETERFPROC							glTexParameterf;
extern PFNGLTEXPARAMETERFVPROC                          glTexParameterfv;
extern PFNGLTEXPARAMETERIPROC							glTexParameteri;
extern PFNGLTEXPARAMETERIVPROC							glTexParameteriv;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC					glCompressedTexImage2D;
extern PFNGLGETTEXIMAGEPROC                             glGetTexImage;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC					glGetCompressedTexImage;
extern PFNGLCOPYTEXIMAGE1DPROC							glCopyTexImage1D;
extern PFNGLCOPYTEXIMAGE2DPROC							glCopyTexImage2D;
extern PFNGLCOPYTEXSUBIMAGE1DPROC						glCopyTexSubImage1D;
extern PFNGLCOPYTEXSUBIMAGE2DPROC						glCopyTexSubImage2D;

extern PFNGLDEBUGMESSAGECALLBACKPROC                    glDebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC                     glDebugMessageControl;

// GL_EXT_depth_bounds_test
inline constexpr GLenum GL_DEPTH_BOUNDS_TEST_EXT = 0x8890;
typedef void (APIENTRYP PFNGLDEPTHBOUNDSEXTPROC) (GLclampd zmin, GLclampd zmax);
extern PFNGLDEPTHBOUNDSEXTPROC              glDepthBoundsEXT;

inline constexpr GLenum GL_SHARED_TEXTURE_PALETTE_EXT     = 0x81FB;
typedef	void ( APIENTRYP PFNGLCOLORTABLEEXTPROC)( GLenum  target, GLenum  internalFormat, GLsizei width, GLenum  format, GLenum  type, const GLvoid  *data );
extern PFNGLCOLORTABLEEXTPROC   glColorTableEXT;

inline constexpr GLenum GL_VERTEX_PROGRAM_ARB           = 0x8620;
inline constexpr GLenum GL_FRAGMENT_PROGRAM_ARB         = 0x8804;
inline constexpr GLenum GL_PROGRAM_STRING_ARB           = 0x8628;
inline constexpr GLenum GL_PROGRAM_ERROR_POSITION_ARB   = 0x864B;
inline constexpr GLenum GL_PROGRAM_ERROR_STRING_ARB     = 0x8874;
inline constexpr GLenum GL_PROGRAM_FORMAT_ASCII_ARB     = 0x8875;
inline constexpr GLenum GL_MAX_TEXTURE_COORDS_ARB       = 0x8871;
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERARBPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRYP PFNGLPROGRAMSTRINGARBPROC) (GLenum target, GLenum format, GLsizei len, const void *string);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);

// ARB_vertex_program / ARB_fragment_program
extern PFNGLVERTEXATTRIBPOINTERARBPROC		glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC	glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC				glBindProgramARB;
extern PFNGLGENPROGRAMSARBPROC				glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC	glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB;

typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
extern PFNGLCLIENTACTIVETEXTUREARBPROC					glClientActiveTextureARB;

typedef void ( APIENTRYP PFNGLPRIORITIZETEXTURESPROC )(GLsizei n, const GLuint *textures, const GLclampf *priorities);
extern PFNGLPRIORITIZETEXTURESPROC glPrioritizeTextures;

inline constexpr GLenum GL_ALPHA_TEST           = 0x0BC0;
typedef void ( APIENTRYP PFNGLALPHAFUNCPROC )(GLenum func, GLclampf ref);
extern PFNGLALPHAFUNCPROC glAlphaFunc;

typedef void ( APIENTRYP PFNGLPIXELZOOMPROC )(GLfloat xfactor, GLfloat yfactor);
extern PFNGLPIXELZOOMPROC glPixelZoom;

inline constexpr GLenum GL_INTENSITY8           = 0x804B;
inline constexpr GLenum GL_LUMINANCE8_ALPHA8    = 0x8045;
inline constexpr GLenum GL_LUMINANCE8           = 0x8040;
inline constexpr GLenum GL_COLOR_INDEX          = 0x1900;
inline constexpr GLenum GL_COLOR_INDEX8_EXT     = 0x80E5;
inline constexpr GLenum GL_BGR_EXT              = 0x80E0;
inline constexpr GLenum GL_BGRA_EXT             = 0x80E1;
inline constexpr GLenum GL_ALPHA8               = 0x803C;
inline constexpr GLenum GL_COMPRESSED_RGB_ARB   = 0x84ED;
inline constexpr GLenum GL_COMPRESSED_RGBA_ARB  = 0x84EE;
inline constexpr GLenum GL_LUMINANCE_ALPHA      = 0x190A;
inline constexpr GLenum GL_LUMINANCE            = 0x1909;
inline constexpr GLenum GL_NORMAL_ARRAY		    = 0x8075;
inline constexpr GLenum GL_COLOR_ARRAY		    = 0x8076;
inline constexpr GLenum GL_TEXTURE_COORD_ARRAY  = 0x8078;

typedef void ( APIENTRYP PFNGLENABLECLIENTSTATEPROC )(GLenum array);
typedef void ( APIENTRYP PFNGLDISABLECLIENTSTATEPROC)(GLenum array);
extern PFNGLENABLECLIENTSTATEPROC           glEnableClientState;
extern PFNGLDISABLECLIENTSTATEPROC          glDisableClientState;

inline constexpr GLenum GL_MODELVIEW    = 0x1700;
inline constexpr GLenum GL_PROJECTION   = 0x1701;
typedef void ( APIENTRYP PFNGLMATRIXMODEPROC )(GLenum mode);
typedef void ( APIENTRYP PFNGLPUSHMATRIXPROC )(void);
typedef void ( APIENTRYP PFNGLPOPMATRIXPROC )(void);
typedef void ( APIENTRYP PFNGLLOADIDENTITYPROC )(void);
typedef void ( APIENTRYP PFNGLLOADMATRIXFPROC )(const GLfloat *m);
typedef void ( APIENTRYP PFNGLORTHOPROC )(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
extern PFNGLMATRIXMODEPROC      glMatrixMode;
extern PFNGLPUSHMATRIXPROC      glPushMatrix;
extern PFNGLPOPMATRIXPROC       glPopMatrix;
extern PFNGLLOADIDENTITYPROC    glLoadIdentity;
extern PFNGLLOADMATRIXFPROC     glLoadMatrixf;
extern PFNGLORTHOPROC           glOrtho;

inline constexpr GLenum GL_S    = 0x2000;
inline constexpr GLenum GL_T    = 0x2001;
inline constexpr GLenum GL_R    = 0x2002;
inline constexpr GLenum GL_Q    = 0x2003;

inline constexpr GLenum GL_MODULATE = 0x2100;
inline constexpr GLenum GL_DECAL    = 0x2101;
inline constexpr GLenum GL_COMBINE_EXT  = 0x8570;
inline constexpr GLenum GL_COMBINE_ARB  = 0x8570;
inline constexpr GLenum GL_COMBINE_RGB_ARB  = 0x8571;
inline constexpr GLenum GL_ADD  = 0x0104;
inline constexpr GLenum GL_OBJECT_LINEAR    = 0x2401;
inline constexpr GLenum GL_OBJECT_PLANE = 0x2501;
inline constexpr GLenum GL_PRIMARY_COLOR_ARB    = 0x8577;
inline constexpr GLenum GL_DOT3_RGBA_ARB    = 0x86AF;
inline constexpr GLenum GL_SOURCE0_RGB_ARB  = 0x8580;
inline constexpr GLenum GL_SOURCE1_RGB_ARB  = 0x8581;
inline constexpr GLenum GL_PREVIOUS_ARB = 0x8578;
inline constexpr GLenum GL_OPERAND0_RGB_ARB = 0x8590;
inline constexpr GLenum GL_OPERAND1_RGB_ARB = 0x8591;
inline constexpr GLenum GL_RGB_SCALE_ARB    = 0x8573;
inline constexpr GLenum GL_ALPHA_SCALE  = 0x0D1C;
inline constexpr GLenum GL_REFLECTION_MAP_EXT   = 0x8512;
inline constexpr GLenum GL_TEXTURE_ENV  = 0x2300;
inline constexpr GLenum GL_TEXTURE_ENV_MODE = 0x2200;
inline constexpr GLenum GL_TEXTURE_GEN_MODE = 0x2500;
inline constexpr GLenum GL_TEXTURE_GEN_S    = 0x0C60;
inline constexpr GLenum GL_TEXTURE_GEN_T    = 0x0C61;
inline constexpr GLenum GL_TEXTURE_GEN_R    = 0x0C62;
inline constexpr GLenum GL_TEXTURE_GEN_Q    = 0x0C63;
inline constexpr GLenum GL_TEXTURE_ENV_COLOR    = 0x2201;
inline constexpr GLenum GL_COMBINE_ALPHA_ARB    = 0x8572;
inline constexpr GLenum GL_SOURCE0_ALPHA_ARB    = 0x8588;
inline constexpr GLenum GL_SOURCE1_ALPHA_ARB    = 0x8589;
inline constexpr GLenum GL_CONSTANT_ARB = 0x8576;
inline constexpr GLenum GL_OPERAND0_ALPHA_ARB   = 0x8598;
inline constexpr GLenum GL_OPERAND1_ALPHA_ARB   = 0x8599;  
typedef void ( APIENTRYP PFNGLTEXENVIPROC )(GLenum target, GLenum pname, GLint param);
typedef void ( APIENTRYP PFNGLTEXENVFVPROC )(GLenum target, GLenum pname, const GLfloat *params);
typedef void ( APIENTRYP PFNGLTEXGENFPROC )(GLenum coord, GLenum pname, GLfloat param);
typedef void ( APIENTRYP PFNGLTEXGENFVPROC )(GLenum coord, GLenum pname, const GLfloat *params);
extern PFNGLTEXENVIPROC     glTexEnvi;
extern PFNGLTEXENVFVPROC    glTexEnvfv;
extern PFNGLTEXGENFPROC     glTexGenf;
extern PFNGLTEXGENFVPROC    glTexGenfv;

inline constexpr GLenum GL_POLYGON				    = 0x0009;
inline constexpr GLenum GL_LIGHTING                 = 0x0B50;
inline constexpr GLenum GL_LINE_STIPPLE				= 0x0B24;
inline constexpr GLenum GL_SMOOTH                   = 0x1D01;
inline constexpr GLbitfield GL_ALL_ATTRIB_BITS			= 0xFFFFFFFF;
typedef void ( APIENTRYP PFNGLSHADEMODELPROC )(GLenum mode);
typedef void ( APIENTRYP PFNGLRASTERPOS2FPROC )(GLfloat x, GLfloat y);
typedef void ( APIENTRYP PFNGLDRAWPIXELSPROC )(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
typedef void ( APIENTRYP PFNGLPOPATTRIBPROC )(void);
typedef void ( APIENTRYP PFNGLPUSHATTRIBPROC )(GLbitfield mask);
typedef void ( APIENTRYP PFNGLVERTEXPOINTERPROC )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void ( APIENTRYP PFNGLTEXCOORDPOINTERPROC )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void ( APIENTRYP PFNGLNORMALPOINTERPROC )(GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void ( APIENTRYP PFNGLCOLORPOINTERPROC )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void ( APIENTRYP PFNGLVERTEX2FPROC )(GLfloat x, GLfloat y);
typedef void ( APIENTRYP PFNGLVERTEX2FVPROC )(const GLfloat *v);
typedef void ( APIENTRYP PFNGLVERTEX3FPROC )(GLfloat x, GLfloat y, GLfloat z);
typedef void ( APIENTRYP PFNGLVERTEX3FVPROC )(const GLfloat *v);
typedef void ( APIENTRYP PFNGLTEXCOORD2FPROC )(GLfloat s, GLfloat t);
typedef void ( APIENTRYP PFNGLTEXCOORD2FVPROC )(const GLfloat *v);
typedef void ( APIENTRYP PFNGLCOLOR3FPROC )(GLfloat red, GLfloat green, GLfloat blue);
typedef void ( APIENTRYP PFNGLCOLOR3FVPROC )(const GLfloat *v);
typedef void ( APIENTRYP PFNGLCOLOR3UBVPROC )(const GLubyte *v);
typedef void ( APIENTRYP PFNGLCOLOR4FPROC )(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void ( APIENTRYP PFNGLCOLOR4FVPROC )(const GLfloat *v);
typedef void ( APIENTRYP PFNGLCOLOR4UBVPROC )(const GLubyte *v);
extern PFNGLSHADEMODELPROC         glShadeModel;
extern PFNGLRASTERPOS2FPROC        glRasterPos2f;
extern PFNGLDRAWPIXELSPROC         glDrawPixels;
extern PFNGLPOPATTRIBPROC          glPopAttrib;
extern PFNGLPUSHATTRIBPROC         glPushAttrib;
extern PFNGLVERTEXPOINTERPROC      glVertexPointer;
extern PFNGLTEXCOORDPOINTERPROC    glTexCoordPointer;
extern PFNGLNORMALPOINTERPROC      glNormalPointer;
extern PFNGLCOLORPOINTERPROC       glColorPointer;
extern PFNGLVERTEX2FPROC           glVertex2f;
extern PFNGLVERTEX2FVPROC          glVertex2fv;
extern PFNGLVERTEX3FPROC           glVertex3f;
extern PFNGLVERTEX3FVPROC          glVertex3fv;
extern PFNGLTEXCOORD2FPROC         glTexCoord2f;
extern PFNGLTEXCOORD2FVPROC        glTexCoord2fv;
extern PFNGLCOLOR3FPROC            glColor3f;
extern PFNGLCOLOR3FVPROC           glColor3fv;
extern PFNGLCOLOR3UBVPROC          glColor3ubv;
extern PFNGLCOLOR4FPROC            glColor4f;
extern PFNGLCOLOR4FVPROC           glColor4fv;
extern PFNGLCOLOR4UBVPROC          glColor4ubv;

typedef void ( APIENTRYP PFNGLBEGINPROC )(GLenum mode);
typedef void ( APIENTRYP PFNGLENDPROC )(void);
typedef void ( APIENTRYP PFNGLARRAYELEMENTPROC )(GLint i);
extern PFNGLBEGINPROC           glBegin;
extern PFNGLENDPROC             glEnd;
extern PFNGLARRAYELEMENTPROC    glArrayElement;

/*

// shared texture palette

// ARB_texture_compression
// multitexture
extern PFNGLMULTITEXCOORD2FARBPROC						glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC						glMultiTexCoord2fvARB;



//===========================================================================

// windows systems use a function pointer for each call so we can do our log file intercepts
extern  void ( APIENTRY * glCallList )(GLuint list);
extern  void ( APIENTRY * glCallLists )(GLsizei n, GLenum type, const GLvoid *lists);
extern  void ( APIENTRY * glColor3ub )(GLubyte red, GLubyte green, GLubyte blue);
extern  void ( APIENTRY * glColor4ub )(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
extern  void ( APIENTRY * glColorMaterial )(GLenum face, GLenum mode);
extern  void ( APIENTRY * glCopyPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
extern  void ( APIENTRY * glDeleteLists )(GLuint list, GLsizei range);
extern  void ( APIENTRY * glEnableClientState )(GLenum array);
extern  void ( APIENTRY * glDisableClientState )(GLenum array);
extern  void ( APIENTRY * glEdgeFlag )(GLboolean flag);
extern  void ( APIENTRY * glEdgeFlagPointer )(GLsizei stride, const GLvoid *pointer);
extern  void ( APIENTRY * glEdgeFlagv )(const GLboolean *flag);
extern  void ( APIENTRY * glEndList )(void);
extern  void ( APIENTRY * glEvalCoord1d )(GLdouble u);
extern  void ( APIENTRY * glEvalCoord1dv )(const GLdouble *u);
extern  void ( APIENTRY * glEvalCoord1f )(GLfloat u);
extern  void ( APIENTRY * glEvalCoord1fv )(const GLfloat *u);
extern  void ( APIENTRY * glEvalCoord2d )(GLdouble u, GLdouble v);
extern  void ( APIENTRY * glEvalCoord2dv )(const GLdouble *u);
extern  void ( APIENTRY * glEvalCoord2f )(GLfloat u, GLfloat v);
extern  void ( APIENTRY * glEvalCoord2fv )(const GLfloat *u);
extern  void ( APIENTRY * glEvalMesh1 )(GLenum mode, GLint i1, GLint i2);
extern  void ( APIENTRY * glEvalMesh2 )(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
extern  void ( APIENTRY * glEvalPoint1 )(GLint i);
extern  void ( APIENTRY * glEvalPoint2 )(GLint i, GLint j);
extern  void ( APIENTRY * glFeedbackBuffer )(GLsizei size, GLenum type, GLfloat *buffer);
extern  void ( APIENTRY * glFogf )(GLenum pname, GLfloat param);
extern  void ( APIENTRY * glFogfv )(GLenum pname, const GLfloat *params);
extern  void ( APIENTRY * glFogi )(GLenum pname, GLint param);
extern  void ( APIENTRY * glFogiv )(GLenum pname, const GLint *params);
extern  void ( APIENTRY * glFrontFace )(GLenum mode);
extern  void ( APIENTRY * glFrustum )(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
extern  GLuint ( APIENTRY * glGenLists )(GLsizei range);
extern  void ( APIENTRY * glGenTextures )(GLsizei n, GLuint *textures);
extern  void ( APIENTRY * glGetBooleanv )(GLenum pname, GLboolean *params);
extern  void ( APIENTRY * glGetClipPlane )(GLenum plane, GLdouble *equation);
extern  void ( APIENTRY * glGetDoublev )(GLenum pname, GLdouble *params);
extern  void ( APIENTRY * glGetFloatv )(GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetLightfv )(GLenum light, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetLightiv )(GLenum light, GLenum pname, GLint *params);
extern  void ( APIENTRY * glGetMapdv )(GLenum target, GLenum query, GLdouble *v);
extern  void ( APIENTRY * glGetMapfv )(GLenum target, GLenum query, GLfloat *v);
extern  void ( APIENTRY * glGetMapiv )(GLenum target, GLenum query, GLint *v);
extern  void ( APIENTRY * glGetMaterialfv )(GLenum face, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetMaterialiv )(GLenum face, GLenum pname, GLint *params);
extern  void ( APIENTRY * glGetPixelMapfv )(GLenum map, GLfloat *values);
extern  void ( APIENTRY * glGetPixelMapuiv )(GLenum map, GLuint *values);
extern  void ( APIENTRY * glGetPixelMapusv )(GLenum map, GLushort *values);
extern  void ( APIENTRY * glGetPointerv )(GLenum pname, GLvoid* *params);
extern  void ( APIENTRY * glGetPolygonStipple )(GLubyte *mask);
extern  const GLubyte * ( APIENTRY *  )(GLenum name);
extern  void ( APIENTRY * glGetTexEnvfv )(GLenum target, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetTexEnviv )(GLenum target, GLenum pname, GLint *params);
extern  void ( APIENTRY * glGetTexGendv )(GLenum coord, GLenum pname, GLdouble *params);
extern  void ( APIENTRY * glGetTexGenfv )(GLenum coord, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetTexGeniv )(GLenum coord, GLenum pname, GLint *params);
extern  void ( APIENTRY * glGetTexImage )(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
extern  void ( APIENTRY * glGetTexLevelParameterfv )(GLenum target, GLint level, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetTexLevelParameteriv )(GLenum target, GLint level, GLenum pname, GLint *params);
extern  void ( APIENTRY * glGetTexParameterfv )(GLenum target, GLenum pname, GLfloat *params);
extern  void ( APIENTRY * glGetTexParameteriv )(GLenum target, GLenum pname, GLint *params);
extern  void ( APIENTRY * glHint )(GLenum target, GLenum mode);
extern  void ( APIENTRY * glIndexMask )(GLuint mask);
extern  void ( APIENTRY * glIndexPointer )(GLenum type, GLsizei stride, const GLvoid *pointer);
extern  void ( APIENTRY * glIndexd )(GLdouble c);
extern  void ( APIENTRY * glIndexdv )(const GLdouble *c);
extern  void ( APIENTRY * glIndexf )(GLfloat c);
extern  void ( APIENTRY * glIndexfv )(const GLfloat *c);
extern  void ( APIENTRY * glIndexi )(GLint c);
extern  void ( APIENTRY * glIndexiv )(const GLint *c);
extern  void ( APIENTRY * glIndexs )(GLshort c);
extern  void ( APIENTRY * glIndexsv )(const GLshort *c);
extern  void ( APIENTRY * glIndexub )(GLubyte c);
extern  void ( APIENTRY * glIndexubv )(const GLubyte *c);
extern  void ( APIENTRY * glInitNames )(void);
extern  void ( APIENTRY * glInterleavedArrays )(GLenum format, GLsizei stride, const GLvoid *pointer);
extern  GLboolean ( APIENTRY * glIsList )(GLuint list);
extern  GLboolean ( APIENTRY * glIsTexture )(GLuint texture);
extern  void ( APIENTRY * glLightModelf )(GLenum pname, GLfloat param);
extern  void ( APIENTRY * glLightModelfv )(GLenum pname, const GLfloat *params);
extern  void ( APIENTRY * glLightModeli )(GLenum pname, GLint param);
extern  void ( APIENTRY * glLightModeliv )(GLenum pname, const GLint *params);
extern  void ( APIENTRY * glLightf )(GLenum light, GLenum pname, GLfloat param);
extern  void ( APIENTRY * glLightfv )(GLenum light, GLenum pname, const GLfloat *params);
extern  void ( APIENTRY * glLighti )(GLenum light, GLenum pname, GLint param);
extern  void ( APIENTRY * glLightiv )(GLenum light, GLenum pname, const GLint *params);
extern  void ( APIENTRY * glLineStipple )(GLint factor, GLushort pattern);
extern  void ( APIENTRY * glLineWidth )(GLfloat width);
extern  void ( APIENTRY * glListBase )(GLuint base);
extern  void ( APIENTRY * glLoadName )(GLuint name);
extern  void ( APIENTRY * glLogicOp )(GLenum opcode);
extern  void ( APIENTRY * glMap1d )(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
extern  void ( APIENTRY * glMap1f )(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
extern  void ( APIENTRY * glMap2d )(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
extern  void ( APIENTRY * glMap2f )(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
extern  void ( APIENTRY * glMapGrid1d )(GLint un, GLdouble u1, GLdouble u2);
extern  void ( APIENTRY * glMapGrid1f )(GLint un, GLfloat u1, GLfloat u2);
extern  void ( APIENTRY * glMapGrid2d )(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
extern  void ( APIENTRY * glMapGrid2f )(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
extern  void ( APIENTRY * glMaterialf )(GLenum face, GLenum pname, GLfloat param);
extern  void ( APIENTRY * glMaterialfv )(GLenum face, GLenum pname, const GLfloat *params);
extern  void ( APIENTRY * glMateriali )(GLenum face, GLenum pname, GLint param);
extern  void ( APIENTRY * glMaterialiv )(GLenum face, GLenum pname, const GLint *params);
extern  void ( APIENTRY * glMultMatrixd )(const GLdouble *m);
extern  void ( APIENTRY * glMultMatrixf )(const GLfloat *m);
extern  void ( APIENTRY * glNewList )(GLuint list, GLenum mode);
extern  void ( APIENTRY * glNormal3b )(GLbyte nx, GLbyte ny, GLbyte nz);
extern  void ( APIENTRY * glNormal3bv )(const GLbyte *v);
extern  void ( APIENTRY * glNormal3d )(GLdouble nx, GLdouble ny, GLdouble nz);
extern  void ( APIENTRY * glNormal3dv )(const GLdouble *v);
extern  void ( APIENTRY * glNormal3f )(GLfloat nx, GLfloat ny, GLfloat nz);
extern  void ( APIENTRY * glNormal3fv )(const GLfloat *v);
extern  void ( APIENTRY * glNormal3i )(GLint nx, GLint ny, GLint nz);
extern  void ( APIENTRY * glNormal3iv )(const GLint *v);
extern  void ( APIENTRY * glNormal3s )(GLshort nx, GLshort ny, GLshort nz);
extern  void ( APIENTRY * glNormal3sv )(const GLshort *v);
extern  void ( APIENTRY * glPassThrough )(GLfloat token);
extern  void ( APIENTRY * glPixelMapfv )(GLenum map, GLsizei mapsize, const GLfloat *values);
extern  void ( APIENTRY * glPixelMapuiv )(GLenum map, GLsizei mapsize, const GLuint *values);
extern  void ( APIENTRY * glPixelMapusv )(GLenum map, GLsizei mapsize, const GLushort *values);
extern  void ( APIENTRY * glPixelStoref )(GLenum pname, GLfloat param);
extern  void ( APIENTRY * glPixelStorei )(GLenum pname, GLint param);
extern  void ( APIENTRY * glPixelTransferf )(GLenum pname, GLfloat param);
extern  void ( APIENTRY * glPixelTransferi )(GLenum pname, GLint param);
extern  void ( APIENTRY * glPixelZoom )(GLfloat xfactor, GLfloat yfactor);
extern  void ( APIENTRY * glPointSize )(GLfloat size);
extern  void ( APIENTRY * glPolygonStipple )(const GLubyte *mask);
extern  void ( APIENTRY * glPopAttrib )(void);
extern  void ( APIENTRY * glPushAttrib )(GLbitfield mask);
extern  void ( APIENTRY * glPopClientAttrib )(void);
extern  void ( APIENTRY * glPopName )(void);
extern  void ( APIENTRY * glPushClientAttrib )(GLbitfield mask);
extern  void ( APIENTRY * glPushName )(GLuint name);
extern  void ( APIENTRY * glRasterPos2d )(GLdouble x, GLdouble y);
extern  void ( APIENTRY * glRasterPos2dv )(const GLdouble *v);
extern  void ( APIENTRY * glRasterPos2fv )(const GLfloat *v);
extern  void ( APIENTRY * glRasterPos2i )(GLint x, GLint y);
extern  void ( APIENTRY * glRasterPos2iv )(const GLint *v);
extern  void ( APIENTRY * glRasterPos2s )(GLshort x, GLshort y);
extern  void ( APIENTRY * glRasterPos2sv )(const GLshort *v);
extern  void ( APIENTRY * glRasterPos3d )(GLdouble x, GLdouble y, GLdouble z);
extern  void ( APIENTRY * glRasterPos3dv )(const GLdouble *v);
extern  void ( APIENTRY * glRasterPos3f )(GLfloat x, GLfloat y, GLfloat z);
extern  void ( APIENTRY * glRasterPos3fv )(const GLfloat *v);
extern  void ( APIENTRY * glRasterPos3i )(GLint x, GLint y, GLint z);
extern  void ( APIENTRY * glRasterPos3iv )(const GLint *v);
extern  void ( APIENTRY * glRasterPos3s )(GLshort x, GLshort y, GLshort z);
extern  void ( APIENTRY * glRasterPos3sv )(const GLshort *v);
extern  void ( APIENTRY * glRasterPos4d )(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
extern  void ( APIENTRY * glRasterPos4dv )(const GLdouble *v);
extern  void ( APIENTRY * glRasterPos4f )(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern  void ( APIENTRY * glRasterPos4fv )(const GLfloat *v);
extern  void ( APIENTRY * glRasterPos4i )(GLint x, GLint y, GLint z, GLint w);
extern  void ( APIENTRY * glRasterPos4iv )(const GLint *v);
extern  void ( APIENTRY * glRasterPos4s )(GLshort x, GLshort y, GLshort z, GLshort w);
extern  void ( APIENTRY * glRasterPos4sv )(const GLshort *v);
extern  void ( APIENTRY * glReadBuffer )(GLenum mode);
extern  void ( APIENTRY * glReadPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
extern  void ( APIENTRY * glRectd )(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
extern  void ( APIENTRY * glRectdv )(const GLdouble *v1, const GLdouble *v2);
extern  void ( APIENTRY * glRectf )(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
extern  void ( APIENTRY * glRectfv )(const GLfloat *v1, const GLfloat *v2);
extern  void ( APIENTRY * glRecti )(GLint x1, GLint y1, GLint x2, GLint y2);
extern  void ( APIENTRY * glRectiv )(const GLint *v1, const GLint *v2);
extern  void ( APIENTRY * glRects )(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
extern  void ( APIENTRY * glRectsv )(const GLshort *v1, const GLshort *v2);
extern  GLint ( APIENTRY * glRenderMode )(GLenum mode);
extern  void ( APIENTRY * glRotated )(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
extern  void ( APIENTRY * glRotatef )(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
extern  void ( APIENTRY * glScaled )(GLdouble x, GLdouble y, GLdouble z);
extern  void ( APIENTRY * glScalef )(GLfloat x, GLfloat y, GLfloat z);
extern  void ( APIENTRY * glSelectBuffer )(GLsizei size, GLuint *buffer);
extern  void ( APIENTRY * glTexCoord1i )(GLint s);
extern  void ( APIENTRY * glTexCoord1iv )(const GLint *v);
extern  void ( APIENTRY * glTexCoord1s )(GLshort s);
extern  void ( APIENTRY * glTexCoord1sv )(const GLshort *v);
extern  void ( APIENTRY * glTexCoord2d )(GLdouble s, GLdouble t);
extern  void ( APIENTRY * glTexCoord2dv )(const GLdouble *v);
extern  void ( APIENTRY * glTexCoord2i )(GLint s, GLint t);
extern  void ( APIENTRY * glTexCoord2iv )(const GLint *v);
extern  void ( APIENTRY * glTexCoord2s )(GLshort s, GLshort t);
extern  void ( APIENTRY * glTexCoord2sv )(const GLshort *v);
extern  void ( APIENTRY * glTexCoord3d )(GLdouble s, GLdouble t, GLdouble r);
extern  void ( APIENTRY * glTexCoord3dv )(const GLdouble *v);
extern  void ( APIENTRY * glTexCoord3f )(GLfloat s, GLfloat t, GLfloat r);
extern  void ( APIENTRY * glTexCoord3fv )(const GLfloat *v);
extern  void ( APIENTRY * glTexCoord3i )(GLint s, GLint t, GLint r);
extern  void ( APIENTRY * glTexCoord3iv )(const GLint *v);
extern  void ( APIENTRY * glTexCoord3s )(GLshort s, GLshort t, GLshort r);
extern  void ( APIENTRY * glTexCoord3sv )(const GLshort *v);
extern  void ( APIENTRY * glTexCoord4d )(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
extern  void ( APIENTRY * glTexCoord4dv )(const GLdouble *v);
extern  void ( APIENTRY * glTexCoord4f )(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern  void ( APIENTRY * glTexCoord4fv )(const GLfloat *v);
extern  void ( APIENTRY * glTexCoord4i )(GLint s, GLint t, GLint r, GLint q);
extern  void ( APIENTRY * glTexCoord4iv )(const GLint *v);
extern  void ( APIENTRY * glTexCoord4s )(GLshort s, GLshort t, GLshort r, GLshort q);
extern  void ( APIENTRY * glTexCoord4sv )(const GLshort *v);
extern  void ( APIENTRY * glTexEnvf )(GLenum target, GLenum pname, GLfloat param);
extern  void ( APIENTRY * glTexEnviv )(GLenum target, GLenum pname, const GLint *params);
extern  void ( APIENTRY * glTexGend )(GLenum coord, GLenum pname, GLdouble param);
extern  void ( APIENTRY * glTexGendv )(GLenum coord, GLenum pname, const GLdouble *params);
extern  void ( APIENTRY * glTranslatef )(GLfloat x, GLfloat y, GLfloat z);
extern  void ( APIENTRY * glVertex4f )(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern  void ( APIENTRY * glVertex4fv )(const GLfloat *v);
*/
#endif
