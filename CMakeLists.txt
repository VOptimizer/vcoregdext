cmake_minimum_required(VERSION 3.3.0)
project(vcoregdext VERSION 0.1.0 LANGUAGES CXX C)

if(Emscripten)
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
    set(CMAKE_STRIP FALSE)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_PTHREADS=1 -sSIDE_MODULE=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_PTHREADS=1 -sSIDE_MODULE=1")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".wasm")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PREBUILT_GODOT_PATH "Path to the prebuilt godot library" "")

if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/third-party/godot-cpp/CMakeLists.txt")
    message(
        FATAL_ERROR
        "[${PROJECT_NAME}] The godot-cpp submodule was not downloaded. Please update submodules: git submodule update --init --recursive."
    )
endif()

if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/third-party/vcore/CMakeLists.txt")
    message(
        FATAL_ERROR
        "[${PROJECT_NAME}] The vcore submodule was not downloaded. Please update submodules: git submodule update --init --recursive."
    )
endif()

# Either this script builds the complete gdextension library of godot, which are currently over ~900 files to compile
# or just use the prebuilt the user has.
if(NOT PREBUILT_GODOT_PATH)
    set(GODOT_CPP_SYSTEM_HEADERS ON CACHE BOOL "" FORCE)
    add_subdirectory(third-party/godot-cpp)
else()
    add_library(godot-cpp STATIC IMPORTED)
    set_target_properties(godot-cpp PROPERTIES
        IMPORTED_LOCATION ${PREBUILT_GODOT_PATH}
    )
endif()

# Make sure you include the GDNative directories before you compile libVCore.
# If you skip this step, VCore might end up using its default config instead of the Godot one.
include_directories("${PROJECT_SOURCE_DIR}/third-party/godot-cpp/gdextension"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/third-party/godot-cpp/gen/include"
                    "${PROJECT_SOURCE_DIR}/include"
)
add_subdirectory(third-party/vcore/lib)

add_dependencies(VCore godot-cpp)

set(SRCS "${PROJECT_SOURCE_DIR}/src/VModel.cpp"
         "${PROJECT_SOURCE_DIR}/src/VMesher.cpp"
         "${PROJECT_SOURCE_DIR}/src/VMaterial.cpp"
         "${PROJECT_SOURCE_DIR}/src/VLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/VExporter.cpp"
         "${PROJECT_SOURCE_DIR}/src/GodotFileStream.cpp")

include_directories("${PROJECT_SOURCE_DIR}/third-party/vcore/lib/include")
add_library(${PROJECT_NAME} SHARED main.cpp ${SRCS})

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:
            TYPED_METHOD_BIND
        >
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
            -static-libgcc
            -static-libstdc++
            -static
            -Wl,-R,'$$ORIGIN'
        >
    )
endif(WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE godot-cpp VCore)
add_dependencies(${PROJECT_NAME} VCore godot-cpp)