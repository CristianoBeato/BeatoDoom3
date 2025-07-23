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

#ifndef __FRONTEND_H__
#define __FRONTEND_H__

typedef struct 
{
	idVec3		tangents[2];
	bool	negativePolarity;
	bool	degenerate;
} faceTangents_t;

// performanceCounters_t
typedef struct performanceCounters_s
{
    
	int		c_sphere_cull_in;
    int     c_sphere_cull_clip;
    int     c_sphere_cull_out;
	int		c_box_cull_in;
    int     c_box_cull_out;
	int		c_createInteractions;	// number of calls to idInteraction::CreateInteraction
	int		c_createLightTris;
	int		c_createShadowVolumes;
	int		c_generateMd5;
	int		c_entityDefCallbacks;
	int		c_alloc;
    int     c_free;	// counts for R_StaticAllc/tr.frameData->StaticFree
	int		c_visibleViewEntities;
	int		c_shadowViewEntities;
	int		c_viewLights;
	int		c_numViews;			// number of total views rendered
	int		c_deformedSurfaces;	// idMD5Mesh::GenerateSurface
	int		c_deformedVerts;	// idMD5Mesh::GenerateSurface
	int		c_deformedIndexes;	// idMD5Mesh::GenerateSurface
	int		c_tangentIndexes;	// R_DeriveTangents()
	int		c_entityUpdates;
	int		c_lightUpdates;
	int		c_entityReferences;
	int		c_lightReferences;
	int		c_guiSurfs;
	int		frontEndMsec;		// sum of time in all RE_RenderScene's in a frame
    
    performanceCounters_s( void ) :
    c_sphere_cull_in( 0 ),
    c_sphere_cull_clip( 0 ),
    c_sphere_cull_out( 0 ),
    c_box_cull_in( 0 ),
    c_box_cull_out( 0 ),
    c_createInteractions( 0 ),
    c_createLightTris( 0 ),
    c_createShadowVolumes( 0 ),
    c_generateMd5( 0 ),
    c_entityDefCallbacks( 0 ),
    c_alloc( 0 ),
    c_free( 0 ),
    c_visibleViewEntities( 0 ),
    c_shadowViewEntities( 0 ),
    c_viewLights( 0 ),
    c_numViews( 0 ),
    c_deformedSurfaces( 0 ),
    c_deformedVerts( 0 ),
    c_deformedIndexes( 0 ),
    c_tangentIndexes( 0 ),
    c_entityUpdates( 0 ),
    c_lightUpdates( 0 ),
    c_entityReferences( 0 ),
    c_lightReferences( 0 ),
    c_guiSurfs( 0 ),
    frontEndMsec( 0 )
    {
    }
} performanceCounters_t;

class crFrontend
{
public:
    crFrontend( void );
    ~crFrontend( void );
    // Frontend_light.cpp
    viewEntity_t*   SetEntityDefViewEntity( idRenderEntityLocal *def );
    viewLight_t*    SetLightDefViewLight( idRenderLightLocal *def );
    bool            IssueEntityDefCallback( idRenderEntityLocal *def );
    bool            CreateAmbientCache( srfTriangles_t *tri, bool needsLighting );
    void            LinkLightSurf( const drawSurf_t **link, const srfTriangles_t *tri, const viewEntity_t *space, const idRenderLightLocal *light, const idMaterial *shader, const idScreenRect &scissor, bool viewInsideShadow );
    void            CreatePrivateShadowCache( srfTriangles_t *tri );
    void            AddAmbientDrawsurfs( viewEntity_t *vEntity );
    void            AddDrawSurf( const srfTriangles_t *tri, const viewEntity_t *space, const renderEntity_t *renderEntity, const idMaterial *shader, const idScreenRect &scissor );

    // Frontend_lightrun.cpp
    void            CreateLightRefs( idRenderLightLocal *light );
    void            CreateEntityRefs( idRenderEntityLocal *def );
    void            ReCreateWorldReferences( void );
    void            FreeDerivedData( void );
    void            FreeEntityDefFadedDecals( idRenderEntityLocal *def, int time );
    
    // Frontend.cpp
    void            ZeroPerformanceCounters( void );
    void            RenderView( viewDefptr_t parms );
    idScreenRect    ScreenRectFromViewFrustumBounds( const idBounds &bounds );
    void            ShowColoredScreenRect( const idScreenRect &rect, int colorIndex );
    void            SetViewMatrix( viewDefptr_t viewDef );
    void            GlobalToNormalizedDeviceCoordinates( const idVec3 &global, idVec3 &ndc );

