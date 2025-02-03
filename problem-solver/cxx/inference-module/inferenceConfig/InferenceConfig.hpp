/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>

#include "utils/Types.hpp"
namespace inference
{
enum GenerationType
{
  GENERATE_UNIQUE_FORMULAS = 1,
  GENERATE_ALL_FORMULAS = 2
};

enum ReplacementsUsingType
{
  REPLACEMENTS_FIRST = 1,
  REPLACEMENTS_ALL = 2
};

enum SolutionTreeType
{
  TREE_FULL = 1,
  TREE_ONLY_SUCCESS_BRANCH = 2,
  TREE_ONLY_OUTPUT_STRUCTURE = 3
};

enum SearchType
{
  SEARCH_IN_ALL_KB = 1,
  SEARCH_IN_STRUCTURES = 2,
  SEARCH_ONLY_MEMBERSHIP_ARCS_IN_STRUCTURES = 3
};

enum OutputStructureFillingType
{
  GENERATED_ONLY = 1,
  SEARCHED_AND_GENERATED = 2
};

enum AtomicLogicalFormulaSearchBeforeGenerationType
{
  SEARCH_WITH_REPLACEMENTS = 1,
  SEARCH_WITHOUT_REPLACEMENTS = 2
};

struct InferenceConfig
{
  GenerationType generationType;
  ReplacementsUsingType replacementsUsingType;
  SolutionTreeType solutionTreeType;
  SearchType searchType;
  OutputStructureFillingType fillingType;
  AtomicLogicalFormulaSearchBeforeGenerationType atomicLogicalFormulaSearchBeforeGenerationType;
};

struct InferenceParams
{
  ScAddr formulasSet;
  ScAddrVector arguments;
  ScAddrUnorderedSet inputStructures;
  ScAddr outputStructure;
  ScAddr targetStructure;
};
}  // namespace inference
