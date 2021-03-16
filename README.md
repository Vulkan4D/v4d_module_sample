## Vulkan4D sample module

This is a sample/template package for an implementation of a Module for V4D

## What is a Module

Vulkan4D aims at being completely Modular, where Modding is not a hack but actually the official way to make a game. 

Modules may add functionality or resources to an application and are compiled into individual libraries (DLL) which will be loaded at runtime into the application.

A Module may affect multiple aspects of the application by defining multiple SubModules, or may simply be a pack of resources that would be used by other Modules. 

## SubModules

SubModules are the actual libraries (DLL) that are loaded at runtime.
A Module may (and should typically) have multiple SubModules, for the purpose of adding functionality to multiple aspects of the engine (rendering, audio, inputs,...). 

A Module may also not contain any SubModule, which means that it will not have any DLL file loaded at runtime, but may for instance only contain resource files to be used by other Modules. 

A Module can have no more than one SubModule for each unique [**ModuleClass**](#ModuleClass).

## Building/Compiling your module

If you are using the official CMAKE configuration that comes with the *v4d_project* template, you may skip this section and go directly to module configuration [`module.conf`](#module.conf). Modules will be compiled automatically when compiling everything. 

If you are using your own build system or you want a minimal development environment that only compiles your SubModules, these are the requirements for all [integrated SubModules](#integrated) : 
* Must be compiled in 64-bit
* Must have the following MACROS defined : 
	* `_RELEASE` or `_DEBUG`
	* `_WINDOWS` or `_LINUX`
	* `THIS_MODULE` should be set to the module name in the format **VENDOR_modulename**
	* `_V4D_MODULE` may be defined at the top of your source file
* `#include <v4d.h>`
* Dynamically link with v4d.dll
* Add the directory containing `v4d.h` to your include path (from `https://github.com/Vulkan4D/v4d_core`)
* Add the directory containing `xvk.hpp` to your include path (from `https://github.com/Vulkan4D/xvk`)
* The compiled SubModule library files (.dll/.so) must be compiled (output) as `modules/VENDOR_modulename/VENDOR_modulename.ModuleClass.dll` relative to the application directory. 
	(see [**Vendor/Module Naming**](#VendorModuleNaming) and [**ModuleClass**](#ModuleClass) below for naming conventions)

## <a name="module.conf"></a> module.conf

The module definition file `module.conf` is where the module information is defined and all the SubModules are listed. 
This file is used within the CMAKE default build environment of the *v4d_project* template, hence it is not required if you are building your modules using your own build scripts. 

To configure this file, the minimum information required are the **Vendor** and the **Module** names. 
These names are strickly regulated for official modules (see [**Vendor/Module Naming**](#VendorModuleNaming) below).
```
ModuleVendor( V4D )
ModuleName( sample )
```

To add a SubModule that will be compiled as an individual library (DLL) and loaded at runtime, you must specify the ModuleClass that it implements and at least one source file(s). 
```
SubModule(V4D_Input "input.cpp")
```
You may add as many `SubModule()` lines as the number of SubModules that you have. 
In the same module, you cannot define two SubModules with the same ModuleClass. 
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

This section is about creating a custom ModuleClass, not an actual SubModule. See [SubModule Source Files](#submoduleSourceFiles) below if you only want to write a simple mod. 

A ModuleClass defines strict function definitions that will be present in a given SubModule. 

Any official ModuleClass must have the following naming convention : `VENDOR_Moduleclass`

The **VENDOR** part must be a valid official vendor name, and **Moduleclass** have the following requirements :
* Must be no more than 12 characters
* Must be all lowercase except the first character which is UPPERCASE
* May contain only letters and numbers (a-z and 0-9)
* Must start with an UPPERCASE letter (A-Z)

As a module developer, you are free to also [define your own ModuleClass](#CreateYourOwnModuleClass) as long as it uses your official VENDOR name. 

You may also use a ModuleClass from another vendor, to add functionality to an existing 3rd party or V4D module. 

## <a name="CreateYourOwnModuleClass"></a> Creating your own ModuleClass

Defining your own ModuleClass is useful when you want your own modules to support modding by other developers.

This means that other developers would be able to use your module as a dependency, and you will call your own defined functions from their modules. 

Defining your ModuleClass must be done within a header file in which you will define all the functions that developers may have to implement in their SubModule source file. 

It's your responsibility to document your ModuleClass and define which function is mandatory or optional to implement.

Defining a ModuleClass and its functions can be done quickly using the following MACROS (defined when including v4d.h) within a new class :
* `V4D_MODULE_CLASS_HEADER` (YourClassName, AllFunctionsToLoad...)
* `V4D_MODULE_FUNC_DECLARE` (returnType, functionName, args...)
* `V4D_MODULE_CLASS_CPP` (YourClassName)

Here is an example for writing a fictive rendering ModuleClass if you are a vendor named X3D : 
```c++
// X3D_Renderer.h
#pragma once
#include <v4d.h>
namespace x3d { // any namespace you want
	class X3D_Renderer {// class name must start with your assigned Vendor
		V4D_MODULE_CLASS_HEADER( X3D_Renderer // must be the exact same as class name
			// below is a list of all your function names, the order does not matter but they must all be there. 
			, Init
			, IsActive
			, Update
			, Draw
		)
		// your own functions must all be declared here before they can be implemented by SubModules
		V4D_MODULE_FUNC_DECLARE(void, Init, v4d::graphics::Renderer*)
		V4D_MODULE_FUNC_DECLARE(bool, IsActive)
		V4D_MODULE_FUNC_DECLARE(void, Update, double deltaTime)
		V4D_MODULE_FUNC_DECLARE(void, Draw, double deltaTime, Image* target)
		// Note: Use typedefs if you need template types that have commas (,)
	};
}

// X3D_Renderer.cpp
#include "X3D_Renderer.h"
namespace x3d {
	V4D_MODULE_CLASS_CPP( X3D_Renderer );
	// nothing else is required here
}
```

Using the example above, loading and calling SubModule functions from anywhere else may be done like this :
```c++
// This precisely tries to load the library 'modules/OTHERVENDOR_somemodule/OTHERVENDOR_somemodule.X3D_Renderer.dll'
// the LoadModule function here may be called many times, it will reuse if already loaded
X3D_Renderer::LoadModule("OTHERVENDOR_somemodule");

// This effectively calls the function named Init in all loaded SubModules that implement the 'X3D_Renderer' ModuleClass
X3D_Renderer::ForEachModule([](auto* mod){
	if (mod->Init) // it is recommended to check if the function exists in the SubModule before running it, otherwise mark it as mandatory in your ModuleClass documentation
		mod->Init(&renderer);
});
```

## <a name="submoduleSourceFiles"></a> SubModule Source files

Each SubModule should have at least one source file that implements some of the declared functions for that ModuleClass. 

All source files used for an individual SubModule will be compiled into the same library (DLL).

You may NOT define multiple SubModules in the same library (DLL).

SubModules should implement functions that are well defined in it's ModuleClass. 

From the ModuleClass' header file, each V4D_MODULE_FUNC_DECLARE may be implemented as raw functions, and their return type and argument types must EXACTLY match the ones declared in the class. 

There are two ways of writing a SubModule. [`Standalone`](#standalone) or [`Integrated`](#integrated). 

### <a name="standalone"></a> Standalone

A standalone SubModule has less requirements but cannot use the v4d core library. 

The only requirement is that the defined functions are to be accessible via dlopen/LoadLibrary with C linkage and that their declarations are correct. 

PROS : 
* Use the language of your choice (C or any version of C++)
* Start coding your module faster with the tools you are used to work with

CONS : 
* Less safe to use because your editor cannot validate your implementation of the class, it's up to you to make sure the function return type and arguments are correct, otherwise it's undefined behaviour at runtime. 

The following examples are defining the function declared as `V4D_MODULE_FUNC_DECLARE(void, Update, double deltaTime)` in the fictive `X3D_Renderer` ModuleClass definition

From the V4D_MODULE_FUNC_DECLARE macro, the first argument is the return type, the second is the function name, and the remaining arguments are the function arguments.

In the C language, you may simply define the functions with the correct return type and arguments. 
```c
void Update(double deltaTime) {
	//...
}
```

In C++, you must declare your functions with an `extern "C"` linkage. 
```c++
extern "C" void Update(double deltaTime) {
	//...
}
```

### <a name="integrated"></a> Integrated

An integrated SubModule can interact with V4D's core functions and objects.
You will link your SubModule with v4d.dll, but v4d.dll does NOT link with your SubModule, it will still load it at runtime exactly like the standalone version, in fact there is no difference for the application between integrated or standalone, once the SubModule is compiled.

PROS: 
* Can use any core functions and objects from v4d.h
* Safer to use, you may simply copy the definitions from the ModuleClass header and rename the macro, and you know it uses the correct types
* If you define the `_V4D_IN_EDITOR` macro in your editor (**NOT for the compiler**), your editor will be able to correct mistakes in your definition (function name, return type, arguments)

CONS: 
* More complex setup (You will need the `v4d_core` source code, and the same compiler requirements as to compile the core itself)

A typical structure for an integrated SubModule would look like this : 

```c++
#define _V4D_MODULE
#include <v4d.h>

//... your own variables and functions for internal use

using namespace x3d; // optional namespace if the ModuleClass is using one
V4D_MODULE_CLASS(X3D_Renderer) { // this macro actually compiles as 'extern "C"', but for the editor's point of view it extends a fictive class for validating with the ModuleClass definitions
	
	// using the same example as above, you simply take the same function declaration as in the ModuleClass and rename the macro (removing the _DECLARE), then implement a body
	V4D_MODULE_FUNC(void, Update, double deltaTime) {
		// your implementation for this function
	}
	
	// other functions...
	
};
```

## Reserved names

Functions starting or ending with the word `Module` or `Modules` are reserved and may not be declared in a ModuleClass, but may be implemented in a SubModule. 

Functions starting with the word `Module` may be written in SubModules for special predefined functionality. 

Currently, the following reserved Module functions are predefined and may be used in SubModules source : 
```c++
// called when loading the SubModule for the first time, useful to load dependencies and init variables
V4D_MODULE_FUNC(void, ModuleLoad) {}

// called when unloading the SubModule, either at the end of execution or when reloading a SubModule
V4D_MODULE_FUNC(void, ModuleUnload) {}

// typically implemented with a switch case over an ENUM, called between SubModules to share data
V4D_MODULE_FUNC(void, ModuleSetCustomPtr, int what, void* ptr) {}
V4D_MODULE_FUNC(void*, ModuleGetCustomPtr, int what) { return nullptr; }

// called for each module when running ModuleClass::ForPrimaryModule(func)
// lambda func will only be called on the first loaded module that returns true
V4D_MODULE_FUNC(bool, ModuleIsPrimary) { return true; }
```

## Recap on the terms used (Definitions)

* `Vendor` is a name that you need to have as a developer, you may reserve your officially assigned name
* `ModuleClass` is a type of class that defines all the functions that a `SubModule` can implement
* `Module` is a Mod, or package containing any number of `SubModules` and resources that modders may create
* `SubModule` is a compiled library (DLL) that is part of a `Module`, and that implements functions defined in a specific `ModuleClass`
* `Integrated` is a type of `SubModule` that links with v4d.dll and can use core objects and functions
* `Standalone` is a type of `SubModule` at it's simplest form with less requirements, unable to use v4d's core objects and functions
