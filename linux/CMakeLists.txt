# 需要最低版本的
cmake_minimum_required(VERSION 3.5)
message("--begin complie---")
# 设置项目的名称，并将其存储在变量 PROJECT_NAME 中
project(pag-linux)
set(CMAKE_CXX_STANDARD 11)

if (APPLE AND NOT IOS)
    SET(CMAKE_SYSTEM_PROCESSOR x86_64)
    SET(CMAKE_OSX_ARCHITECTURES x86_64)
endif ()

# 将 -D 定义标志添加到源文件的编译中，代码用 #ifdef ... #endif 使用
add_definitions(-DSWIFTSHADER)
add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DEGL_EGLEXT_PROTOTYPES)

# 使用指定的源文件将可执行文件添加到项目中
# 添加一个名为 <name> 的可执行目标，并且在项目中必须是全局唯一的。该目标是从命令调用中列出的源文件构建的
# 构建的可执行文件的实际文件名是根据本机平台的约定构建的（例如 <name>.exe 或只是 <name>
add_executable(pag-linux src/main.cpp)

# 将指定目录添加到编译器的头文件搜索路径之下，指定的目录被解释成当前源码路径的相对路径，dir1 dir2 ...
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/pag/include src)

if (APPLE)
    set(CMAKE_CXX_FLAGS "-stdlib=libc++")

    # 该命令用于搜索指定动态文件路径，里面的内容为自定义的变量名、动态文件名、具体路径
    find_library(APPLICATION_SERVICES_FRAMEWORK ApplicationServices REQUIRED)

    # 对列表的操作分为读取、查找、修改、排序等4个大类
    # APPEND：子命令APPEND用于将元素追加到列表
    list(APPEND my_libs ${APPLICATION_SERVICES_FRAMEWORK})
    find_library(QUARTZ_CORE QuartzCore REQUIRED)
    list(APPEND my_libs ${QUARTZ_CORE})
    find_library(COCOA Cocoa REQUIRED)
    list(APPEND my_libs ${COCOA})
    find_library(FOUNDATION Foundation REQUIRED)
    list(APPEND my_libs ${FOUNDATION})
    find_library(VIDEOTOOLBOX VideoToolbox)
    list(APPEND my_libs ${VIDEOTOOLBOX})

    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/pag/mac/x64/libpag.a)
    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/mac/x64/libEGL.dylib)
    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/mac/x64/libGLESv2.dylib)

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cppFlags} -pthread")
    add_compile_options(-fPIC)
    add_definitions(-D__STDC_FORMAT_MACROS)
    SET(CMAKE_EXE_LINKER_FLAGS " -no-pie")

    # 使用find_package引入外部依赖包
    find_package(Threads)
    list(APPEND my_libs ${CMAKE_THREAD_LIBS_INIT})
    find_package(ZLIB)
    include_directories(${ZLIB_INCLUDE_DIRS})
    list(APPEND my_libs ${ZLIB_LIBRARIES})
    list(APPEND my_libs dl)

    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/pag/linux/x64/libpag.a)
    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/linux/x64/libEGL.so)
    list(APPEND my_libs ${CMAKE_CURRENT_SOURCE_DIR}/vendor/swiftshader/linux/x64/libGLESv2.so)

endif()

# 动态库的添加
# link_directories(${PROJECT_SOURCE_DIR}/lib) # 添加动态连接库的路径
# target_link_libraries(project_name -lmxnet ) # 添加libmxnet.so

# 静态库的添加
# set_property(TARGET mxnet PROPERTY IMPORTED_LOCATION /path/to/libmxnet.a)
# target_link_libraries(project_name mxnet) # 添加libmxnet.a

target_link_libraries(${PROJECT_NAME} ${my_libs})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
endif()

