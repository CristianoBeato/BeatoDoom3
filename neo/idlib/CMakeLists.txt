##################################
##	Beato idTech4 Game Engine	##
##	idLib: Engine utility		##
##################################

add_definitions( -D__IDLIB__ )

##
## Common utils
##
SET( IDLIB_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/BitMsg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dict.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Heap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LangDict.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Lib.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MapFile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/precompiled.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SmartPointer.cpp


	${CMAKE_CURRENT_SOURCE_DIR}/BitMsg.h
	${CMAKE_CURRENT_SOURCE_DIR}/Dict.h
	${CMAKE_CURRENT_SOURCE_DIR}/Heap.h
	${CMAKE_CURRENT_SOURCE_DIR}/LangDict.h
	${CMAKE_CURRENT_SOURCE_DIR}/Lib.h
	${CMAKE_CURRENT_SOURCE_DIR}/MapFile.h
	${CMAKE_CURRENT_SOURCE_DIR}/precompiled.h
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.h
	${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
	${CMAKE_CURRENT_SOURCE_DIR}/SmartPointer.h

	) 

source_group("" FILES ${IDLIB_SOURCES} ${IDLIB_INCLUDES} )

##
## Bound Volume
##
SET ( IDLIB_BV_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Bounds.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Box.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Frustum.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Sphere.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/bv/Bounds.h
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Box.h
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Frustum.h
	${CMAKE_CURRENT_SOURCE_DIR}/bv/Sphere.h
	)
	
source_group("BV" FILES ${IDLIB_BV_SOURCES} ${IDLIB_BV_INCLUDES} )
	
##
## Containers
##
SET ( IDLIB_CONTAINERS_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/containers/HashIndex.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/containers/Array.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/containers/BinSearch.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/BTree.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/HashIndex.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/HashTable.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/Hierarchy.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/LinkList.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/List.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/PlaneSet.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/Queue.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/Stack.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/StaticList.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/StrList.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/StrPool.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/VectorSet.h
	${CMAKE_CURRENT_SOURCE_DIR}/containers/Array.h
	) 

source_group("Containers" FILES ${IDLIB_CONTAINERS_SOURCES} )

##
## Geometry
##
SET( IDLIB_GEOMETRY_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/DrawVert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/JointTransform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Patch.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Polytope.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_SweptSpline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/TraceModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding2D.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/geometry/DrawVert.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/JointTransform.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Patch.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Polytope.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_SweptSpline.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/TraceModel.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding.h
	${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding2D.h
	)

source_group("Geometry" FILES ${IDLIB_GEOMETRY_SOURCES} )

##
## Hashing
##
SET( IDLIB_HASHING_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/hashing/CRC32.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD4.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD5.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/hashing/CRC32.h
	${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD4.h
	${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD5.h
	)

source_group("Hashing" FILES ${IDLIB_HASHING_SOURCES}  )

##
## Hashing
##
SET( IDLIB_MATH_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/math/Angles.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Complex.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Lcp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Math.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Matrix.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Ode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Plane.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Pluecker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Polynomial.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Quat.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Rotation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Vector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_Generic.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_3DNow.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_AltiVec.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_MMX.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE2.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE3.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/math/Angles.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Complex.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Curve.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Extrapolate.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Interpolate.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Lcp.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Math.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Matrix.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Ode.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Plane.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Pluecker.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Polynomial.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Quat.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Random.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Rotation.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Vector.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd.h
	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_Generic.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_3DNow.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_AltiVec.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_MMX.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE2.h
#	${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE3.h
	)

source_group("Math" FILES ${IDLIB_MATH_SOURCES} )

##
## Text 
##
SET( IDLIB_TEXT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Base64.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CmdArgs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Lexer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Str.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Token.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/Base64.h
	${CMAKE_CURRENT_SOURCE_DIR}/CmdArgs.h
	${CMAKE_CURRENT_SOURCE_DIR}/Lexer.h
	${CMAKE_CURRENT_SOURCE_DIR}/Parser.h
	${CMAKE_CURRENT_SOURCE_DIR}/Str.h
	${CMAKE_CURRENT_SOURCE_DIR}/Token.h
	)

source_group("Text" FILES ${IDLIB_TEXT_SOURCES} )

##
## Our OpenGL Based GUI for the tools 
##
SET( IDLIB_TGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/BaseGui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/Canvas.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/RenderGui.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/BaseGui.h
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/Canvas.h
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/RenderGui.h
	${CMAKE_CURRENT_SOURCE_DIR}/toolsGui/GuiCommon.h
	)

source_group("GUI" FILES ${IDLIB_TGUI_SOURCES}  )



##
## The Library 
##
SET( IDLIB_SOURCES_ALL 
	${IDLIB_SOURCES}
	${IDLIB_BV_SOURCES}
	${IDLIB_CONTAINERS_SOURCES}
	${IDLIB_GEOMETRY_SOURCES}
	${IDLIB_HASHING_SOURCES}
	${IDLIB_MATH_SOURCES}
	${IDLIB_TEXT_SOURCES}
	${IDLIB_TGUI_SOURCES}
	)

add_library( idlib STATIC ${IDLIB_SOURCES_ALL} )
target_precompile_headers( idlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/precompiled.h)