set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

if(NOT TARGET glad)
    file(GLOB_RECURSE glad_header ${CMAKE_CURRENT_SOURCE_DIR}/glad/*.h)
    file(GLOB_RECURSE glad_source ${CMAKE_CURRENT_SOURCE_DIR}/glad/*.c)
    add_library(glad ${glad_header} ${glad_source})
    target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
    set_target_properties(glad PROPERTIES FOLDER ${third_party_folder}/glad)
endif()

if(NOT TARGET imgui)
    file(GLOB imgui_header 
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.h
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h)
    file(GLOB imgui_source 
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
    add_library(imgui ${imgui_header} ${imgui_source})
    target_link_libraries(imgui PUBLIC glfw)
    target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui LINKER_LANGUAGE CXX)
endif()

if(NOT TARGET stb)
    file(GLOB stb_sources CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h)
    add_library(stb INTERFACE ${stb_sources})
    target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
    set_target_properties(json11 PROPERTIES FOLDER ${third_party_folder}/json11)
endif()

if(NOT TARGET tbb)
    add_subdirectory(tbb)
    set_target_properties(tbb PROPERTIES FOLDER ${third_party_folder}/tbb)
    endif()
    
if(NOT TARGET OpenImageDenoise)
    find_package(Python3 REQUIRED)
    add_subdirectory(oidn)
    set_target_properties(OpenImageDenoise PROPERTIES FOLDER ${third_party_folder}/oidn)
endif()
