# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Breaking changes
- Direct inference manager was removed. To create DirectInferenceManagerTarget use `InferenceManagerFactory::constructDirectInferenceManagerTarget` with config {GENERATE_ALL_FORMULAS, ALL, TREE_ONLY_OUTPUT_STRUCTURE}

### Added
- Inference flow config to control generation unique formulas, only first formula and solution tree
- Solution tree manager abstract with new implementation: SolutionTreeManagerEmpty
- Template manager abstract with new implementation: TemplateManagerFixedArguments
- Template searcher abstract with new implementation: TemplateSearcherInStructures
- Inference config and factory to create inference manager
- Inference manager abstract with new implementation: InferenceManagerAll
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
- Clang formatter
- Fixed template params creating with no arguments
- Fixed applying rules chain

### Changed
- Replace DirectInferenceManager logic with configured InferenceManagerTarget
- Remove return logic expression result in LogicExpressionNode
- Separate manager and searcher files to the subdirectories
- Separate logical connectives classes
- Rename variables, log message concepts and methods according to the Standard concepts
- Separate tests for simple formulas, complex formulas and agent arguments
- Move to 0.7.0 ostis-web-platform
- Changed condition for applying generation(generate if this rule with this parameters wasn't applied yet)

### Removed
- Unused and duplicated keynodes
- `check` method of LogicExpressionNode
- Unused and unimplemented agents