/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include "sc-memory/sc_memory.hpp"
#include "inferenceConfig/InferenceConfig.hpp"

namespace inference
{
/// Class to create template params to search and generate atomic logical formulas.
/// Control generation with flow with `generateOnlyFirst` and `generateOnlyUnique` flags
class TemplateManagerAbstract
{
public:
  explicit TemplateManagerAbstract(ScMemoryContext * context)
    : context(context)
  {
    replacementsUsingType = REPLACEMENTS_ALL;
    generationType = GENERATE_ALL_FORMULAS;
    fillingType = GENERATED_ONLY;
  }

  virtual ~TemplateManagerAbstract() = default;

  virtual std::vector<ScTemplateParams> createTemplateParams(ScAddr const & scTemplate) = 0;

  void addFixedArgument(ScAddr const & fixedArgument)
  {
    fixedArguments.push_back(fixedArgument);
  }

  ReplacementsUsingType getReplacementsUsingType() const
  {
    return replacementsUsingType;
  }

  GenerationType getGenerationType() const
  {
    return generationType;
  }

  OutputStructureFillingType getFillingType() const
  {
    return fillingType;
  }

  void setReplacementsUsingType(ReplacementsUsingType const otherReplacementsUsingType)
  {
    replacementsUsingType = otherReplacementsUsingType;
  }

  ScAddrVector getArguments() const
  {
    return arguments;
  }

  void setArguments(ScAddrVector const & otherArguments)
  {
    arguments = otherArguments;
  }

  void setGenerationType(GenerationType otherGenType)
  {
    generationType = otherGenType;
  }

  void setFillingType(OutputStructureFillingType otherFillingType)
  {
    fillingType = otherFillingType;
  }

protected:
  ScMemoryContext * context;

  ScAddrVector arguments;
  ReplacementsUsingType replacementsUsingType;
  OutputStructureFillingType fillingType;
  GenerationType generationType;
  ScAddrVector fixedArguments;
};
}  // namespace inference
