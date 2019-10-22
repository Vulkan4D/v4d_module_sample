# V4D Sample Module
set(ModuleName "sample")

file(GLOB_RECURSE moduleSourceFiles **.cpp)
add_library(${ModuleName} SHARED 
	${moduleSourceFiles}
)
target_link_libraries(${ModuleName}
	v4d
)
target_include_directories(${ModuleName}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
set_target_properties(${ModuleName}
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/debug/modules/${ModuleName}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/debug/modules/${ModuleName}"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/release/modules/${ModuleName}"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/release/modules/${ModuleName}"
)
