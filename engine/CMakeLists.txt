set(engine_folder "Engine")
set(ENGINE_ROOT_DIR "${PROJ_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/thirdparty")

add_subdirectory(editor)
add_subdirectory(runtime)
add_subdirectory(thirdparty)
add_subdirectory(parser)

set(CODEGEN_TARGET "PreCompile")
include(parser/precompile/precompile.cmake)

set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine" )
add_dependencies(Runtime "${CODEGEN_TARGET}")
add_dependencies("${CODEGEN_TARGET}" "Parser")
