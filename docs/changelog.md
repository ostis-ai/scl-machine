# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Formula iteration strategy to generate all constructions by template
- Template searcher in structures
- Inference builder to configure inference flow using different components
- Finish agent work if exception occurred
- Unify interfaces with old 3-argument applying inference
- New tests for logic module with argument checking and with complex rules
- Add pre-commit
- Add clang format check
- Add opportunity to generate and search in structure for DirectInferenceAgent
- Add opportunity to only search and check formula, but not generate
- Add mechanism for substitution values into variables
- Documentation in scn-tex format
- README
- Changelog
- Add always generating for conclusion in implication
- Add arguments to sub formulas in conjunction and disjunction
- Add conjunction generation
- Add parameters to solution tree

### Fixed
- Fixed template params creating with no arguments
- Fixed applying rules chain

### Changed
- Separate logical connectives classes
- Rename variables, log message concepts and methods according to the Standard concepts
- Separate tests for simple formulas, complex formulas and agent arguments
- Move to 0.7.0 ostis-web-platform
- Changed condition for applying generation(generate if this rule with this parameters wasn't applied yet)
