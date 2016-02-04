set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
/usr/include/libxml2
)

include_directories(${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

#message("Copying file: union.C to bin directory")
set(DST_FILE_TARGET ${EXECUTABLE_OUTPUT_PATH}/union.C)
add_custom_command(
	OUTPUT ${DST_FILE_TARGET}
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/union.C ${DST_FILE_TARGET}
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/union.C
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/union.C
)
add_custom_target(copy-union ALL DEPENDS ${DST_FILE_TARGET})

#message("Copying file: mpd.qsub to bin directory")
set(DST_FILE_TARGET ${EXECUTABLE_OUTPUT_PATH}/mpd.qsub)
add_custom_command(
	OUTPUT ${DST_FILE_TARGET}
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/qsub/mpd.qsub ${DST_FILE_TARGET}
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/qsub/mpd.qsub
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/qsub/mpd.qsub
)
add_custom_target(copy-mpd-qsub ALL DEPENDS ${DST_FILE_TARGET})

#message("Copying file: union.qsub to bin directory")
set(DST_FILE_TARGET ${EXECUTABLE_OUTPUT_PATH}/union.qsub)
add_custom_command(
	OUTPUT ${DST_FILE_TARGET}
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/qsub/union.qsub ${DST_FILE_TARGET}
	MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/qsub/union.qsub
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/qsub/union.qsub
)
add_custom_target(copy-union-qsub ALL DEPENDS ${DST_FILE_TARGET})

add_executable(mpd-scheduler src/mpd-scheduler.cpp)
target_link_libraries(mpd-scheduler ${ROOT_LIBRARIES} xml2)