    // Frontend_trisurf.cpp
    void            InitTriSurfData( void );
    void            ShutdownTriSurfData( void );
    srfTriangles_t*	AllocStaticTriSurf( void );
    srfTriangles_t*	CopyStaticTriSurf( const srfTriangles_t *tri );
    void            AllocStaticTriSurfVerts( srfTriangles_t *tri, int numVerts );
    void            AllocStaticTriSurfIndexes( srfTriangles_t *tri, int numIndexes );
    void            AllocStaticTriSurfShadowVerts( srfTriangles_t *tri, int numVerts );
    void            AllocStaticTriSurfPlanes( srfTriangles_t *tri, int numIndexes );
    void            ResizeStaticTriSurfVerts( srfTriangles_t *tri, int numVerts );
    void            ResizeStaticTriSurfIndexes( srfTriangles_t *tri, int numIndexes );
    void            ResizeStaticTriSurfShadowVerts( srfTriangles_t *tri, int numVerts );
    void            ReferenceStaticTriSurfVerts( srfTriangles_t *tri, const srfTriangles_t *reference );
    void            ReferenceStaticTriSurfIndexes( srfTriangles_t *tri, const srfTriangles_t *reference );
    void            FreeStaticTriSurfSilIndexes( srfTriangles_t *tri );
    void            FreeStaticTriSurf( srfTriangles_t *tri );
    void            FreeStaticTriSurfVertexCaches( srfTriangles_t *tri );
    void            ReallyFreeStaticTriSurf( srfTriangles_t *tri );
    void            FreeDeferredTriSurfs( frameData_t *frame );
    int             TriSurfMemory( const srfTriangles_t *tri );
    
    void            BoundTriSurf( srfTriangles_t *tri );
    void            RemoveDuplicatedTriangles( srfTriangles_t *tri );
    void            CreateSilIndexes( srfTriangles_t *tri );
    void            RemoveDegenerateTriangles( srfTriangles_t *tri );
    void            RemoveUnusedVerts( srfTriangles_t *tri );
    void            RangeCheckIndexes( const srfTriangles_t *tri );
    void            CreateVertexNormals( srfTriangles_t *tri );	// also called by dmap
    void            DeriveFacePlanes( srfTriangles_t *tri );		// also called by renderbump
    void            CleanupTriangles( srfTriangles_t *tri, bool createNormals, bool identifySilEdges, bool useUnsmoothedTangents );
    void            ReverseTriangles( srfTriangles_t *tri );

    // if the deformed verts have significant enough texture coordinate changes to reverse the texture
    // polarity of a triangle, the tangents will be incorrect
    void            DeriveTangents( srfTriangles_t *tri, bool allocFacePlanes = true );

    // Frontend_stencilshadrow.cpp
    srfTriangles_t* CreateShadowVolume( const idRenderEntityLocal *ent, const srfTriangles_t *tri, const idRenderLightLocal *light, shadowGen_t optimize, srfCullInfo_t &cullInfo );

    void            SetViewCount( int viewCount ) { this->viewCount = viewCount; }
    void            SetViewDef( const viewDefptr_t &viewDef ) { this->viewDef = viewDef; }
    int             GetViewCount( void ) const { return viewCount; }
    viewDefptr_t    GetViewDef( void ) const { return viewDef; }
    const viewEntity_t          GetWorldSpace( void )const { return viewDef->worldSpace; }
    performanceCounters_t&      GetPerformanceCounters( void ) { return pc; }
private:
    int						viewCount;		// incremented every view (twice a scene if subviewed)
                                            // and every R_MarkFragments call
    performanceCounters_t	pc;             // performance counters
    viewDefptr_t            viewDef;        // current view definition 

    // Frontend_light.cpp
    idScreenRect    CalcEntityScissorRectangle( viewEntity_t *vEntity );
    void            AddLightSurfaces( void );
    idRenderModel*  EntityDefDynamicModel( idRenderEntityLocal *def ); 
    void            AddModelSurfaces( void );
    void            RemoveUnecessaryViewLights( void );
    idScreenRect    CalcLightScissorRectangle( viewLight_t *vLight ); 

    // Frontend_lightrun.cpp
    void            FreeEntityDefCachedDynamicModel( idRenderEntityLocal *def );
    
