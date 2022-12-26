set(TARGET_NAME "Editor")

file(GLOB EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB EDITOR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(${TARGET_NAME} ${EDITOR_SOURCES} ${EDITOR_HEADERS})

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${ENGINE_ROOT_DIR}
)

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

target_link_libraries(${TARGET_NAME} glfw)
target_link_libraries(${TARGET_NAME} glad)
target_link_libraries(${TARGET_NAME} imgui)
target_link_libraries(${TARGET_NAME} stb)
target_link_libraries(${TARGET_NAME} Runtime)

file(GLOB ASSETS "./asset" "./shader" "./config/editor.ini")
file(COPY ${ASSETS} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${engine_folder})