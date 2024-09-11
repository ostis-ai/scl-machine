# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v0.2.0]

### Breaking changes
- Direct inference manager was removed. To create DirectInferenceManagerTarget use `InferenceManagerFactory::constructDirectInferenceManagerTarget` with config {GENERATE_ALL_FORMULAS, ALL, TREE_ONLY_OUTPUT_STRUCTURE}

### Added
- InferenceConfig uses ScAddrUnorderedSet to store input structures
- Allow edges substitution from replacements in atomic logical formula search and generation
- Solution removal agent
- Template searcher abstract with new implementation: TemplateSearcherOnlyAccessEdgesInStructures
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
- Atomic logical formula generation with GENERATE_UNIQUE_FORMULAS mode
- Agent initiation in tests
- Clang formatter
- Fixed template params creating with no arguments
- Fixed applying rules chain

### Changed
- All questions are replaced with actions
- ScAddrHashSet is renamed to ScAddrUnorderedSet
- InferenceConfig is added to inference namespace
- Template search in structures checks if element is in structure instead of check if structure has element
- Logic formula generation results are passed as method arguments instead of return values
- Replacements results are passed as method arguments instead of return values
- Replacements union use hashes to improve performance
- Replacements operations use hashes to improve performance
- Replacements are now calculated for all variables in atomic logical formulas
- Replace DirectInferenceManager logic with configured InferenceManagerTarget
- Remove return logic expression result in LogicExpressionNode
- Separate manager and searcher files to the subdirectories
- Separate logical connectives classes
- Rename variables, log message concepts and methods according to the Standard concepts
- Separate tests for simple formulas, complex formulas and agent arguments
- Move to 0.7.0 ostis-web-platform
- Changed condition for applying generation(generate if this rule with this parameters wasn't applied yet)

### Removed
- Cache of input structures in TemplateSearcherInStructures
- Unused and duplicated keynodes
- `check` method of LogicExpressionNode
- Unused and unimplemented agents

## [v0.1.0]

- Direct inference agent for implications
- Template generation agent
- Reverse inference agent for implications
