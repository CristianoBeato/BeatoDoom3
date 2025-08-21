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

#include <crvkCore.hpp>
#include "Backend_apiwrapper.h"
#include "ShaderStorage.hpp"


typedef struct backEndCounters_s
{
	int		c_surfaces;
	int		c_shaders;
	int		c_vertexes;
	int		c_indexes;		// one set per pass
	int		c_totalIndexes;	// counting all passes

	int		c_drawElements;
	int		c_drawIndexes;
	int		c_drawVertexes;
	int		c_drawRefIndexes;
	int		c_drawRefVertexes;

	int		c_shadowElements;
	int		c_shadowIndexes;
	int		c_shadowVertexes;

	int		c_vboIndexes;
	float	c_overDraw;	

	float	maxLightValue;	// for light scale
	int		msec;			// total msec for backend run

    backEndCounters_s( void ) :
        c_surfaces( 0 ),
        c_shaders( 0 ),
        c_vertexes( 0 ),
        c_indexes( 0 ),
        c_totalIndexes( 0 ),
        c_drawElements( 0 ),
        c_drawIndexes( 0 ),
        c_drawVertexes( 0 ),
        c_drawRefIndexes( 0 ),
        c_drawRefVertexes( 0 ),
        c_shadowElements( 0 ),
        c_shadowIndexes( 0 ),
        c_shadowVertexes( 0 ),
        c_vboIndexes( 0 ),
        c_overDraw( 0.0f ),
        maxLightValue( 0.0f ),
        msec( 0 )
    {
    }
} backEndCounters_t;

// all state modified by the back end is separated
// from the front end state
class crBackend
{
public:
    crBackend( void );
    ~crBackend( void );
    
    void        StartUp( void );
    void        ShutDown( void );

    // Backend.cpp
    void                            ZeroPerformanceCounters( void );
    void                            ExecuteBackEndCommands( const emptyCommand_t *cmds ); 
    
    /// @brief Copy the content of the current frame buffer ( will stall render )
    /// @param in_x horizontal rect position 
    /// @param in_y vertical rect position
    /// @param in_width rect width 
    /// @param in_height rect height 
    /// @param pixels 
    void                            ReadCurrentFrameBuffer( const int32_t in_x, int32_t in_y, uint32_t in_width, uint32_t in_height, byte *pixels );

    //
    void                            SelectTexture( const uint32_t unit ) { currentTextureUnit = unit; }
    uint32_t                        SwapChainImages( void ) const { return m_swapChain->ImageCount(); }
    uint32_t                        GetFrameCount( void ) const { return frameCount; }
    crAutoPointer<crvkDevice>       GetRenderDevice( void ) const { return m_renderDevice; }

    viewDefptr_t                    GetViewDef( void ) const { return viewDef; };
    backEndCounters_t               &GetPerformanceCounters( void ) { return pc; }
    
protected:
    friend class idImage;
    uint32_t                        GetCurrentTextureUnit( void ) { return currentTextureUnit; }
    
private:
    bool				                currentRenderCopied;	    // true if any material has already referenced _currentRender
    uint32_t                            currentTextureUnit;
    uint64_t                            frameCount;		            // used to track all images used in a frame
    uint32_t                            frameID;
    backEndCounters_t	                pc;
    int					                c_copyFrameBuffer;
//	int					                depthFunc;			        // GLS_DEPTHFUNC_EQUAL, or GLS_DEPTHFUNC_LESS for translucent
    float				                lightScale;			        // Every light color calaculation will be multiplied by this,
                                                                    // which will guarantee that the result is < tr.backEndRendererMaxLight
                                                                    // A card with high dynamic range will have this set to 1.0
    float				                overBright;			        // The amount that all light interactions must be multiplied by
                                                                    // with post processing to get the desired total light level.
                                                                    // A high dynamic range card will have this set to 1.0.
    float				                lightTextureMatrix[16];	    // only if lightStage->texture.hasMatrix
	float				                lightColor[4];		        // evaluation of current light's color stage
	idScreenRect		                currentScissor;             // for scissor clipping, local inside renderView viewport
	const viewEntity_t*                 currentSpace;		        // for detecting when a matrix must change
	crAutoPointer<viewLight_t>          viewLight;                  //
	viewDefptr_t                        viewDef;                    //

    crAutoPointer<crvkDevice>           m_renderDevice;
    crAutoPointer<crvkSwapchain>        m_swapChain;                // swap chain

