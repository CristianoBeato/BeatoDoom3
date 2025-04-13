/*
===========================================================================

Beato idTech 4 Source Code 
Copyright (C) 2016-2024 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

This file is part of the Beato idTech 4  GPL Source Code (?Beato idTech 4  Source Code?).

Beato idTech 4  Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato idTech 4  Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato idTech 4  Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#ifndef __GL_CONTEXT_H__
#define __GL_CONTEXT_H__

#include "qgl.h"
#include "glCommandQueue.h"

struct glSuportedFeatures
{
    bool    multitextureAvailable = false;
	bool    textureCompressionAvailable = false;
	bool    anisotropicAvailable = false;
	bool    textureEnvCombineAvailable = false;
	bool    cubeMapAvailable = false;
    bool    cubeMapArrayAvailable = false;
	bool    vertexBufferObjectAvailable = false;
	bool    twoSidedStencilAvailable = false;
	bool    textureNonPowerOfTwoAvailable = false;
    bool    depthClamp = false;
    bool    depthBoundsTestAvailable = false;
    bool    directStateAccessAvailable = false;
    bool    bufferStorageAvailable = false;
    bool    shaderStorageBufferObjectAvailable = false;
    bool    mapBufferRangeAvailable = false;
    bool    multiBindAvailable = false;
    bool    drawElementsBaseVertexAvailable = false;
    bool    drawIndirectAvailable = false;
    bool    textureStorageAvailable = false;
    bool    textureStorageMultisampleAvailable = false;
    bool    samplerObjectsAvailable = false;
    bool    separateShaderObjectsAvailable = false;
    bool    syncAvailable = false;
    bool    viewportArrayAvailable = false;
    bool    debugOutputAvailable = false;
};



//TODO:
// GL_NUM_COMPRESSED_TEXTURE_FORMATS
// GL_COMPRESSED_TEXTURE_FORMATS

struct glProperties
{
    GLfloat maxTextureLodBias = 0;                      // GL_MAX_TEXTURE_LOD_BIAS
    GLfloat maxTextureAnisostropy = 0;                  // GL_MAX_TEXTURE_MAX_ANISOTROPY
    GLfloat maxClipDistances = 0;                       // GL_MAX_CLIP_DISTANCES
    GLfloat maxViewportWidth = 0.0f;                    // GL_MAX_VIEWPORT_DIMS
    GLfloat maxViewportheight = 0.0f;                   // GL_MAX_VIEWPORT_DIMS
    GLint   minMapBufferAlignment = 0;                  // GL_MIN_MAP_BUFFER_ALIGNMENT
    GLint   majorVersion = 0;                           // GL_MAJOR_VERSION
    GLint   minorVersion = 0;                           // GL_MINOR_VERSION
    GLint   maxVetexAttribBindings = 0;                 // GL_MAX_VERTEX_ATTRIB_BINDINGS
    GLint   maxSamples = 0;                             // GL_MAX_SAMPLES
    GLint   maxImageSamples = 0;                        // GL_MAX_IMAGE_SAMPLES
    GLint   maxFrameBufferSamples = 0;                  // GL_MAX_FRAMEBUFFER_SAMPLES
    GLint   maxArrayTextureLayers = 0;                  // GL_MAX_ARRAY_TEXTURE_LAYERS
    GLint   maxFrameBufferLayers = 0;                   // GL_MAX_FRAMEBUFFER_LAYERS
    GLint   maxColorAttachments = 0;                    // GL_MAX_COLOR_ATTACHMENTS
    GLint   maxDrawBuffers = 0;                         // GL_MAX_DRAW_BUFFERS
    GLint   maxRenderBufferSize = 0;                    // GL_MAX_RENDERBUFFER_SIZE
    GLint   maxViewports = 0;                           // GL_MAX_VIEWPORTS
    GLint   maxTextureSize = 0;                         // GL_MAX_TEXTURE_SIZE
    GLint   maxUniformBlockSize = 0;                    // GL_MAX_UNIFORM_BLOCK_SIZE
    GLint   maxUniformBufferBindings = 0;               // GL_MAX_UNIFORM_BUFFER_BINDINGS
    GLint   maxUniformBufferLocations = 0;              // GL_MAX_UNIFORM_LOCATIONS
    GLint   maxComputeSahderStorageBlocks = 0;          // GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    GLint   maxCubeMapTextureSize = 0;                  // GL_MAX_CUBE_MAP_TEXTURE_SIZE
    GLint   maxCombinedImageUniforms = 0;               // GL_MAX_COMBINED_IMAGE_UNIFORMS
    GLint   maxCombinedVertexUniformComponents = 0;     // GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    GLint   maxCombinedFragmentUniformComponents = 0;   // GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    GLint   maxCombinedComputeUniformComponents = 0;    // GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
    GLint   maxColorTextureSamples = 0;                 // GL_MAX_COLOR_TEXTURE_SAMPLES
    GLint   maxAtomicCounterBufferBinding = 0;          // GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS
    GLint   maxAtomicCounterBufferSize = 0;             // GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE
    GLint   max3dTextureSize = 0;                       // GL_MAX_3D_TEXTURE_SIZE
    GLint   maxCombinedUniformBlocks = 0;               // GL_MAX_COMBINED_UNIFORM_BLOCKS
    GLint   maxFragmentShaderStorageBlocks = 0;         // GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
    GLint   maxDepthTextureSamples = 0;                 // GL_MAX_DEPTH_TEXTURE_SAMPLES
    GLint   maxTextureBufferSize = 0;                   // GL_MAX_TEXTURE_BUFFER_SIZE
    GLint   maxTextureImageUnits = 0;                   // GL_MAX_TEXTURE_IMAGE_UNITS
    //GLint   max = 0;// GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
    //GLint   max = 0;// GL_MAX_COMPUTE_WORK_GROUP_COUNT
    //GLint   max = 0;// GL_MAX_COMPUTE_WORK_GROUP_SIZE

};

// store our openGL State
struct glState
{
    GLuint      vertexArray;
    GLuint      vertexArrayVertexBuffer;
    GLuint      vertexArrayElementBuffer;
    GLuint      programPipeline;
    GLuint      programPipelineVertexProgram;
    GLuint      programPipelineGeometryProgram;
    GLuint      programPipelineFragmentPRogram;
};

// OpenGL command thread
class crGLContext : private idSysThread
{
public:
    crGLContext( void );
    ~crGLContext( void );

    void    StartUp( void );
    void    ShutDown( void );
    void    SwapBuffer( void );
    void    SubmitQueue( crAutoPointer<glCommandQueue> queue );
    const glSuportedFeatures    GetSuportedFeatures( void ) const { return m_features; }
    const glProperties          GetPropertties( void ) const { return m_properties; }
private:
    glSuportedFeatures              m_features;
    glProperties                    m_properties;
    glState                         m_state;
    SDL_GLContext                   m_renderContext;
    SDL_Window*                     m_window;
    crMutex*                        m_queueLock;    
    crAutoPointer<glCommandQueue>   m_tail;
    crAutoPointer<glCommandQueue>   m_head;

    void        InitLibrary( void );
    void        ShutDownLibrary( void );
    void        LoadOpenGLFunctions( void );
    void        CreateContext( void );
    void        DestroyContext( void );
    void        CheckPortableExtensions( void );
    void        CheckProperties( void );
    void        InitDebugOutput( void );
};

#endif //__GL_CONTEXT_H__