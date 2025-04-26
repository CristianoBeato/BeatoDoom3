#include "idlib/precompiled.h"
#include "qgl.h"
#include "glContext.h"
#include <SDL3/SDL_video.h>

//===========================================================================

PFNGLGETERRORPROC						    glGetError = nullptr;
PFNGLGETSTRINGPROC						    glGetString = nullptr;
PFNGLGETFLOATVPROC						    glGetFloatv = nullptr;
PFNGLGETINTEGERVPROC					    glGetIntegerv = nullptr;
PFNGLENABLEPROC							    glEnable = nullptr;
PFNGLDISABLEPROC						    glDisable = nullptr;
PFNGLISENABLEDPROC						    glIsEnabled = nullptr;
PFNGLCLEARPROC							    glClear = nullptr;
PFNGLFLUSHPROC							    glFlush = nullptr;
PFNGLFINISHPROC							    glFinish = nullptr;
PFNGLVIEWPORTPROC						    glViewport = nullptr;

PFNGLCLEARCOLORPROC						    glClearColor = nullptr;
PFNGLCOLORMASKPROC						    glColorMask = nullptr;

// blend
PFNGLBLENDFUNCPROC						    glBlendFunc = nullptr;

// depth
PFNGLCLEARDEPTHPROC						    glClearDepth = nullptr;
PFNGLDEPTHMASKPROC						    glDepthMask = nullptr;
PFNGLDEPTHFUNCPROC						    glDepthFunc = nullptr;
PFNGLDEPTHRANGEPROC						    glDepthRange = nullptr;

// stencil
PFNGLSCISSORPROC                     	    glScissor = nullptr;
PFNGLSTENCILOPPROC						    glStencilOp = nullptr;
PFNGLSTENCILFUNCPROC                        glStencilFunc = nullptr;
PFNGLSTENCILMASKPROC                        glStencilMask = nullptr;
PFNGLCLEARSTENCILPROC					    glClearStencil = nullptr;
PFNGLSTENCILOPSEPARATEPROC				    glStencilOpSeparate = nullptr;
PFNGLSTENCILFUNCSEPARATEPROC                glStencilFuncSeparate = nullptr;

// poligon 
PFNGLCULLFACEPROC						    glCullFace = nullptr;
PFNGLPOLYGONMODEPROC                        glPolygonMode = nullptr;
PFNGLPOLYGONOFFSETPROC					    glPolygonOffset = nullptr;
PFNGLLINEWIDTHPROC						    glLineWidth = nullptr;
PFNGLPOINTSIZEPROC						    glPointSize = nullptr;

// draw buffers 
PFNGLDRAWBUFFERPROC						    glDrawBuffer = nullptr;
PFNGLREADBUFFERPROC						    glReadBuffer = nullptr;
PFNGLREADPIXELSPROC						    glReadPixels = nullptr;
PFNGLPIXELSTOREIPROC					    glPixelStorei = nullptr;

// ARB_vertex_buffer_object, ARB_direct_state_access ARB_buffer_storage, ARB_uniform_buffer_object ARB_map_buffer_range GL_ARB_multi_bind
PFNGLBINDBUFFERPROC						    glBindBuffer = nullptr;
PFNGLBINDBUFFERRANGEPROC            	    glBindBufferRange = nullptr;
PFNGLBINDBUFFERSRANGEPROC            	    glBindBuffersRange = nullptr;
PFNGLCREATEBUFFERSPROC					    glCreateBuffers = nullptr;
PFNGLDELETEBUFFERSPROC	 				    glDeleteBuffers = nullptr;
PFNGLISBUFFERPROC	 					    glIsBuffer = nullptr;
PFNGLNAMEDBUFFERSTORAGEPROC				    glNamedBufferStorage = nullptr;
PFNGLNAMEDBUFFERDATAPROC				    glNamedBufferData = nullptr;
PFNGLNAMEDBUFFERSUBDATAPROC				    glNamedBufferSubData = nullptr;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC			    glCopyNamedBufferSubData = nullptr;
PFNGLCLEARNAMEDBUFFERDATAPROC			    glClearNamedBufferData = nullptr;
PFNGLMAPNAMEDBUFFERRANGEPROC			    glMapNamedBufferRange = nullptr;
PFNGLUNMAPNAMEDBUFFERPROC				    glUnmapNamedBuffer = nullptr;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC        glFlushMappedNamedBufferRange = nullptr;
PFNGLGETBUFFERPARAMETERIVPROC 			    glGetBufferParameteriv = nullptr;
PFNGLGETBUFFERPOINTERVPROC 				    glGetBufferPointerv = nullptr;

