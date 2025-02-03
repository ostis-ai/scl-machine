# Quick Start for Contributors

This guide provides short information for developers to start to work with scl-machine quickly. You can always learn more about the scl-machine's [build system](build_system.md).

## Check CMake

Install pipx first using [**pipx installation guide**](https://pipx.pypa.io/stable/installation/) if not already installed.

Ensure you are using **CMake version 3.24** or newer. Verify your version with:

```sh
cmake --version
```

To upgrade CMake, run:
  
```sh
# Use pipx to install cmake if not already installed
pipx install cmake
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

Install Ninja generator for CMake, to use sc-machine CMake presets:

```sh
# Use pipx to install ninja if not already installed
pipx install ninja
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

## Start develop scl-machine with Conan

### Install Conan

Install Conan, to build scl-machine dependencies with Conan-provided dependencies:

```sh
# Use pipx to install conan if not already installed
pipx install conan
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

### Use scl-machine in Debug

#### Install dependencies with Conan

scl-machine is an extension to sc-machine, so sc-machine is main dependency for scl-machine. To install it, run in the root of the project:

```sh
conan install . -s build_type=Debug --build=missing
```

#### Build scl-machine in Debug

To build scl-machine in debug mode using Conan-provided dependencies, run:

```sh
# debug build type
cmake --preset debug-conan
cmake --build --preset debug
```

Note: By default, configure preset `debug` enables building scl-machine tests.

#### Run scl-machine tests in Debug

After that, you can go to `build/Debug` and run tests via `ctest`:

```sh
cd build/Debug
ctest -V
```

You can also check code formatting. To learn more, go to the [CMake flags](cmake_flags.md) page.

### Use scl-machine in Release

#### Install dependencies with Conan

To install it, run in the root of the project:

```sh
conan install . -s build_type=Release --build=missing
```

#### Build scl-machine in Release

To build scl-machine in release mode using Conan-provided dependencies, run:

```sh
# release build type without tests
cmake --preset release-conan
cmake --build --preset release
```

To build scl-machine with tests in release mode using Conan-provided dependencies, run:

```sh
# release build type with tests
cmake --preset release-with-tests-conan
cmake --build --preset release
```

#### Run scl-machine tests in Release

After that, you can run tests:

```sh
cd build/Release
ctest -V
```

You can also check code formatting. To learn more, go to the [CMake flags](cmake_flags.md) page.

### Run scl-machine

Before launching scl-machine, [extract sc-machine from GitHub Releases](https://ostis-ai.github.io/sc-machine/quick_start/#github-releases) or [build it](https://ostis-ai.github.io/sc-machine/build/quick_start/).

To launch scl-machine, run:

```sh
./path/to/sc-machine/binary -s path/to/kb.bin \
    -e "path/to/scl-machine/lib/extensions;path/to/sc-machine/lib/extensions"
# if several paths to extensions are provided then they should be separated 
# by semicolon and wrapped in double quotes
```
