##################################
##	Beato idTech4 Game Engine	##
##	TypeInfo: 					##
##################################

add_definitions( -DID_TYPEINFO -D__DOOM_DLL__ )

SET( TYPE_INFO
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/TypeInfoGen.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/TypeInfoGen.h
	)

SET( TYPE_INFO_FRAMEWORK
	${CMAKE_SOURCE_DIR}/framework/CmdSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/CVarSystem.cpp
	${CMAKE_SOURCE_DIR}/framework/File.cpp
	${CMAKE_SOURCE_DIR}/framework/FileSystem.cpp

	${CMAKE_SOURCE_DIR}/framework/CmdSystem.h
	${CMAKE_SOURCE_DIR}/framework/CVarSystem.h
	${CMAKE_SOURCE_DIR}/framework/File.h
	${CMAKE_SOURCE_DIR}/framework/FileSystem.h
	${CMAKE_SOURCE_DIR}/framework/Licensee.h
#	${CMAKE_SOURCE_DIR}/framework/Unzip.h
	)

source_group("" FILES ${TYPE_INFO})
source_group("Framework" FILES ${TYPE_INFO_FRAMEWORK})

add_executable( TypeInfo ${TYPE_INFO} ${TYPE_INFO_FRAMEWORK} )
add_dependencies( TypeInfo idlib contrib )
target_precompile_headers( TypeInfo PUBLIC ${CMAKE_SOURCE_DIR}/idlib/precompiled.h )
target_link_libraries( TypeInfo idlib contrib ${SDL_LIBRARIES} ${ZLIB_LIBRARY})
