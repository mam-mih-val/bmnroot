# Remove trailing slash from a path passed as argument
Macro(Remove_Trailing_Slash _variable)
  String(FIND ${_variable} "/" _pos_last_slash REVERSE)
  STRING(LENGTH ${_variable} _length)
  Math(EXPR _last_pos ${_pos_last_slash}+1)
  If(${_last_pos} EQUAL ${_length})  
    String(SUBSTRING ${_variable} 0 ${_pos_last_slash} _ret_val)
  Else()
    Set(_ret_val ${_variable})
  EndIf()
EndMacro()

Macro(FairRootVersion)

  Execute_Process(COMMAND $ENV{FAIRROOTPATH}/bin/fairroot-config --version
                  OUTPUT_VARIABLE _fairroot_version
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                 )
  String(FIND ${_fairroot_version} v- old_version_scheme)
  If (old_version_scheme GREATER -1)
    String(REGEX MATCH "v-([0-9]+)\\.([0-9]+)([a-z]*)" _version_matches "${_fairroot_version}")
    Set(FairRoot_VERSION_MAJOR ${CMAKE_MATCH_1})
    Set(FairRoot_VERSION_MINOR ${CMAKE_MATCH_2})
    Set(FairRoot_VERSION_PATCH ${CMAKE_MATCH_3})

    If(FairRoot_VERSION_PATCH MATCHES "a")
      Set(FairRoot_VERSION_PATCH 1)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "b")
      Set(FairRoot_VERSION_PATCH 2)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "c")
      Set(FairRoot_VERSION_PATCH 3)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "d")
      Set(FairRoot_VERSION_PATCH 4)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "e")
      Set(FairRoot_VERSION_PATCH 4)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "f")
      Set(FairRoot_VERSION_PATCH 5)
    ElseIf(FairRoot_VERSION_PATCH MATCHES "g")
      Set(FairRoot_VERSION_PATCH 6)
    Else()
      Set(FairRoot_VERSION_PATCH 0)
    EndIf()
  Else()
    Message("${_fairroot_version}")
    String(REGEX MATCH "v([0-9]+)\\.([0-9]+)\\.([0-9]+)" _version_matches "${_fairroot_version}")
    Set(FairRoot_VERSION_MAJOR ${CMAKE_MATCH_1})
    Set(FairRoot_VERSION_MINOR ${CMAKE_MATCH_2})
    Set(FairRoot_VERSION_PATCH ${CMAKE_MATCH_3})
  EndIf()

  Set(FairRoot_VERSION
      ${FairRoot_VERSION_MAJOR}.${FairRoot_VERSION_MINOR}.${FairRoot_VERSION_PATCH}
     ) 
EndMacro()

Macro(FairSoftVersion)

  Execute_Process(COMMAND $ENV{SIMPATH}/bin/fairsoft-config --version
                  OUTPUT_VARIABLE _fairsoft_version
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                 )
  String(REGEX MATCH "([a-z][a-z][a-z])([0-9]+)" _version_matches "${_fairsoft_version}")

  Set(FairSoft_VERSION_MAJOR ${CMAKE_MATCH_2})
  Set(FairSoft_VERSION_MINOR ${CMAKE_MATCH_1})


  If(FairSoft_VERSION_MINOR MATCHES "jan")
    Set(FairSoft_VERSION_MINOR 1)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "feb")
    Set(FairSoft_VERSION_MINOR 2)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "mar")
    Set(FairSoft_VERSION_MINOR 3)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "apr")
    Set(FairSoft_VERSION_MINOR 4)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "may")
    Set(FairSoft_VERSION_MINOR 5)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "jun")
    Set(FairSoft_VERSION_MINOR 6)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "jul")
    Set(FairSoft_VERSION_MINOR 6)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "jul")
    Set(FairSoft_VERSION_MINOR 7)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "aug")
    Set(FairSoft_VERSION_MINOR 8)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "sep")
    Set(FairSoft_VERSION_MINOR 9)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "oct")
    Set(FairSoft_VERSION_MINOR 10)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "nov")
    Set(FairSoft_VERSION_MINOR 11)
  ElseIf(FairSoft_VERSION_MINOR MATCHES "dec")
    Set(FairSoft_VERSION_MINOR 12)
  EndIf()

  Set(FairSoft_VERSION
      ${FairSoft_VERSION_MAJOR}.${FairSoft_VERSION_MINOR}
     ) 
EndMacro()

