
#include "tr_local.h"
#include "draw_glsl.hpp"

constexpr uint32_t k_MAX_SHADER_PROGRAMS = 256;
static GLuint k_VertexArrays[2] { 0, 0 }; 
static GLuint k_ShaderPrograms[k_MAX_SHADER_PROGRAMS];

static struct draw_glsl_t
{
    GLuint  vao;
    GLuint  program;
} glsl;

enum
{
    VAO_DRAW_VERT,
    VAO_SHADOW_VERT,
    VAO_COUNT
};

enum
{
    PROGRAM_TEST,
    PROGRAM_SHADOW,
    PROGRAM_INTERACTION
};

enum
{
    UNIFORM_LOCATION_PROJECTION_MATRIX,
    UNIFORM_LOCATION_MODEL_VIEW_MATRIX,
	UNIFORM_LOCATION_COLOR_MODULATE,
	UNIFORM_LOCATION_COLOR_ADD,
    UNIFORM_LOCATION_VERTEX_COLOR,
	UNIFORM_LOCATION_COLOR_DIFUSE,
	UNIFORM_LOCATION_COLOR_SPECULAR,
    UNIFORM_LOCATION_FRAGMENT_COLOR,
    UNIFORM_LOCATION_LIGHT_ORIGIN,
    UNIFORM_LOCATION_VIEW_ORIGIN,
    UNIFORM_LOCATION_LIGHT_PROJECT_S,
    UNIFORM_LOCATION_LIGHT_PROJECT_T,
    UNIFORM_LOCATION_LIGHT_PROJECT_Q,
    UNIFORM_LOCATION_LIGHT_FALLOFF_S,
    UNIFORM_LOCATION_BUMP_MATRIX_S,
    UNIFORM_LOCATION_BUMP_MATRIX_T,
    UNIFORM_LOCATION_DIFFUSE_MATRIX_S,
    UNIFORM_LOCATION_DIFFUSE_MATRIX_T,
    UNIFORM_LOCATION_SPECULAR_MATRIX_S,
    UNIFORM_LOCATION_SPECULAR_MATRIX_T,
};

enum
{
    TEXTUNIT_AMBIENT_NORMAL,
    TEXTUNIT_BUMP_MAP,
    TEXTUNIT_FALLOFF,
    TEXTUNIT_PROJECTION,
    TEXTUNIT_DIFFUSE,
    TEXTUNIT_SPECULAR,
    TEXTUNIT_SPECULAR_LOOKUP
};

static void InitVertexArrays( void )
{
    glCreateVertexArrays( VAO_COUNT, k_VertexArrays );

    /// Create default draw vertex
	glEnableVertexArrayAttrib( k_VertexArrays[VAO_DRAW_VERT], 0 ); // Position
	glVertexArrayAttribBinding( k_VertexArrays[VAO_DRAW_VERT], 0, 0 );
	glVertexArrayAttribFormat( k_VertexArrays[VAO_DRAW_VERT], 0, 3, GL_FLOAT, GL_FALSE, offsetof( idDrawVert, st ) );
	
	glEnableVertexArrayAttrib( k_VertexArrays[VAO_DRAW_VERT], 1 ); // TexCoord
	glVertexArrayAttribBinding( k_VertexArrays[VAO_DRAW_VERT], 1, 0 );
	glVertexArrayAttribFormat( k_VertexArrays[VAO_DRAW_VERT], 1, 2, GL_FLOAT, GL_FALSE, offsetof(idDrawVert, st));
	
	glEnableVertexArrayAttrib( k_VertexArrays[VAO_DRAW_VERT], 2 ); // Normal
	glVertexArrayAttribBinding( k_VertexArrays[VAO_DRAW_VERT], 2, 0 );
	glVertexArrayAttribFormat( k_VertexArrays[VAO_DRAW_VERT], 2, 3, GL_FLOAT, GL_FALSE, offsetof(idDrawVert, normal));
	
	glEnableVertexArrayAttrib( k_VertexArrays[VAO_DRAW_VERT], 3 ); // Tangent
	glVertexArrayAttribBinding( k_VertexArrays[VAO_DRAW_VERT], 3, 0 );
	glVertexArrayAttribFormat( k_VertexArrays[VAO_DRAW_VERT], 3, 3, GL_FLOAT, GL_FALSE, offsetof(idDrawVert, tangents[0] ) );
	
	glEnableVertexArrayAttrib( k_VertexArrays[VAO_DRAW_VERT], 4 ); // Binormal (Bitangent)
	glVertexArrayAttribBinding( k_VertexArrays[VAO_DRAW_VERT], 4, 0 );
	glVertexArrayAttribFormat( k_VertexArrays[VAO_DRAW_VERT], 4, 3, GL_FLOAT, GL_FALSE, offsetof(idDrawVert, tangents[1] ) );
	
    /// Create shadow vertex 
}

