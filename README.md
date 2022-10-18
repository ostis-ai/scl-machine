# SCL-machine
Inference module for [OSTIS project](https://github.com/ostis-ai/ostis-project)

# Installation
```sh
git clone https://github.com/ostis-ai/scl-machine.git
cd scl-machine/
git submodule update --init --recursive
```

## Build scl-machine
Before build scl-machine make sure that sc-machine is assembled. Sc-machine and scl-machine must be allocated in the same folder.

```sh
cd scl-machine/scripts
./make_all.sh #You can also pass all CMake generation arguments there
```

Or, alternatively (requires CMake 3.13+)
```sh
cd scl-machine
cmake -B build -DCMAKE_BUILD_TYPE=Release # or Debug type for development
cmake --build build -j$(nproc) # -j flag for paralleled build process
```

To include scl-machine knowledge base add `<path to >/scl-machine/kb` to repo.path file.

## Documentation

We document all information about the project development and its components' implementation in sources of its knowledge base
to provide opportunity to use it in information processing and knowledge generation.

You can access the current version of the documentation in [docs/main.pdf](docs/main.pdf) file of this project.

Documentation is written with the help of LaTeX tools in SCn-code representation.
To build documentation manually, you'll need a LaTeX distribution installed on your computer. 
Alternatively, we provide a Docker image to build the documentation in case you can't / don't want to install LaTeX on your PC.

### Download scn-tex-plugin and documentation for subprojects

- ### Build steps (using LaTeX)

    ```sh
    cd scl-machine/docs
    TEXINPUTS=./scn: latexmk -pdf -bibtex main.tex
    ```
- ### Build steps (using Docker)

  ```sh
  cd scl-machine
  docker run -v ${PWD}:/workdir --rm -it ostis/scn-latex-plugin:latest "docs/main.tex"
  ```

After the compilation, the `main.pdf` file should appear at `scl-machine/docs/`.

## Feedback

Contributions, bug reports and feature requests are welcome! Feel free to check our 
[issues page](https://github.com/ostis-ai/scl-machine/issues) and file a new issue (or comment in existing ones).

## License

Distributed under the Apache License. See [LICENSE](LICENSE) for more information.

##### _This repository continues the development of [ostis-apps/ostis-inference](https://github.com/ostis-apps/ostis-inference)_
