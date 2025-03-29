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

In addition, the Doom 3 Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, 
you may contact in writing id Software LLC, 
c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
/*
** QGL.H
*/

#ifndef __QGL_H__
#define __QGL_H__

#if defined( _WIN32 )
#	include <gl/glcorearb.h>
#else
#	include <GL/glcorearb.h>
#endif

typedef struct 
{
	int			width;
	int			height;
	bool		fullScreen;
	bool		stereo;
	int			displayHz;
	int			multiSamples;
} glimpParms_t;

extern void         LoadOpenGLFunctions( void );
extern void			InitOpenGLDebugOutput( void );
extern void         Sys_StartUpOpenGLAPI( void );
extern void         Sys_ShutDownOpenGLAPI( void );
extern const bool   Sys_InitOpenGLContext( glimpParms_t parms );
extern void			Sys_SwapBuffersGL( void );
extern void         Sys_ShutDownOpenGLContext( void ); 
extern void         GL_CheckErrors( void );
extern void         GL_SelectTexture( int unit );
extern void         GL_ClearStateDelta( void );
extern void         GL_State( int stateVector );
extern void         GL_TexEnv( int env );
extern void         GL_Cull( int cullType );

#ifndef GL_MAX_TEXTURE_COORDS
#define GL_MAX_TEXTURE_COORDS             0x8871
#endif //!GL_MAX_TEXTURE_COORDS

#ifndef GL_MAX_TEXTURE_UNITS
#define GL_MAX_TEXTURE_UNITS              0x84E2
#endif //!GL_MAX_TEXTURE_UNITS

#ifndef GL_DEPTH_BOUNDS_TEST_EXT
#define GL_DEPTH_BOUNDS_TEST_EXT			0x8890
#endif //!GL_DEPTH_BOUNDS_TEST_EXT

// theise are not core and need to be deprecated
#if 1
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLACTIVESTENCILFACEEXTPROC) (GLenum face);
typedef void (APIENTRYP PFNGLPROGRAMSTRINGARBPROC) (GLenum target, GLenum format, GLsizei len, const GLvoid *string);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRYP PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRYP PFNGLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRYP PFNGLDEPTHBOUNDSEXTPROC) (GLclampd zmin, GLclampd zmax);
#endif 

///
/// OpenGL utilities
///
extern GLint 			glGetInteger( const GLenum pname );
extern GLboolean		glSetState( const GLenum flag, const GLboolean enable );
extern GLenum 		glGetActiveTexture( void );
extern const GLuint	glGetBuferBinding( const GLenum target );
extern const GLuint	glGetTextureBinding( const GLenum target );

extern PFNGLGETERRORPROC					glGetError;
extern PFNGLGETSTRINGPROC					glGetString;
extern PFNGLGETFLOATVPROC					glGetFloatv;
extern PFNGLGETINTEGERVPROC					glGetIntegerv;
extern PFNGLENABLEPROC						glEnable;
extern PFNGLDISABLEPROC						glDisable;
extern PFNGLISENABLEDPROC					glIsEnabled;
extern PFNGLCLEARPROC						glClear;
extern PFNGLFLUSHPROC						glFlush;
extern PFNGLFINISHPROC						glFinish;
extern PFNGLVIEWPORTPROC					glViewport;

extern PFNGLCLEARCOLORPROC					glClearColor;
extern PFNGLCOLORMASKPROC					glColorMask;

// blend
extern PFNGLBLENDFUNCPROC					glBlendFunc;

// depth
extern PFNGLCLEARDEPTHPROC					glClearDepth;
extern PFNGLDEPTHMASKPROC					glDepthMask;
extern PFNGLDEPTHFUNCPROC					glDepthFunc;
extern PFNGLDEPTHRANGEPROC					glDepthRange;

// stencil
extern PFNGLSCISSORPROC                     glScissor;
extern PFNGLSTENCILOPPROC					glStencilOp;
extern PFNGLSTENCILFUNCPROC					glStencilFunc;
extern PFNGLSTENCILMASKPROC					glStencilMask;
extern PFNGLCLEARSTENCILPROC				glClearStencil;
extern PFNGLSTENCILOPSEPARATEPROC			glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC			glStencilFuncSeparate;

// poligon 
extern PFNGLCULLFACEPROC					glCullFace;
extern PFNGLPOLYGONMODEPROC					glPolygonMode;
extern PFNGLPOLYGONOFFSETPROC				glPolygonOffset;
extern PFNGLLINEWIDTHPROC					glLineWidth;
extern PFNGLPOINTSIZEPROC					glPointSize;

// draw buffers 
extern PFNGLDRAWBUFFERPROC					glDrawBuffer;
extern PFNGLREADBUFFERPROC					glReadBuffer;
extern PFNGLREADPIXELSPROC					glReadPixels;
extern PFNGLPIXELSTOREIPROC					glPixelStorei;

