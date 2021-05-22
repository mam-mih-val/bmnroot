Find_Package(Git)

If(GIT_FOUND AND EXISTS "${SOURCE_DIR}/.git")
  Execute_Process(COMMAND ${GIT_EXECUTABLE} describe --tags
                  OUTPUT_VARIABLE BMNROOT_GIT_VERSION
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  WORKING_DIRECTORY ${SOURCE_DIR}
                 )
  Execute_Process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd
                  OUTPUT_VARIABLE BMNROOT_GIT_DATE
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  WORKING_DIRECTORY ${SOURCE_DIR}
                 )
  Message(STATUS "BmnRoot Version - ${BMNROOT_GIT_VERSION} from - ${BMNROOT_GIT_DATE}")
  Configure_File(${SOURCE_DIR}/cmake/scripts/BmnRootVersion.h.tmp ${BINARY_DIR}/BmnRootVersion.h @ONLY)
EndIf()
