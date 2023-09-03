# 设置 boost 查找条件
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_LIBS ON)
set(Boost_USE_RELEASE_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# 设置 boost 目录
if(CMAKE_CXX_PLATFORM_ID MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "D:\\ThirdPart\\boost_1_80_0\\stage\\lib\\cmake")
elseif(CMAKE_CXX_PLATFORM_ID MATCHES "MinGW")
elseif(CMAKE_CXX_PLATFORM_ID MATCHES "Linux")
endif()

# 添加 boost 链接库
macro(AddBoostLib ProjectName LibList)
    foreach(boost_library ${LibList})
        find_package(Boost REQUIRED COMPONENTS ${boost_library})
        target_link_libraries(${ProjectName} ${Boost_LIBRARIES})
    endforeach()

    include_directories(${Boost_INCLUDE_DIRS})
endmacro()
