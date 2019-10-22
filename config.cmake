set(ModuleName "sample")

file(GLOB_RECURSE ModuleSourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/**.cpp")

add_library(${ModuleName} SHARED 
	${ModuleSourceFiles}
)

target_link_libraries(${ModuleName}
	v4d
)

target_include_directories(${ModuleName}
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)