static void RB_GLSL_UseVertexArray( const uint32_t vertexID )
{
    if ( glsl.vao == k_VertexArrays[vertexID] )
        return;

     glsl.vao = k_VertexArrays[vertexID];
     glBindVertexArray( glsl.vao );
}

static void RB_GLSL_UseProgram( const uint32_t programID )
{
    if ( glsl.program == k_ShaderPrograms[programID] )
        return;

    glsl.program = k_ShaderPrograms[programID];
    glUseProgram( glsl.program );
}

/*
====================
GL_SelectTextureNoClient
====================
*/
static void GL_SelectTextureNoClient( int unit ) 
{
	backEnd.glState.currenttmu = unit;
	qglActiveTextureARB( GL_TEXTURE0_ARB + unit );
	RB_LogComment( "glActiveTextureARB( %i )\n", unit );
}

/*
==================
RB_GLSL_DrawInteractions
==================
*/
void RB_GLSL_DrawInteractions( void ) 
{
	viewLight_t		*vLight;
	const idMaterial	*lightShader;

	GL_SelectTexture( 0 );

	//
	// for each light, perform adding and shadowing
	//
	for ( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next ) 
    {
		backEnd.vLight = vLight;

		// do fogging later
		if ( vLight->lightShader->IsFogLight() ) 
			continue;
		
		if ( vLight->lightShader->IsBlendLight() )
			continue;
		
		if ( !vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions ) 
			continue;

		lightShader = vLight->lightShader;

		// clear the stencil buffer if needed
		if ( vLight->globalShadows || vLight->localShadows ) 
        {
			backEnd.currentScissor = vLight->scissorRect;
			if ( r_useScissor.GetBool() ) 
            {
				qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
					backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
					backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
					backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}

			qglClear( GL_STENCIL_BUFFER_BIT );
		} 
        else 
        {
			// no shadows, so no need to read or write the stencil buffer
			// we might in theory want to use GL_ALWAYS instead of disabling
			// completely, to satisfy the invarience rules
			qglStencilFunc( GL_ALWAYS, 128, 255 );
		}

		if ( r_useShadowVertexProgram.GetBool() ) 
        {
            RB_GLSL_StencilShadowPass( vLight->globalShadows );
			RB_GLSL_CreateDrawInteractions( vLight->localInteractions );

			RB_GLSL_StencilShadowPass( vLight->localShadows );
			RB_GLSL_CreateDrawInteractions( vLight->globalInteractions );
		} 
        else 
        {
			RB_GLSL_StencilShadowPass( vLight->globalShadows );
			RB_GLSL_CreateDrawInteractions( vLight->localInteractions );
			RB_GLSL_StencilShadowPass( vLight->localShadows );
			RB_GLSL_CreateDrawInteractions( vLight->globalInteractions );
		}

		// translucent surfaces never get stencil shadowed
		if ( r_skipTranslucent.GetBool() )
			continue;

		qglStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		RB_GLSL_CreateDrawInteractions( vLight->translucentInteractions );

		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	qglStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
}

/*
===============
RB_GLSL_EnterWeaponDepthHack
===============
*/
static void RB_GLSL_EnterWeaponDepthHack( void )
{
	qglDepthRange( 0, 0.5 );

	float	matrix[16];

	memcpy( matrix, backEnd.viewDef->projectionMatrix, sizeof( matrix ) );

	matrix[14] *= 0.25;

    /// send matrix to shader
    glUniformMatrix4fv( UNIFORM_LOCATION_PROJECTION_MATRIX, 1, GL_FALSE, matrix );
}

/*
===============
RB_GLSL_EnterModelDepthHack
===============
*/
static void RB_GLSL_EnterModelDepthHack( float depth )
{
	qglDepthRange( 0.0f, 1.0f );

	float	matrix[16];

	memcpy( matrix, backEnd.viewDef->projectionMatrix, sizeof( matrix ) );

	matrix[14] -= depth;

    glUniformMatrix4fv( UNIFORM_LOCATION_PROJECTION_MATRIX, 1, GL_FALSE, matrix );
}

/*
===============
RB_GLSL_LeaveDepthHack
===============
*/
static void RB_GLSL_LeaveDepthHack( void )
{
	qglDepthRange( 0, 1 );
    glUniformMatrix4fv( UNIFORM_LOCATION_PROJECTION_MATRIX, 1, GL_FALSE, backEnd.viewDef->projectionMatrix );
}


/*
==================
R_SetDrawInteractions
==================
*/
static void R_SetDrawInteraction( const shaderStage_t *surfaceStage, const float *surfaceRegs, idImage **image, idVec4 matrix[2], float color[4] ) 
                          {
	*image = surfaceStage->texture.image;
	if ( surfaceStage->texture.hasMatrix ) 
    {
		matrix[0][0] = surfaceRegs[surfaceStage->texture.matrix[0][0]];
		matrix[0][1] = surfaceRegs[surfaceStage->texture.matrix[0][1]];
		matrix[0][2] = 0;
		matrix[0][3] = surfaceRegs[surfaceStage->texture.matrix[0][2]];

		matrix[1][0] = surfaceRegs[surfaceStage->texture.matrix[1][0]];
		matrix[1][1] = surfaceRegs[surfaceStage->texture.matrix[1][1]];
		matrix[1][2] = 0;
		matrix[1][3] = surfaceRegs[surfaceStage->texture.matrix[1][2]];

		// we attempt to keep scrolls from generating incredibly large texture values, but
		// center rotations and center scales can still generate offsets that need to be > 1
		if ( matrix[0][3] < -40 || matrix[0][3] > 40 ) 
			matrix[0][3] -= (int)matrix[0][3];
		
        if ( matrix[1][3] < -40 || matrix[1][3] > 40 ) 
			matrix[1][3] -= (int)matrix[1][3];
	} 
    else 
    {
		matrix[0][0] = 1;
		matrix[0][1] = 0;
		matrix[0][2] = 0;
		matrix[0][3] = 0;

		matrix[1][0] = 0;
		matrix[1][1] = 1;
		matrix[1][2] = 0;
		matrix[1][3] = 0;
	}

	if ( color ) 
    {
		for ( int i = 0 ; i < 4 ; i++ ) 
        {
			color[i] = surfaceRegs[surfaceStage->color.registers[i]];
			// clamp here, so card with greater range don't look different.
			// we could perform overbrighting like we do for lights, but
			// it doesn't currently look worth it.
			if ( color[i] < 0 )
				color[i] = 0;
			else if ( color[i] > 1.0 ) 
				color[i] = 1.0;
		}
	}
}

/*
=================
RB_SubmittInteraction
=================
*/
static void RB_SubmittInteraction( drawInteraction_t *din, void (*DrawInteraction)(const drawInteraction_t *) ) 
{
	if ( !din->bumpImage ) 
		return;

	if ( !din->diffuseImage || r_skipDiffuse.GetBool() ) din->diffuseImage = globalImages->blackImage;
	if ( !din->specularImage || r_skipSpecular.GetBool() || din->ambientLight ) din->specularImage = globalImages->blackImage;
	if ( !din->bumpImage || r_skipBump.GetBool() ) din->bumpImage = globalImages->flatNormalMap;

	// if we wouldn't draw anything, don't call the Draw function
	if ( 
		( ( din->diffuseColor[0] > 0 || 
		din->diffuseColor[1] > 0 || 
		din->diffuseColor[2] > 0 ) && din->diffuseImage != globalImages->blackImage )
		|| ( ( din->specularColor[0] > 0 || 
		din->specularColor[1] > 0 || 
		din->specularColor[2] > 0 ) && din->specularImage != globalImages->blackImage ) ) {
		DrawInteraction( din );
	}
}

/*
=============
RB_GLSL_CreateSingleDrawInteractions

This can be used by different draw_* backends to decompose a complex light / surface
interaction into primitive interactions
=============
*/
void RB_GLSL_CreateSingleDrawInteractions( const drawSurf_t *surf, void (*DrawInteraction)(const drawInteraction_t *) )
 {
	const idMaterial	*surfaceShader = surf->material;
	const float			*surfaceRegs = surf->shaderRegisters;
	const viewLight_t	*vLight = backEnd.vLight;
	const idMaterial	*lightShader = vLight->lightShader;
	const float			*lightRegs = vLight->shaderRegisters;
	drawInteraction_t	inter;

	if ( r_skipInteractions.GetBool() || !surf->geo || !surf->geo->ambientCache )
		return;

	if ( tr.logFile )
		RB_LogComment( "---------- RB_GLSL_CreateSingleDrawInteractions %s on %s ----------\n", lightShader->GetName(), surfaceShader->GetName() );

	// change the matrix and light projection vectors if needed
	if ( surf->space != backEnd.currentSpace ) 
    {
		backEnd.currentSpace = surf->space;
		glUniformMatrix4fv( UNIFORM_LOCATION_MODEL_VIEW_MATRIX, 1, GL_FALSE, surf->space->modelViewMatrix );
	}

	// change the scissor if needed
	if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( surf->scissorRect ) ) 
    {
		backEnd.currentScissor = surf->scissorRect;
		qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
			backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
			backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
			backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
	}

	// hack depth range if needed
	if ( surf->space->weaponDepthHack ) 
		RB_GLSL_EnterWeaponDepthHack();

	if ( surf->space->modelDepthHack ) 
		RB_GLSL_EnterModelDepthHack( surf->space->modelDepthHack );

	inter.surf = surf;
	inter.lightFalloffImage = vLight->falloffImage;

	R_GlobalPointToLocal( surf->space->modelMatrix, vLight->globalLightOrigin, inter.localLightOrigin.ToVec3() );
	R_GlobalPointToLocal( surf->space->modelMatrix, backEnd.viewDef->renderView.vieworg, inter.localViewOrigin.ToVec3() );
	inter.localLightOrigin[3] = 0;
	inter.localViewOrigin[3] = 1;
	inter.ambientLight = lightShader->IsAmbientLight();

	// the base projections may be modified by texture matrix on light stages
	idPlane lightProject[4];
	for ( int i = 0 ; i < 4 ; i++ ) 
    {
		R_GlobalPlaneToLocal( surf->space->modelMatrix, backEnd.vLight->lightProject[i], lightProject[i] );
	}

	for ( int lightStageNum = 0 ; lightStageNum < lightShader->GetNumStages() ; lightStageNum++ ) 
    {
		const shaderStage_t	*lightStage = lightShader->GetStage( lightStageNum );

		// ignore stages that fail the condition
		if ( !lightRegs[ lightStage->conditionRegister ] ) 
			continue;
		
		inter.lightImage = lightStage->texture.image;

		std::memcpy( inter.lightProjection, lightProject, sizeof( inter.lightProjection ) );
		// now multiply the texgen by the light texture matrix
		if ( lightStage->texture.hasMatrix ) 
        {
			RB_GetShaderTextureMatrix( lightRegs, &lightStage->texture, backEnd.lightTextureMatrix );
			RB_BakeTextureMatrixIntoTexgen( reinterpret_cast<class idPlane *>(inter.lightProjection), backEnd.lightTextureMatrix );
		}

		inter.bumpImage = NULL;
		inter.specularImage = NULL;
		inter.diffuseImage = NULL;
		inter.diffuseColor[0] = inter.diffuseColor[1] = inter.diffuseColor[2] = inter.diffuseColor[3] = 0;
		inter.specularColor[0] = inter.specularColor[1] = inter.specularColor[2] = inter.specularColor[3] = 0;

		float lightColor[4];

		// backEnd.lightScale is calculated so that lightColor[] will never exceed
		// tr.backEndRendererMaxLight
		lightColor[0] = backEnd.lightScale * lightRegs[ lightStage->color.registers[0] ];
		lightColor[1] = backEnd.lightScale * lightRegs[ lightStage->color.registers[1] ];
		lightColor[2] = backEnd.lightScale * lightRegs[ lightStage->color.registers[2] ];
		lightColor[3] = lightRegs[ lightStage->color.registers[3] ];

		// go through the individual stages
		for ( int surfaceStageNum = 0 ; surfaceStageNum < surfaceShader->GetNumStages() ; surfaceStageNum++ ) 
        {
			const shaderStage_t	*surfaceStage = surfaceShader->GetStage( surfaceStageNum );

			switch( surfaceStage->lighting ) 
            {
				case SL_AMBIENT: 
                {
					// ignore ambient stages while drawing interactions
					break;
				}
				case SL_BUMP: 
                {
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
                    {
						break;
					}
					// draw any previous interaction
					RB_SubmittInteraction( &inter, DrawInteraction );
					inter.diffuseImage = nullptr;
					inter.specularImage = nullptr;
					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.bumpImage, inter.bumpMatrix, NULL );
					break;
				}
				case SL_DIFFUSE: 
                {
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
						break;
					
					if ( inter.diffuseImage ) 
						RB_SubmittInteraction( &inter, DrawInteraction );
					

					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.diffuseImage, inter.diffuseMatrix, inter.diffuseColor.ToFloatPtr() );
					inter.diffuseColor[0] *= lightColor[0];
					inter.diffuseColor[1] *= lightColor[1];
					inter.diffuseColor[2] *= lightColor[2];
					inter.diffuseColor[3] *= lightColor[3];
					inter.vertexColor = surfaceStage->vertexColor;
					break;
				}
				case SL_SPECULAR: 
                {
					// ignore stage that fails the condition
					if ( !surfaceRegs[ surfaceStage->conditionRegister ] ) 
						break;
					
					if ( inter.specularImage ) 
						RB_SubmittInteraction( &inter, DrawInteraction );
					

					R_SetDrawInteraction( surfaceStage, surfaceRegs, &inter.specularImage,
											inter.specularMatrix, inter.specularColor.ToFloatPtr() );
					inter.specularColor[0] *= lightColor[0];
					inter.specularColor[1] *= lightColor[1];
					inter.specularColor[2] *= lightColor[2];
					inter.specularColor[3] *= lightColor[3];
					inter.vertexColor = surfaceStage->vertexColor;
					break;
				}
			}
		}

		// draw the final interaction
		RB_SubmittInteraction( &inter, DrawInteraction );
	}

	// unhack depth range if needed
	if ( surf->space->weaponDepthHack || surf->space->modelDepthHack != 0.0f ) 
		RB_GLSL_LeaveDepthHack();
}


