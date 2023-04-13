/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>

namespace inference
{
class TemplateManagerAbstract
{
public:
  explicit TemplateManagerAbstract(ScMemoryContext * context) : context(context)
  {
  };

  virtual ~TemplateManagerAbstract() = default;

  virtual std::vector<ScTemplateParams> createTemplateParams(ScAddr const & scTemplate, ScAddrVector const & argumentList) = 0;

  void addFixedArgumentIdentifier(std::string const & fixedArgumentIdentifier)
  {
    fixedArgumentIdentifiers.push_back(fixedArgumentIdentifier);
  }

protected:
  ScMemoryContext * context;

  std::vector<std::string> fixedArgumentIdentifiers;
};
}  // namespace inference
