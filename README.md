# ostis-inference
Inference module for [OSTIS project](https://github.com/ostis-ai/ostis-project)

# This repository is deprecated! Use https://github.com/ostis-ai/scl-machine

# Installation
```sh
- git clone https://github.com/ostis-apps/ostis-inference.git
- cd ostis-inference/
- git submodule update --init --recursive
- cd ostis-inference/scripts
- ./install_ostis.sh
```

## Using as subsystem

You can use this project as subsystem of any ostis-system using gitmodules like that
```
[submodule "subsystems/ostis-inference"]
path = subsystems/ostis-inference
url = https://github.com/ostis-apps/ostis-inference
```

Remember to add subdirectory in CMakeLists.txt of root project
```cmake
set(SUBSYSTEMS_PATH "${CMAKE_CURRENT_LIST_DIR}/subsystems")
subdir_list(SUBDIRS ${SUBSYSTEMS_PATH})

foreach(SUBDIR ${SUBDIRS})
  add_subdirectory(${SUBSYSTEMS_PATH}/${SUBDIR}/problem-solver)
endforeach()
```

To include ostis-inference knowledge base add `subsystems/ostis-inference/kb` to repo.path file.

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
    cd ostis-inference/docs
    latexmk -pdf -bibtex main.tex
    ```
- ### Build steps (using Docker)

  ```sh
  cd ostis-inference
  docker run -v ${PWD}:/workdir --rm -it ostis/scn-latex-plugin:latest "docs/main.tex"
  ```

After the compilation, the `main.pdf` file should appear at `ostis-inference/docs/`.

## Feedback

Contributions, bug reports and feature requests are welcome! Feel free to check our 
[issues page](https://github.com/ostis-apps/ostis-inference/issues) and file a new issue (or comment in existing ones).

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for more information.