Macro(Gen_Exe_Script _ExeName) 

  set(shell_script_name "${_ExeName}.sh")

  string(REPLACE ${PROJECT_SOURCE_DIR}
         ${PROJECT_BINARY_DIR} new_path ${CMAKE_CURRENT_SOURCE_DIR}
        )

  set(my_exe_name ${EXECUTABLE_OUTPUT_PATH}/${_ExeName})

  configure_file(${PROJECT_SOURCE_DIR}/cmake/scripts/run_binary.sh.in
                   ${new_path}/${shell_script_name}
                  )

  Execute_Process(COMMAND /bin/chmod u+x  ${new_path}/${shell_script_name}
                  OUTPUT_QUIET
                 )

EndMacro(Gen_Exe_Script)

function(download_project_if_needed)
  include(DownloadProject)
  set(oneValueArgs PROJECT GIT_REPOSITORY GIT_TAG GIT_STASH SOURCE_DIR TEST_FILE PATCH_COMMAND)
  cmake_parse_arguments(MY "" "${oneValueArgs}"
                       "" ${ARGN} )

  Set(ProjectUpdated FALSE PARENT_SCOPE)

  If(NOT EXISTS ${MY_SOURCE_DIR}/${MY_TEST_FILE})
    download_project(PROJ            ${MY_PROJECT}
                     GIT_REPOSITORY  ${MY_GIT_REPOSITORY}
                     GIT_TAG         ${MY_GIT_TAG}
                     SOURCE_DIR      ${MY_SOURCE_DIR}
                     PATCH_COMMAND   ${MY_PATCH_COMMAND}
                    )
  Else()
    Execute_process(COMMAND git rev-parse HEAD
                    WORKING_DIRECTORY ${MY_SOURCE_DIR}
                    OUTPUT_VARIABLE CURRENT_SPADIC_HASH
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                   )
    If(NOT ${MY_GIT_TAG} STREQUAL ${CURRENT_SPADIC_HASH})
      If(MY_GIT_STASH)
        Execute_Process(COMMAND git stash  WORKING_DIRECTORY ${MY_SOURCE_DIR}) 
        Execute_Process(COMMAND git stash clear  WORKING_DIRECTORY ${MY_SOURCE_DIR}) 
      EndIF()
      download_project(PROJ            ${MY_PROJECT}
                       GIT_REPOSITORY  ${MY_GIT_REPOSITORY}
                       GIT_TAG         ${MY_GIT_TAG}
                       SOURCE_DIR      ${MY_SOURCE_DIR}
                       PATCH_COMMAND   ${MY_PATCH_COMMAND}
                      )
      Set(ProjectUpdated TRUE PARENT_SCOPE)
    EndIf()
  EndIf()
EndFunction()


#----- Macro GENERATE_CBM_TEST_SCRIPT  --------------------------------------
#----- Macro for generating an executable test script from a ROOT macro.
#----- This macro extends GENERATE_ROOT_TEST_SCRIPT from FairRoot such that
#----  a second argument specifyies the destination directory. This allows
#----  to generate scripts from macros in a different directory.
#---   V.F. 18/12/10
MACRO (GENERATE_CBM_TEST_SCRIPT SCRIPT_FULL_NAME DEST_DIR)

  get_filename_component(path_name ${SCRIPT_FULL_NAME} PATH)
  get_filename_component(file_extension ${SCRIPT_FULL_NAME} EXT)
  get_filename_component(file_name ${SCRIPT_FULL_NAME} NAME_WE)
  set(shell_script_name "${file_name}.sh")

  set(new_path ${DEST_DIR})
  file(MAKE_DIRECTORY ${new_path}/data)

  CONVERT_LIST_TO_STRING(${LD_LIBRARY_PATH})
  set(MY_LD_LIBRARY_PATH ${output})

  CONVERT_LIST_TO_STRING(${ROOT_INCLUDE_PATH})
  set(MY_ROOT_INCLUDE_PATH ${output})

  set(my_script_name ${SCRIPT_FULL_NAME})

  configure_file(${FAIRROOTPATH}/share/fairbase/cmake/scripts/root_macro.sh.in
                 ${DEST_DIR}/${shell_script_name}
                )

  execute_process(COMMAND "/bin/chmod" "u+x" "${DEST_DIR}/${shell_script_name}")
  MESSAGE("Created ${DEST_DIR}/${shell_script_name}")

ENDMACRO (GENERATE_CBM_TEST_SCRIPT)
#----- Macro GENERATE_CBM_TEST_SCRIPT  --------------------------------------


