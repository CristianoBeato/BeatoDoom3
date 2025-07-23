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

#include "precompiled.h"
#pragma hdrstop

#include "Frontend.h"

#include "renderer/renderer_common.h"

crFrontend::crFrontend( void ) : 
	viewCount( 0 ),
	viewDef()
{
	viewCount = 1;		// so cleared structures never match viewCount
}

crFrontend::~crFrontend( void )
{
}

//====================================================================


/*
======================
crFrontend::ScreenRectFromViewFrustumBounds
======================
*/
idScreenRect crFrontend::ScreenRectFromViewFrustumBounds( const idBounds &bounds ) 
{
	idScreenRect screenRect;

	screenRect.x1 = idMath::FtoiFast( 0.5f * ( 1.0f - bounds[1].y ) * ( viewDef->viewport.x2 - viewDef->viewport.x1 ) );
	screenRect.x2 = idMath::FtoiFast( 0.5f * ( 1.0f - bounds[0].y ) * ( viewDef->viewport.x2 - viewDef->viewport.x1 ) );
	screenRect.y1 = idMath::FtoiFast( 0.5f * ( 1.0f + bounds[0].z ) * ( viewDef->viewport.y2 - viewDef->viewport.y1 ) );
	screenRect.y2 = idMath::FtoiFast( 0.5f * ( 1.0f + bounds[1].z ) * ( viewDef->viewport.y2 - viewDef->viewport.y1 ) );

	if ( r_useDepthBoundsTest.GetInteger() ) 
	{
		screenRect.zmin = viewDef->projectionMatrix.TransformEyeZToWin( -bounds[0].x );
		screenRect.zmax = viewDef->projectionMatrix.TransformEyeZToWin( -bounds[1].x );
	}

	return screenRect;
}


/*
======================
ShowColoredScreenRect
======================
*/
void crFrontend::ShowColoredScreenRect( const idScreenRect &rect, int colorIndex ) 
{
	if ( !rect.IsEmpty() ) 
	{
		static idVec4 colors[] = { colorRed, colorGreen, colorBlue, colorYellow, colorMagenta, colorCyan, colorWhite, colorPurple };
		viewDef->renderWorld->DebugScreenRect( colors[colorIndex & 7], rect, viewDef );
	}
}

//==========================================================================

/*
=================
crFrontend::RadiusCullLocalBox

A fast, conservative center-to-corner culling test
Returns true if the box is outside the given global frustum, (positive sides are out)
=================
*/
bool crFrontend::RadiusCullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes ) 
{
	int			i;
	float		d;
	idVec3		worldOrigin;
	float		worldRadius;
	const idPlane	*frust;

	if ( r_useCulling.GetInteger() == 0 )
		return false;

	// transform the surface bounds into world space
	idVec3	localOrigin = ( bounds[0] + bounds[1] ) * 0.5;

	worldOrigin = modelMatrix.LocalPointToGlobal( localOrigin );
	worldRadius = (bounds[0] - localOrigin).Length();	// FIXME: won't be correct for scaled objects

	for ( i = 0 ; i < numPlanes ; i++ ) {
		frust = planes + i;
		d = frust->Distance( worldOrigin );
		if ( d > worldRadius ) {
			return true;	// culled
		}
	}

	return false;		// no culled
}

/*
=================
crFrontend::CornerCullLocalBox

Tests all corners against the frustum.
Can still generate a few false positives when the box is outside a corner.
Returns true if the box is outside the given global frustum, (positive sides are out)
=================
*/
bool crFrontend::CornerCullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes ) 
{
	int			i, j;
	idVec3		transformed[8];
	float		dists[8];
	idVec3		v;
	const idPlane *frust;

	// we can disable box culling for experimental timing purposes
	if ( r_useCulling.GetInteger() < 2 ) 
		return false;
	

	// transform into world space
	for ( i = 0 ; i < 8 ; i++ ) 
	{
		v[0] = bounds[i&1][0];
		v[1] = bounds[(i>>1)&1][1];
		v[2] = bounds[(i>>2)&1][2];

		transformed[i] = modelMatrix.LocalPointToGlobal( v );
	}

	// check against frustum planes
	for ( i = 0 ; i < numPlanes ; i++ ) 
	{
		frust = planes + i;
		for ( j = 0 ; j < 8 ; j++ ) 
		{
			dists[j] = frust->Distance( transformed[j] );
			if ( dists[j] < 0 ) 
				break;
		}
		
		if ( j == 8 ) 
		{
			// all points were behind one of the planes
			tr.frontend->GetPerformanceCounters().c_box_cull_out++;
			return true;
		}
	}

	tr.frontend->GetPerformanceCounters().c_box_cull_in++;

	return false;		// not culled
}

