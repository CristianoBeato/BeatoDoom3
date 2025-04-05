/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
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

#ifndef __RENDER_VIEW_H__
#define __RENDER_VIEW_H__

#include "renderer/ScreenRect.h"

static const uint32_t MAX_GLOBAL_SHADER_PARMS	= 12;
static const uint32_t MAX_CLIP_PLANES	= 1;				// we may expand this to six for some subview issues

class idMaterial;
class idRenderWorldLocal;
class idRenderEntityLocal;
typedef struct drawSurf_s drawSurf_t;

typedef struct renderView_s 
{
	// player views will set this to a non-zero integer for model suppress / allow
	// subviews (mirrors, cameras, etc) will always clear it to zero
	int						viewID;

	// sized from 0 to SCREEN_WIDTH / SCREEN_HEIGHT (640/480), not actual resolution
	int						x, y, width, height;

	float					fov_x, fov_y;
	idVec3					vieworg;
	idMat3					viewaxis;			// transformation matrix, view looks down the positive X axis

	bool					cramZNear;			// for cinematics, we want to set ZNear much lower
	bool					forceUpdate;		// for an update 

	// time in milliseconds for shader effects and other time dependent rendering issues
	int						time;
	float					shaderParms[MAX_GLOBAL_SHADER_PARMS];		// can be used in any way by shader
	const idMaterial		*globalMaterial;							// used to override everything draw
} renderView_t;

// a viewEntity is created whenever a idRenderEntityLocal is considered for inclusion
// in the current view, but it may still turn out to be culled.
// viewEntity are allocated on the frame temporary stack memory
// a viewEntity contains everything that the back end needs out of a idRenderEntityLocal,
// which the front end may be modifying simultaniously if running in SMP mode.
// A single entityDef can generate multiple viewEntity_t in a single frame, as when seen in a mirror
typedef struct viewEntity_s 
{
	struct viewEntity_s	*next;

	// back end should NOT reference the entityDef, because it can change when running SMP
	idRenderEntityLocal	*entityDef;

	// for scissor clipping, local inside renderView viewport
	// scissorRect.Empty() is true if the viewEntity_t was never actually
	// seen through any portals, but was created for shadow casting.
	// a viewEntity can have a non-empty scissorRect, meaning that an area
	// that it is in is visible, and still not be visible.
	idScreenRect		scissorRect;

	bool				weaponDepthHack;
	float				modelDepthHack;

	float				modelMatrix[16];		// local coords to global coords
	float				modelViewMatrix[16];	// local coords to eye coords
} viewEntity_t;

// viewDefs are allocated on the frame temporary stack memory
typedef struct viewDef_s 
{
	// specified in the call to DrawScene()
	renderView_t		renderView;

	float				projectionMatrix[16];
	viewEntity_t		worldSpace;

	idRenderWorldLocal *renderWorld;

	float				floatTime;

	idVec3				initialViewAreaOrigin;
	// Used to find the portalArea that view flooding will take place from.
	// for a normal view, the initialViewOrigin will be renderView.viewOrg,
	// but a mirror may put the projection origin outside
	// of any valid area, or in an unconnected area of the map, so the view
	// area must be based on a point just off the surface of the mirror / subview.
	// It may be possible to get a failed portal pass if the plane of the
	// mirror intersects a portal, and the initialViewAreaOrigin is on
	// a different side than the renderView.viewOrg is.

	bool				isSubview;				// true if this view is not the main view
	bool				isMirror;				// the portal is a mirror, invert the face culling
	bool				isXraySubview;

	bool				isEditor;

	int					numClipPlanes;			// mirrors will often use a single clip plane
	idPlane				clipPlanes[MAX_CLIP_PLANES];		// in world space, the positive side
												// of the plane is the visible side
	idScreenRect		viewport;				// in real pixels and proper Y flip

	idScreenRect		scissor;
	// for scissor clipping, local inside renderView viewport
	// subviews may only be rendering part of the main view
	// these are real physical pixel values, possibly scaled and offset from the
	// renderView x/y/width/height

	struct viewDef_s *	superView;				// never go into an infinite subview loop 
	struct drawSurf_s *	subviewSurface;

	// drawSurfs are the visible surfaces of the viewEntities, sorted
	// by the material sort parameter
	drawSurf_t **		drawSurfs;				// we don't use an idList for this, because
	int					numDrawSurfs;			// it is allocated in frame temporary memory
	int					maxDrawSurfs;			// may be resized

	struct viewLight_s	*viewLights;			// chain of all viewLights effecting view
	struct viewEntity_s	*viewEntitys;			// chain of all viewEntities effecting view, including off screen ones casting shadows
	// we use viewEntities as a check to see if a given view consists solely
	// of 2D rendering, which we can optimize in certain ways.  A 2D view will
	// not have any viewEntities

	idPlane				frustum[5];				// positive sides face outward, [4] is the front clip plane
	idFrustum			viewFrustum;

	int					areaNum;				// -1 = not in a valid area

	bool *				connectedAreas;
	// An array in frame temporary memory that lists if an area can be reached without
	// crossing a closed door.  This is used to avoid drawing interactions
	// when the light is behind a closed door.

} viewDef_t;

#endif //__RENDER_VIEW_H__