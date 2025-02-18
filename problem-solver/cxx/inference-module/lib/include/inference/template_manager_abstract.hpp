/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>

#include "inference/inference_config.hpp"

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

  virtual std::vector<ScTemplateParams> CreateTemplateParams(ScAddr const & scTemplate) = 0;

  void AddFixedArgument(ScAddr const & fixedArgument)
  {
    fixedArguments.push_back(fixedArgument);
  }

  ReplacementsUsingType GetReplacementsUsingType() const
  {
    return replacementsUsingType;
  }

  GenerationType GetGenerationType() const
  {
    return generationType;
  }

  OutputStructureFillingType GetFillingType() const
  {
    return fillingType;
  }

  void SetReplacementsUsingType(ReplacementsUsingType const otherReplacementsUsingType)
  {
    replacementsUsingType = otherReplacementsUsingType;
  }

  ScAddrVector GetArguments() const
  {
    return arguments;
  }

  void SetArguments(ScAddrVector const & otherArguments)
  {
    arguments = otherArguments;
  }

  void SetGenerationType(GenerationType otherGenType)
  {
    generationType = otherGenType;
  }

  void SetFillingType(OutputStructureFillingType otherFillingType)
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
