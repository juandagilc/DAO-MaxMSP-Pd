# Xcode project to write Max/MSP and Pd externals

## Steps to create a new project

### Create the project
1. Create a new project:
	- OS X > Framework & Library > Bundle
	- Name: Max
	- Bundle extension: mxo
1. Add a new target:
	- OS X > Framework & Library > Library
	- Product name: Pd
	- Framework: None (Plain C/C++ Library)
	- Type: Dynamic
1. Rename the project to `template~`:
	- Uncheck the targets from the project content items
	- Close the project
	- Reorganize the files in Finder following the next structure:
```
	    DAO-MaxMSP-Pd
	    ├── _SDK_
	    │   ├── jit-includes (copied from max sdk)
	    │   ├── max-includes (copied from max sdk)
	    │   ├── msp-includes (copied from max sdk)
	    │   ├── pd-includes (contents from pd src)
	    │   ├── ConfigMax.xcconfig
	    │   └── ConfigMax.xcconfig
	    └── template~
	        ├── Products
	        │   ├── template.maxpat
	        │   └── template.pd
	        ├── Source
	        │   ├── template~common.c
	        │   ├── template~common.h
	        │   ├── template~max.c
	        │   └── template~pd.c
	        └── Xcode
	            ├── Info.plist
	            └── template~.xcodeproj
```

### Setup the project
1. Open the project
1. Open the `Info.plist` file and change:
	- Bundle OS Type code: iLaX
	- Bundle Creator OS Type code: max2
1. Add a group named Config:
	- Add the `ConfigMax.xcconfig` and `ConfigPd.xcconfig` files
1. Select the project info:
	- Set the configuration targets for debug and release
1. Select the Max target and delete the next build settings:
	- Installation directory
	- Skip install
	- Info.plist file
1. Select the Pd target and delete the next build settings:
	- Executable prefix
	- Product name
1. Edit the schemes and add the executables for debugging:
	- For Max: `/Applications/Max.app`
	- For Pd: `/Applications/Pd.app/Contents/Resources/bin/pd`
1. Add a scheme named All:
	- Add both targets to its Build phase
	- Manage schemes and check the Shared box in all schemes

### Add source code
1. Add a group named Source
	- Add new C files and select the corresponding target for each file:
		- `template~max.c`
		- `template~pd.c`
		- `template~common.c`
		- `template~common.h`

### Code, build, and enjoy!

## Steps to copy an existing project

1. Copy an existing project and paste it into a folder with the new name
1. Open the Xcode project
1. Rename the project (and the targets)
1. Rename all the source files:
	- Separator `~` for msp and `_` for max objects
1. Search for all occurrences of the old name and replace them with the new name:
    - Search for `oldname` replace with `newname`
    - If the new external is a max-only object:
        - Search for `oldname~` replace with `newname`
        - In the Pd initialization routine name: `void name_tilde_setup(void)`, remove `_tilde`
    - Adjust indentation in the initialization of the class
1. Close the Xcode project and delete the `DerivedData` folder
1. Open the Xcode project and build and test the externals

********************************************************************************

# Visual Studio project to write Max/MSP and Pd externals

## Steps to create a new project

### Create the project
1. Create a Win32 Console application:
	- Name it ``template~.vcxproj``
	- DLL
	- Empty project
1. Close the project
1. Reorganize the file in Windows Explorer following the next structure:
```
	    DAO-MaxMSP-Pd
	    ├── _SDK_
	    │   ├── jit-includes (copied from max sdk)
	    │   ├── max-includes (copied from max sdk)
	    │   ├── msp-includes (copied from max sdk)
	    │   ├── pd-includes (contents from pd src and bin/pd.lib)
	    │   ├── ConfigMax.props
	    │   └── ConfigPd.props
	    └── template~
	        ├── Products
	        │   ├── template.maxpat
	        │   └── template.pd
	        ├── Source
	        │   ├── template~common.c
	        │   ├── template~common.h
	        │   ├── template~max.c
	        │   └── template~pd.c
	        └── VisualStudio
	            ├── template~.vcxproj
	            └── template~.vcxproj.user
```

### Setup the project
1. Open the project
1. Open the Build Configuration Manager
1. Rename the Project Configurations to ``MaxDebug`` and ``MaxRelease``
1. Create new configurations called ``PdDebug`` and ``PdRelease`` based on the two existing ones
1. Open the Property Manager
1. Add the existing property sheets to the configurations of Max and Pd
1. Open the Project Properties:
	- Select All Configurations and All Platforms
	- Debugging > Command > $(APPLICATION)

### Add source code
1. Add new C files
	- `template~common.c`
	- `template~common.h`
	- `template~max.c`
	-	`template~pd.c`
1. Exclude Max and Pd source files from build:
	- Right-click on the file ``template~max.c`` and select *Properties*
		- Select All Platforms
		- For Configurations ``PdDebug`` and ``PdRelease``, select *Exclude From Build = Yes*
	- Right-click on the file ``template~pd.c`` and select *Properties*
		- Select All Platforms
		- For Configurations ``MaxDebug`` and ``MaxRelease``, select *Exclude From Build = Yes*

### Add custom Clean target
1. Right-click on the project and select *Unload Project*
	- Right-click on the project and select *Edit target~.vcxproj*
	- Paste the next text before the tag ``</Project>``:
	```
	<Target Name="AfterClean">
		<Delete Files="$(PRODUCTS)\$(ProjectName).$(EXTENSION)" ContinueOnError="true" />
	</Target>
	```
	- Close the text editor of the project
	- Right-click on the project and select *Reload Project*

### Code, build, and enjoy!

## Steps to copy an existing project

1. Copy an existing project and paste it into a folder with the new name
1. Rename the Visual Studio project and the source code files:
	- Separator `~` for msp and `_` for max objects
1. Open the project and the source code files with a text editor (not with Visual Studio)
1. Search for all occurrences of the old name and replace them with the new name:
    - Search for `oldname` replace with `newname`
    - If the new external is a max-only object:
        - Search for `oldname~` replace with `newname_`
        - In the Pd initialization routine name: `void name_tilde_setup(void)`, remove `_tilde`
    - Adjust indentation in the initialization of the class
1. Open the Visual Studio project and build and test the externals

********************************************************************************

# Makefiles to build Pd externals (for Mac, Windows, and Linux)

The makefile in the folder of each external detects the name of the external to build, so no action is needed to create a new project when copying an existing project as a template. The local makefile in each external's folder includes a global makefile that is located in the \_SDK_ folder and is shared among all externals in this repository. The global makefile is the one that defines the build targets and they are selected automatically according to the operating system where it is run. Below is the folder structure that is assumed:

```
	    DAO-MaxMSP-Pd
	    ├── _SDK_
	    │   ├── pd-includes (contents from pd src)
	    │   └── MakefilePd.mk (the global makefile)
	    └── template~
	        ├── Products
	        │   ├── template.maxpat
	        │   └── template.pd
	        ├── Source
	        │   ├── template~common.c
	        │   ├── template~common.h
	        │   ├── template~max.c
	        │   └── template~pd.c
	        └── Makefile
	            └── Makefile.mk (the local makefile)
```

********************************************************************************
