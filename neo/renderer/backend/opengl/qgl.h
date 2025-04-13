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

///
/// OpenGL utilities
///
extern GLint 		glGetInteger( const GLenum pname );
extern GLboolean	glSetState( const GLenum flag, const GLboolean enable );
extern GLenum 		glGetActiveTexture( void );
extern const GLuint	glGetBuferBinding( const GLenum target );
extern const GLuint	glGetTextureBinding( const GLenum target );

extern PFNGLGETERRORPROC						glGetError;
extern PFNGLGETSTRINGPROC						glGetString;
extern PFNGLGETFLOATVPROC						glGetFloatv;
extern PFNGLGETINTEGERVPROC						glGetIntegerv;
extern PFNGLENABLEPROC							glEnable;
extern PFNGLDISABLEPROC							glDisable;
extern PFNGLISENABLEDPROC						glIsEnabled;
extern PFNGLCLEARPROC							glClear;
extern PFNGLFLUSHPROC							glFlush;
extern PFNGLFINISHPROC							glFinish;
extern PFNGLVIEWPORTPROC						glViewport;

extern PFNGLCLEARCOLORPROC						glClearColor;
extern PFNGLCOLORMASKPROC						glColorMask;

// blend
extern PFNGLBLENDFUNCPROC						glBlendFunc;

// depth
extern PFNGLCLEARDEPTHPROC						glClearDepth;
extern PFNGLDEPTHMASKPROC						glDepthMask;
extern PFNGLDEPTHFUNCPROC						glDepthFunc;
extern PFNGLDEPTHRANGEPROC						glDepthRange;

// stencil
extern PFNGLSCISSORPROC                     	glScissor;
extern PFNGLSTENCILOPPROC						glStencilOp;
extern PFNGLSTENCILFUNCPROC						glStencilFunc;
extern PFNGLSTENCILMASKPROC						glStencilMask;
extern PFNGLCLEARSTENCILPROC					glClearStencil;
extern PFNGLSTENCILOPSEPARATEPROC				glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC				glStencilFuncSeparate;

// poligon 
extern PFNGLCULLFACEPROC						glCullFace;
extern PFNGLPOLYGONMODEPROC						glPolygonMode;
extern PFNGLPOLYGONOFFSETPROC					glPolygonOffset;
extern PFNGLLINEWIDTHPROC						glLineWidth;
extern PFNGLPOINTSIZEPROC						glPointSize;

// draw buffers 
extern PFNGLDRAWBUFFERPROC						glDrawBuffer;
extern PFNGLREADBUFFERPROC						glReadBuffer;
extern PFNGLREADPIXELSPROC						glReadPixels;
extern PFNGLPIXELSTOREIPROC						glPixelStorei;

// ARB_vertex_buffer_object, ARB_direct_state_access ARB_buffer_storage, ARB_uniform_buffer_object ARB_map_buffer_range GL_ARB_multi_bind
extern PFNGLBINDBUFFERPROC						glBindBuffer;
extern PFNGLBINDBUFFERRANGEPROC            		glBindBufferRange;
extern PFNGLBINDBUFFERSRANGEPROC            	glBindBuffersRange;
extern PFNGLCREATEBUFFERSPROC					glCreateBuffers;
extern PFNGLDELETEBUFFERSPROC	 				glDeleteBuffers;
extern PFNGLISBUFFERPROC	 					glIsBuffer;
extern PFNGLNAMEDBUFFERSTORAGEPROC				glNamedBufferStorage;
extern PFNGLNAMEDBUFFERDATAPROC					glNamedBufferData;
extern PFNGLNAMEDBUFFERSUBDATAPROC				glNamedBufferSubData;
extern PFNGLCOPYNAMEDBUFFERSUBDATAPROC			glCopyNamedBufferSubData;
extern PFNGLCLEARNAMEDBUFFERDATAPROC			glClearNamedBufferData;
extern PFNGLMAPNAMEDBUFFERRANGEPROC				glMapNamedBufferRange;
extern PFNGLUNMAPNAMEDBUFFERPROC				glUnmapNamedBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC 			glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC 				glGetBufferPointerv;

