/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateManagerFixedArguments.hpp"

#include <algorithm>

using namespace inference;

TemplateManagerFixedArguments::TemplateManagerFixedArguments(ScMemoryContext * context)
  : TemplateManagerAbstract(context)
{
}

std::vector<ScTemplateParams> TemplateManagerFixedArguments::createTemplateParams(ScAddr const & scTemplate)
{
  std::vector<ScTemplateParams> templateParamsVector;
  size_t const size = std::min(arguments.size(), fixedArgumentIdentifiers.size());
  for (size_t i = 0; i < size; ++i)
  {
    ScTemplateParams param;
    param.Add(fixedArgumentIdentifiers.at(i), arguments.at(i));
    templateParamsVector.push_back(param);
  }

  return templateParamsVector;
}
