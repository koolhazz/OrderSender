cmake_minimum_required(VERSION 2.8)

project(order_keeper CXX C)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall -std=c++98")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

file(GLOB_RECURSE CXX_LIST "src/*.cc")
file(GLOB_RECURSE C_LIST "src/*.c")
file(GLOB_RECURSE HEADER_LIST "inc/*.h")
file(GLOB_RECURSE SRC_LIST "*.cc|*.h")

message(STATUS "[include files]") 
FOREACH(c ${HEADER_LIST})
MESSAGE(STATUS ${c} )
ENDFOREACH()

message(STATUS "[src files]") 
FOREACH(cxx ${CXX_LIST})
MESSAGE(STATUS ${cxx} )
ENDFOREACH()
FOREACH(c ${C_LIST})
MESSAGE(STATUS ${c})
ENDFOREACH()

set(SRC ${CXX_LIST} ${C_LIST} ${HEADER_LIST})

include_directories(. 
					inc 
					/home/AustinChen/tools/jsoncpp-0.6.0/include 
					/home/AustinChen/lib/include
					/usr/local/include/
					/usr/local/mysql/include
)
link_directories(/home/AustinChen/tools/jsoncpp-0.6.0/libs/linux-gcc-4.1.2 
				 /home/AustinChen/lib
				 /usr/local/lib/
				 /usr/local/mysql/lib
)

add_executable(order_keeper ${SRC})
add_definitions(
	-D__DEBUG__)

target_link_libraries(order_keeper libhiredis.a libjson.a mysqlclient)
	

