cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(MiniEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

set(PROJ_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${PROJ_ROOT_DIR}/bin")
set(BIN_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

add_subdirectory(engine)