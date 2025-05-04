##################################
##	Beato idTech4 Game Engine	##
##	contrib: 					##
##################################

##
## Minizip for pk4 file load
##
SET( MINIZIP_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/minizip/ioapi.c
	${CMAKE_CURRENT_SOURCE_DIR}/minizip/unzip.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/minizip/unzip.h
	${CMAKE_CURRENT_SOURCE_DIR}/minizip/ioapi.h
	) 

source_group("MiniZip" FILES ${MINIZIP_SOURCES} )

set( VECTOR_CLASS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/instrset_detect.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/instrset.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorclass.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vector_convert.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorf128.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorf256e.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorf256.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorf512e.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorf512.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorfp16e.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectorfp16.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori128.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori256e.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori256.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori512e.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori512.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori512se.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectori512s.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectormath_common.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectormath_exp.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectormath_hyp.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectormath_lib.h
	${CMAKE_CURRENT_SOURCE_DIR}/VectoClassLib/vectormath_trig.h
	)

	source_group("VectorClass" FILES ${VECTOR_CLASS_SOURCES} )

###
### GLui for tool gui
###
SET( GLUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_checkbox_0.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_checkbox_0_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_checkbox_1.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_downarrow.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_leftarrow.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_listbox_down.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_listbox_up.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_listbox_up_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_radiobutton_0.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_radiobutton_0_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_radiobutton_1.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_radiobutton_1_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_rightarrow.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spindown_0.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spindown_1.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spindown_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spinup_0.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spinup_1.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_spinup_dis.c
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_img_uparrow.c
	
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/algebra3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/arcball.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_add_controls.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_bitmap_img_data.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_bitmaps.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_button.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_checkbox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_column.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_commandline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_control.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_edittext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_filebrowser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_list.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_listbox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_mouse_iaction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_panel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_radio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_rollout.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_rotation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_scrollbar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_separator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_spinner.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_statictext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_string.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_textbox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_translation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_tree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_treepanel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_window.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/quaternion.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/viewmodel.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/arcball.h
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_internal.h
	${CMAKE_CURRENT_SOURCE_DIR}/glui/src/glui_internal_control.h
	
	${CMAKE_CURRENT_SOURCE_DIR}/src/glui/include/GL/glui.h
	) 

source_group("GLui" FILES ${GLUI_SOURCES} )

##
## The Library 
##
SET( LIBS_SOURCES_ALL 
	${MINIZIP_SOURCES}
	#${GLUI_SOURCES}
	#${VECTOR_CLASS_SOURCES}
	)

add_library( contrib STATIC ${LIBS_SOURCES_ALL} )
#target_precompile_headers( idlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/precompiled.h)