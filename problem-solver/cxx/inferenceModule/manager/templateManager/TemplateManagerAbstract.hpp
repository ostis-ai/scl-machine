/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include "sc-memory/sc_memory.hpp"

namespace inference
{
class TemplateManagerAbstract
{
public:
  explicit TemplateManagerAbstract(ScMemoryContext * context) : context(context)
  {
    generateOnlyFirst = true;
    generateOnlyUnique = false;
  }

  virtual ~TemplateManagerAbstract() = default;

  virtual std::vector<ScTemplateParams> createTemplateParams(ScAddr const & scTemplate) = 0;

  void addFixedArgumentIdentifier(std::string const & fixedArgumentIdentifier)
  {
    fixedArgumentIdentifiers.push_back(fixedArgumentIdentifier);
  }

  bool getGenerateOnlyFirst() const
  {
    return generateOnlyFirst;
  }

  bool getGenerateOnlyUnique() const
  {
    return generateOnlyUnique;
  }

  void setGenerateOnlyFirst(bool const otherGenerateOnlyFirst)
  {
    generateOnlyFirst = otherGenerateOnlyFirst;
  }

  ScAddrVector getArguments() const
  {
    return arguments;
  }

  void setArguments(ScAddrVector const & otherArguments)
  {
    arguments = otherArguments;
  }

  void setGenerateOnlyUnique(bool const otherGenerateOnlyUnique)
  {
    generateOnlyUnique = otherGenerateOnlyUnique;
  }

protected:
  ScMemoryContext * context;

  ScAddrVector arguments;
  bool generateOnlyFirst;
  bool generateOnlyUnique;
  std::vector<std::string> fixedArgumentIdentifiers;
};
}  // namespace inference
