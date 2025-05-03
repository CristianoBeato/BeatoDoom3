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

#ifndef __RENDER_ENTITY_H__
#define __RENDER_ENTITY_H__

// guis
static const int MAX_RENDERENTITY_GUI = 3;

typedef bool(*deferredEntityCallback_t)( renderEntity_s *, const renderView_s * );

class idRenderModel;
class idRenderModelDecal;
class idRenderModelOverlay;
class idJointMat;
class idMaterial;
class idDeclSkin;
class idInteraction;
typedef struct areaReference_s areaReference_t;

typedef struct renderEntity_s 
{
	idRenderModel *			hModel;				// this can only be null if callback is set

	int						entityNum;
	int						bodyId;

	// Entities that are expensive to generate, like skeletal models, can be
	// deferred until their bounds are found to be in view, in the frustum
	// of a shadowing light that is in view, or contacted by a trace / overlay test.
	// This is also used to do visual cueing on items in the view
	// The renderView may be NULL if the callback is being issued for a non-view related
	// source.
	// The callback function should clear renderEntity->callback if it doesn't
	// want to be called again next time the entity is referenced (ie, if the
	// callback has now made the entity valid until the next updateEntity)
	idBounds				bounds;					// only needs to be set for deferred models and md5s
	deferredEntityCallback_t	callback;

	void *					callbackData;			// used for whatever the callback wants

	// player bodies and possibly player shadows should be suppressed in views from
	// that player's eyes, but will show up in mirrors and other subviews
	// security cameras could suppress their model in their subviews if we add a way
	// of specifying a view number for a remoteRenderMap view
	int						suppressSurfaceInViewID;
	int						suppressShadowInViewID;

	// world models for the player and weapons will not cast shadows from view weapon
	// muzzle flashes
	int						suppressShadowInLightID;

	// if non-zero, the surface and shadow (if it casts one)
	// will only show up in the specific view, ie: player weapons
	int						allowSurfaceInViewID;

	// positioning
	// axis rotation vectors must be unit length for many
	// R_LocalToGlobal functions to work, so don't scale models!
	// axis vectors are [0] = forward, [1] = left, [2] = up
	idVec3					origin;
	idMat3					axis;

	// texturing
	const idMaterial *		customShader;			// if non-0, all surfaces will use this
	const idMaterial *		referenceShader;		// used so flares can reference the proper light shader
	const idDeclSkin *		customSkin;				// 0 for no remappings
	class idSoundEmitter *	referenceSound;			// for shader sound tables, allowing effects to vary with sounds
	float					shaderParms[ MAX_ENTITY_SHADER_PARMS ];	// can be used in any way by shader or model generation

	// networking: see WriteGUIToSnapshot / ReadGUIFromSnapshot
	class idUserInterface * gui[ MAX_RENDERENTITY_GUI ];

	struct renderView_s	*	remoteRenderView;		// any remote camera surfaces will use this

	int						numJoints;
	idJointMat *			joints;					// array of joints that will modify vertices.
													// NULL if non-deformable model.  NOT freed by renderer

	float					modelDepthHack;			// squash depth range so particle effects don't clip into walls

	// options to override surface shader flags (replace with material parameters?)
	bool					noSelfShadow;			// cast shadows onto other objects,but not self
	bool					noShadow;				// no shadow at all

	bool					noDynamicInteractions;	// don't create any light / shadow interactions after
													// the level load is completed.  This is a performance hack
													// for the gigantic outdoor meshes in the monorail map, so
													// all the lights in the moving monorail don't touch the meshes

	bool					weaponDepthHack;		// squash depth range so view weapons don't poke into walls
													// this automatically implies noShadow
	int						forceUpdate;			// force an update (NOTE: not a bool to keep this struct a multiple of 4 bytes)
	int						timeGroup;
	int						xrayIndex;
} renderEntity_t;

// idRenderEntity should become the new public interface replacing the qhandle_t to entity defs in the idRenderWorld interface
class idRenderEntity 
{
public:
	virtual					~idRenderEntity( void ) {}
	virtual void			FreeRenderEntity( void ) = 0;
	virtual void			UpdateRenderEntity( const renderEntity_t *re, bool forceUpdate = false ) = 0;
	virtual void			GetRenderEntity( renderEntity_t *re ) = 0;
	virtual void			ForceUpdate( void ) = 0;
	virtual int				GetIndex( void ) const = 0;

	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay( const idPlane localTextureAxis[2], const idMaterial *material ) = 0;
	virtual void			RemoveDecals( void ) = 0;
};

// keep it here, or move outside ? 
class idRenderEntityLocal : public idRenderEntity 
{
public:
							idRenderEntityLocal( void );
	virtual void			FreeRenderEntity( void );
	virtual void			UpdateRenderEntity( const renderEntity_t *re, bool forceUpdate = false );
	virtual void			GetRenderEntity( renderEntity_t *re );
	virtual void			ForceUpdate( void );
	virtual int				GetIndex( void ) const;

	// overlays are extra polygons that deform with animating models for blood and damage marks
	virtual void			ProjectOverlay( const idPlane localTextureAxis[2], const idMaterial *material );
	virtual void			RemoveDecals( void );

	renderEntity_t			parms;

	crRenderMatrix			modelMatrix;			// this is just a rearrangement of parms.axis and parms.origin

	idRenderWorldLocal *	world;
	int						index;					// in world entityDefs

	int						lastModifiedFrameNum;	// to determine if it is constantly changing,
													// and should go in the dynamic frame memory, or kept
													// in the cached memory
	bool					archived;				// for demo writing

	idRenderModel *			dynamicModel;			// if parms.model->IsDynamicModel(), this is the generated data
	int						dynamicModelFrameCount;	// continuously animating dynamic models will recreate
													// dynamicModel if this doesn't == tr.viewCount
	idRenderModel *			cachedDynamicModel;

	idBounds				referenceBounds;		// the local bounds used to place entityRefs, either from parms or a model

	// a viewEntity_t is created whenever a idRenderEntityLocal is considered for inclusion
	// in a given view, even if it turns out to not be visible
	int						viewCount;				// if tr.viewCount == viewCount, viewEntity is valid,
													// but the entity may still be off screen
	struct viewEntity_s *	viewEntity;				// in frame temporary memory

	int						visibleCount;
	// if tr.viewCount == visibleCount, at least one ambient
	// surface has actually been added by R_AddAmbientDrawsurfs
	// note that an entity could still be in the view frustum and not be visible due
	// to portal passing

	idRenderModelDecal *	decals;					// chain of decals that have been projected on this model
	idRenderModelOverlay *	overlay;				// blood overlays on animated models

	areaReference_t *		entityRefs;				// chain of all references
	idInteraction *			firstInteraction;		// doubly linked list
	idInteraction *			lastInteraction;

	bool					needsPortalSky;
};

#endif // __RENDER_ENTITY_H__