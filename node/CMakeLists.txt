cmake_minimum_required(VERSION 3.5)
message("--begin complie---")
project(node-pag)
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DSWIFTSHADER)
add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DEGL_EGLEXT_PROTOTYPES)

# 从`node-pag.cpp` 中的文件建立一个以项目命名的共享库
file(GLOB SOURCE_FILES "node-pag.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
# 给予我们的库文件一个.node的扩展名，没有任何 "lib "的前缀
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
# 构建节点插件的基本包含文件
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
# 构建一个node插件所必需的包含文件
# 你应该在每个基于CMake.js的项目中添加这一行
include_directories(${CMAKE_JS_INC})

# 使用 node-addon-api 而不是 node-api，类似于 C++ 之于 C
# Include N-API wrappers
# $ node -p "require('node-addon-api').include"
# "/home/will/projects/financialcpp/financialcpp/node_modules/node-addon-api"
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
# strip `"` and `\n` from the output above
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/pag/include)

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

list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/pag/linux/x64/libpag.so)
list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/linux/x64/libEGL.so)
list(APPEND libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/linux/x64/libGLESv2.so)

target_link_libraries(${PROJECT_NAME} ${libs})

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
# endif()

# Define NAPI_VERSION
add_definitions(-DNAPI_VERSION=3)
