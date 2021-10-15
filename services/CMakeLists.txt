if(EXISTS "${CMAKE_SOURCE_DIR}/services/nica_scheduler/CMakeLists.txt")
 add_subdirectory (nica_scheduler)
endif()

INSTALL(DIRECTORY ./ DESTINATION ${CMAKE_INSTALL_PREFIX}/services
	PATTERN "./nica_scheduler" EXCLUDE
	PATTERN "./CMakeLists.txt" EXCLUDE
)

INSTALL(FILES ./json.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
