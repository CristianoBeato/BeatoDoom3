##################################
##	Beato idTech4 Game Engine	##
##	Sys Engine Main Loop system	##
##################################

SET( ENGINE_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/sys_local.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/sys_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/sys_public.h
	)

source_group("" FILES ${ENGINE_SOURCES} )

# 
# Linux platform souce code
# TO DO:
#SET( ENGINE_LINUX_SOURCES 
#	
#	)

#source_group("" FILES ${ENGINE_SOURCES} )

# 
# Windows platform souce code
#
SET( ENGINE_WINDOWS_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/win32/gl_logfuncs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_cpu.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_gamma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_glimp.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_net.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_qgl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_shared.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_snd.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_syscon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_taskkeyhook.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_wndproc.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/win32/win_local.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/win32/rc/doom.rc
	)

source_group("Win32" FILES ${ENGINE_WINDOWS_SOURCES} )


#
# 
#
SET( ENGINE_SOURCES_ALL 
	${ENGINE_SOURCES}
	)
	
if(WIN32)
	list(APPEND ENGINE_SOURCES_ALL 	${ENGINE_WINDOWS_SOURCES} )
endif()
	
add_executable( beatoDOOM3 WIN32 ${ENGINE_SOURCES_ALL} )
add_dependencies( beatoDOOM3 idlib framework sound renderer tools contrib )
target_precompile_headers( beatoDOOM3
	PUBLIC
		${CMAKE_SOURCE_DIR}/idlib/precompiled.h
	PRIVATE
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/sys_local.h>"
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/sys_public.h>"
		)
		
target_link_libraries( beatoDOOM3 
	idlib
	framework
	sound
	renderer
	tools
	contrib
	${OPENGL_LIBRARIES} 
	${SDL_LIBRARIES} 
	${ZLIB_LIBRARY} 
	${OPENAL_LIBRARY} 
	${JPEG_LIBRARY} 
	${OGG_LIBRARY} 
	${VORBIS_LIBRARY} 
	)
