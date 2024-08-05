# GameSort

![image](https://github.com/user-attachments/assets/6c3a939c-92b5-4df4-bf2a-ba60965ced3d)

## To build this project, install the following C++ libraries:

[simdjson](https://github.com/simdjson/simdjson)

[SFML](https://github.com/SFML/SFML)

Alternatively, install these libraries with vcpkg. This can be done by running

> vcpkg install sfml  \
> vcpkg install simdjson

Next, clone into the repository and build with cmake as appropriate according to CMakeLists.txt. Note that this project requires a C++ compiler that supports C++20.
The cmake command should look something like:

> cmake -S . -B build  \
> cmake --build build

appending -G in the first command with your appropriate generator as necessary.

After building is complete, cd back up to the project's root, cd into the bin/ directory and run GameSort.

Alternatively, you can clone through Visual Studio Code or CLion and it should automagically do the cmake build process for you.