/*
=================
crFrontend::CullLocalBox

Performs quick test before expensive test
Returns true if the box is outside the given global frustum, (positive sides are out)
=================
*/
bool crFrontend::CullLocalBox( const idBounds &bounds, const crRenderMatrix modelMatrix, int numPlanes, const idPlane *planes ) 
{
	if ( RadiusCullLocalBox( bounds, modelMatrix, numPlanes, planes ) ) 
		return true;
	
	return CornerCullLocalBox( bounds, modelMatrix, numPlanes, planes );
}

/*
==========================
R_GlobalToNormalizedDeviceCoordinates

-1 to 1 range in x, y, and z
==========================
*/
void crFrontend::GlobalToNormalizedDeviceCoordinates( const idVec3 &global, idVec3 &ndc ) 
{
	int		i = 0;
	idPlane	view;
	idPlane	clip;

	// _D3XP added work on primaryView when no viewDef
	if ( !viewDef ) 
	{
		for ( i = 0 ; i < 4 ; i ++ ) 
		{
			view[i] = 
				global[0] * tr.primaryView->worldSpace.modelViewMatrix[ i + 0 * 4 ] +
				global[1] * tr.primaryView->worldSpace.modelViewMatrix[ i + 1 * 4 ] +
				global[2] * tr.primaryView->worldSpace.modelViewMatrix[ i + 2 * 4 ] +
					tr.primaryView->worldSpace.modelViewMatrix[ i + 3 * 4 ];
		}

		for ( i = 0 ; i < 4 ; i ++ ) 
		{
			clip[i] = 
				view[0] * tr.primaryView->projectionMatrix[ i + 0 * 4 ] +
				view[1] * tr.primaryView->projectionMatrix[ i + 1 * 4 ] +
				view[2] * tr.primaryView->projectionMatrix[ i + 2 * 4 ] +
				view[3] * tr.primaryView->projectionMatrix[ i + 3 * 4 ];
		}
	} 
	else 
	{
		for ( i = 0 ; i < 4 ; i ++ ) 
		{
			view[i] = 
				global[0] * viewDef->worldSpace.modelViewMatrix[ i + 0 * 4 ] +
				global[1] * viewDef->worldSpace.modelViewMatrix[ i + 1 * 4 ] +
				global[2] * viewDef->worldSpace.modelViewMatrix[ i + 2 * 4 ] +
				viewDef->worldSpace.modelViewMatrix[ i + 3 * 4 ];
		}

		for ( i = 0 ; i < 4 ; i ++ ) {
			clip[i] = 
				view[0] * viewDef->projectionMatrix[ i + 0 * 4 ] +
				view[1] * viewDef->projectionMatrix[ i + 1 * 4 ] +
				view[2] * viewDef->projectionMatrix[ i + 2 * 4 ] +
				view[3] * viewDef->projectionMatrix[ i + 3 * 4 ];
		}
	}

	ndc[0] = clip[0] / clip[3];
	ndc[1] = clip[1] / clip[3];
	ndc[2] = ( clip[2] + clip[3] ) / ( 2 * clip[3] );
}

