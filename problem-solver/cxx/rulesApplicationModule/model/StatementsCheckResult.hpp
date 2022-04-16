/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "comparator/ComparatorByNesting.hpp"
#include "templateSearcher/TemplateSearcher.hpp"

namespace rulesApplicationModule
{

using Replacements = std::map<std::string, ScAddrVector>;
using StructuresByNesting = std::set<std::pair<ScAddr, size_t>, ComparatorByNesting>;

class StatementsCheckResult
{
public:
  StatementsCheckResult();

  void add(
        ScMemoryContext * context,
        ScAddr const & atomicStatement,
        SearchWithContentResult const & searchResult,
        ScTemplateParams const & params);

  void add(
        ScMemoryContext * context,
        ScAddr const & atomicStatement,
        ScTemplateGenResult const & genResult);

  void addByReplacements(std::map<std::string, ScAddrVector> const & otherReplacements);

  bool extend(StatementsCheckResult const & other);

  bool extend(
        ScMemoryContext * context,
        ScAddr const & atomicStatement,
        size_t const & nestingLevel,
        SearchWithContentResult const & searchResult,
        ScTemplateParams const & params = ScTemplateParams());

  std::map<std::string, ScAddrVector> getUniqueReplacements(std::vector<std::string> const & identifiers) const;

  Replacements replacements;
  StructuresByNesting notFoundStructures;
  StructuresByNesting notSearchedStructures;

private:
  std::vector<std::string> getCommonVariablesIdtf(std::map<std::string, ScAddrVector> const & otherReplacements);

  std::map<std::string, ScAddrVector> createReplacements(
        ScMemoryContext * context,
        ScAddr const & atomicStatement,
        SearchWithContentResult const & searchResult,
        ScTemplateParams const & params = ScTemplateParams());

  std::map<std::string, ScAddrVector> createReplacements(
        ScMemoryContext * context,
        ScAddr const & atomicStatement,
        ScTemplateGenResult const & genResult);

  bool update(std::map<std::string, ScAddrVector> const & replacements);

  bool commonKeysInReplacementsCombinationAreEqual(
        std::vector<std::string>  const & commonKeys,
        Replacements const & otherReplacements,
        size_t const & otherReplacementsCombinationIndex,
        size_t const & replacementsCombinationIndex);

  std::vector<std::string> getAdditionalKeys(Replacements const & otherReplacements);

  bool updateWithCommonKeys(
        std::vector<std::string>  const & commonKeys,
        std::map<std::string, ScAddrVector> const & otherReplacements);

  bool updateWithoutKeys(std::map<std::string, ScAddrVector> const & otherReplacements);

  bool replacementsKeysIsEqual(
        std::map<std::string, ScAddrVector> const & first,
        std::map<std::string, ScAddrVector> const & second);

  bool replacementCombinationsForKeysAreDifferent(
        size_t const & firstCombinationIndex,
        size_t const & secondCombinationIndex,
        std::vector<std::string> const & keys) const;
};

} // namespace rulesApplicationModule
