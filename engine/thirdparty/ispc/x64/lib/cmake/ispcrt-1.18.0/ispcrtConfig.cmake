## Copyright 2020-2022 Intel Corporation
## SPDX-License-Identifier: BSD-3-Clause


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ispcrtConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include("${CMAKE_CURRENT_LIST_DIR}/ispcrt_Exports.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ispc.cmake")

check_required_components("ispcrt")

## Setup ISPC includes ##

set(ISPCRT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../include/ispcrt)
set(ISPCRT_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories_ispc(${ISPCRT_INCLUDE_DIR})

## Set the variable regarding pre-built tasking support
if (ON)
  set(ISPCRT_TASKING_ENABLED TRUE)
  set(ISPCRT_TASKING_MODEL Threads)
else()
  set(ISPCRT_TASKING_ENABLED FALSE)
endif()

## Find level_zero if required ##

if(ON)
  include(CMakeFindDependencyMacro)

  set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

  find_dependency(level_zero)

  set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH})
  unset(OLD_CMAKE_MODULE_PATH)
endif()

## Print info about found ISPCRT version
include(FindPackageMessage)
find_package_MESSAGE(ispcrt "Found ispcrt: ${ISPCRT_DIR}" "[${ISPCRT_DIR}]")

## Standard signal that the package was found ##
set(ISPCRT_FOUND TRUE)