/*
=================
crFrontend::SetViewMatrix

Sets up the world to view matrix for a given viewParm
=================
*/
void crFrontend::SetViewMatrix( viewDefptr_t viewDef ) 
{
	idVec3	origin;
	viewEntity_t *world;
	alignas( 16 ) crRenderMatrix viewerMatrix;
	alignas( 16 ) crRenderMatrix s_flipMatrix; 

	// convert from our coordinate system (looking down X)
	// to OpenGL's coordinate system (looking down -Z)
	s_flipMatrix[0] =  0.0f;
	s_flipMatrix[1] =  0.0f;
	s_flipMatrix[2] = -1.0f;
	s_flipMatrix[3] =  0.0f;

	s_flipMatrix[4] = -1.0f;
	s_flipMatrix[5] =  0.0f;
	s_flipMatrix[6] =  0.0f;
	s_flipMatrix[7] =  0.0f;

	s_flipMatrix[8] =  0.0f;
	s_flipMatrix[9] =  1.0f;
	s_flipMatrix[10] = 0.0f;
	s_flipMatrix[11] = 0.0f;

	s_flipMatrix[12] = 0.0f;
	s_flipMatrix[13] = 0.0f;
	s_flipMatrix[14] = 0.0f;
	s_flipMatrix[15] = 0.1f;

	world = &viewDef->worldSpace;

	memset( world, 0x00, sizeof(*world) );

	// the model matrix is an identity
	world->modelMatrix[0*4+0] = 1;
	world->modelMatrix[1*4+1] = 1;
	world->modelMatrix[2*4+2] = 1;

	// transform by the camera placement
	origin = viewDef->renderView.vieworg;

	// BEATO TODO: use SIMD for this
	viewerMatrix[0] = viewDef->renderView.viewaxis[0][0];
	viewerMatrix[4] = viewDef->renderView.viewaxis[0][1];
	viewerMatrix[8] = viewDef->renderView.viewaxis[0][2];
	viewerMatrix[12] = -origin[0] * viewerMatrix[0] + -origin[1] * viewerMatrix[4] + -origin[2] * viewerMatrix[8];

	viewerMatrix[1] = viewDef->renderView.viewaxis[1][0];
	viewerMatrix[5] = viewDef->renderView.viewaxis[1][1];
	viewerMatrix[9] = viewDef->renderView.viewaxis[1][2];
	viewerMatrix[13] = -origin[0] * viewerMatrix[1] + -origin[1] * viewerMatrix[5] + -origin[2] * viewerMatrix[9];

	viewerMatrix[2] = viewDef->renderView.viewaxis[2][0];
	viewerMatrix[6] = viewDef->renderView.viewaxis[2][1];
	viewerMatrix[10] = viewDef->renderView.viewaxis[2][2];
	viewerMatrix[14] = -origin[0] * viewerMatrix[2] + -origin[1] * viewerMatrix[6] + -origin[2] * viewerMatrix[10];

	viewerMatrix[3] = 0;
	viewerMatrix[7] = 0;
	viewerMatrix[11] = 0;
	viewerMatrix[15] = 1;

	// convert from our coordinate system (looking down X)
	// to OpenGL's coordinate system (looking down -Z)
	world->modelViewMatrix = viewerMatrix * s_flipMatrix;
}

