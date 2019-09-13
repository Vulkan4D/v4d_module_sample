/**
 * V4D System Sample
 * 
 * This is a demonstration code for implementation of a System for V4D
 * Systems are compiled into individual libraries (DLL) which will be loaded at runtime into the main application.
 * All systems MUST be compiled in 64bit and MUST have the following MACROS defined in compiler flags : 
 * 		_V4D_SYSTEM
 * 		_RELEASE or _DEBUG
 * 		_WINDOWS or _LINUX
 * 
 * All systems must define the required metadata and include v4d.h
 * 
 * The keyword V4DSYSTEM is a macro to put in front of every function that should be available to the main application. 
 * 
 * To load a system into the main application, first instantiate a SystemsLoader or use a global one : 
		v4d::SystemsLoader systemsLoader;
 * 	Then load the system and check if it has been loaded successfully :
		auto sampleSystem = systemsLoader.Load("sample");
		if (!sampleSystem) LOG_ERROR("ERROR : Could not load sample system library")
 * 
 * Systems can include predefined optional functions and custom functions
 * 
 * To call a predefined function, simply call it using the instance pointer : 
 * 		sampleSystem->OnLoad();
 * 
 * To call a custom function from a system, first get the function pointer using the Macro and check if it is valid : 
		LOAD_DLL_FUNC(sampleSystem, int, test1, int)
		if (!test1) LOG_ERROR("Error getting symbol pointer. " << LOAD_DLL_ERR)
 * 	The first parameter is the instance of the system (the return value of SystemsLoader.Load())
 * 	The second parameter is the return type of the function we want to load
 * 	The 3rd parameter is the name of the function we are trying to load, and also the resulting function pointer name
 * 	All the remaining parameters are the argument types of that function
 * 	Error message can be fetched using the macro LOAD_DLL_ERR
 *  Then, we can simply call the function like so : 
 * 		int x = test1(5);
 * 
 * Predefined accessible system members are :
	v4d::SystemsLoader* systemsLoader;
	v4d::CoreInstance* v4dCore;
 */

// Metadata
#define THIS_SYSTEM_NAME "Sample"
#define THIS_SYSTEM_REVISION 1
#define THIS_SYSTEM_DESCRIPTION "Sample V4D System"

#include <v4d.h>

////////////////////////////////////////////////////////////////////
// Predefined optional functions

V4DSYSTEM void OnLoad() {
	// LOG("Sample system Loaded")
}

V4DSYSTEM void OnDestroy() {
	// LOG("Sample system Destroyed")
}

////////////////////////////////////////////////////////////////////
// Example of a custom system function that can be accessed via the main application or other systems
V4DSYSTEM int test1(int b) {
	if (v4dCore->GetProjectName() == "Test Project") {
		return b * 2;
	} else {
		return 0;
	}
}

