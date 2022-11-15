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
