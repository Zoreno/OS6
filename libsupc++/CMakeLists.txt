#==============================================================================
# Cmake file for compiling the OS6 operating system c++ support library module
#==============================================================================

project(libsupc++)

add_library(${LIBSUPCXX} "")

#==============================================================================
# Flags and compiler configuration
#==============================================================================

set_target_properties(${LIBSUPCXX} PROPERTIES COMPILE_FLAGS ${CPPFLAGS})
set_target_properties(${LIBSUPCXX} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${LIBSUPCXX} PROPERTIES SKIP_BUILD_RPATH 1)

set(CMAKE_CXX_LINK_EXECUTABLE "${OS6_COMPILER_PATH}/ld <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> \
    -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} ${ASMFLAGS} -o <OBJECT> <SOURCE>")

#==============================================================================
# Path macros
#==============================================================================

#==============================================================================
# Includes and dependencies
#==============================================================================

target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBSUPCXX_INCLUDE})
target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBUNWIND_INCLUDE})
target_include_directories(${LIBSUPCXX} SYSTEM PRIVATE ${LIBC_INCLUDE})

target_link_libraries(${LIBSUPCXX} PRIVATE ${LIBC})
target_link_libraries(${LIBSUPCXX} PRIVATE ${LIBUNWIND})

#==============================================================================
# Sources
#==============================================================================

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/bad_alloc.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/bad_array_new_length.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/cxxabi.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/exception.cpp)


target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_op.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opa.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opant.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opnt.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opv.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opva.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opvant.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_opvnt.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_op.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opa.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opant.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opnt.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_ops.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opsa.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opv.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opva.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opvant.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opvnt.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opvs.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/delete_opvsa.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/new_handler.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/pure_virtual.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/deleted_virtual.cpp)

target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/fundamental_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/array_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/function_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/enum_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/pbase_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/pointer_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/pointer_to_member_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/class_type_info.cpp)
target_sources(${LIBSUPCXX} PRIVATE ${LIBSUPCXX_SRC}/si_class_type_info.cpp)


#==============================================================================
# Tests
#==============================================================================