/*
==================
RB_GLSL_DrawInteraction
==================
*/
static void	RB_GLSL_DrawInteraction( const drawInteraction_t *din ) 
{
	static const float zero[4] = { 0, 0, 0, 0 };
	static const float one[4] = { 1, 1, 1, 1 };
	static const float negOne[4] = { -1, -1, -1, -1 };

	// load all the vertex program parameters
	glUniform4fv( UNIFORM_LOCATION_LIGHT_ORIGIN, 1, din->localLightOrigin.ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_VIEW_ORIGIN, 1, din->localViewOrigin.ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_LIGHT_PROJECT_S, 1, din->lightProjection[0].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_LIGHT_PROJECT_T, 1, din->lightProjection[1].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_LIGHT_PROJECT_Q, 1, din->lightProjection[2].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_LIGHT_FALLOFF_S, 1, din->lightProjection[3].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_BUMP_MATRIX_S, 1, din->bumpMatrix[0].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_BUMP_MATRIX_T, 1, din->bumpMatrix[1].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_DIFFUSE_MATRIX_S, 1, din->diffuseMatrix[0].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_DIFFUSE_MATRIX_T, 1, din->diffuseMatrix[1].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_SPECULAR_MATRIX_S, 1, din->specularMatrix[0].ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_SPECULAR_MATRIX_T, 1, din->specularMatrix[1].ToFloatPtr() );
	switch ( din->vertexColor ) 
	{
	case SVC_IGNORE:
		glUniform4fv( UNIFORM_LOCATION_COLOR_MODULATE, 1, zero );
		glUniform4fv( UNIFORM_LOCATION_COLOR_ADD, 1, one );
		break;
	case SVC_MODULATE:
		glUniform4fv( UNIFORM_LOCATION_COLOR_MODULATE, 1, one );
		glUniform4fv( UNIFORM_LOCATION_COLOR_ADD, 1, zero );
		break;
	case SVC_INVERSE_MODULATE:
		glUniform4fv( UNIFORM_LOCATION_COLOR_MODULATE, 1, negOne );
		glUniform4fv( UNIFORM_LOCATION_COLOR_ADD, 1, one );
		break;
	}

	// set the constant colors
	glUniform4fv( UNIFORM_LOCATION_COLOR_DIFUSE, 1, din->diffuseColor.ToFloatPtr() );
	glUniform4fv( UNIFORM_LOCATION_COLOR_SPECULAR, 1, din->specularColor.ToFloatPtr() );

	// set the textures

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( TEXTUNIT_BUMP_MAP );
	din->bumpImage->Bind();

	// texture 2 will be the light falloff texture
	GL_SelectTextureNoClient( TEXTUNIT_FALLOFF );
	din->lightFalloffImage->Bind();

	// texture 3 will be the light projection texture
	GL_SelectTextureNoClient( TEXTUNIT_PROJECTION );
	din->lightImage->Bind();

	// texture 4 is the per-surface diffuse map
	GL_SelectTextureNoClient( TEXTUNIT_DIFFUSE );
	din->diffuseImage->Bind();

	// texture 5 is the per-surface specular map
	GL_SelectTextureNoClient( TEXTUNIT_SPECULAR );
	din->specularImage->Bind();

	// draw it
	RB_DrawElementsWithCounters( din->surf->geo );
}

