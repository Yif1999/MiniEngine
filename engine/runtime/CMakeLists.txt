set(TARGET_NAME "Runtime")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${ENGINE_ROOT_DIR}
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/tinyobjloader
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/glm
)

target_include_directories(
    ${TARGET_NAME} 
    PUBLIC ${THIRD_PARTY_DIR}/glad/include
)

target_link_libraries(${TARGET_NAME} glfw)
target_link_libraries(${TARGET_NAME} glad)
target_link_libraries(${TARGET_NAME} stb)
target_link_libraries(${TARGET_NAME} glad)
target_link_libraries(${TARGET_NAME} spdlog::spdlog)

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${engine_folder})

### precompile
# set global vari used by precompile
set(RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)