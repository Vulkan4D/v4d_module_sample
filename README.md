## Vulkan4D sample module

This is a sample/template package for an implementation of a Module for V4D

## What is a Module

Vulkan4D aims at being completely Modular, where Modding is not a hack but actually the official way to make a game. 

Modules may add functionality or resources to an application and are compiled into individual libraries (DLL) which will be loaded at runtime into the application.

A Module may affect multiple aspects of the application by defining multiple SubModules, or may simply be a pack of resources that would be used by other Modules. 

## SubModules

SubModules are the actual libraries (DLL) that are loaded at runtime.
A Module may (and should typically) have multiple SubModules, for the purpose of adding functionality to multiple aspects of the engine. 

A Module can also have no SubModule, which means that it will not have any DLL file loaded at runtime, but may for instance only contain resource files to be used by other Modules. 

A Module can have no more than one SubModule for each unique [**ModuleClass**](#ModuleClass).

## Building/Compiling your module

If you are using the official CMAKE configuration that comes with the *v4d_project* template, you may skip this section and go directly to module configuration [`module.conf`](#module.conf). Modules will be compiled automatically when compiling everything. 

If you are using your own build system or you want a minimal development environment that only compiles your modules, these are the requirements for all V4D modules : 
* Must be compiled in 64-bit
* Must have the following MACROS defined : 
	* `_RELEASE` or `_DEBUG`
	* `_WINDOWS` or `_LINUX`
	* `THIS_MODULE` should be set to the module name in the format **VENDOR_modulename**
* If you want to use V4D objects within your module, you must : 
	* Dynamically link with v4d.dll
	* Define the `_V4D_MODULE` MACRO just before the `#include <v4d.h>` directive
	* Add the directory containing `v4d.h` to your include path (from `https://github.com/Vulkan4D/v4d_core`)
	* Add the directory containing `xvk.hpp` to your include path (from `https://github.com/Vulkan4D/xvk`)
* The compiled submodule library files (.dll/.so) must be compiled to `modules/VENDOR_modulename/VENDOR_modulename.ModuleClass.dll` relative to the application directory. 
	(see [**Vendor/Module Naming**](#VendorModuleNaming) and [**ModuleClass**](#ModuleClass) below for naming conventions)

## <a name="module.conf"></a> module.conf

The module definition file `module.conf` is where the module information is defined and all the submodules are listed. 
This file is used within the CMAKE default build environment of the *v4d_project* template, hence it is not required if you are building your modules using your own build scripts. 

To configure this file, the minimum information required are the **Vendor** and the **Module** names. 
These names are strickly regulated for official modules (see [**Vendor/Module Naming**](#VendorModuleNaming) below).
```
ModuleVendor( V4D )
ModuleName( sample )
```

To add SubModules that will be compiled as individual libraries (DLL) and imported at runtime, you must specify ONE source file and a ModuleClass. 
```
SubModule("input.cpp" V4D_Input)
```
You may add as many `SubModule()` lines as the number of SubModules that you have. 
You cannot define two SubModules with the same ModuleClass. 
See [**ModuleClass**](#ModuleClass) below

## <a name="VendorModuleNaming"></a> Vendor/Module Naming

An official vendor name must be assigned to you and a custom name can be requested if available. 

**Vendor names** have the following requirements : 
* Must be no more than 12 characters
* Must be ALL CAPS
* May contain only letters and numbers (A-Z and 0-9)
* Must start with a letter (A-Z)
* Official vendor names must not end with a zero (`0`)

Before getting your official vendor name, you may use one ending with a zero (`0`) it will be valid for testing but not in the official modules repository.

**Module names** must be unique on a per-vendor basis and have the following requirements : 
* Must be no more than 12 characters
* Must be all lowercase
* May contain only letters and numbers (a-z and 0-9)
* Must start with a letter (a-z)

It is recommended to use not more than 10 or 11 characters for your module name for future versioning purposes, or have it end with the version number (`0` for beta)

## <a name="ModuleClass"></a> ModuleClass

A ModuleClass defines strict function definitions that will be present in a given SubModule. 

Any official ModuleClass must have the following naming convention : `VENDOR_Moduleclass`

The **VENDOR** part must be a valid official vendor name, and **Moduleclass** have the following requirements :
* Must be no more than 12 characters
* Must be all lowercase except the first character which is UPPERCASE
* May contain only letters and numbers (a-z and 0-9)
* Must start with an UPPERCASE letter (A-Z)

As a module developer, you are free to [define your own ModuleClass](#CreateYourOwnModuleClass) as long as it uses your official VENDOR name. 

You may also use a ModuleClass from another vendor, to add functionality to an existing 3rd party or V4D module. 

## <a name="CreateYourOwnModuleClass"></a> Creating your own ModuleClass

Defining your own module class is useful when you want your own modules to support modding by other developers.

This means that other developers would be able to use your module as a dependency, and you will call your own defined functions in other developers modules. 

Defining your module class must be done within a header file in which you will define all the functions that developers will have to write in their SubModule. 

It's your responsibility to document your ModuleClass and define which function is mandatory or optional to implement.

Defining a module class and its functions can be done quickly using the following MACROS (defined when including v4d.h) within a new class :
* `V4D_MODULE_CLASS_H` (YourClassName, AllFunctionsToLoad...)
* `V4D_MODULE_FUNC` (returnType, functionName, args...)
* `V4D_MODULE_CLASS_CPP` (YourClassName)

Here is an example for a rendering module class using a vendor named X3D : 
```c++
// X3D_Renderer.h
#pragma once
#include <v4d.h>
namespace x3d { // any namespace you want
	class X3D_Renderer {// class name must start with your assigned Vendor
		V4D_MODULE_CLASS_H( X3D_Renderer // must be the exact same as class name
			// below is a list of all your functions, the order does not matter
			, Init
			, IsActive
			, Update
			, Draw
		)
		// your functions must all be defined here
		V4D_MODULE_FUNC(void, Init, v4d::graphics::Renderer*)
		V4D_MODULE_FUNC(bool, IsActive)
		V4D_MODULE_FUNC(void, Update, double deltaTime)
		V4D_MODULE_FUNC(void, Draw, double deltaTime, Image* target)
		// Note: Use typedefs if you need template types that have commas (,)
	};
}

// X3D_Renderer.cpp
#include "X3D_Renderer.h"
namespace x3d {
	V4D_MODULE_CLASS_CPP( X3D_Renderer );
}
```

Using the example above, loading and calling functions from SubModules may be done like this :
```c++
// This precisely tries to load the library 'modules/OTHERVENDOR_somemodule/OTHERVENDOR_somemodule.X3D_Renderer.dll'
// the Load function here may be called many times, it will not try to load it again if already loaded
X3D_Renderer::LoadModule("OTHERVENDOR_somemodule");

// This effectively calls the function named Init in all loaded submodules that extend X3D_Renderer
X3D_Renderer::ForEachModule([](auto* mod){
	if (mod->Init) // it is recommended to check if the function exists in the SubModule before running it
		mod->Init(&renderer);
});
```

Functions starting or ending with the word `Module` or `Modules` are reserved and may not be defined in your ModuleClass. 

Functions starting with the word `Module` may be written in SubModules for special predefined functionality. 

Currently, the following reserved Module functions are predefined and may be used in SubModules source : 
```c++
// called when loading the SubModule for the first time, useful to load dependencies and init variables
void ModuleLoad() {}

// called when unloading the SubModule, either at the end of execution or when reloading a SubModule
void ModuleUnload() {}

// typically implemented with a switch case over an ENUM, called between SubModules to share data
void ModuleSetCustomPtr(int what, void* ptr) {}
void* ModuleGetCustomPtr(int what) {}

// called for each module when running ModuleClass::ForPrimaryModule(func)
// lambda func will only be called on the first loaded module that returns true
bool ModuleIsPrimary() {}
```

## SubModule Source files

Each `.cpp` source file in your module's directory defines an individual SubModule that should be compiled in its own individual library (DLL).

All functions that are part of the ModuleClass definitions must be in an `extern "C"` linkage below your include directives and any variables or internal-use functions. 

A typical structure for a SubModule using V4D objects would look like this : 

```c++
#define _V4D_MODULE // this is needed only if you are including v4d.h below
#include <v4d.h> // this is optional, should be used only if you need references to V4D objects/helpers

//... your own variables and functions for internal use

extern "C" {
	
	//... all functions defined in ModuleClass
	
}
```