// ARB_vertex_buffer_object
extern PFNGLBINDBUFFERPROC					glBindBuffer;
extern PFNGLDELETEBUFFERSPROC	 			glDeleteBuffers;
extern PFNGLGENBUFFERSPROC	 				glGenBuffers;
extern PFNGLISBUFFERPROC	 				glIsBuffer;
extern PFNGLBUFFERDATAPROC	 				glBufferData;
extern PFNGLBUFFERSUBDATAPROC	 			glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC	 		glGetBufferSubData;
extern PFNGLCOPYBUFFERSUBDATAPROC			glCopyBufferSubData;

// BEATO Begin
extern PFNGLCREATEBUFFERSPROC				glCreateBuffers;
extern PFNGLNAMEDBUFFERSTORAGEPROC			glNamedBufferStorage;
extern PFNGLNAMEDBUFFERSUBDATAPROC			glNamedBufferSubData;
extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC		glCopyNamedBufferSubData;
extern PFNGLCLEARNAMEDBUFFERDATAPROC		glClearNamedBufferData;
extern PFNGLMAPNAMEDBUFFERRANGEPROC			glMapNamedBufferRange;
extern PFNGLUNMAPNAMEDBUFFERPROC			glUnmapNamedBuffer;
// BEATO End

extern PFNGLMAPBUFFERPROC	 				glMapBuffer;
extern PFNGLUNMAPBUFFERPROC 				glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC 		glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC 			glGetBufferPointerv;

// Draw Command
extern PFNGLDRAWELEMENTSPROC				glDrawElements;

// Textures
extern PFNGLACTIVETEXTUREPROC				glActiveTexture;
extern PFNGLGENTEXTURESPROC					glGenTextures;
extern PFNGLDELETETEXTURESPROC				glDeleteTextures;
extern PFNGLBINDTEXTUREPROC					glBindTexture;
extern PFNGLTEXIMAGE2DPROC					glTexImage2D;
extern PFNGLTEXIMAGE3DPROC					glTexImage3D;
extern PFNGLTEXSUBIMAGE2DPROC				glTexSubImage2D;
extern PFNGLTEXSUBIMAGE3DPROC				glTexSubImage3D;
extern PFNGLTEXPARAMETERIPROC				glTexParameteri;
extern PFNGLTEXPARAMETERFPROC				glTexParameterf;
extern PFNGLTEXPARAMETERFVPROC				glTexParameterfv;
extern PFNGLGETTEXIMAGEPROC					glGetTexImage;
extern PFNGLCOPYTEXIMAGE2DPROC				glCopyTexImage2D;
//extern PFNGLCOPYTEXIMAGE3DPROC				glCopyTexImage3D;
extern PFNGLCOPYTEXSUBIMAGE2DPROC			glCopyTexSubImage2D;
extern PFNGLCOPYTEXSUBIMAGE3DPROC			glCopyTexSubImage3D;

// ARB_texture_compression
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC		glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC		glCompressedTexImage3D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC		glGetCompressedTexImage;

//===========================================================================
// DEPRECATE TO REMOVE and WORK ARROUND
//===========================================================================
// GL_EXT_depth_bounds_test
extern PFNGLDEPTHBOUNDSEXTPROC              glDepthBoundsEXT;

// EXT_stencil_two_side
extern PFNGLACTIVESTENCILFACEEXTPROC		glActiveStencilFaceEXT;

// ARB_vertex_program / ARB_fragment_program
extern PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
extern PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC				glBindProgramARB;
extern PFNGLGENPROGRAMSARBPROC				glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC	glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB;

extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC			glMultiTexCoord2fvARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;

