/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>
#include <algorithm>

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"

#include "inferenceConfig/InferenceConfig.hpp"

#include "utils/ReplacementsUtils.hpp"

namespace inference
{
/// Class to search atomic logical formulas and get replacements
class TemplateSearcherAbstract
{
public:
  explicit TemplateSearcherAbstract(
      ScMemoryContext * context,
      ReplacementsUsingType replacementsUsingType = ReplacementsUsingType::REPLACEMENTS_FIRST,
      OutputStructureFillingType outputStructureFillingType = OutputStructureFillingType::GENERATED_ONLY);

  virtual ~TemplateSearcherAbstract() = default;

  // TODO(MksmOrlov): implement searcher with default search template, configure searcher to use smart search or default
  virtual void searchTemplate(
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams,
      ScAddrHashSet const & variables,
      Replacements & result) = 0;

  virtual void searchTemplate(
      ScAddr const & templateAddr,
      vector<ScTemplateParams> const & scTemplateParamsVector,
      ScAddrHashSet const & variables,
      Replacements & result);

  void getVariables(ScAddr const & formula, ScAddrHashSet & variables);

  void getConstants(ScAddr const & formula, ScAddrHashSet & constants);

  bool isContentIdentical(
      ScTemplateSearchResultItem const & item,
      std::map<std::string, std::string> const & linksContentMap);

  void setInputStructures(ScAddrVector const & otherInputStructures);

  ScAddrVector getInputStructures() const;

  void setReplacementsUsingType(ReplacementsUsingType const otherReplacementsUsingType)
  {
    replacementsUsingType = otherReplacementsUsingType;
  }

  void setOutputStructureFillingType(OutputStructureFillingType const otherOutputStructureFillingType)
  {
    outputStructureFillingType = otherOutputStructureFillingType;
  }

  void setAtomicLogicalFormulaSearchBeforeGenerationType(
      AtomicLogicalFormulaSearchBeforeGenerationType const otherAtomicLogicalFormulaSearchBeforeGenerationType)
  {
    atomicLogicalFormulaSearchBeforeGenerationType = otherAtomicLogicalFormulaSearchBeforeGenerationType;
  }

  ReplacementsUsingType getReplacementsUsingType() const
  {
    return replacementsUsingType;
  }

  OutputStructureFillingType getOutputStructureFillingType() const
  {
    return outputStructureFillingType;
  }

  AtomicLogicalFormulaSearchBeforeGenerationType getAtomicLogicalFormulaSearchBeforeGenerationType() const
  {
    return atomicLogicalFormulaSearchBeforeGenerationType;
  }

protected:
  virtual void searchTemplateWithContent(
      ScTemplate const & searchTemplate,
      ScAddr const & templateAddr,
      ScTemplateParams const & templateParams,
      Replacements & result) = 0;

  virtual std::map<std::string, std::string> getTemplateLinksContent(ScAddr const & templateAddr) = 0;

  ScMemoryContext * context;
  std::unique_ptr<ScTemplateSearchResult> searchWithoutContentResult;
  ScAddrVector inputStructures;
  ReplacementsUsingType replacementsUsingType;
  OutputStructureFillingType outputStructureFillingType;
  AtomicLogicalFormulaSearchBeforeGenerationType atomicLogicalFormulaSearchBeforeGenerationType;
};
}  // namespace inference
