##################################
##	Beato idTech4 Game Engine	##
##	Framework: Engine Core		##
##################################

##
## Framework
## Engine Core Structures
SET( FRAMEWORK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/CmdSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Compressor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Console.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CVarSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclAF.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclEntityDef.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclFX.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclParticle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclPDA.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclSkin.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeclTable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DemoFile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/EditField.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/EventLoop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/File.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/FileSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/KeyInput.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Session_menu.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/Unzip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/UsercmdGen.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/BuildDefines.h
	${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.h
	${CMAKE_CURRENT_SOURCE_DIR}/CmdSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/Common.h
	${CMAKE_CURRENT_SOURCE_DIR}/Compressor.h
	${CMAKE_CURRENT_SOURCE_DIR}/Console.h
	${CMAKE_CURRENT_SOURCE_DIR}/CVarSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclAF.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclEntityDef.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclFX.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclParticle.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclPDA.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclSkin.h
	${CMAKE_CURRENT_SOURCE_DIR}/DeclTable.h
	${CMAKE_CURRENT_SOURCE_DIR}/DemoChecksum.h
	${CMAKE_CURRENT_SOURCE_DIR}/DemoFile.h
	${CMAKE_CURRENT_SOURCE_DIR}/EditField.h
	${CMAKE_CURRENT_SOURCE_DIR}/EventLoop.h
	${CMAKE_CURRENT_SOURCE_DIR}/File.h
	${CMAKE_CURRENT_SOURCE_DIR}/FileSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/KeyInput.h
	${CMAKE_CURRENT_SOURCE_DIR}/Licensee.h
	${CMAKE_CURRENT_SOURCE_DIR}/Session.h
	${CMAKE_CURRENT_SOURCE_DIR}/Session_local.h
#	${CMAKE_CURRENT_SOURCE_DIR}/Unzip.h
	${CMAKE_CURRENT_SOURCE_DIR}/UsercmdGen.h
	) 

source_group("" FILES ${FRAMEWORK_SOURCES} )

##
## Async
## Server Client utils 
SET( FRAMEWORK_ASYNC_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncClient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncNetwork.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/async/MsgChannel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/async/NetworkSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/async/ServerScan.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncClient.h
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncNetwork.h
	${CMAKE_CURRENT_SOURCE_DIR}/async/AsyncServer.h
	${CMAKE_CURRENT_SOURCE_DIR}/async/MsgChannel.h
	${CMAKE_CURRENT_SOURCE_DIR}/async/NetworkSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/async/ServerScan.h
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
	${CMAKE_SOURCE_DIR}/ui/ChoiceWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/DeviceContext.cpp
	${CMAKE_SOURCE_DIR}/ui/EditWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/FieldWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameBearShootWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameBustOutWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GameSSDWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/GuiScript.cpp
	${CMAKE_SOURCE_DIR}/ui/ListGUI.cpp
	${CMAKE_SOURCE_DIR}/ui/ListWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/MarkerWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/RegExp.cpp
	${CMAKE_SOURCE_DIR}/ui/RenderWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/SimpleWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/SliderWindow.cpp
	${CMAKE_SOURCE_DIR}/ui/UserInterface.cpp
	${CMAKE_SOURCE_DIR}/ui/Window.cpp
	${CMAKE_SOURCE_DIR}/ui/Winvar.cpp
	
	${CMAKE_SOURCE_DIR}/ui/BindWindow.h
	${CMAKE_SOURCE_DIR}/ui/ChoiceWindow.h
	${CMAKE_SOURCE_DIR}/ui/DeviceContext.h
	${CMAKE_SOURCE_DIR}/ui/EditWindow.h
	${CMAKE_SOURCE_DIR}/ui/FieldWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameBearShootWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameBustOutWindow.h
	${CMAKE_SOURCE_DIR}/ui/GameSSDWindow.h
	${CMAKE_SOURCE_DIR}/ui/GuiScript.h
	${CMAKE_SOURCE_DIR}/ui/ListGUI.h
	${CMAKE_SOURCE_DIR}/ui/ListGUILocal.h
	${CMAKE_SOURCE_DIR}/ui/ListWindow.h
	${CMAKE_SOURCE_DIR}/ui/MarkerWindow.h
	${CMAKE_SOURCE_DIR}/ui/Rectangle.h
	${CMAKE_SOURCE_DIR}/ui/RegExp.h
	${CMAKE_SOURCE_DIR}/ui/RenderWindow.h
	${CMAKE_SOURCE_DIR}/ui/SimpleWindow.h
	${CMAKE_SOURCE_DIR}/ui/SliderWindow.h
	${CMAKE_SOURCE_DIR}/ui/UserInterface.h
	${CMAKE_SOURCE_DIR}/ui/UserInterfaceLocal.h
	${CMAKE_SOURCE_DIR}/ui/Window.h
	${CMAKE_SOURCE_DIR}/ui/Winvar.h
	) 

source_group("Ui" FILES ${FRAMEWORK_UI_SOURCES} )

##
## The Library 
##
SET( FRAMEWORK_SOURCES_ALL 
	${FRAMEWORK_SOURCES}
	${FRAMEWORK_ASYNC_SOURCES}
	${FRAMEWORK_COLISION_SOURCES}
	${FRAMEWORK_UI_SOURCES}
	)

add_library( framework STATIC ${FRAMEWORK_SOURCES_ALL} )
set_target_properties( framework PROPERTIES COMPILE_DEFINITIONS "__DOOM_DLL__")
add_dependencies( framework idlib )
target_precompile_headers( framework 
		PUBLIC 
			${CMAKE_SOURCE_DIR}/idlib/precompiled.h 
		PRIVATE 
			"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/BuildDefines.h>" 
			"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Licensee.h>" 
			"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/CVarSystem.h>" 
		)