/*
===============
R_SetupProjection

This uses the "infinite far z" trick
===============
*/
void crFrontend::SetupProjection( void ) 
{
	float	xmin, xmax, ymin, ymax;
	float	width, height;
	float	zNear;
	float	jitterx, jittery;
	static	idRandom random;

	// random jittering is usefull when multiple
	// frames are going to be blended together
	// for motion blurred anti-aliasing
	if ( r_jitter.GetBool() ) 
	{
		jitterx = random.RandomFloat();
		jittery = random.RandomFloat();
	} 
	else 
	{
		jitterx = jittery = 0;
	}

	//
	// set up projection matrix
	//
	zNear	= r_znear.GetFloat();
	if ( viewDef->renderView.cramZNear )
		zNear *= 0.25;

	ymax = zNear * tan( viewDef->renderView.fov_y * idMath::PI / 360.0f );
	ymin = -ymax;

	xmax = zNear * tan( viewDef->renderView.fov_x * idMath::PI / 360.0f );
	xmin = -xmax;

	width = xmax - xmin;
	height = ymax - ymin;

	jitterx = jitterx * width / ( viewDef->viewport.x2 - viewDef->viewport.x1 + 1 );
	xmin += jitterx;
	xmax += jitterx;
	jittery = jittery * height / ( viewDef->viewport.y2 - viewDef->viewport.y1 + 1 );
	ymin += jittery;
	ymax += jittery;

	viewDef->projectionMatrix[0] = 2 * zNear / width;
	viewDef->projectionMatrix[4] = 0;
	viewDef->projectionMatrix[8] = ( xmax + xmin ) / width;	// normally 0
	viewDef->projectionMatrix[12] = 0;

	viewDef->projectionMatrix[1] = 0;
	viewDef->projectionMatrix[5] = 2 * zNear / height;
	viewDef->projectionMatrix[9] = ( ymax + ymin ) / height;	// normally 0
	viewDef->projectionMatrix[13] = 0;

	// this is the far-plane-at-infinity formulation, and
	// crunches the Z range slightly so w=0 vertexes do not
	// rasterize right at the wraparound point
	viewDef->projectionMatrix[2] = 0;
	viewDef->projectionMatrix[6] = 0;
	viewDef->projectionMatrix[10] = -0.999f;
	viewDef->projectionMatrix[14] = -2.0f * zNear;

	viewDef->projectionMatrix[3] = 0;
	viewDef->projectionMatrix[7] = 0;
	viewDef->projectionMatrix[11] = -1;
	viewDef->projectionMatrix[15] = 0;
}

/*
=================
crFrontend::SetupViewFrustum

Setup that culling frustum planes for the current view
FIXME: derive from modelview matrix times projection matrix
=================
*/
void crFrontend::SetupViewFrustum( void ) 
{
	int		i = 0;
	float	xs = 0.0f, xc = 0.0f;
	float	ang = 0.0f;

	ang = DEG2RAD( viewDef->renderView.fov_x ) * 0.5f;
	idMath::SinCos( ang, xs, xc );

	viewDef->frustum[0] = xs * viewDef->renderView.viewaxis[0] + xc * viewDef->renderView.viewaxis[1];
	viewDef->frustum[1] = xs * viewDef->renderView.viewaxis[0] - xc * viewDef->renderView.viewaxis[1];

	ang = DEG2RAD( viewDef->renderView.fov_y ) * 0.5f;
	idMath::SinCos( ang, xs, xc );

	viewDef->frustum[2] = xs * viewDef->renderView.viewaxis[0] + xc * viewDef->renderView.viewaxis[2];
	viewDef->frustum[3] = xs * viewDef->renderView.viewaxis[0] - xc * viewDef->renderView.viewaxis[2];

	// plane four is the front clipping plane
	viewDef->frustum[4] = /* vec3_origin - */ viewDef->renderView.viewaxis[0];

	for ( i = 0; i < 5; i++ ) 
	{
		// flip direction so positive side faces out (FIXME: globally unify this)
		viewDef->frustum[i] = - viewDef->frustum[i].Normal();
		viewDef->frustum[i][3] = -( viewDef->renderView.vieworg * viewDef->frustum[i].Normal() );
	}

	// eventually, plane five will be the rear clipping plane for fog

	float dNear, dFar, dLeft, dUp;

	dNear = r_znear.GetFloat();
	if ( viewDef->renderView.cramZNear ) 
		dNear *= 0.25f;

	dFar = MAX_WORLD_SIZE;
	dLeft = dFar * tan( DEG2RAD( viewDef->renderView.fov_x * 0.5f ) );
	dUp = dFar * tan( DEG2RAD( viewDef->renderView.fov_y * 0.5f ) );
	viewDef->viewFrustum.SetOrigin( viewDef->renderView.vieworg );
	viewDef->viewFrustum.SetAxis( viewDef->renderView.viewaxis );
	viewDef->viewFrustum.SetSize( dNear, dFar, dLeft, dUp );
}

