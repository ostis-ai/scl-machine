# scl-machine

[![CI](https://github.com/ostis-ai/scl-machine/actions/workflows/test_conan.yml/badge.svg)](https://github.com/ostis-ai/scl-machine/actions/workflows/test_conan.yml)
[![license](https://img.shields.io/badge/License-Apache-yellow.svg)](LICENSE)

[![Quick Start](https://img.shields.io/badge/-Quick%20Start-black?style=for-the-badge&logo=rocket)](https://ostis-ai.github.io/scl-machine/quick_start)
[![Docs](https://img.shields.io/badge/Docs-gray?style=for-the-badge&logo=read-the-docs)](https://ostis-ai.github.io/scl-machine)
[![Community](https://img.shields.io/badge/-Community-teal?style=for-the-badge&logo=matrix)](https://app.element.io/index.html#/room/#ostis_tech_support:matrix.org)

scl-machine is an inference module designed for the [OSTIS Technology](https://github.com/ostis-ai/scl-machine). scl-machine serves as a crucial inference engine within the OSTIS Technology, leveraging semantic technologies to enhance intelligent systems' capabilities in reasoning and knowledge processing.

## Getting started

To get started, check out our [quick start guide](https://ostis-ai.github.io/scl-machine/quick_start).

## Documentation

- A brief user manual and developer docs are hosted on our [GitHub Pages](https://ostis-ai.github.io/scl-machine).
  - <details>
      <summary>Build documentation locally</summary>

    ```sh
    pip3 install mkdocs mkdocs-material
    mkdocs serve
    # and open http://127.0.0.1:8006/ in your browser
    ```
    </details>

- Documentation about implementation of components is located in [docs/main.pdf](docs/main.pdf) file of this project.
  - <details>
       <summary>Build documentation locally</summary>
      
    - ### Build steps (using LaTeX)
      ```sh
      cd docs
      TEXINPUTS=./scn: latexmk -pdf -bibtex main.tex
      ```

    - ### Build steps (using Docker)

      ```sh
      docker run -v ${PWD}:/workdir --rm -it ostis/scn-latex-plugin:latest "docs/main.tex"
      ```

    - ### Download scn-tex-plugin and documentation for subprojects

      After the compilation, the `main.pdf` file should appear at `scl-machine/docs/`. You can find more information about [scn-latex-plugin here](https://github.com/ostis-ai/scn-latex-plugin).
    </details>

## Feedback

Contributions, bug reports and feature requests are welcome! Feel free to check our [issues page](https://github.com/ostis-ai/scl-machine/issues) and file a new issue (or comment in existing ones).

## License

Distributed under the Apache License. Check [LICENSE](LICENSE) for more information.
