##################################
##	Beato idTech4 Game Engine	##
##################################

cmake_minimum_required(VERSION 3.16)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

##################
## Project Name ##
##################
project(Beato-idTech4)

## STD 17
set( CMAKE_CXX_STANDARD 17)

############################
## Current Build Platform ##
############################
if(CMAKE_CL_64)
		set( BUILD_PLATFORM_POSTFIX "x64" )
else(CMAKE_CL_64)
		set( BUILD_PLATFORM_POSTFIX "x32" )
endif(CMAKE_CL_64)

############################
##    Project Options     ##
############################
option( BUILD_GAME				"Build Defalt D3 Game logic" ON )
option( BUILD_GAME_D3XP			"Build D3 RoE Game Locig" ON )
option( BUILD_DEDICATED			"Build Dedicated to server" OFF )
option( BUILD_MONOLITH			"" OFF ) 
option(	BUILD_TOOLS				"" OFF )

############################
##  Build Configuration   ##
############################
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID STREQUAL "Clang")

	# memory debugging
#	add_compile_definitions( ID_DEBUG_MEMORY )
#	add_compile_definitions( ID_DEBUG_UNINITIALIZED_MEMORY )
	
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	add_compile_options(-fPIC)
	add_compile_options(-Wunknown-pragmas)

	set(CMAKE_C_FLAGS_DEBUG "-g -ggdb -D_DEBUG -O0")
	set(CMAKE_C_FLAGS_DEBUGALL "-g -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_PROFILE "-g -ggdb -D_DEBUG -O1 -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -fno-math-errno -fno-trapping-math -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -ggdb -O2 -fno-math-errno -fno-trapping-math -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -fno-math-errno -fno-trapping-math -fomit-frame-pointer")

	set(CMAKE_CXX_FLAGS_DEBUGALL ${CMAKE_C_FLAGS_DEBUGALL})
	set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()


###########################
##   	 Libraries     	 ##
###########################
find_package( ZLIB REQUIRED )
find_package( OpenAL REQUIRED )
find_package( SDL2 REQUIRED )
find_package( JPEG REQUIRED )
#find_package( OpenGL REQUIRED )
#find_package( OGG REQUIRED )
#find_package( VORBIS REQUIRED )

include_directories(
				${CMAKE_SOURCE_DIR}
				${SDL2_INCLUDE_DIR}
					)
			
###########################
##		 Engine 		 ##
###########################			
add_subdirectory( ${CMAKE_SOURCE_DIR}/idlib )		# Engine utils library
add_subdirectory( ${CMAKE_SOURCE_DIR}/TypeInfo )	#
add_subdirectory( ${CMAKE_SOURCE_DIR}/contrib )		# Thirdy party libs

###########################
##		Game logic Lib	 ##
###########################
if( BUILD_GAME )
	add_subdirectory( ${CMAKE_SOURCE_DIR}/game )		# Doom 3 Game Logic
endif( BUILD_GAME )

###########################
##		D3XP logic Lib	 ##
###########################
if( BUILD_GAME_D3XP )
	add_subdirectory( ${CMAKE_SOURCE_DIR}/d3xp )		# Doom 3 Ressurection of Evil Expansion Game logic 
endif( BUILD_GAME_D3XP )

###########################
##		 Engine 		 ##
###########################			


##
## Framework
## Engine Core Structures
SET( FRAMEWORK_SOURCES
	${CMAKE_SOURCE_DIR}/framework/EditField.cpp
	${CMAKE_SOURCE_DIR}/framework/BuildDefines.h
	${CMAKE_SOURCE_DIR}/framework/BuildVersion.h
	${CMAKE_SOURCE_DIR}/framework/CmdSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/CmdSystem.h
	${CMAKE_SOURCE_DIR}/framework/Common.cpp
	${CMAKE_SOURCE_DIR}/framework/Common.h
	${CMAKE_SOURCE_DIR}/framework/Compressor.cpp
	${CMAKE_SOURCE_DIR}/framework/Compressor.h
	${CMAKE_SOURCE_DIR}/framework/Console.cpp
	${CMAKE_SOURCE_DIR}/framework/Console.h
	${CMAKE_SOURCE_DIR}/framework/CVarSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/CVarSystem.h
	${CMAKE_SOURCE_DIR}/framework/DeclEntityDef.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclEntityDef.h
	${CMAKE_SOURCE_DIR}/framework/DeclAF.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclAF.h
	${CMAKE_SOURCE_DIR}/framework/DeclFX.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclFX.h
	${CMAKE_SOURCE_DIR}/framework/DeclManager.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclManager.h
	${CMAKE_SOURCE_DIR}/framework/DeclParticle.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclParticle.h
	${CMAKE_SOURCE_DIR}/framework/DeclPDA.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclPDA.h
	${CMAKE_SOURCE_DIR}/framework/DeclSkin.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclSkin.h
	${CMAKE_SOURCE_DIR}/framework/DeclTable.cpp
	${CMAKE_SOURCE_DIR}/framework/DeclTable.h
	${CMAKE_SOURCE_DIR}/framework/DemoChecksum.h
	${CMAKE_SOURCE_DIR}/framework/DemoFile.cpp
	${CMAKE_SOURCE_DIR}/framework/DemoFile.h
	${CMAKE_SOURCE_DIR}/framework/EditField.h
	${CMAKE_SOURCE_DIR}/framework/EventLoop.cpp
	${CMAKE_SOURCE_DIR}/framework/EventLoop.h
	${CMAKE_SOURCE_DIR}/framework/File.cpp
	${CMAKE_SOURCE_DIR}/framework/File.h
	${CMAKE_SOURCE_DIR}/framework/FileSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/FileSystem.h
	${CMAKE_SOURCE_DIR}/framework/KeyInput.cpp
	${CMAKE_SOURCE_DIR}/framework/KeyInput.h
	${CMAKE_SOURCE_DIR}/framework/Licensee.h
	${CMAKE_SOURCE_DIR}/framework/Session.cpp
	${CMAKE_SOURCE_DIR}/framework/Session.h
	${CMAKE_SOURCE_DIR}/framework/Session_menu.cpp
	${CMAKE_SOURCE_DIR}/framework/Session_local.h
#	${CMAKE_SOURCE_DIR}/framework/Unzip.cpp
#	${CMAKE_SOURCE_DIR}/framework/Unzip.h
	${CMAKE_SOURCE_DIR}/framework/UsercmdGen.cpp
	${CMAKE_SOURCE_DIR}/framework/UsercmdGen.h
) 

source_group("" FILES ${FRAMEWORK_SOURCES} )

##
## Async
## Server Client utils 
SET( FRAMEWORK_ASYNC_SOURCES
	${CMAKE_SOURCE_DIR}/framework/async/AsyncClient.cpp
	${CMAKE_SOURCE_DIR}/framework/async/AsyncNetwork.cpp
	${CMAKE_SOURCE_DIR}/framework/async/AsyncServer.cpp
	${CMAKE_SOURCE_DIR}/framework/async/MsgChannel.cpp
	${CMAKE_SOURCE_DIR}/framework/async/NetworkSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/async/ServerScan.cpp
	
	${CMAKE_SOURCE_DIR}/framework/async/AsyncClient.h
	${CMAKE_SOURCE_DIR}/framework/async/AsyncNetwork.h
	${CMAKE_SOURCE_DIR}/framework/async/AsyncServer.h
	${CMAKE_SOURCE_DIR}/framework/async/MsgChannel.h
	${CMAKE_SOURCE_DIR}/framework/async/NetworkSystem.h
	${CMAKE_SOURCE_DIR}/framework/async/ServerScan.h
	) 

source_group("Async" FILES ${FRAMEWORK_ASYNC_SOURCES} )

##
## CollisionModel
## Colision detection utils
SET( FRAMEWORK_COLISION_SOURCES
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_contacts.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_contents.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_debug.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_files.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_load.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_rotate.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_trace.cpp
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_translate.cpp
	
	${CMAKE_SOURCE_DIR}/cm/CollisionModel.h
	${CMAKE_SOURCE_DIR}/cm/CollisionModel_local.h
	) 

source_group("CollisionModel" FILES ${FRAMEWORK_COLISION_SOURCES} )

##
## Ui
## Game Ui structures
SET( FRAMEWORK_UI_SOURCES
	${CMAKE_SOURCE_DIR}/ui/BindWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/BindWindow.h
	${CMAKE_SOURCE_DIR}/ui/ChoiceWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/ChoiceWindow.h
	${CMAKE_SOURCE_DIR}/ui/DeviceContext.cpp
	${CMAKE_SOURCE_DIR}/ui/DeviceContext.h
	${CMAKE_SOURCE_DIR}/ui/EditWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/EditWindow.h
	${CMAKE_SOURCE_DIR}/ui/FieldWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/FieldWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameBearShootWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameBearShootWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameBustOutWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameBustOutWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameSSDWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameSSDWindow.h
	${CMAKE_SOURCE_DIR}/ui/GuiScript.cpp
	${CMAKE_SOURCE_DIR}/ui/GuiScript.h
	${CMAKE_SOURCE_DIR}/ui/ListGUI.cpp
	${CMAKE_SOURCE_DIR}/ui/ListGUI.h
	${CMAKE_SOURCE_DIR}/ui/ListGUILocal.h
	${CMAKE_SOURCE_DIR}/ui/ListWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/ListWindow.h
	${CMAKE_SOURCE_DIR}/ui/MarkerWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/MarkerWindow.h
	${CMAKE_SOURCE_DIR}/ui/RegExp.cpp
	${CMAKE_SOURCE_DIR}/ui/RegExp.h
	${CMAKE_SOURCE_DIR}/ui/RenderWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/RenderWindow.h
	${CMAKE_SOURCE_DIR}/ui/Rectangle.h
	${CMAKE_SOURCE_DIR}/ui/SimpleWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/SimpleWindow.h
	${CMAKE_SOURCE_DIR}/ui/SliderWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/SliderWindow.h
	${CMAKE_SOURCE_DIR}/ui/UserInterface.cpp
	${CMAKE_SOURCE_DIR}/ui/UserInterface.h
	${CMAKE_SOURCE_DIR}/ui/UserInterfaceLocal.h
	${CMAKE_SOURCE_DIR}/ui/Window.cpp
	${CMAKE_SOURCE_DIR}/ui/Window.h
	${CMAKE_SOURCE_DIR}/ui/Winvar.cpp
	${CMAKE_SOURCE_DIR}/ui/Winvar.h
	) 

source_group("Ui" FILES ${FRAMEWORK_UI_SOURCES} )

##
## Renderer
## Core renderer engine 
SET( RENDERER_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/Cinematic.cpp
	${CMAKE_SOURCE_DIR}/renderer/Cinematic.h
	${CMAKE_SOURCE_DIR}/renderer/Material.cpp
	${CMAKE_SOURCE_DIR}/renderer/Material.h
	${CMAKE_SOURCE_DIR}/renderer/MegaTexture.cpp
	${CMAKE_SOURCE_DIR}/renderer/MegaTexture.h
	${CMAKE_SOURCE_DIR}/renderer/RenderSystem.cpp
	${CMAKE_SOURCE_DIR}/renderer/RenderSystem_init.cpp
	${CMAKE_SOURCE_DIR}/renderer/RenderSystem.h
	${CMAKE_SOURCE_DIR}/renderer/VertexCache.cpp
	${CMAKE_SOURCE_DIR}/renderer/VertexCache.h
	${CMAKE_SOURCE_DIR}/renderer/simplex.h
	)
	
source_group("" FILES ${RENDERER_SOURCES} )


##
## Backend
## 
SET( RENDERER_BAKCEND_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/draw_arb.cpp
	${CMAKE_SOURCE_DIR}/renderer/draw_arb2.cpp
	${CMAKE_SOURCE_DIR}/renderer/draw_common.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_backend.cpp
	)

source_group("Backend" FILES ${RENDERER_BAKCEND_SOURCES} )

##
## Frontend
## 
SET( RENDERER_FRONTEND_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/Interaction.cpp
	${CMAKE_SOURCE_DIR}/renderer/Interaction.h
	${CMAKE_SOURCE_DIR}/renderer/tr_deform.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_font.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_font.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_guisurf.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_light.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_lightrun.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_main.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_orderIndexes.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_polytope.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_render.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_rendertools.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_shadowbounds.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_stencilshadow.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_subview.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_trace.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_trisurf.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_turboshadow.cpp
	${CMAKE_SOURCE_DIR}/renderer/tr_local.h
	)
	
source_group("Frontend" FILES ${RENDERER_FRONTEND_SOURCES} )

##
## Imagens
## 
SET( RENDERER_IMAGENS_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/Image_files.cpp
	${CMAKE_SOURCE_DIR}/renderer/Image_init.cpp
	${CMAKE_SOURCE_DIR}/renderer/Image_load.cpp
	${CMAKE_SOURCE_DIR}/renderer/Image_process.cpp
	${CMAKE_SOURCE_DIR}/renderer/Image_program.cpp
	${CMAKE_SOURCE_DIR}/renderer/Image.h
	)
	
source_group("Imagens" FILES ${RENDERER_IMAGENS_SOURCES} )

##
## Models
## 
SET( RENDERER_MODELS_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/GuiModel.cpp
	${CMAKE_SOURCE_DIR}/renderer/GuiModel.h
	${CMAKE_SOURCE_DIR}/renderer/Model.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model.h
	${CMAKE_SOURCE_DIR}/renderer/Model_ase.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_ase.h
	${CMAKE_SOURCE_DIR}/renderer/Model_beam.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_liquid.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_lwo.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_lwo.h
	${CMAKE_SOURCE_DIR}/renderer/Model_ma.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_ma.h
	${CMAKE_SOURCE_DIR}/renderer/Model_md3.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_md3.h
	${CMAKE_SOURCE_DIR}/renderer/Model_md5.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_prt.cpp
	${CMAKE_SOURCE_DIR}/renderer/Model_sprite.cpp
	${CMAKE_SOURCE_DIR}/renderer/ModelDecal.cpp
	${CMAKE_SOURCE_DIR}/renderer/ModelDecal.h
	${CMAKE_SOURCE_DIR}/renderer/ModelManager.cpp
	${CMAKE_SOURCE_DIR}/renderer/ModelManager.h
	${CMAKE_SOURCE_DIR}/renderer/ModelOverlay.cpp
	${CMAKE_SOURCE_DIR}/renderer/ModelOverlay.h
	${CMAKE_SOURCE_DIR}/renderer/Model_local.h
	)
	
source_group("Models" FILES ${RENDERER_MODELS_SOURCES} )

##
## OpenGL
## 
SET( RENDERER_OPENGL_SOURCES
	${CMAKE_SOURCE_DIR}/renderer/qgl.cpp
	${CMAKE_SOURCE_DIR}/renderer/qgl.h
	${CMAKE_SOURCE_DIR}/renderer/qvk.cpp
	${CMAKE_SOURCE_DIR}/renderer/qvk.h
	)
	
source_group("OpenGL" FILES ${RENDERER_OPENGL_SOURCES} )

##
## Render World
## 
SET( RENDERER_RENDERWORL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderEntity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/ModelOverlay.h
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld.h
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld_load.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/renderer/RenderWorld_portals.cpp

	)
	
source_group("RenderWorld" FILES ${RENDERER_RENDERWORL_SOURCES} )

##
## Sound
## Sound Engine structures
SET( SOUND_SOURCES
	${CMAKE_SOURCE_DIR}/sound/snd_cache.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_decoder.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_efxfile.cpp
	${CMAKE_SOURCE_DIR}/sound/efxlib.h
	${CMAKE_SOURCE_DIR}/sound/snd_emitter.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_shader.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_system.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_wavefile.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_world.cpp
	${CMAKE_SOURCE_DIR}/sound/snd_local.h
	${CMAKE_SOURCE_DIR}/sound/sound.h
	)

source_group("" FILES ${SOUND_SOURCES} )

SET( TOOLS_SOURECES 
	${CMAKE_SOURCE_DIR}/tools/edit_stub.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/compiler_public.h
	${CMAKE_SOURCE_DIR}/tools/edit_public.h
	)

source_group("" FILES ${TOOLS_SOURECES} )

##
## IA Area Awareness System compiler
## Generate AI wakable coordinate geometry
SET( TOOLS_COMPILER_AAS
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild_file.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild_local.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild_gravity.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild_ledge.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASBuild_merge.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASCluster.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASCluster.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFile.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFile.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFile_local.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFile_optimize.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFile_sample.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFileManager.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASFileManager.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASReach.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/AASReach.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/Brush.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/Brush.h
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/BrushBSP.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/aas/BrushBSP.h
	)

source_group("compilers\\AAS" FILES ${TOOLS_COMPILER_AAS} )

##
## compiles MAP files into the BSP
## Generate the visible Binare Space Partition brushes, and ocluders
SET( TOOLS_COMPILER_DMAP
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/dmap.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/dmap.h
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/facebsp.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/gldraw.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/glfile.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/leakfile.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/map.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/optimize.cpp
#	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/optimize_gcc.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/output.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/portals.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/shadowopt3.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/tritjunction.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/tritools.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/ubrush.cpp
	${CMAKE_SOURCE_DIR}/tools/compilers/dmap/usurface.cpp
	)

source_group("compilers\\dmap" FILES ${TOOLS_COMPILER_DMAP} )

##
## renderbump
## Obsolete: Whos gonna need a bump/normal map tool,
## when the modern modelers and 3D softwares have this tool ( and better )
##

#SET( TOOLS_COMPILER_RENDEBUMP ${CMAKE_CURRENT_SOURCE_DIR}/compilers/renderbump/renderbump.cpp )
#source_group("compilers\\renderbump" FILES ${TOOLS_COMPILER_RENDEBUMP} )

##
## roqvq
## id's Quake video format for cinematics ( a pre historic 'bink' video )
SET( TOOLS_COMPILER_ROQ
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/codec.cpp 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/codec.h 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/NSBitmapImageRep.cpp 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/roq.cpp 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/roq.h 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/roqParam.cpp 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/roqParam.h 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/quaddefs.h 
	${CMAKE_SOURCE_DIR}/tools/compilers/roqvq/gdefs.h 
	)
	
source_group("compilers\\roqvq" FILES ${TOOLS_COMPILER_ROQ} )

SET( ENGINE_SOURCES 
	${CMAKE_SOURCE_DIR}/sys/sys_consoleOut.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_net.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_cpuinfo.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_events.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_file.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_local.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_local.h
	${CMAKE_SOURCE_DIR}/sys/sys_main.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_main.h
	${CMAKE_SOURCE_DIR}/sys/sys_openGL.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_paths.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_platform.h
	${CMAKE_SOURCE_DIR}/sys/sys_process.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_public.h
	${CMAKE_SOURCE_DIR}/sys/sys_video.cpp
	${CMAKE_SOURCE_DIR}/sys/sys_vulkanContext.cpp
	)

source_group("" FILES ${ENGINE_SOURCES} )

set( BEATO_DOOM_SOURCES 
	${FRAMEWORK_SOURCES}
	${FRAMEWORK_ASYNC_SOURCES}
	${FRAMEWORK_COLISION_SOURCES}
	${FRAMEWORK_UI_SOURCES}
	${RENDERER_SOURCES}
	${RENDERER_BAKCEND_SOURCES}
	${RENDERER_FRONTEND_SOURCES}
	${RENDERER_RENDERWORL_SOURCES}
	${RENDERER_IMAGENS_SOURCES}
	${RENDERER_MODELS_SOURCES}
	${RENDERER_OPENGL_SOURCES}
	${SOUND_SOURCES}
	${TOOLS_SOURECES}
	${TOOLS_COMPILER_AAS}
	${TOOLS_COMPILER_DMAP} 
	${TOOLS_COMPILER_ROQ}
	${ENGINE_SOURCES}
	)

##################################
##	Beato idTech4 Game Engine	##
##	Sys Engine Main Loop system	##
##################################

set( LIN_LIBS 
	ld
	)

if( CMAKE_HOST_WIN32 )
	set( PLATFORM_LIBS 
		iphlpapi
		odbc32
		odbccp32
		winmm
		wsock32;
	)
elseif( CMAKE_HOST_LINUX )
	set( PLATFORM_LIBS 
		dl	
	)
endif()

#
# 
#
SET( ENGINE_SOURCES_ALL 
	${CMAKE_CURRENT_SOURCE_DIR}/edit_stub.cpp
	${TOOLS_SOURECES}
	${TOOLS_COMPILER_AAS}
	${TOOLS_COMPILER_DMAP}
#		${TOOLS_COMPILER_RENDEBUMP}
	${TOOLS_COMPILER_ROQ}
	${ENGINE_SOURCES}
#	${ENGINE_COMMON_SOURCES}
	)
	


SET( ENGINE_LIBS_ALL 
	idlib
	contrib
	${CORE_LIBRARIES}
	${PLATFORM_LIBS}
	${ZLIB_LIBRARY}
	${JPEG_LIBRARY}
	${SDL2_LIBRARIES}	
	openal
	ogg
	vorbis
	vorbisfile
	)

if(WIN32)
	list(APPEND ENGINE_SOURCES_ALL 	${ENGINE_WINDOWS_SOURCES} )
	list(APPEND ENGINE_LIBS_ALL ${WIN_LIBS})
endif()

#add_definitions( -D__DOOM__ -D__DOOM_DLL__ )
add_executable( beatoDOOM3 WIN32 ${BEATO_DOOM_SOURCES} )
set_target_properties( beatoDOOM3 PROPERTIES COMPILE_DEFINITIONS "__DOOM__;__DOOM_DLL__")
add_dependencies( beatoDOOM3 idlib contrib )
target_link_libraries( beatoDOOM3 ${ENGINE_LIBS_ALL} )

target_precompile_headers( beatoDOOM3
	PUBLIC
		${CMAKE_SOURCE_DIR}/idlib/precompiled.h
	PRIVATE
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/sys/sys_local.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/sys/sys_public.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/framework/Common.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/renderer/tr_local.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/sound/sound.h>"
		)