/*
=============
RB_GLSL_CreateDrawInteractions

=============
*/
void RB_GLSL_CreateDrawInteractions( const drawSurf_t *surf ) 
{
	if ( !surf )
		return;

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// bind the vertex program
	if ( r_testARBProgram.GetBool() ) 
        RB_GLSL_UseProgram( PROGRAM_TEST );
    else 
        RB_GLSL_UseProgram( PROGRAM_INTERACTION );

	// enable the vertex arrays
    RB_GLSL_UseVertexArray( VAO_DRAW_VERT );

	// texture 0 is the normalization cube map for the vector towards the light
	GL_SelectTextureNoClient( TEXTUNIT_AMBIENT_NORMAL );
	if ( backEnd.vLight->lightShader->IsAmbientLight() ) 
		globalImages->ambientNormalMap->Bind();
    else
		globalImages->normalCubeMapImage->Bind();

	// texture 6 is the specular lookup table
	GL_SelectTextureNoClient( TEXTUNIT_SPECULAR_LOOKUP );
	if ( r_testARBProgram.GetBool() ) 
		globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
    else 
		globalImages->specularTableImage->Bind();


	for ( ; surf ; surf = surf->nextOnLight ) 
    {
		// perform setup here that will not change over multiple interaction passes

		// set the vertex pointers
		idDrawVert	*ac = (idDrawVert *)vertexCache.Position( surf->geo->ambientCache );
		
		// this may cause RB_ARB2_DrawInteraction to be exacuted multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_GLSL_CreateSingleDrawInteractions( surf, RB_GLSL_DrawInteraction );
	}

	// disable features
	GL_SelectTextureNoClient( TEXTUNIT_SPECULAR );
	globalImages->BindNull();

	GL_SelectTextureNoClient( TEXTUNIT_SPECULAR );
	globalImages->BindNull();

	GL_SelectTextureNoClient( TEXTUNIT_DIFFUSE );
	globalImages->BindNull();

	GL_SelectTextureNoClient( TEXTUNIT_PROJECTION );
	globalImages->BindNull();

	GL_SelectTextureNoClient( TEXTUNIT_FALLOFF );
	globalImages->BindNull();

	GL_SelectTextureNoClient( TEXTUNIT_BUMP_MAP );
	globalImages->BindNull();

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( TEXTUNIT_AMBIENT_NORMAL );
}

