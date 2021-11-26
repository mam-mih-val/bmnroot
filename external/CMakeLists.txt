# Build all libraries which come from external sources like other subversion or git repositories

# Exclude dowload of external packages. The external packages are not needed
# to run the code format checker and slows down the execution.
# Without the externals it isn't possible to build CbmRoot so don't switch
# it off execept for the checker.
Option(DOWNLOAD_EXTERNALS "Download the code from the external repositories." ON)
include(ExternalProject)

if(DOWNLOAD_EXTERNALS)
  Include(InstallAnalysisTree.cmake)
  Include(InstallAnalysisTreeQA.cmake)
else()
  # Define targets which are needed by CbmRoot but are not available
  # whithout the external packages
  add_library(ANALYSISTREE SHARED IMPORTED GLOBAL)
  add_library(ANALYSISTREEQA SHARED IMPORTED GLOBAL)
endif()
