cmake_minimum_required(VERSION 3.22)

project(Code-Juliette C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/arc)

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/Romeo)

file(GLOB_RECURSE PROJECT_SOURCE
    PRIVATE ${CMAKE_SOURCE_DIR}/src/*.c
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Romeo/include
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_C_COMPILER MATCHES "clang-cl")
        target_compile_options(${PROJECT_NAME} PRIVATE
            /Zi /Od /W4 /permissive- /GS /WX /wd4324
    )
    elseif(CMAKE_C_COMPILER MATCHES "clang")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -g -O0 -Wall -Werror -Wextra -Wshadow -Wpedantic -Wconversion
            -Wnull-dereference -Wunused-result -Wno-strict-prototypes
            -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-value
            -fstack-protector-strong
    )
    elseif(CMAKE_C_COMPILER MATCHES "cl")
        target_compile_options(${PROJECT_NAME} PRIVATE
            /Zi /Od /W4 /permissive- /GS /WX /wd4324
    )
    elseif(CMAKE_C_COMPILER MATCHES "gcc")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -g -O0 -Wall -Werror -Wextra -Wshadow -Wpedantic -Wconversion
            -Wnull-dereference -Wunused-result -Wno-strict-prototypes
            -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-command-line-argument
            -Wno-unused-value -fstack-protector-strong
    )
    else()
        message(WARNING "Unknown/unsupported compiler. No specific compiler flags set.")
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    if(CMAKE_C_COMPILER MATCHES "clang-cl")
        target_compile_options(${PROJECT_NAME} PRIVATE
            /O2 /DNDEBUG
        )
    elseif(CMAKE_C_COMPILER MATCHES "clang")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -O3 -DNDEBUG
        )
    elseif(CMAKE_C_COMPILER MATCHES "cl")
        target_compile_options(${PROJECT_NAME} PRIVATE
            /O2 /DNDEBUG
        )
    elseif(CMAKE_C_COMPILER MATCHES "gcc")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -O3 -DNDEBUG
        )
    else()
        message(WARNING "Unknown/unsupported compiler. No specific compiler flags set.")
    endif()
else()
    message(WARNING "Unknown build type. No specific compiler flags set.")
endif()

if((ENABLE_ASAN OR ENABLE_UBSAN) AND NOT CMAKE_C_COMPILER MATCHES "clang")
    message(FATAL_ERROR "Sanitizers are only compatible with LLVM")
endif()

if(ENABLE_ASAN)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
    message(STATUS "Address Sanitizer enabled")
endif()

if(ENABLE_UBSAN)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
    message(STATUS "Undefined Behavior Sanitizer enabled")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Code-Romeo
)

set(PROJECT_RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(PROJECT_BUILD_RESOURCE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")
if (EXISTS ${PROJECT_RESOURCE_DIR})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_RESOURCE_DIR}"
        "${PROJECT_BUILD_RESOURCE_DIR}"
        COMMENT "Copying resources to build directory"
    )
    message(STATUS "Project resource directory copied to: ${PROJECT_BUILD_RESOURCE_DIR}")
else()
    message(FATAL_ERROR "Project resource directory not found: ${PROJECT_RESOURCE_DIR}")
endif()