    // Frontend_subview.cpp
    bool            PreciseCullSurface( const drawSurf_t *drawSurf, idBounds &ndcBounds );
    bool            GenerateSubViews( void );
    bool            GenerateSurfaceSubview( drawSurf_t *drawSurf );
    viewDefptr_t    MirrorViewBySurface( drawSurf_t *drawSurf );
    void            RemoteRender( drawSurf_t *surf, textureStage_t *stage );
    void            MirrorRender( drawSurf_t *surf, textureStage_t *stage, idScreenRect scissor );
    void            XrayRender( drawSurf_t *surf, textureStage_t *stage, idScreenRect scissor );
    viewDefptr_t    XrayViewBySurface( drawSurf_t *drawSurf );

    // Frontend_deform.cpp
    void            FinishDeform( drawSurf_t *drawSurf, srfTriangles_t *newTri, idDrawVert *ac );
    void            DeformDrawSurf( drawSurf_t *drawSurf );
    void            AutospriteDeform( drawSurf_t *surf );
    void            TubeDeform( drawSurf_t *surf );
    void            FlareDeform( drawSurf_t *surf );
    void            ExpandDeform( drawSurf_t *surf );
    void            MoveDeform( drawSurf_t *surf );
    void            TurbulentDeform( drawSurf_t *surf );
    void            EyeballDeform( drawSurf_t *surf );
    void            ParticleDeform( drawSurf_t *surf, bool useArea );

    // Frontend_guisurf.cpp
    void            RenderGuiSurf( idUserInterface *gui, drawSurf_t *drawSurf );
    
    // Frontend_stencilshadow.cpp
    static void     MakeShadowFrustums( idRenderLightLocal *def );
    
    // Frontend.cpp
    void            SetupProjection( void );
    void            SetupViewFrustum( void );
    void            ConstrainViewFrustum( void );
    void            SortDrawSurfs( void ); 

    // Frontend_trisurf.cpp
    void            BuildDominantTris( srfTriangles_t *tri ); 
    void            PurgeTriSurfData( frameData_t *frame );
    srfTriangles_t* MergeSurfaceList( const srfTriangles_t **surfaces, int numSurfaces );
    srfTriangles_t* MergeTriangles( const srfTriangles_t *tri1, const srfTriangles_t *tri2 );
    void            CreateDupVerts( srfTriangles_t *tri );
    deformInfo_t*   BuildDeformInfo( int numVerts, const idDrawVert *verts, int numIndexes, const int *indexes, bool useUnsmoothedTangents );
    void            IdentifySilEdges( srfTriangles_t *tri, bool omitCoplanarEdges );
    void            DeriveUnsmoothedTangents( srfTriangles_t *tri );
    void            DeriveTangentsWithoutNormals( srfTriangles_t *tri );

public:
    static bool     CullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes );
    static bool     RadiusCullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes );
    static bool     CornerCullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes );
    
    // Fontend_lightrun.cpp
    static void     CheckForEntityDefsUsingModel( idRenderModel *model );
    static void     FreeEntityDefDerivedData( idRenderEntityLocal *def, bool keepDecals, bool keepCachedDynamicModel );
    static void     ClearEntityDefDynamicModel( idRenderEntityLocal *def );
    static void     SetLightProject( idPlane lightProject[4], const idVec3 origin, const idVec3 targetPoint, const idVec3 rightVector, const idVec3 upVector, const idVec3 start, const idVec3 stop );
    static void     FreeLightDefDerivedData( idRenderLightLocal *light );
    static void     DeriveLightData( idRenderLightLocal *light );
    static void     CreateLightDefFogPortals( idRenderLightLocal *ldef );
    static void     FreeEntityDefDecals( idRenderEntityLocal *def );
    static void     FreeEntityDefOverlay( idRenderEntityLocal *def );
    
    // Frontend_light.cpp
    static void     CreateVertexProgramShadowCache( srfTriangles_t *tri );
    
    // Frontend_guisurf.cpp
    static void     SurfaceToTextureAxis( const srfTriangles_t *tri, idVec3 &origin, idVec3 axis[3] );
    
    // returns the frustum planes in world space
    static void     RenderLightFrustum( const struct renderLight_s &renderLight, idPlane lightFrustum[6] );

    // Frontend_trisurf.cpp
    static void     DuplicateMirroredVertexes( srfTriangles_t *tri );
    static void     DeriveFaceTangents( const srfTriangles_t *tri, faceTangents_t *faceTangents );

};

#endif //!__FRONTEND_H__