This is a basic plugin template for use with CommonLibSSE-NG

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE-NG](https://github.com/alandtse/CommonLibVR/tree/ng)
	* Add the environment variable `CommonLibSSEPath_NG` with the value as the path to the folder containing CommonLibSSE-NG

## Building
```
git clone https://github.com/ForestJ316/WaitYourTurnPatch
cd WaitYourTurnPatch
git submodule init
git submodule update

cmake --preset vs2022-windows
cmake --build build --config Release
```