/*
===================
R_ConstrainViewFrustum
===================
*/
void crFrontend::ConstrainViewFrustum( void ) 
{
	idBounds bounds;

	// constrain the view frustum to the total bounds of all visible lights and visible entities
	bounds.Clear();
	for ( viewLight_t *vLight = viewDef->viewLights; vLight; vLight = vLight->next )
	{
		bounds.AddBounds( vLight->lightDef->frustumTris->bounds );
	}
	
	for ( viewEntity_t *vEntity = viewDef->viewEntitys; vEntity; vEntity = vEntity->next ) 
	{
		bounds.AddBounds( vEntity->entityDef->referenceBounds );
	}

	viewDef->viewFrustum.ConstrainToBounds( bounds );

	if ( r_useFrustumFarDistance.GetFloat() > 0.0f ) 
	{
		viewDef->viewFrustum.MoveFarDistance( r_useFrustumFarDistance.GetFloat() );
	}
}

/*
==========================================================================================

DRAWSURF SORTING

==========================================================================================
*/


/*
=======================
R_QsortSurfaces

=======================
*/
static int QsortSurfaces( const void *a, const void *b ) 
{
	const drawSurf_t	*ea, *eb;

	ea = *(drawSurf_t **)a;
	eb = *(drawSurf_t **)b;

	if ( ea->sort < eb->sort ) 
		return -1;
	
	if ( ea->sort > eb->sort ) 
		return 1;

	return 0;
}


/*
=================
crFrontend::SortDrawSurfs
=================
*/
void crFrontend::SortDrawSurfs( void )
{
	// sort the drawsurfs by sort type, then orientation, then shader
	qsort( viewDef->drawSurfs, viewDef->numDrawSurfs, sizeof( viewDef->drawSurfs[0] ), QsortSurfaces );
}



//========================================================================


//==============================================================================

void crFrontend::ZeroPerformanceCounters(void)
{
	pc = performanceCounters_t(); 
}

/*
================
crFrontend::RenderView

A view may be either the actual camera view,
a mirror / remote location, or a 3D view on a gui surface.

Parms will typically be allocated with R_FrameAlloc
================
*/
void crFrontend::RenderView( viewDefptr_t parms ) 
{
	viewDefptr_t oldView;

	if ( parms->renderView.width <= 0 || parms->renderView.height <= 0 )
		return;

	viewCount++;

	// save view in case we are a subview
	oldView = viewDef;

	viewDef = parms;

	tr.sortOffset = 0;

	// set the matrix for world space to eye space
	SetViewMatrix( viewDef );

	// the four sides of the view frustum are needed
	// for culling and portal visibility
	SetupViewFrustum();

	// we need to set the projection matrix before doing
	// portal-to-screen scissor box calculations
	SetupProjection();

	// identify all the visible portalAreas, and the entityDefs and
	// lightDefs that are in them and pass culling.
	static_cast<idRenderWorldLocal *>(parms->renderWorld)->FindViewLightsAndEntities();

	// constrain the view frustum to the view lights and entities
	ConstrainViewFrustum();

	// make sure that interactions exist for all light / entity combinations
	// that are visible
	// add any pre-generated light shadows, and calculate the light shader values
	AddLightSurfaces();

	// adds ambient surfaces and create any necessary interaction surfaces to add to the light
	// lists
	AddModelSurfaces();

	// any viewLight that didn't have visible surfaces can have it's shadows removed
	RemoveUnecessaryViewLights();

	// sort all the ambient surfaces for translucency ordering
	SortDrawSurfs();

	// generate any subviews (mirrors, cameras, etc) before adding this view
	if ( GenerateSubViews() ) 
	{
		// if we are debugging subviews, allow the skipping of the
		// main view draw
		if ( r_subviewOnly.GetBool() ) 
			return;
	}

	// write everything needed to the demo file
	if ( session->writeDemo ) 
	{
		static_cast<idRenderWorldLocal *>(parms->renderWorld)->WriteVisibleDefs( viewDef );
	}

	// add the rendering commands for this viewDef
	tr.drawQueue->AddDrawViewCmd( parms );

	// restore view in case we are a subview
	viewDef = oldView;
}
