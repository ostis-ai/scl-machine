# Quick Start

!!! note
    The scl-machine can't be used on Windows.

## Use scl-machine as an extension to sc-machine

1. Download pre-built artifacts of sc-machine from [GitHub Releases](https://github.com/ostis-ai/sc-machine/releases) and extract it to any location.

    To learn more about how to install and use sc-machine artifacts, see [sc-machine quick start](https://ostis-ai.github.io/sc-machine/quick_start/).

2. Download pre-built artifacts of scl-machine from [GitHub Releases](https://github.com/ostis-ai/scl-machine/releases) and extract it to any location.

3. Then specify the folder path to `lib/extensions` from extracted scl-machine folder when running the sc-machine binary:
   
    ```sh
    ./path/to/extracted/sc-machine/binary -s path/to/kb.bin \
        -e "path/to/extracted/scl-machine/lib/extensions;path/to/extracted/sc-machine/lib/extensions"
    # if several paths to extensions are provided then they should be separated 
    # by semicolon and wrapped in double quotes
    ```

## Use scl-machine as a C++ library in your project

### Conan

You can use Conan to install scl-machine. To integrate scl-machine into your project using Conan, follow these steps:

1. Create a `conanfile.txt` in your project root with the following content:

    ```ini
    [requires]
    scl-machine/<version>
    ```

2. Install pipx first using guide: [https://pipx.pypa.io/stable/installation/](https://pipx.pypa.io/stable/installation/).

3. Install Conan if not already installed:

    ```sh
    pipx install conan
    pipx ensurepath
    ```

3. Update CMake to 3.24:

    ```sh
    pipx install cmake
    pipx ensurepath
    ```

4. Relaunch your shell after installation.

    ```sh
    exec $SHELL
    ```

5. Add the OSTIS-AI remote Conan repository:

    ```sh
    conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-sc-machine
    ```

6. Install scl-machine and its dependencies:

    ```sh
    conan install . --build=missing
    ```

7. Import scl-machine targets into your CMake project by using:

    ```cmake
    find_package(scl-machine REQUIRED)
    ```

8. Link `scl-machine::inference` target to your target:

    ```cmake
    target_link_libraries(my-module
        LINK_PUBLIC scl-machine::inference
    )
    ```

9.  Go to [problem solver documentation](main.pdf) to learn how to use scl-machine agents.

In case you want to make changes to the project sources, please refer to the [build system docs](build/build_system.md).
