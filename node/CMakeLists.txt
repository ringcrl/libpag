cmake_minimum_required(VERSION 3.5)
message("--begin complie---")
project(node-pag)
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DSWIFTSHADER)
add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DEGL_EGLEXT_PROTOTYPES)

# add_executable(pag-linux src/main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../linux/vendor/pag/include
        src)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cppFlags} -pthread")
add_compile_options(-fPIC)
add_definitions(-D__STDC_FORMAT_MACROS)
SET(CMAKE_EXE_LINKER_FLAGS " -no-pie")

find_package(Threads)
list(APPEND libs ${CMAKE_THREAD_LIBS_INIT})
find_package(ZLIB)
include_directories(${ZLIB_INCLUDE_DIRS})
list(APPEND libs ${ZLIB_LIBRARIES})
list(APPEND libs dl)

list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/../linux/vendor/pag/linux/x64/libpag.a)
list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/../linux/vendor/swiftshader/linux/x64/libEGL.so)
list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/../linux/vendor/swiftshader/linux/x64/libGLESv2.so)

include_directories(${CMAKE_JS_INC})
file(GLOB SOURCE_FILES "node-pag.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
endif()

# Define NAPI_VERSION
add_definitions(-DNAPI_VERSION=3)
