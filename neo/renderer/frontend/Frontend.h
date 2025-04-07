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

class crFrontend
{
public:
    crFrontend( void );
    ~crFrontend( void );

    // Frontend_light.cpp
    viewEntity_t*               SetEntityDefViewEntity( idRenderEntityLocal *def );
    bool                        IssueEntityDefCallback( idRenderEntityLocal *def );

    // Frontend_lightrun.cpp
    void                        CreateLightRefs( idRenderLightLocal *light );
    void                        CreateEntityRefs( idRenderEntityLocal *def );
    void                        ReCreateWorldReferences( void );
    void                        FreeDerivedData( void );
    
    // Frontend.cpp
    void                        RenderView( crAutoPointer<viewDef_t> parms );
    idScreenRect                ScreenRectFromViewFrustumBounds( const idBounds &bounds );
    void                        ShowColoredScreenRect( const idScreenRect &rect, int colorIndex );
    void                        SetViewMatrix( crAutoPointer<viewDef_t> viewDef );
    void                        GlobalToNormalizedDeviceCoordinates( const idVec3 &global, idVec3 &ndc );

    crAutoPointer<viewDef_t>    GetViewDef( void ) const { return viewDef; }
    void                        SetViewDef( const crAutoPointer<viewDef_t> &viewDef ) { this->viewDef = viewDef; }
    void                        SetViewCount( int viewCount ) { this->viewCount = viewCount; }
    int                         GetViewCount( void ) const { return viewCount; }

private:
    int						viewCount;		// incremented every view (twice a scene if subviewed)
                                            // and every R_MarkFragments call
    crAutoPointer<viewDef_t>    viewDef; // current view definition 

    // Frontend_light.cpp
    idScreenRect    CalcEntityScissorRectangle( viewEntity_t *vEntity );
    void            AddLightSurfaces( void );
    void            CreatePrivateShadowCache( srfTriangles_t *tri );
    void            LinkLightSurf( const drawSurf_t **link, const srfTriangles_t *tri, const viewEntity_t *space, const idRenderLightLocal *light,
        const idMaterial *shader, const idScreenRect &scissor, bool viewInsideShadow );
    idRenderModel*  EntityDefDynamicModel( idRenderEntityLocal *def ); 
    void            AddDrawSurf( const srfTriangles_t *tri, const viewEntity_t *space, const renderEntity_t *renderEntity, const idMaterial *shader,
        const idScreenRect &scissor );
    bool            CreateAmbientCache( srfTriangles_t *tri, bool needsLighting );
    bool            CreateLightingCache( const idRenderEntityLocal *ent, const idRenderLightLocal *light, srfTriangles_t *tri );
    void            CreateVertexProgramShadowCache( srfTriangles_t *tri );
    void            AddAmbientDrawsurfs( viewEntity_t *vEntity );
    void            AddModelSurfaces( void );
    void            RemoveUnecessaryViewLights( void );
    viewLight_t*    SetLightDefViewLight( idRenderLightLocal *def );
    idScreenRect    CalcLightScissorRectangle( viewLight_t *vLight ); 


    // Frontend_lightrun.cpp
    void            FreeEntityDefCachedDynamicModel( idRenderEntityLocal *def );
    void            FreeEntityDefFadedDecals( idRenderEntityLocal *def, int time );
    
    // Frontend_subview.cpp
    bool            PreciseCullSurface( const drawSurf_t *drawSurf, idBounds &ndcBounds );
    bool            GenerateSubViews( void );
    bool            GenerateSurfaceSubview( drawSurf_t *drawSurf );
    viewDef_t*      MirrorViewBySurface( drawSurf_t *drawSurf );
    void            RemoteRender( drawSurf_t *surf, textureStage_t *stage );
    void            MirrorRender( drawSurf_t *surf, textureStage_t *stage, idScreenRect scissor );
    void            XrayRender( drawSurf_t *surf, textureStage_t *stage, idScreenRect scissor );
    viewDef_t*      XrayViewBySurface( drawSurf_t *drawSurf );

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


public:
    static bool     CullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    static bool     RadiusCullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    static bool     CornerCullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    
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
    
    // Frontend_guisurf.cpp
    static void     SurfaceToTextureAxis( const srfTriangles_t *tri, idVec3 &origin, idVec3 axis[3] );
    
    // returns the frustum planes in world space
    static void     RenderLightFrustum( const struct renderLight_s &renderLight, idPlane lightFrustum[6] );

};

#endif //!__FRONTEND_H__