
#ifndef __DRAW_GLSL_HPP__
#define __DRAW_GLSL_HPP__

void	R_GLSL_Init( void );
void	RB_GLSL_DrawInteractions( void );
void	R_ReloadGLSLPrograms_f( const idCmdArgs &args );
int		R_FindGLSLProgram( GLenum target, const char *program );

///
static void RB_GLSL_CreateSingleDrawInteractions( const drawSurf_t *surf, void (*DrawInteraction)(const drawInteraction_t *) );
static void RB_GLSL_RenderDrawSurfChainWithFunction( const drawSurf_t *drawSurfs, void (*triFunc_)( const drawSurf_t *) );
static void RB_GLSL_CreateDrawInteractions( const drawSurf_t *surf );
static void RB_GLSL_StencilShadowPass( const drawSurf_t *drawSurfs );
#endif //__DRAW_GLSL_HPP__