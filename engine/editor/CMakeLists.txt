set(TARGET_NAME "Editor")

file(GLOB EDITOR_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
include_directories(include)
add_executable(${TARGET_NAME} ${EDITOR_SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${engine_folder})

target_link_libraries(${TARGET_NAME} glfw)
target_link_libraries(${TARGET_NAME} glad)
target_link_libraries(${TARGET_NAME} imgui)
target_link_libraries(${TARGET_NAME} stb)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/glad/include
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/imgui/backends
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/glm
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/tinyobjloader
)
