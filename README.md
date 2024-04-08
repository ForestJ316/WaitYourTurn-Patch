## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG)
	* Add the environment variable `CommonLibSSEPath_NG` with the value as the path to the folder containing CommonLibSSE-NG

## Building
```
git clone https://github.com/ForestJ316/WaitYourTurn-Patch
cd WaitYourTurn-Patch

cmake --preset vs2022-windows
cmake --build build --config Release
```

## Optional
* [Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `SkyrimPath` to point to the folder where you want the .dll to be copied after it's finished building