// ARB_vertex_array_object, ARB_direct_state_access
PFNGLBINDVERTEXARRAYPROC                    glBindVertexArray = nullptr;
PFNGLCREATEVERTEXARRAYSPROC				    glCreateVertexArrays = nullptr;
PFNGLDELETEVERTEXARRAYSPROC				    glDeleteVertexArrays = nullptr;
PFNGLISVERTEXARRAYPROC					    glIsVertexArray = nullptr;
PFNGLENABLEVERTEXARRAYATTRIBPROC            glEnableVertexArrayAttrib = nullptr;
PFNGLDISABLEVERTEXARRAYATTRIBPROC		    glDisableVertexArrayAttrib = nullptr;
PFNGLVERTEXARRAYATTRIBBINDINGPROC		    glVertexArrayAttribBinding = nullptr;
PFNGLVERTEXARRAYATTRIBFORMATPROC            glVertexArrayAttribFormat = nullptr;
PFNGLVERTEXARRAYVERTEXBUFFERPROC            glVertexArrayVertexBuffer = nullptr;
PFNGLVERTEXARRAYELEMENTBUFFERPROC		    glVertexArrayElementBuffer = nullptr;

// Draw Command
PFNGLDRAWELEMENTSPROC					    glDrawElements = nullptr;
PFNGLDRAWELEMENTSBASEVERTEXPROC			    glDrawElementsBaseVertex = nullptr;
PFNGLDRAWELEMENTSINDIRECTPROC			    glDrawElementsIndirect = nullptr;


// Textures
// ARB_texture_storage, ARB_direct_state_access, ARB_multi_bind, ARB_texture_compression, ARB_texture_storage_multisample
PFNGLBINDTEXTUREPROC                        glBindTexture = nullptr;
PFNGLBINDTEXTURESPROC					    glBindTextures = nullptr;
PFNGLCREATETEXTURESPROC					    glCreateTextures = nullptr;
PFNGLDELETETEXTURESPROC					    glDeleteTextures = nullptr;
PFNGLTEXTURESTORAGE1DPROC				    glTextureStorage1D = nullptr;
PFNGLTEXTURESTORAGE2DPROC				    glTextureStorage2D = nullptr;
PFNGLTEXTURESTORAGE3DPROC				    glTextureStorage3D = nullptr;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC        glTextureStorage2DMultisample = nullptr;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC        glTextureStorage3DMultisample = nullptr;
PFNGLTEXTURESUBIMAGE1DPROC				    glTextureSubImage1D = nullptr;
PFNGLTEXTURESUBIMAGE2DPROC				    glTextureSubImage2D = nullptr;
PFNGLTEXTURESUBIMAGE3DPROC				    glTextureSubImage3D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE1DPROC              glCopyTextureSubImage1D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE2DPROC              glCopyTextureSubImage2D = nullptr;
PFNGLCOPYTEXTURESUBIMAGE3DPROC              glCopyTextureSubImage3D = nullptr;
PFNGLTEXTUREPARAMETERIPROC				    glTextureParameteri = nullptr;
PFNGLTEXTUREPARAMETERFPROC				    glTextureParameterf = nullptr;
PFNGLGETTEXTUREIMAGEPROC                    glGetTextureImage = nullptr;

PFNGLCOPYIMAGESUBDATAPROC                   glCopyImageSubData = nullptr;