    /// uniform blocks 
    crAutoPointer<crUniformBlock>               m_vertexUniformBlock;       // store the vertex uniform block
    crAutoPointer<crUniformBlock>               m_fragmentUniformBlock;     // store the fragment uniform block
    crAutoPointer<crUniformBlock>               m_samplersUniformBlock;     // store the samplers binding block 
    crAutoPointer<crUniformBlock>               m_lightUniformBlock;        // store the light uniform block
    
    // render commands
    VkSemaphore                                 m_renderFinished;
    idStaticList<VkCommandBuffer, SMP_FRAMES>   m_commandBuffers;
    crAutoPointer<crvkGraphicPipeline>          m_currentPipeline;

    // these pipelines are used for post passes, that don't use material/shader pipelines
    crAutoPointer<crvkGraphicPipeline>          m_blendLightPipeline;
    crAutoPointer<crvkGraphicPipeline>          m_fogLightPipeline;
 
    void    CreateFrameBuffers( void );
    void    DestroyFrameBuffers( void );

    void    Framebuffer( const uint32_t framebufferID );
    void    Viewport( const int x, const int y, const int width, const int height );
    void    Scissor( const idScreenRect &in_scissor );

    //  Backend_render.cpp
    void    DrawView( const void *data );
    void    ShowOverdraw( void );

    void    FillDepthBuffer( const drawSurf_t *surf ); 
    void    STD_FillDepthBuffer( drawSurf_t **drawSurfs, int numDrawSurfs );
    void    StencilShadowPass( const drawSurf_t *drawSurfs );
    void    STD_FogAllLights( void );
    void    LeaveDepthHack( void );
    void    RenderTriangleSurface( const srfTriangles_t *tri );
    void    RenderTriangleSurface( const drawSurf_t *surf );
    void    DrawShaderPasses( drawSurf_t **drawSurfs, int numDrawSurfs );
    
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
    void        RenderDrawSurfChainWithFunction( const drawSurf_t *drawSurfs, std::function<void( const drawSurf_t *)> triFunc_ );
    void        GetShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture, float matrix[16] );
    void        LoadShaderTextureMatrix( const float *shaderRegisters, const textureStage_t *texture );
    void        BindVariableStageImage( const textureStage_t *texture, const float *shaderRegisters );
    void        BindStageTexture( const float *shaderRegisters, const textureStage_t *texture, const drawSurf_t *surf );
    void        FinishStageTexture( const textureStage_t *texture, const drawSurf_t *surf );
    void        DetermineLightScale( void );
    void        BeginDrawingView (void);
    void        RenderDrawSurfListWithFunction( drawSurf_t **drawSurfs, int numDrawSurfs, std::function<void( const drawSurf_t *)> triFunc_ );
    void        CreateSingleDrawInteractions( const drawSurf_t *surf,  std::function<void(const drawInteraction_t *)> DrawInteraction );
    static void SubmittInteraction( drawInteraction_t *din, std::function<void(const drawInteraction_t *)> DrawInteraction );

    // Backend_draw.cpp
    void        BakeTextureMatrixIntoTexgen( idPlane lightProject[3], const float *textureMatrix ); 
    void        PrepareStageTexturing( const shaderStage_t *pStage,  const drawSurf_t *surf );
    void        FinishStageTexturing( const shaderStage_t *pStage, const drawSurf_t *surf );
    void        SetProgramEnvironment( void );
    void        SetProgramEnvironmentSpace( void );
    int         STD_DrawShaderPasses( drawSurf_t **drawSurfs, int numDrawSurfs ); 
    void        RenderShaderPasses( const drawSurf_t *surf );
    void        SetVertexColorParms( stageVertexColor_t svc );
    void        SetPipeline( const crAutoPointer<crvkGraphicPipeline> m_pipeline );
    void        T_BlendLight( const drawSurf_t *surf );
    void        BlendLight( const drawSurf_t *drawSurfs,  const drawSurf_t *drawSurfs2 );
    void        T_BasicFog( const drawSurf_t *surf ); 
    void        FogPass( const drawSurf_t *drawSurfs,  const drawSurf_t *drawSurfs2 );
    void        STD_FogAllLights( void );
    void        STD_LightScale( void );

    // Backend_draw_interactions.cpp
    void        DrawInteraction( const drawInteraction_t *din );
    void        CreateDrawInteractions( const drawSurf_t *surf );
    void        DrawInteractions( void );

    // Backend
    void                            SetBuffer( const void *data );
    void                            SwapBuffers( const void *data );
    void                            CopyRender( const void *data ); 

    // now set on material
#if 0
    void                            SetCull( const cullType_t culling );
#endif
};

#endif //!__BACKEND_COMMON_H__