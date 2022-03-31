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
** QGL.H
*/

#ifndef __QGL_H__
#define __QGL_H__

// BEATO Begin
#define NO_SDL_GLEXT
#include <SDL_opengl.h>
#include <gl/glext.h>

///
/// OpenGL utilities
///
extern ID_INLINE GLint glGetInteger( const GLenum pname );
extern ID_INLINE GLboolean glGetState( const GLenum flag );
extern ID_INLINE GLboolean glSetState( const GLenum flag, const GLboolean enable );
extern ID_INLINE GLenum glGetActiveTexture( void );
extern ID_INLINE const GLuint glGetBuferBinding( const GLenum target );
extern ID_INLINE const GLuint glGetTextureBinding( const GLenum target );

// ARB_vertex_buffer_object
extern PFNGLBINDBUFFERARBPROC glBindBuffer;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
extern PFNGLGENBUFFERSARBPROC glGenBuffers;
extern PFNGLISBUFFERARBPROC glIsBuffer;
extern PFNGLBUFFERDATAARBPROC glBufferData;
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubData;
extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;

// EXT_stencil_two_side
extern	PFNGLACTIVESTENCILFACEEXTPROC	glActiveStencilFaceEXT;

// separate_stencil
extern PFNGLSTENCILOPSEPARATEPROC		glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC		glStencilFuncSeparate;

extern PFNGLCOMBINERPARAMETERINVPROC	glCombinerParameteriNV;

// ARB_vertex_program / ARB_fragment_program
extern PFNGLVERTEXATTRIBPOINTERARBPROC		glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC	glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC				glBindProgramARB;
extern PFNGLGENPROGRAMSARBPROC				glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC	glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB;

// GL_EXT_depth_bounds_test
extern PFNGLDEPTHBOUNDSEXTPROC              glDepthBoundsEXT;



// BEATO End

#endif // !__QGL_H__
