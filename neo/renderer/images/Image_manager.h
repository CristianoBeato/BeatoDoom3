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
#ifndef __IMAGE_MANAGER_H__
#define __IMAGE_MANAGER_H__


// data is RGBA
void	R_WriteTGA( const char *filename, const byte *data, int width, int height, bool flipVertical = false );
// data is an 8 bit index into palette, which is RGB (no A)
void	R_WritePalTGA( const char *filename, const byte *data, const byte *palette, int width, int height, bool flipVertical = false );
// data is in top-to-bottom raster order unless flipVertical is set

class idImageManager 
{
public:
	void				Init( void );
	void				Shutdown( void );

	// If the exact combination of parameters has been asked for already, an existing
	// image will be returned, otherwise a new image will be created.
	// Be careful not to use the same image file with different filter / repeat / etc parameters
	// if possible, because it will cause a second copy to be loaded.
	// If the load fails for any reason, the image will be filled in with the default
	// grid pattern.
	// Will automatically resample non-power-of-two images and execute image programs if needed.
	idImage *			ImageFromFile( const char *name,
							 textureFilter_t filter, bool allowDownSize,
							 textureRepeat_t repeat, textureDepth_t depth, cubeFiles_t cubeMap = CF_2D );

	// look for a loaded image, whatever the parameters
	idImage *			GetImage( const char *name ) const;

	// The callback will be issued immediately, and later if images are reloaded or vid_restart
	// The callback function should call one of the idImage::Generate* functions to fill in the data
	idImage *			ImageFromFunction( const char *name, void (*generatorFunction)( idImage *image ));

	// called once a frame to allow any background loads that have been completed
	// to turn into textures.
	void				CompleteBackgroundImageLoads();

	// returns the number of bytes of image data bound in the previous frame
	int					SumOfUsedImages();

	// called each frame to allow some cvars to automatically force changes
	void				CheckCvars();

	// purges all the images before a vid_restart
	void				PurgeAllImages();

	// reloads all apropriate images after a vid_restart
	void				ReloadAllImages();

	// disable the active texture unit
	void				BindNull();

	// Mark all file based images as currently unused,
	// but don't free anything.  Calls to ImageFromFile() will
	// either mark the image as used, or create a new image without
	// loading the actual data.
	// Called only by renderSystem::BeginLevelLoad
	void				BeginLevelLoad();

	// Free all images marked as unused, and load all images that are necessary.
	// This architecture prevents us from having the union of two level's
	// worth of data present at one time.
	// Called only by renderSystem::EndLevelLoad
	void				EndLevelLoad();

	// used to clear and then write the dds conversion batch file
	void				StartBuild();
	void				FinishBuild( bool removeDups = false );
	void				AddDDSCommand( const char *cmd );

	void				PrintMemInfo( MemInfo_t *mi );

	// cvars
	static idCVar		image_roundDown;			// round bad sizes down to nearest power of two
	static idCVar		image_colorMipLevels;		// development aid to see texture mip usage
	static idCVar		image_downSize;				// controls texture downsampling
	static idCVar		image_useCompression;		// 0 = force everything to high quality
	static idCVar		image_filter;				// changes texture filtering on mipmapped images
	static idCVar		image_anisotropy;			// set the maximum texture anisotropy if available
	static idCVar		image_lodbias;				// change lod bias on mipmapped images
	static idCVar		image_useAllFormats;		// allow alpha/intensity/luminance/luminance+alpha
	static idCVar		image_usePrecompressedTextures;	// use .dds files if present
	static idCVar		image_writePrecompressedTextures; // write .dds files if necessary
	static idCVar		image_writeNormalTGA;		// debug tool to write out .tgas of the final normal maps
	static idCVar		image_writeNormalTGAPalletized;		// debug tool to write out palletized versions of the final normal maps
	static idCVar		image_writeTGA;				// debug tool to write out .tgas of the non normal maps
	static idCVar		image_useNormalCompression;	// 1 = use 256 color compression for normal maps if available, 2 = use rxgb compression
	static idCVar		image_useOffLineCompression; // will write a batch file with commands for the offline compression
	static idCVar		image_preload;				// if 0, dynamically load all images
	static idCVar		image_cacheMinK;			// maximum K of precompressed files to read at specification time,
													// the remainder will be dynamically cached
	static idCVar		image_cacheMegs;			// maximum bytes set aside for temporary loading of full-sized precompressed images
	static idCVar		image_useCache;				// 1 = do background load image caching
	static idCVar		image_showBackgroundLoads;	// 1 = print number of outstanding background loads
	static idCVar		image_forceDownSize;		// allows the ability to force a downsize
	static idCVar		image_downSizeSpecular;		// downsize specular
	static idCVar		image_downSizeSpecularLimit;// downsize specular limit
	static idCVar		image_downSizeBump;			// downsize bump maps
	static idCVar		image_downSizeBumpLimit;	// downsize bump limit
	static idCVar		image_ignoreHighQuality;	// ignore high quality on materials
	static idCVar		image_downSizeLimit;		// downsize diffuse limit

	// built-in images
	idImage *			defaultImage;
	idImage *			flatNormalMap;				// 128 128 255 in all pixels
	idImage *			ambientNormalMap;			// tr.ambientLightVector encoded in all pixels
	idImage *			rampImage;					// 0-255 in RGBA in S
	idImage *			alphaRampImage;				// 0-255 in alpha, 255 in RGB
	idImage *			alphaNotchImage;			// 2x1 texture with just 1110 and 1111 with point sampling
	idImage *			whiteImage;					// full of 0xff
	idImage *			blackImage;					// full of 0x00
	idImage *			normalCubeMapImage;			// cube map to normalize STR into RGB
	idImage *			noFalloffImage;				// all 255, but zero clamped
	idImage *			fogImage;					// increasing alpha is denser fog
	idImage *			fogEnterImage;				// adjust fogImage alpha based on terminator plane
	idImage *			cinematicImage;
	idImage *			scratchImage;
	idImage *			scratchImage2;
	idImage *			accumImage;
	idImage *			currentRenderImage;			// for SS_POST_PROCESS shaders
	idImage *			scratchCubeMapImage;
	idImage *			specularTableImage;			// 1D intensity texture with our specular function
	idImage *			specular2DTableImage;		// 2D intensity texture with our specular function with variable specularity
	idImage *			borderClampImage;			// white inside, black outside

	//--------------------------------------------------------
	
	idImage *			AllocImage( const char *name );
//	void				SetNormalPalette();
	void				ChangeTextureFilter();

	idList<idImage*>	images;
	idStrList			ddsList;
	idHashIndex			ddsHash;

	bool				insideLevelLoad;			// don't actually load images now

	byte				originalToCompressed[256];	// maps normal maps to 8 bit textures
	byte				compressedPalette[768];		// the palette that normal maps use

	// default filter modes for images
	uint32_t			textureMinFilter;
	uint32_t			textureMaxFilter;
	float				textureAnisotropy;
	float				textureLODBias;

	idImage *			imageHashTable[FILE_HASH_SIZE];

	idImage *			backgroundImageLoads;		// chain of images that have background file loads active
	idImage				cacheLRU;					// head/tail of doubly linked list
	int					totalCachedImageSize;		// for determining when something should be purged

	int	numActiveBackgroundImageLoads;
	const static int MAX_BACKGROUND_IMAGE_LOADS = 8;
};

extern idImageManager	*globalImages;		// pointer to global list for the rest of the system

#endif //!__IMAGE_MANAGER_H__
