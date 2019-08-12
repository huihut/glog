[![Build Status](https://img.shields.io/travis/google/glog/master.svg?label=Travis)](https://travis-ci.org/google/glog/builds)
[![Grunt status](https://img.shields.io/appveyor/ci/google-admin/glog/master.svg?label=Appveyor)](https://ci.appveyor.com/project/google-admin/glog/history)

This repository contains a C++ implementation of the Google logging
module.  Documentation for the implementation is in doc/.

See INSTALL for (generic) installation instructions for C++: basically
```sh
./autogen.sh && ./configure && make && make install
```

## Build

1. Build VS project with CMake
    * Check `BUILD_SHARED_LIBS`
    * Uncheck `WITH_GFLASS`
    * `CMAKE_INSTALL_PREFIX` fill in the install path
    * Configure
    * Generate
2. Compile and install using VS
    * Debug: Generate INSTALL project
    * Release: Generate INSTALL project


<details><summary>Some dependent libraries that glog projects may need</summary>

```
kernel32.lib
user32.lib
gdi32.lib
winspool.lib
shell32.lib
ole32.lib
oleaut32.lib
uuid.lib
comdlg32.lib
advapi32.lib
```

</details>

## Usage

1. To add glog's DLL header to your include path
2. To add glog's DLL import library to your project
3. To add `GLOG_NO_ABBREVIATED_SEVERITIES` to Preprocessor Definitions
4. Additional Dependencies add
    * Debug：`glogd.lib`
    * Release：`glog.lib`
5. To copy the DLL in a post-build event
    * Debug：`xcopy /y /d "..\path_to_install\bin\glogd.dll" "$(OutDir)"`
    * Release：`xcopy /y /d "..\path_to_install\bin\glog.dll" "$(OutDir)"`