// texture handler
PFNGLGETTEXTUREHANDLEARBPROC                glGetTextureHandleARB = nullptr;
PFNGLGETTEXTURESAMPLERHANDLEARBPROC		    glGetTextureSamplerHandleARB = nullptr;
PFNGLMAKETEXTUREHANDLERESIDENTARBPROC	    glMakeTextureHandleResidentARB = nullptr;
PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC	glMakeTextureHandleNonResidentARB = nullptr;
PFNGLMAKEIMAGEHANDLERESIDENTARBPROC 		glMakeImageHandleResidentARB = nullptr;
PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC 	    glMakeImageHandleNonResidentARB = nullptr;
PFNGLUNIFORMHANDLEUI64ARBPROC 			    glUniformHandleui64ARB = nullptr;
PFNGLUNIFORMHANDLEUI64VARBPROC 			    glUniformHandleui64vARB = nullptr;
PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC 	    glProgramUniformHandleui64ARB = nullptr;
PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC 	    glProgramUniformHandleui64vARB = nullptr;
PFNGLISTEXTUREHANDLERESIDENTARBPROC		    glIsTextureHandleResidentARB = nullptr;
PFNGLISIMAGEHANDLERESIDENTARBPROC		    glIsImageHandleResidentARB = nullptr;

// GLSL and Progrma Pipelines
PFNGLCREATESHADERPROC					    glCreateShader = nullptr;
PFNGLDELETESHADERPROC					    glDeleteShader = nullptr;
PFNGLSHADERSOURCEPROC					    glShaderSource = nullptr;
PFNGLSHADERBINARYPROC					    glShaderBinary = nullptr;
PFNGLSPECIALIZESHADERPROC				    glSpecializeShader = nullptr;
PFNGLCOMPILESHADERPROC					    glCompileShader = nullptr;
PFNGLGETSHADERIVPROC					    glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC				    glGetShaderInfoLog = nullptr;
PFNGLCREATEPROGRAMPROC					    glCreateProgram = nullptr;
PFNGLDELETEPROGRAMPROC					    glDeleteProgram = nullptr;
PFNGLPROGRAMPARAMETERIPROC				    glProgramParameteri = nullptr;
PFNGLATTACHSHADERPROC					    glAttachShader = nullptr;
PFNGLDETACHSHADERPROC					    glDetachShader = nullptr;
PFNGLLINKPROGRAMPROC					    glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC						    glUseProgram = nullptr;
PFNGLGETPROGRAMIVPROC					    glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC				    glGetProgramInfoLog = nullptr;
PFNGLGETACTIVEATTRIBPROC				    glGetActiveAttrib = nullptr;
PFNGLGETACTIVEUNIFORMPROC				    glGetActiveUniform = nullptr;
PFNGLGETATTRIBLOCATIONPROC				    glGetAttribLocation = nullptr;
PFNGLGETUNIFORMLOCATIONPROC				    glGetUniformLocation = nullptr;

// GL_ARB_separate_shader_objects
PFNGLGENPROGRAMPIPELINESPROC			    glGenProgramPipelines = nullptr;
PFNGLUSEPROGRAMSTAGESPROC				    glUseProgramStages = nullptr;
PFNGLACTIVESHADERPROGRAMPROC			    glActiveShaderProgram = nullptr;
PFNGLBINDPROGRAMPIPELINEPROC			    glBindProgramPipeline = nullptr;
PFNGLDELETEPROGRAMPIPELINESPROC			    glDeleteProgramPipelines = nullptr;
PFNGLISPROGRAMPIPELINEPROC				    glIsProgramPipeline = nullptr;
PFNGLPROGRAMUNIFORM1FPROC				    glProgramUniform1f = nullptr;
PFNGLPROGRAMUNIFORM1FVPROC				    glProgramUniform1fv = nullptr;
PFNGLPROGRAMUNIFORM1IPROC				    glProgramUniform1i = nullptr;
PFNGLPROGRAMUNIFORM1IVPROC				    glProgramUniform1iv = nullptr;
PFNGLPROGRAMUNIFORM2FPROC				    glProgramUniform2f = nullptr;
PFNGLPROGRAMUNIFORM2FVPROC				    glProgramUniform2fv = nullptr;
PFNGLPROGRAMUNIFORM2IPROC				    glProgramUniform2i = nullptr;
PFNGLPROGRAMUNIFORM2IVPROC				    glProgramUniform2iv = nullptr;	