/*
=====================
RB_GLSL_T_Shadow

the shadow volumes face INSIDE
=====================
*/
static void RB_GLSL_T_Shadow( const drawSurf_t *surf ) 
{
	const srfTriangles_t	*tri;

	// set the light position if we are using a vertex program to project the rear surfaces
	if ( tr.backEndRendererHasVertexPrograms && r_useShadowVertexProgram.GetBool() && surf->space != backEnd.currentSpace ) 
    {
		idVec4 localLight;

		R_GlobalPointToLocal( surf->space->modelMatrix, backEnd.vLight->globalLightOrigin, localLight.ToVec3() );
		localLight.w = 0.0f;
		qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_ORIGIN, localLight.ToFloatPtr() );
	}

	tri = surf->geo;

	if ( !tri->shadowCache ) 
		return;

	// we always draw the sil planes, but we may not need to draw the front or rear caps
	int	numIndexes;
	bool external = false;

	if ( !r_useExternalShadows.GetInteger() ) 
    {
		numIndexes = tri->numIndexes;
	} 
    else if ( r_useExternalShadows.GetInteger() == 2 ) 
    { 
        // force to no caps for testing
		numIndexes = tri->numShadowIndexesNoCaps;
	} 
    else if ( !(surf->dsFlags & DSF_VIEW_INSIDE_SHADOW) ) 
    { 
		// if we aren't inside the shadow projection, no caps are ever needed needed
		numIndexes = tri->numShadowIndexesNoCaps;
		external = true;
	} 
    else if ( !backEnd.vLight->viewInsideLight && !(surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE) ) 
    {
		// if we are inside the shadow projection, but outside the light, and drawing
		// a non-infinite shadow, we can skip some caps
		if ( backEnd.vLight->viewSeesShadowPlaneBits & surf->geo->shadowCapPlaneBits ) 
        {
			// we can see through a rear cap, so we need to draw it, but we can skip the
			// caps on the actual surface
			numIndexes = tri->numShadowIndexesNoFrontCaps;
		} 
        else 
        {
			// we don't need to draw any caps
			numIndexes = tri->numShadowIndexesNoCaps;
		}
		external = true;
	} 
    else 
    {
		// must draw everything
		numIndexes = tri->numIndexes;
	}

	// set depth bounds
	if( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() )
		qglDepthBoundsEXT( surf->scissorRect.zmin, surf->scissorRect.zmax );

	// debug visualization
	if ( r_showShadows.GetInteger() ) 
    {
		if ( r_showShadows.GetInteger() == 3 ) 
        {
			if ( external ) 
				glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright, 1.0f ); 
            else 
				// these are the surfaces that require the reverse
				glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright , 1.0f );
		} 
        else 
        {
			// draw different color for turboshadows
			if ( surf->geo->shadowCapPlaneBits & SHADOW_CAP_INFINITE ) 
            {
				if ( numIndexes == tri->numIndexes ) 
					glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 1/backEnd.overBright, 0.1/backEnd.overBright, 0.1/backEnd.overBright, 1.0f );
                else 
					glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 1/backEnd.overBright, 0.4/backEnd.overBright, 0.1/backEnd.overBright, 1.0f );
			} 
            else 
            {
				if ( numIndexes == tri->numIndexes ) 
					glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 0.1/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright, 1.0f );
                else if ( numIndexes == tri->numShadowIndexesNoFrontCaps ) 
					glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 0.1/backEnd.overBright, 1/backEnd.overBright, 0.6/backEnd.overBright, 1.0f );
                else 
					glUniform4f( UNIFORM_LOCATION_VERTEX_COLOR, 0.6/backEnd.overBright, 1/backEnd.overBright, 0.1/backEnd.overBright, 1.0f );	
			}
		}

		qglStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
		qglDisable( GL_STENCIL_TEST );
		GL_Cull( CT_TWO_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		GL_Cull( CT_FRONT_SIDED );
		qglEnable( GL_STENCIL_TEST );

		return;
	}

	// patent-free work around
	if ( !external ) 
    {
		// "preload" the stencil buffer with the number of volumes
		// that get clipped by the near or far clip plane
		qglStencilOp( GL_KEEP, tr.stencilDecr, tr.stencilDecr );
		GL_Cull( CT_FRONT_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
		qglStencilOp( GL_KEEP, tr.stencilIncr, tr.stencilIncr );
		GL_Cull( CT_BACK_SIDED );
		RB_DrawShadowElementsWithCounters( tri, numIndexes );
	}

	// traditional depth-pass stencil shadows
	qglStencilOp( GL_KEEP, GL_KEEP, tr.stencilIncr );
	GL_Cull( CT_FRONT_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );

	qglStencilOp( GL_KEEP, GL_KEEP, tr.stencilDecr );
	GL_Cull( CT_BACK_SIDED );
	RB_DrawShadowElementsWithCounters( tri, numIndexes );
}

