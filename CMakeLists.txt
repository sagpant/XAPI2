cmake_minimum_required(VERSION 3.0.2)
project(XAPI2)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# add XAPI_CPP
file(GLOB_RECURSE XAPI_CPP_h ${CMAKE_CURRENT_LIST_DIR}/languages/CPP/XAPI_CPP/*.h)
file(GLOB_RECURSE XAPI_CPP_cpp ${CMAKE_CURRENT_LIST_DIR}/languages/CPP/XAPI_CPP/*.cpp)
if (WIN32)
	add_library(XAPI_CPP SHARED
		${XAPI_CPP_h}
		${XAPI_CPP_cpp}
		${CMAKE_CURRENT_LIST_DIR}/include/XApiC.h
		${CMAKE_CURRENT_LIST_DIR}/include/XApiCpp.h
		${CMAKE_CURRENT_LIST_DIR}/include/CrossPlatform.h
	)
	set_target_properties(XAPI_CPP
		PROPERTIES
		FOLDER "languages/CPP"
	)
else()
	add_library(XAPI_CPP SHARED
		${XAPI_CPP_cpp}
	)
endif()
target_compile_definitions(XAPI_CPP
	PRIVATE BUILDING_DLL
	PUBLIC XAPI_CPP_EXPORTS
)

# get common files
set(common_h
	${CMAKE_CURRENT_LIST_DIR}/include/toolkit.h
	${CMAKE_CURRENT_LIST_DIR}/include/inirw.h
)
set(common_cpp
	${CMAKE_CURRENT_LIST_DIR}/include/toolkit.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/inirw.cpp
)
set(common_quote_h
	${CMAKE_CURRENT_LIST_DIR}/include/ApiProcess.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager1.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager2.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSyntheticCalculate_DepthMarketDataNField.h
)
set(common_quote_cpp
	${CMAKE_CURRENT_LIST_DIR}/include/ApiProcess.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager1.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager2.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSyntheticCalculate_DepthMarketDataNField.cpp
)
file(GLOB synthetic_h ${CMAKE_CURRENT_LIST_DIR}/include/synthetic/*.h)
file(GLOB synthetic_cpp ${CMAKE_CURRENT_LIST_DIR}/include/synthetic/*.cpp)

# functions
function(move_into_filter filter files)
	foreach(FILE ${files}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	
		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
	
		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")
	
		source_group("${filter}" FILES "${FILE}")
	endforeach()
endfunction(move_into_filter)

function(add_quote name folders)
	set(src_files "")
	set(h_files "")
	foreach(folder ${folders})
		file(GLOB tmp_h ${folder}/*.h)
		file(GLOB tmp_cpp ${folder}/*.cpp)
		list(APPEND h_files ${tmp_h})
		list(APPEND src_files ${tmp_cpp})
	endforeach(folder)
	
	if (WIN32)
		add_library(${name} SHARED
			${h_files}
			${src_files}
			${common_h}
			${common_cpp}
			${common_quote_h}
			${common_quote_cpp}
			${synthetic_h}
			${synthetic_cpp}
		)
		move_into_filter(common "${common_h}")
		move_into_filter(common "${common_cpp}")
		move_into_filter(common_quote "${common_quote_h}")
		move_into_filter(common_quote "${common_quote_cpp}")
		move_into_filter(synthetic "${synthetic_h}")
		move_into_filter(synthetic "${synthetic_cpp}")
	else()
		add_library(${name} SHARED
			${src_files}
			${common_cpp}
			${common_quote_cpp}
			${synthetic_cpp}
		)
	endif()
	
	target_include_directories(${name} PRIVATE
		${folders}
	)
	target_compile_definitions(${name}
		PRIVATE USE_CMAKE BUILDING_DLL
	)
	set_target_properties(${name}
		PROPERTIES
		FOLDER "apis"
	)
endfunction(add_quote)

function(add_trade name folders)
	set(src_files "")
	set(h_files "")
	foreach(folder ${folders})
		file(GLOB tmp_h ${folder}/*.h)
		file(GLOB tmp_cpp ${folder}/*.cpp)
		list(APPEND h_files ${tmp_h})
		list(APPEND src_files ${tmp_cpp})
	endforeach(folder)
	
	if (WIN32)
		add_library(${name} SHARED
			${h_files}
			${src_files}
			${common_h}
			${common_cpp}
			${synthetic_h}
			${synthetic_cpp}
		)
		move_into_filter(common "${common_h}")
		move_into_filter(common "${common_cpp}")
		move_into_filter(synthetic "${synthetic_h}")
		move_into_filter(synthetic "${synthetic_cpp}")
	else()
		add_library(${name} SHARED
			${src_files}
			${common_cpp}
			${synthetic_cpp}
		)
	endif()
	
	target_include_directories(${name} PRIVATE
		${folders}
	)
	target_compile_definitions(${name}
		PRIVATE USE_CMAKE BUILDING_DLL
	)
	set_target_properties(${name}
		PROPERTIES
		FOLDER "apis"
	)
endfunction(add_trade)

# add CTP Quote
set(folder "")
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Quote)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP_Quote)
add_quote(CTP_Quote "${folder}")

# add CTP Trade
set(folder "")
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP_Trade)
add_trade(CTP_Trade "${folder}")