// GL_ARB_sampler_objects
PFNGLDELETESAMPLERSPROC			            glDeleteSamplers = nullptr;
PFNGLCREATESAMPLERSPROC			            glCreateSamplers = nullptr;
PFNGLSAMPLERPARAMETERIPROC		            glSamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERIVPROC		            glSamplerParameteriv = nullptr;
PFNGLSAMPLERPARAMETERFPROC		            glSamplerParameterf = nullptr;
PFNGLSAMPLERPARAMETERFVPROC		            glSamplerParameterfv = nullptr;
PFNGLGETSAMPLERPARAMETERIVPROC	            glGetSamplerParameteriv = nullptr;

// GL_ARB_framebuffer_object
PFNGLBINDFRAMEBUFFERPROC				    glBindFramebuffer = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC				    glDeleteFramebuffers = nullptr;
PFNGLCREATEFRAMEBUFFERSPROC				    glCreateFramebuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC		    glNamedFramebufferTexture = nullptr;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC	    glNamedFramebufferRenderbuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC		    glNamedFramebufferDrawBuffer = nullptr;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC	    glNamedFramebufferDrawBuffers = nullptr;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC		    glNamedFramebufferReadBuffer = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC		    glFramebufferRenderbuffer = nullptr;
PFNGLFRAMEBUFFERTEXTURE1DPROC			    glFramebufferTexture1D = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC			    glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTURE3DPROC			    glFramebufferTexture3D = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC		    glFramebufferTextureLayer = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC				    glFramebufferTexture = nullptr;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC	    glCheckNamedFramebufferStatus = nullptr;
PFNGLBLITNAMEDFRAMEBUFFERPROC               glBlitNamedFramebuffer = nullptr;

// rendebuffers 
PFNGLISFRAMEBUFFERPROC                          glIsFramebuffer = nullptr;
PFNGLCREATERENDERBUFFERSPROC                    glCreateRenderbuffers = nullptr;
PFNGLDELETERENDERBUFFERSPROC                    glDeleteRenderbuffers = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC               glNamedRenderbufferStorage = nullptr;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC    glNamedRenderbufferStorageMultisample = nullptr;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC        glGetNamedRenderbufferParameteriv = nullptr;

// ARB_sync
PFNGLFENCESYNCPROC						    glFenceSync = nullptr;
PFNGLISSYNCPROC 						    glIsSync = nullptr;
PFNGLDELETESYNCPROC 					    glDeleteSync = nullptr;
PFNGLCLIENTWAITSYNCPROC 				    glClientWaitSync = nullptr;
PFNGLWAITSYNCPROC 						    glWaitSync = nullptr;
PFNGLGETSYNCIVPROC						    glGetSynciv = nullptr;

// ARB_viewport_array
PFNGLVIEWPORTARRAYVPROC					    glViewportArrayv = nullptr;
PFNGLVIEWPORTINDEXEDFPROC				    glViewportIndexedf = nullptr;
PFNGLVIEWPORTINDEXEDFVPROC				    glViewportIndexedfv = nullptr;
PFNGLSCISSORARRAYVPROC					    glScissorArrayv = nullptr;
PFNGLSCISSORINDEXEDPROC					    glScissorIndexed = nullptr;
PFNGLSCISSORINDEXEDVPROC				    glScissorIndexedv = nullptr;
PFNGLDEPTHRANGEARRAYVPROC				    glDepthRangeArrayv = nullptr;
PFNGLDEPTHRANGEINDEXEDPROC				    glDepthRangeIndexed = nullptr;

// GL_ARB_debug_output
PFNGLDEBUGMESSAGECONTROLPROC			    glDebugMessageControl = nullptr;
PFNGLDEBUGMESSAGEINSERTARBPROC			    glDebugMessageInsert = nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC			    glDebugMessageCallback = nullptr;
PFNGLGETDEBUGMESSAGELOGPROC				    glGetDebugMessageLog = nullptr;
PFNGLPUSHDEBUGGROUPPROC					    glPushDebugGroup = nullptr;
PFNGLPOPDEBUGGROUPPROC					    glPopDebugGroup = nullptr;

