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
    #Message("${_fairroot_version}")
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