extern void (*glEnd)(void);
extern void (*glBegin)(GLenum mode);
extern void (*glArrayElement)(GLint i);
extern void (*glPopMatrix)(void);
extern void (*glPushMatrix)(void);
extern void (*glPopAttrib)(void);
extern void (*glPushAttrib)(GLbitfield mask);
extern void (*glColor3f)(GLfloat red, GLfloat green, GLfloat blue);
extern void (*glColor3fv)(const GLfloat *v);
extern void (*glColor3ubv)(const GLubyte *v);
extern void (*glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (*glColor4fv)(const GLfloat *v);
extern void (*glColor4ubv)(const GLubyte *v);
extern void (*glVertex2f)(GLfloat x, GLfloat y);
extern void (*glVertex3f)(GLfloat x, GLfloat y, GLfloat z);
extern void (*glVertex3fv)(const GLfloat *v);
extern void (*glTexCoord2f)(GLfloat s, GLfloat t);
extern void (*glTexCoord2fv)(const GLfloat *v);
extern void (*glMatrixMode)(GLenum mode);
extern void (*glLoadMatrixf)(const GLfloat *m);
extern void (*glLoadIdentity)(void);
extern void (*glTexEnvi)(GLenum target, GLenum pname, GLint param);
extern void (*glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
extern void (*glTexGenfv)(GLenum coord, GLenum pname, const GLfloat *params);
extern void (*glTexGenf)(GLenum coord, GLenum pname, GLfloat param);
extern void (*glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (*glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (*glNormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (*glEnableClientState)(GLenum array);
extern void (*glDisableClientState)(GLenum array);
extern void (*glPrioritizeTextures)(GLsizei n, const GLuint *textures, const GLclampf *priorities);
extern void (*glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (*glAlphaFunc)(GLenum func, GLclampf ref);
extern void (*glOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
extern void (*glDrawPixels)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
extern void (*glRasterPos2f)(GLfloat x, GLfloat y);
extern void (*glPixelZoom)(GLfloat xfactor, GLfloat yfactor);
extern void (*glShadeModel)(GLenum mode);

#define GL_ALPHA_TEST						0x0BC0
#define GL_VERTEX_PROGRAM_ARB             	0x8620
#define GL_FRAGMENT_PROGRAM_ARB           	0x8804
#define GL_PROGRAM_FORMAT_ASCII_ARB       	0x8875
#define GL_PROGRAM_ERROR_POSITION_ARB     	0x864B
#define GL_PROGRAM_ERROR_STRING_ARB       	0x8874
#define GL_TEXTURE_ENV						0x2300
#define GL_COMBINE_ARB                    	0x8570
#define GL_COMBINE_RGB_ARB                	0x8571
#define GL_SOURCE0_RGB_ARB                	0x8580
#define GL_SOURCE1_RGB_ARB                	0x8581
#define GL_OPERAND0_RGB_ARB               	0x8590
#define GL_OPERAND1_RGB_ARB               	0x8591
#define GL_RGB_SCALE_ARB                  	0x8573
#define GL_MODULATE							0x2100
#define GL_VERTEX_ARRAY						0x8074
#define GL_NORMAL_ARRAY						0x8075
#define GL_COLOR_ARRAY						0x8076
#define GL_INDEX_ARRAY						0x8077
#define GL_TEXTURE_COORD_ARRAY				0x8078
#define GL_TEXTURE_ENV_COLOR				0x2201
#define GL_PRIMARY_COLOR_ARB              	0x8577
#define GL_TEXTURE_ENV_MODE					0x2200
#define GL_S								0x2000
#define GL_T								0x2001
#define GL_R								0x2002
#define GL_Q								0x2003
#define GL_DOT3_RGBA_ARB                  	0x86AF
#define GL_TEXTURE_GEN_S					0x0C60
#define GL_TEXTURE_GEN_T					0x0C61
#define GL_TEXTURE_GEN_R					0x0C62
#define GL_TEXTURE_GEN_Q					0x0C63
#define GL_TEXTURE_GEN_MODE					0x2500
#define GL_REFLECTION_MAP_EXT             	0x8512
#define GL_OBJECT_LINEAR					0x2401
#define GL_OBJECT_PLANE						0x2501
#define GL_MATRIX_MODE						0x0BA0
#define GL_MODELVIEW						0x1700
#define GL_PROJECTION						0x1701
#define GL_TEXTURE							0x1702
#define GL_PREVIOUS_ARB                   	0x8578
#define GL_CONSTANT_ARB                   	0x8576
#define GL_COMBINE_ALPHA_ARB              	0x8572
#define GL_SOURCE0_ALPHA_ARB              	0x8588
#define GL_SOURCE1_ALPHA_ARB              	0x8589
#define GL_OPERAND0_ALPHA_ARB             	0x8598
#define GL_OPERAND1_ALPHA_ARB             	0x8599
#define GL_ALPHA_SCALE						0x0D1C
#define GL_COMBINE_EXT                    	0x8570
#define GL_DECAL							0x2101
#define GL_ADD								0x0104
#define GL_ALL_ATTRIB_BITS					0xFFFFFFFF
#define GL_POLYGON							0x0009
#define GL_INTENSITY8				        0x804B
#define GL_LUMINANCE_ALPHA			        0x190A
#define GL_LUMINANCE8_ALPHA8		        0x8045
#define GL_LUMINANCE				        0x1909
#define GL_LUMINANCE8				        0x8040
#define GL_ALPHA8					        0x803C
#define GL_SMOOTH				            0x1D01
#define GL_LINE_STIPPLE				        0x0B24
#define GL_LIGHTING				            0x0B50

#endif // !__QGL_H__
