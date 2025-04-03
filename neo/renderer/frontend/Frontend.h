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
    viewEntity_t*   SetEntityDefViewEntity( idRenderEntityLocal *def );
    
    // Frontend.cpp
    void            RenderView( viewDef_t *parms );
    idScreenRect    ScreenRectFromViewFrustumBounds( const idBounds &bounds );
    void            ShowColoredScreenRect( const idScreenRect &rect, int colorIndex );

    
private:
	viewDef_t *				viewDef;

    // Frontend_light.cpp
    void            AddLightSurfaces( void );
    void            CreatePrivateShadowCache( srfTriangles_t *tri );
    void            LinkLightSurf( const drawSurf_t **link, const srfTriangles_t *tri, const viewEntity_t *space, const idRenderLightLocal *light,
        const idMaterial *shader, const idScreenRect &scissor, bool viewInsideShadow );
    void            AddDrawSurf( const srfTriangles_t *tri, const viewEntity_t *space, const renderEntity_t *renderEntity, const idMaterial *shader,
        const idScreenRect &scissor );
    bool            CreateAmbientCache( srfTriangles_t *tri, bool needsLighting );
    bool            CreateLightingCache( const idRenderEntityLocal *ent, const idRenderLightLocal *light, srfTriangles_t *tri );
    void            CreateVertexProgramShadowCache( srfTriangles_t *tri );
    void            AddAmbientDrawsurfs( viewEntity_t *vEntity );
    void            AddModelSurfaces( void );
    void            RemoveUnecessaryViewLights( void );

    // Frontend_subview.cpp
    bool            PreciseCullSurface( const drawSurf_t *drawSurf, idBounds &ndcBounds );
    bool            GenerateSubViews( void );
    

    // Frontend.cpp
    void            SetViewMatrix( viewDef_t *viewDef );
    void            SetupProjection( void );
    void            SetupViewFrustum( void );
    void            GlobalToNormalizedDeviceCoordinates( const idVec3 &global, idVec3 &ndc );
    void            ConstrainViewFrustum( void );
    void            SortDrawSurfs( void ); 
    
public:
    static bool     CullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    static bool     RadiusCullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    static bool     CornerCullLocalBox( const idBounds &bounds, const float modelMatrix[16], int numPlanes, const idPlane *planes );
    static void     AxisToModelMatrix( const idMat3 &axis, const idVec3 &origin, float modelMatrix[16] );
    static void     LocalPointToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    static void     GlobalPointToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    static void     PointTimesMatrix( const float modelMatrix[16], const idVec4 &in, idVec4 &out );
    static void     LocalVectorToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    static void     GlobalVectorToLocal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    static void     GlobalPlaneToLocal( const float modelMatrix[16], const idPlane &in, idPlane &out );
    static void     LocalPointToGlobal( const float modelMatrix[16], const idVec3 &in, idVec3 &out );
    static void     LocalPlaneToGlobal( const float modelMatrix[16], const idPlane &in, idPlane &out );
    static void     TransformEyeZToWin( float src_z, const float *projectionMatrix, float &dst_z );
    static void     TransformModelToClip( const idVec3 &src, const float *modelMatrix, const float *projectionMatrix, idPlane &eye, idPlane &dst );
    static void     TransformClipToDevice( const idPlane &clip, const viewDef_t *view, idVec3 &normalized );
    static void     TransposeGLMatrix( const float in[16], float out[16] );
    static void     myGlMultMatrix( const float *a, const float *b, float *out );
};

#endif //!__FRONTEND_H__