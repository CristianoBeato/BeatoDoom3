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
		set( BUILD_PLATFORM_POSTFIX "x86" )
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
	add_compile_options(-pipe)
	add_compile_options(-Wall)
	
	set(CMAKE_C_FLAGS_DEBUG "-g -ggdb -D_DEBUG -O0")
	set(CMAKE_C_FLAGS_DEBUGALL "-g -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_PROFILE "-g -ggdb -D_DEBUG -O1 -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -fno-math-errno -fno-trapping-math -fomit-frame-pointer")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -ggdb -O2 -fno-math-errno -fno-trapping-math -fno-omit-frame-pointer")
	set(CMAKE_C_FLAGS_MINSIZEREL "-Os -fno-math-errno -fno-trapping-math -fomit-frame-pointer")

	set(CMAKE_CXX_FLAGS_DEBUGALL ${CMAKE_C_FLAGS_DEBUGALL})
	set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
elseif(MSVC)
	add_compile_options(/MP) # parallel build (use all cores, or as many as configured in VS)
	set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG /Od /Zi /MDd")
	set(CMAKE_C_FLAGS_RELEASE "/Ox /Oy /MD")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Ox /Oy /Zi /MD")
	set(CMAKE_C_FLAGS_MINSIZEREL "/Ox /Oy /Os /MD")
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()


###########################
##   	 Libraries     	 ##
###########################
find_package( OPENGL REQUIRED )
find_package( SDL REQUIRED )
find_package( ZLIB REQUIRED )
find_package( OPENAL REQUIRED )
find_package( OGG REQUIRED )
find_package( VORBIS REQUIRED )
find_package( JPEG REQUIRED )

include_directories(
				${CMAKE_SOURCE_DIR}
				${CMAKE_SOURCE_DIR}/contrib/OpenGL
				${SDL_INCLUDE_DIR}
				${ZLIB_INCLUDE_DIR}
					)
			
###########################
##		 Engine 		 ##
###########################			
add_subdirectory( ${CMAKE_SOURCE_DIR}/idlib )		# Engine utils library
add_subdirectory( ${CMAKE_SOURCE_DIR}/framework )	# Engine framework commons 
add_subdirectory( ${CMAKE_SOURCE_DIR}/renderer )	# Renderer system
add_subdirectory( ${CMAKE_SOURCE_DIR}/sound )		# Sound system 
add_subdirectory( ${CMAKE_SOURCE_DIR}/sys )			# Nomportable system
add_subdirectory( ${CMAKE_SOURCE_DIR}/tools )		# Engine SDK
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