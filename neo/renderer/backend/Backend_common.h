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
#ifndef __BACKEND_COMMON_H__
#define __BACKEND_COMMON_H__

#include "Backend_apiwrapper.h"

#if CR_USE_VULKAN
#include "vulkan/qvk.h"
#include "vulkan/vkFence.h"
#include "vulkan/vkBuffer.h"
#include "vulkan/vkTexture.h"
#include "vulkan/vkFramebuffer.h"
#include "vulkan/vkShaderStorage.h"
#include "vulkan/vkPipeline.h"
#include "vulkan/vkCommandQueue.h"
#include "vulkan/vkSwapChain.h"
#endif // CR_USE_VULKAN

#if CR_USE_OPENGL
#include "opengl/qgl.h"
#include "opengl/glFence.h"
#include "opengl/glBuffer.h"
#include "opengl/glTexture.h"
#include "opengl/glFramebuffer.h"
#include "opengl/glShaderStorage.h"
#include "opengl/glPipeline.h"
#include "opengl/glCommandQueue.h"
#endif // CR_USE_OPENGL

// all state modified by the back end is separated
// from the front end state
class crBackend
{
public:
    crBackend( void );
    ~crBackend( void );
    
    void        StartUp( void );
    void        ShutDown( void );

    void        DrawView( const void *data );
    
    void        STD_FillDepthBuffer( drawSurf_t **drawSurfs, int numDrawSurfs );
    void        StencilShadowPass( const drawSurf_t *drawSurfs );
    void        STD_FogAllLights( void );
    void        BakeTextureMatrixIntoTexgen( idPlane lightProject[3], const float textureMatrix[16] );
    void        LeaveDepthHack( void );
    void        RenderTriangleSurface( const srfTriangles_t *tri );
    void        RenderTriangleSurface( const drawSurf_t *surf );
    void        DrawShaderPasses( drawSurf_t **drawSurfs, int numDrawSurfs );
    
    const shaderStage_t *RB_SetLightTexture( const idRenderLightLocal *light );
    
    void        STD_LightScale( void );
    
    // draw_common.cpp
    void        STD_DrawView( void );

    // Backend_render.cpp
    void        DrawElementsImmediate( const srfTriangles_t *tri );
    void        DrawElementsWithCounters( const srfTriangles_t *tri );
    void        DrawShadowElementsWithCounters( const srfTriangles_t *tri, int numIndexes );
    void        EnterWeaponDepthHack( void );
    void        EnterModelDepthHack( float depth );
    void        RenderDrawSurfListWithFunction( drawSurf_t **drawSurfs, int numDrawSurfs, void (*triFunc_)( const drawSurf_t *) );
    void        RenderDrawSurfChainWithFunction( const drawSurf_t *drawSurfs, void (*triFunc_)( const drawSurf_t *) );
    void        GetShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture, float matrix[16] );
    void        LoadShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture );
    void        BindVariableStageImage( const textureStage_t *texture, const float *shaderRegisters );
    void        BindStageTexture( const float *shaderRegisters, const textureStage_t *texture, const drawSurf_t *surf );
    void        FinishStageTexture( const textureStage_t *texture, const drawSurf_t *surf );
    void        DetermineLightScale( void );
    void        BeginDrawingView (void);
    void        CreateSingleDrawInteractions( const drawSurf_t *surf, void (*DrawInteraction)(const drawInteraction_t *) );
    
    // Backend.cpp
    void                            SetBuffer( const void *data );
    void                            SwapBuffers( const void *data ); 
    uint32_t                        SwapChainImages( void ) const { return m_swapChain->GetImageCount(); }

private:
    bool				            currentRenderCopied;	// true if any material has already referenced _currentRender
    int					            c_copyFrameBuffer;
    int								frameCount;		        // used to track all images used in a frame
	int					            depthFunc;			    // GLS_DEPTHFUNC_EQUAL, or GLS_DEPTHFUNC_LESS for translucent
    float				            lightScale;			    // Every light color calaculation will be multiplied by this,
                                                            // which will guarantee that the result is < tr.backEndRendererMaxLight
                                                            // A card with high dynamic range will have this set to 1.0
    float				            overBright;			    // The amount that all light interactions must be multiplied by
                                                            // with post processing to get the desired total light level.
                                                            // A high dynamic range card will have this set to 1.0.
    float				            lightTextureMatrix[16];	// only if lightStage->texture.hasMatrix
	float				            lightColor[4];		    // evaluation of current light's color stage
    glstate_t			            glState;                // our OpenGL state deltas
	idScreenRect		            currentScissor;         // for scissor clipping, local inside renderView viewport
	const viewEntity_t*             currentSpace;		    // for detecting when a matrix must change
	backEndCounters_t	            pc;
	crAutoPointer<viewLight_t>      viewLight;
	crAutoPointer<viewDef_t>	    viewDef;
    crAutoPointer<crSwapChain>      m_swapChain;            //
    crAutoPointer<crCommandQueue>   m_graphicQueue;         //
    crAutoPointer<crPipeline>       m_currentPipeline;      //
    crAutoPointer<crShaderStorage>  m_uniforms;             //
};

#endif //!__BACKEND_COMMON_H__