// ARB_vertex_array_object, ARB_direct_state_access
extern PFNGLBINDVERTEXARRAYPROC					glBindVertexArray;
extern PFNGLCREATEVERTEXARRAYSPROC				glCreateVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC				glDeleteVertexArrays;
extern PFNGLISVERTEXARRAYPROC					glIsVertexArray;
extern PFNGLENABLEVERTEXARRAYATTRIBPROC			glEnableVertexArrayAttrib;
extern PFNGLDISABLEVERTEXARRAYATTRIBPROC		glDisableVertexArrayAttrib;
extern PFNGLVERTEXARRAYATTRIBBINDINGPROC		glVertexArrayAttribBinding;
extern PFNGLVERTEXARRAYATTRIBFORMATPROC			glVertexArrayAttribFormat;
extern PFNGLVERTEXARRAYVERTEXBUFFERPROC			glVertexArrayVertexBuffer;
extern PFNGLVERTEXARRAYELEMENTBUFFERPROC		glVertexArrayElementBuffer;

// Draw Command
extern PFNGLDRAWELEMENTSPROC					glDrawElements;
// ARB_draw_elements_base_vertex
extern PFNGLDRAWELEMENTSBASEVERTEXPROC			glDrawElementsBaseVertex;
// ARB_draw_indirect
extern PFNGLDRAWELEMENTSINDIRECTPROC			glDrawElementsIndirect;


// Textures
// ARB_texture_storage, ARB_direct_state_access, ARB_multi_bind, ARB_texture_compression, ARB_texture_storage_multisample
extern PFNGLBINDTEXTUREPROC						glBindTexture;
extern PFNGLBINDTEXTURESPROC					glBindTextures;
extern PFNGLCREATETEXTURESPROC					glCreateTextures;
extern PFNGLDELETETEXTURESPROC					glDeleteTextures;
extern PFNGLTEXTURESTORAGE1DPROC				glTextureStorage1D;
extern PFNGLTEXTURESTORAGE2DPROC				glTextureStorage2D;
extern PFNGLTEXTURESTORAGE3DPROC				glTextureStorage3D;
extern PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC		glTextureStorage2DMultisample;
extern PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC		glTextureStorage3DMultisample;
extern PFNGLTEXTURESUBIMAGE1DPROC				glTextureSubImage1D;
extern PFNGLTEXTURESUBIMAGE2DPROC				glTextureSubImage2D;
extern PFNGLTEXTURESUBIMAGE3DPROC				glTextureSubImage3D;
extern PFNGLTEXTUREPARAMETERIPROC				glTextureParameteri;
extern PFNGLTEXTUREPARAMETERFPROC				glTextureParameterf;
extern PFNGLGETTEXTUREIMAGEPROC					glGetTextureImage;

// texture handler
extern PFNGLGETTEXTUREHANDLEARBPROC				glGetTextureHandleARB;
extern PFNGLGETTEXTURESAMPLERHANDLEARBPROC		glGetTextureSamplerHandleARB;
extern PFNGLMAKETEXTUREHANDLERESIDENTARBPROC	glMakeTextureHandleResidentARB;
extern PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC	glMakeTextureHandleNonResidentARB;
extern PFNGLMAKEIMAGEHANDLERESIDENTARBPROC 		glMakeImageHandleResidentARB;
extern PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC 	glMakeImageHandleNonResidentARB;
extern PFNGLUNIFORMHANDLEUI64ARBPROC 			glUniformHandleui64ARB;
extern PFNGLUNIFORMHANDLEUI64VARBPROC 			glUniformHandleui64vARB;
extern PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC 	glProgramUniformHandleui64ARB;
extern PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC 	glProgramUniformHandleui64vARB;
extern PFNGLISTEXTUREHANDLERESIDENTARBPROC		glIsTextureHandleResidentARB;
extern PFNGLISIMAGEHANDLERESIDENTARBPROC		glIsImageHandleResidentARB;

// GLSL and Progrma Pipelines
extern PFNGLCREATESHADERPROC					glCreateShader;
extern PFNGLDELETESHADERPROC					glDeleteShader;
extern PFNGLSHADERSOURCEPROC					glShaderSource;
extern PFNGLSHADERBINARYPROC					glShaderBinary;
extern PFNGLSPECIALIZESHADERPROC				glSpecializeShader;
extern PFNGLCOMPILESHADERPROC					glCompileShader;
extern PFNGLGETSHADERIVPROC						glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC				glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC					glCreateProgram;
extern PFNGLDELETEPROGRAMPROC					glDeleteProgram;
extern PFNGLPROGRAMPARAMETERIPROC				glProgramParameteri;
extern PFNGLATTACHSHADERPROC					glAttachShader;
extern PFNGLDETACHSHADERPROC					glDetachShader;
extern PFNGLLINKPROGRAMPROC						glLinkProgram;
extern PFNGLUSEPROGRAMPROC						glUseProgram;
extern PFNGLGETPROGRAMIVPROC					glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC				glGetProgramInfoLog;
extern PFNGLGETACTIVEATTRIBPROC					glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC				glGetActiveUniform;
extern PFNGLGETATTRIBLOCATIONPROC				glGetAttribLocation;
extern PFNGLGETUNIFORMLOCATIONPROC				glGetUniformLocation;

