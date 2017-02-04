cmake_minimum_required (VERSION 3.2)

project (Tēncatl
  VERSION 0.1
  LANGUAGES CXX)

# Specify strict C++14 standard
set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# Build type default set to Release
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
    CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
endif ()

# Use architecture-specific optimization in Release mode
if (CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo)$")
  if (CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
    add_compile_options(-march=native)
  endif ()
endif ()

# Options to build static and/or shared libraries (both by default)
option(BUILD_SHARED_LIBS "build shared libraries" ON)
option(BUILD_STATIC_LIBS "build static libraries" ON)
if (NOT ${BUILD_STATIC_LIBS} AND NOT ${BUILD_SHARED_LIBS})
  warning(FATAL_ERROR "both shared and static libraries are disabled")
endif ()

# Convenience macro to build the libraries
macro(add_tencatl_lib NAME SRC)
  # Prefix all library names
  set(LIBNAME tencatl-${NAME})
  # Build the libraries
  if (${BUILD_SHARED_LIBS})
    add_library(${NAME} SHARED ${SRC} ${ARGN})
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${LIBNAME})
    install(TARGETS ${NAME} DESTINATION lib)
  endif ()
  if (${BUILD_STATIC_LIBS})
    add_library(${NAME}_static STATIC ${SRC} ${ARGN})
    set_target_properties(${NAME}_static PROPERTIES OUTPUT_NAME ${LIBNAME})
    install(TARGETS ${NAME} DESTINATION lib)
  endif ()
endmacro()

# Build the libraries
add_tencatl_lib(lexer src/lexer.cc)

# Install the headers
install(DIRECTORY include/tencatl
  DESTINATION include
  FILES_MATCHING PATTERN "*.h")