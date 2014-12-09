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
add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/union.C
     COMMAND ${CMAKE_COMMAND}
     -E copy ${CMAKE_CURRENT_SOURCE_DIR}/union.C
)

add_executable (mpd-scheduler src/mpd-scheduler.cpp)
target_link_libraries(mpd-scheduler ${ROOT_LIBRARIES} xml2)