// GL_ARB_separate_shader_objects
extern PFNGLGENPROGRAMPIPELINESPROC				glGenProgramPipelines;
extern PFNGLUSEPROGRAMSTAGESPROC				glUseProgramStages;
extern PFNGLACTIVESHADERPROGRAMPROC				glActiveShaderProgram;
extern PFNGLBINDPROGRAMPIPELINEPROC				glBindProgramPipeline;
extern PFNGLDELETEPROGRAMPIPELINESPROC			glDeleteProgramPipelines;
extern PFNGLISPROGRAMPIPELINEPROC				glIsProgramPipeline;
extern PFNGLPROGRAMUNIFORM1FPROC				glProgramUniform1f;
extern PFNGLPROGRAMUNIFORM1FVPROC				glProgramUniform1fv;
extern PFNGLPROGRAMUNIFORM1IPROC				glProgramUniform1i;
extern PFNGLPROGRAMUNIFORM1IVPROC				glProgramUniform1iv;
extern PFNGLPROGRAMUNIFORM2FPROC				glProgramUniform2f;
extern PFNGLPROGRAMUNIFORM2FVPROC				glProgramUniform2fv;
extern PFNGLPROGRAMUNIFORM2IPROC				glProgramUniform2i;
extern PFNGLPROGRAMUNIFORM2IVPROC				glProgramUniform2iv;	

// GL_ARB_sampler_objects
extern PFNGLDELETESAMPLERSPROC					glDeleteSamplers;
extern PFNGLCREATESAMPLERSPROC					glCreateSamplers;
extern PFNGLSAMPLERPARAMETERIPROC				glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERIVPROC				glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFPROC				glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERFVPROC				glSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIVPROC			glGetSamplerParameteriv;

// GL_ARB_framebuffer_object
extern PFNGLBINDFRAMEBUFFERPROC					glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC				glDeleteFramebuffers;
extern PFNGLCREATEFRAMEBUFFERSPROC				glCreateFramebuffers;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREPROC			glNamedFramebufferTexture;
extern PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC	glNamedFramebufferRenderbuffer;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC		glNamedFramebufferDrawBuffer;
extern PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC		glNamedFramebufferDrawBuffers;
extern PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC		glNamedFramebufferReadBuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC			glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC			glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC			glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC			glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC			glFramebufferTextureLayer;
extern PFNGLFRAMEBUFFERTEXTUREPROC				glFramebufferTexture;
extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC		glCheckNamedFramebufferStatus;

// ARB_sync
extern PFNGLFENCESYNCPROC						glFenceSync;
extern PFNGLISSYNCPROC 							glIsSync;
extern PFNGLDELETESYNCPROC 						glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC 					glClientWaitSync;
extern PFNGLWAITSYNCPROC 						glWaitSync;
extern PFNGLGETSYNCIVPROC						glGetSynciv;

// ARB_viewport_array
extern PFNGLVIEWPORTARRAYVPROC					glViewportArrayv;
extern PFNGLVIEWPORTINDEXEDFPROC				glViewportIndexedf;
extern PFNGLVIEWPORTINDEXEDFVPROC				glViewportIndexedfv;
extern PFNGLSCISSORARRAYVPROC					glScissorArrayv;
extern PFNGLSCISSORINDEXEDPROC					glScissorIndexed;
extern PFNGLSCISSORINDEXEDVPROC					glScissorIndexedv;
extern PFNGLDEPTHRANGEARRAYVPROC				glDepthRangeArrayv;
extern PFNGLDEPTHRANGEINDEXEDPROC				glDepthRangeIndexed;

// GL_ARB_debug_output
extern PFNGLDEBUGMESSAGECONTROLPROC				glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTARBPROC			glDebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC			glDebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC				glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC					glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC					glPopDebugGroup;

#endif // !__QGL_H__