//===========================================================================

template< typename _t >
inline void GL_GetExtensionPointer( _t &func, const char* name )
{
    func = reinterpret_cast<_t>( SDL_GL_GetProcAddress( name ) );
}

#define GL_LOAD_PROC( X ) GL_GetExtensionPointer( X, #X )

void crGLContext::LoadOpenGLFunctions( void )
{
    GL_LOAD_PROC( glGetError );
    GL_LOAD_PROC( glGetString );
    GL_LOAD_PROC( glGetFloatv );
    GL_LOAD_PROC( glGetIntegerv );
    GL_LOAD_PROC( glEnable );
    GL_LOAD_PROC( glDisable );
    GL_LOAD_PROC( glIsEnabled );
    GL_LOAD_PROC( glClear );
    GL_LOAD_PROC( glFlush );
    GL_LOAD_PROC( glFinish );
    GL_LOAD_PROC( glViewport );

    GL_LOAD_PROC( glClearColor );
    GL_LOAD_PROC( glColorMask );

    // blend
    GL_LOAD_PROC( glBlendFunc );

    // depth
    GL_LOAD_PROC( glClearDepth );
    GL_LOAD_PROC( glDepthMask );
    GL_LOAD_PROC( glDepthFunc );
    GL_LOAD_PROC( glDepthRange );

    // stencil
    GL_LOAD_PROC( glScissor );
    GL_LOAD_PROC( glStencilOp );
    GL_LOAD_PROC( glStencilFunc );
    GL_LOAD_PROC( glStencilMask );
    GL_LOAD_PROC( glClearStencil );
    GL_LOAD_PROC( glStencilOpSeparate );
    GL_LOAD_PROC( glStencilFuncSeparate );

    // poligon 
    GL_LOAD_PROC( glCullFace );
    GL_LOAD_PROC( glPolygonMode );
    GL_LOAD_PROC( glPolygonOffset );
    GL_LOAD_PROC( glLineWidth );
    GL_LOAD_PROC( glPointSize );

    // draw buffers 
    GL_LOAD_PROC( glDrawBuffer );
    GL_LOAD_PROC( glReadBuffer );
    GL_LOAD_PROC( glReadPixels );
    GL_LOAD_PROC( glPixelStorei );

    // ARB_vertex_buffer_object, ARB_direct_state_access ARB_buffer_storage, ARB_uniform_buffer_object ARB_map_buffer_range GL_ARB_multi_bind
    GL_LOAD_PROC( glBindBuffer );
    GL_LOAD_PROC( glBindBufferRange );
    GL_LOAD_PROC( glBindBuffersRange );
    GL_LOAD_PROC( glCreateBuffers );
    GL_LOAD_PROC( glDeleteBuffers );
    GL_LOAD_PROC( glIsBuffer );
    GL_LOAD_PROC( glNamedBufferStorage );
    GL_LOAD_PROC( glNamedBufferData );
    GL_LOAD_PROC( glNamedBufferSubData );
    GL_LOAD_PROC( glCopyNamedBufferSubData );
    GL_LOAD_PROC( glClearNamedBufferData );
    GL_LOAD_PROC( glMapNamedBufferRange );
    GL_LOAD_PROC( glUnmapNamedBuffer );
    GL_LOAD_PROC( glFlushMappedNamedBufferRange );
    GL_LOAD_PROC( glGetBufferParameteriv );
    GL_LOAD_PROC( glGetBufferPointerv );

    // ARB_vertex_array_object, ARB_direct_state_access
    GL_LOAD_PROC( glBindVertexArray );
    GL_LOAD_PROC( glCreateVertexArrays );
    GL_LOAD_PROC( glDeleteVertexArrays );
    GL_LOAD_PROC( glIsVertexArray );
    GL_LOAD_PROC( glEnableVertexArrayAttrib );
    GL_LOAD_PROC( glDisableVertexArrayAttrib );
    GL_LOAD_PROC( glVertexArrayAttribBinding );
    GL_LOAD_PROC( glVertexArrayAttribFormat );
    GL_LOAD_PROC( glVertexArrayVertexBuffer );
    GL_LOAD_PROC( glVertexArrayElementBuffer );

    // Draw Command
    GL_LOAD_PROC( glDrawElements );
    GL_LOAD_PROC( glDrawElementsBaseVertex );
    GL_LOAD_PROC( glDrawElementsIndirect );


    // Textures
    // ARB_texture_storage, ARB_direct_state_access, ARB_multi_bind, ARB_texture_compression, ARB_texture_storage_multisample
    GL_LOAD_PROC( glBindTexture );
    GL_LOAD_PROC( glBindTextures );
    GL_LOAD_PROC( glCreateTextures );
    GL_LOAD_PROC( glDeleteTextures );
    GL_LOAD_PROC( glTextureStorage1D );
    GL_LOAD_PROC( glTextureStorage2D );
    GL_LOAD_PROC( glTextureStorage3D );
    GL_LOAD_PROC( glTextureStorage2DMultisample );
    GL_LOAD_PROC( glTextureStorage3DMultisample );
    GL_LOAD_PROC( glTextureSubImage1D );
    GL_LOAD_PROC( glTextureSubImage2D );
    GL_LOAD_PROC( glTextureSubImage3D );
    GL_LOAD_PROC( glCopyTextureSubImage1D );
    GL_LOAD_PROC( glCopyTextureSubImage2D );
    GL_LOAD_PROC( glCopyTextureSubImage3D );
    GL_LOAD_PROC( glTextureParameteri );
    GL_LOAD_PROC( glTextureParameterf );
    GL_LOAD_PROC( glGetTextureImage );

    GL_LOAD_PROC( glCopyImageSubData );

    // texture handler
    GL_LOAD_PROC( glGetTextureHandleARB );
    GL_LOAD_PROC( glGetTextureSamplerHandleARB );
    GL_LOAD_PROC( glMakeTextureHandleResidentARB );
    GL_LOAD_PROC( glMakeTextureHandleNonResidentARB );
    GL_LOAD_PROC( glMakeImageHandleResidentARB );
    GL_LOAD_PROC( glMakeImageHandleNonResidentARB );
    GL_LOAD_PROC( glUniformHandleui64ARB );
    GL_LOAD_PROC( glUniformHandleui64vARB );
    GL_LOAD_PROC( glProgramUniformHandleui64ARB );
    GL_LOAD_PROC( glProgramUniformHandleui64vARB );
    GL_LOAD_PROC( glIsTextureHandleResidentARB );
    GL_LOAD_PROC( glIsImageHandleResidentARB );

    // GLSL and Progrma Pipelines
    GL_LOAD_PROC( glCreateShader );
    GL_LOAD_PROC( glDeleteShader );
    GL_LOAD_PROC( glShaderSource );
    GL_LOAD_PROC( glShaderBinary );
    GL_LOAD_PROC( glSpecializeShader );
    GL_LOAD_PROC( glCompileShader );
    GL_LOAD_PROC( glGetShaderiv );
    GL_LOAD_PROC( glGetShaderInfoLog );
    GL_LOAD_PROC( glCreateProgram );
    GL_LOAD_PROC( glDeleteProgram );
    GL_LOAD_PROC( glProgramParameteri );
    GL_LOAD_PROC( glAttachShader );
    GL_LOAD_PROC( glDetachShader );
    GL_LOAD_PROC( glLinkProgram );
    GL_LOAD_PROC( glUseProgram );
    GL_LOAD_PROC( glGetProgramiv );
    GL_LOAD_PROC( glGetProgramInfoLog );
    GL_LOAD_PROC( glGetActiveAttrib );
    GL_LOAD_PROC( glGetActiveUniform );
    GL_LOAD_PROC( glGetAttribLocation );
    GL_LOAD_PROC( glGetUniformLocation );

    // GL_ARB_separate_shader_objects
    GL_LOAD_PROC( glGenProgramPipelines );
    GL_LOAD_PROC( glUseProgramStages );
    GL_LOAD_PROC( glActiveShaderProgram );
    GL_LOAD_PROC( glBindProgramPipeline );
    GL_LOAD_PROC( glDeleteProgramPipelines );
    GL_LOAD_PROC( glIsProgramPipeline );
    GL_LOAD_PROC( glProgramUniform1f );
    GL_LOAD_PROC( glProgramUniform1fv );
    GL_LOAD_PROC( glProgramUniform1i );
    GL_LOAD_PROC( glProgramUniform1iv );
    GL_LOAD_PROC( glProgramUniform2f );
    GL_LOAD_PROC( glProgramUniform2fv );
    GL_LOAD_PROC( glProgramUniform2i );
    GL_LOAD_PROC( glProgramUniform2iv );	

    // GL_ARB_sampler_objects
    GL_LOAD_PROC( glDeleteSamplers );
    GL_LOAD_PROC( glCreateSamplers );
    GL_LOAD_PROC( glSamplerParameteri );
    GL_LOAD_PROC( glSamplerParameteriv );
    GL_LOAD_PROC( glSamplerParameterf );
    GL_LOAD_PROC( glSamplerParameterfv );
    GL_LOAD_PROC( glGetSamplerParameteriv );

    // GL_ARB_framebuffer_object
    GL_LOAD_PROC( glBindFramebuffer );
    GL_LOAD_PROC( glDeleteFramebuffers );
    GL_LOAD_PROC( glCreateFramebuffers );
    GL_LOAD_PROC( glNamedFramebufferTexture );
    GL_LOAD_PROC( glNamedFramebufferRenderbuffer );
    GL_LOAD_PROC( glNamedFramebufferDrawBuffer );
    GL_LOAD_PROC( glNamedFramebufferDrawBuffers );
    GL_LOAD_PROC( glNamedFramebufferReadBuffer );
    GL_LOAD_PROC( glFramebufferRenderbuffer );
    GL_LOAD_PROC( glFramebufferTexture1D );
    GL_LOAD_PROC( glFramebufferTexture2D );
    GL_LOAD_PROC( glFramebufferTexture3D );
    GL_LOAD_PROC( glFramebufferTextureLayer );
    GL_LOAD_PROC( glFramebufferTexture );
    GL_LOAD_PROC( glCheckNamedFramebufferStatus );
    GL_LOAD_PROC( glBlitNamedFramebuffer );

    //
    GL_LOAD_PROC( glIsFramebuffer );
    GL_LOAD_PROC( glCreateRenderbuffers );
    GL_LOAD_PROC( glDeleteRenderbuffers );
    GL_LOAD_PROC( glNamedRenderbufferStorage );
    GL_LOAD_PROC( glNamedRenderbufferStorageMultisample );
    GL_LOAD_PROC( glGetNamedRenderbufferParameteriv );

    // ARB_sync
    GL_LOAD_PROC( glFenceSync );
    GL_LOAD_PROC( glIsSync );
    GL_LOAD_PROC( glDeleteSync );
    GL_LOAD_PROC( glClientWaitSync );
    GL_LOAD_PROC( glWaitSync );
    GL_LOAD_PROC( glGetSynciv );

    // ARB_viewport_array
    GL_LOAD_PROC( glViewportArrayv );
    GL_LOAD_PROC( glViewportIndexedf );
    GL_LOAD_PROC( glViewportIndexedfv );
    GL_LOAD_PROC( glScissorArrayv );
    GL_LOAD_PROC( glScissorIndexed );
    GL_LOAD_PROC( glScissorIndexedv );
    GL_LOAD_PROC( glDepthRangeArrayv );
    GL_LOAD_PROC( glDepthRangeIndexed );

    // GL_ARB_debug_output
    GL_LOAD_PROC( glDebugMessageControl );
    GL_LOAD_PROC( glDebugMessageInsert );
    GL_LOAD_PROC( glDebugMessageCallback );
    GL_LOAD_PROC( glGetDebugMessageLog );
    GL_LOAD_PROC( glPushDebugGroup );
    GL_LOAD_PROC( glPopDebugGroup );
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