/*
======================
RB_GLSL_RenderDrawSurfChainWithFunction
======================
*/
void RB_GLSL_RenderDrawSurfChainWithFunction( const drawSurf_t *drawSurfs, void (*triFunc_)( const drawSurf_t *) ) 
{
	const drawSurf_t		*drawSurf = nullptr;

	backEnd.currentSpace = nullptr;

	for ( drawSurf = drawSurfs ; drawSurf ; drawSurf = drawSurf->nextOnLight ) 
    {
		// change the matrix if needed
		if ( drawSurf->space != backEnd.currentSpace ) 
			glUniformMatrix4fv( UNIFORM_LOCATION_MODEL_VIEW_MATRIX, 1, GL_FALSE, drawSurf->space->modelViewMatrix );

		if ( drawSurf->space->weaponDepthHack ) 
			RB_GLSL_EnterWeaponDepthHack();

		if ( drawSurf->space->modelDepthHack ) 
			RB_GLSL_EnterModelDepthHack( drawSurf->space->modelDepthHack );

		// change the scissor if needed
		if ( r_useScissor.GetBool() && !backEnd.currentScissor.Equals( drawSurf->scissorRect ) ) 
        {
			backEnd.currentScissor = drawSurf->scissorRect;
			qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1, 
				backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
				backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
				backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
		}

		// render it
		triFunc_( drawSurf );

		if ( drawSurf->space->weaponDepthHack || drawSurf->space->modelDepthHack != 0.0f )
			RB_GLSL_LeaveDepthHack();

		backEnd.currentSpace = drawSurf->space;
	}
}

