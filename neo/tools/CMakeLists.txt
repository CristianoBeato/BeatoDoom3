##################################
##	Beato idTech4 Game Engine	##
##	Tools:		 Engine SDK		##
##################################

include_directories( ${JPEG_INCLUDE_DIR} )

SET( TOOLS_SOURECES 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/compiler_public.h
	${CMAKE_CURRENT_SOURCE_DIR}/edit_public.h
	)

source_group("" FILES ${TOOLS_SOURECES} )

##
## IA Area Awareness System compiler
## Generate AI wakable coordinate geometry
SET( TOOLS_COMPILER_AAS
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild_file.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild_gravity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild_ledge.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild_merge.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASCluster.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFile_optimize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFile_sample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFileManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASReach.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/Brush.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/BrushBSP.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASBuild_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASCluster.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFile.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFile_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASFileManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/AASReach.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/Brush.h
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/aas/BrushBSP.h
	)

source_group("compilers\\AAS" FILES ${TOOLS_COMPILER_AAS} )

##
## compiles MAP files into the BSP
## Generate the visible Binare Space Partition brushes, and ocluders
SET( TOOLS_COMPILER_DMAP
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/dmap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/facebsp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/gldraw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/glfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/leakfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/map.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/optimize.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/optimize_gcc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/output.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/portals.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/shadowopt3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/tritjunction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/tritools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/ubrush.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/usurface.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/dmap/dmap.h
	)

source_group("compilers\\dmap" FILES ${TOOLS_COMPILER_DMAP} )

##
## renderbump
## 
SET( TOOLS_COMPILER_RENDEBUMP ${CMAKE_CURRENT_SOURCE_DIR}/compilers/renderbump/renderbump.cpp )
source_group("compilers\\renderbump" FILES ${TOOLS_COMPILER_RENDEBUMP} )

##
## roqvq
## id's Quake video format for cinematics ( a pre historic 'bink' video )
SET( TOOLS_COMPILER_ROQ
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/codec.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/NSBitmapImageRep.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/roq.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/roqParam.cpp 
	
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/codec.h 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/gdefs.h 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/quaddefs.h 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/roq.h 
	${CMAKE_CURRENT_SOURCE_DIR}/compilers/roqvq/roqParam.h 
	)
	
source_group("compilers\\roqvq" FILES ${TOOLS_COMPILER_ROQ} )

#if( BUILD_TOOLS )
#	SET( TOOLS_SOURCES_ALL 
#	)
#else( BUILD_TOOLS)
	SET( TOOLS_SOURCES_ALL
		${CMAKE_CURRENT_SOURCE_DIR}/edit_stub.cpp
		${TOOLS_SOURECES}
		${TOOLS_COMPILER_AAS}
		${TOOLS_COMPILER_DMAP}
		${TOOLS_COMPILER_RENDEBUMP}
		${TOOLS_COMPILER_ROQ}
		)
#endif( BUILD_TOOLS )

add_library( tools STATIC ${TOOLS_SOURCES_ALL} )
add_dependencies( tools idlib )
target_precompile_headers( tools
	PUBLIC
		${CMAKE_SOURCE_DIR}/idlib/precompiled.h
	PRIVATE
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/edit_public.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/compilers/compiler_public.h>"
		)