/*
=====================
RB_GLSL_StencilShadowPass

Stencil test should already be enabled, and the stencil buffer should have
been set to 128 on any surfaces that might receive shadows
=====================
*/
void RB_GLSL_StencilShadowPass( const drawSurf_t *drawSurfs ) 
{
	if ( !r_shadows.GetBool() ) 
		return;

	if ( !drawSurfs ) 
		return;

	RB_LogComment( "---------- RB_GLSL_StencilShadowPass ----------\n" );

	globalImages->BindNull();

    /// Bind shadow shader program
    RB_GLSL_UseProgram( PROGRAM_SHADOW );
    
    /// bind vertex shader 
    RB_GLSL_UseVertexArray( VAO_SHADOW_VERT );

	// for visualizing the shadows
	if ( r_showShadows.GetInteger() ) 
    {
		if ( r_showShadows.GetInteger() == 2 ) 
			// draw filled in
			GL_State( GLS_DEPTHMASK | GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHFUNC_LESS  );
        else 
			// draw as lines, filling the depth buffer
			GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO | GLS_POLYMODE_LINE | GLS_DEPTHFUNC_ALWAYS  );
		
	} 
    else 
    {
		// don't write to the color buffer, just the stencil buffer
		GL_State( GLS_DEPTHMASK | GLS_COLORMASK | GLS_ALPHAMASK | GLS_DEPTHFUNC_LESS );
	}

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() ) 
    {
		qglPolygonOffset( r_shadowPolygonFactor.GetFloat(), -r_shadowPolygonOffset.GetFloat() );
		qglEnable( GL_POLYGON_OFFSET_FILL );
	}

	qglStencilFunc( GL_ALWAYS, 1, 255 );

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) 
		qglEnable( GL_DEPTH_BOUNDS_TEST_EXT );

	RB_GLSL_RenderDrawSurfChainWithFunction( drawSurfs, RB_GLSL_T_Shadow );

	GL_Cull( CT_FRONT_SIDED );

	if ( r_shadowPolygonFactor.GetFloat() || r_shadowPolygonOffset.GetFloat() ) 
		qglDisable( GL_POLYGON_OFFSET_FILL );

	if ( glConfig.depthBoundsTestAvailable && r_useDepthBoundsTest.GetBool() ) 
		qglDisable( GL_DEPTH_BOUNDS_TEST_EXT );

	qglEnableClientState( GL_TEXTURE_COORD_ARRAY );

	qglStencilFunc( GL_GEQUAL, 128, 255 );
	qglStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
}