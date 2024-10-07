/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "EraseSolutionManager.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace solutionModule
{
EraseSolutionManager::EraseSolutionManager(ScMemoryContext * context)
  : context(context)
{
}

void EraseSolutionManager::eraseSolution(ScAddr const & solution) const
{
  if (context->IsElement(solution) == SC_FALSE)
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "EraseSolutionManager: solution is not valid");
  ScAddrList const & ruleAndSubstitutionPairs = getListFromSet(solution);
  SC_LOG_DEBUG("EraseSolutionManager: Solution has " << ruleAndSubstitutionPairs.size() << " elements");
  safeEraseElement(solution);
  eraseRuleAndSubstitutionsPairs(ruleAndSubstitutionPairs);
}

ScAddrList EraseSolutionManager::getListFromSet(ScAddr const & set) const
{
  ScAddrList setElements;
  ScIterator3Ptr const & fromSetIterator =
      context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::ConstNode);
  while (fromSetIterator->Next())
    setElements.push_back(fromSetIterator->Get(2));
  return setElements;
}

void EraseSolutionManager::safeEraseElement(ScAddr const & element) const
{
  if (context->IsElement(element))
    context->EraseElement(element);
}

void EraseSolutionManager::eraseRuleAndSubstitutionsPairs(ScAddrList const & ruleAndSubstitutionPairs) const
{
  for (ScAddr const & ruleAndSubstitutionPair : ruleAndSubstitutionPairs)
  {
    if (!context->IsElement(ruleAndSubstitutionPair))
      continue;
    ScAddr const & substitutions =
        utils::IteratorUtils::getAnyByOutRelation(context, ruleAndSubstitutionPair, ScKeynodes::rrel_2);
    safeEraseElement(ruleAndSubstitutionPair);
    eraseSubstitutions(substitutions);
  }
}

void EraseSolutionManager::eraseSubstitutions(ScAddr const & substitutions) const
{
  if (context->IsElement(substitutions))
  {
    ScAddrList const & substitutionPairs = getListFromSet(substitutions);
    safeEraseElement(substitutions);
    for (ScAddr const & substitutionPair : substitutionPairs)
    {
      if (!context->IsElement(substitutionPair))
        continue;
      ScAddr const & replacement =
          utils::IteratorUtils::getAnyByOutRelation(context, substitutionPair, ScKeynodes::rrel_1);
      ScAddr const & variable =
          utils::IteratorUtils::getAnyByOutRelation(context, substitutionPair, ScKeynodes::rrel_2);
      safeEraseElement(substitutionPair);
      if (context->IsElement(replacement) && context->IsElement(variable))
        eraseConnectors(variable, ScType::ConstTempPosArc, replacement);
      else
        SC_LOG_WARNING("EraseSolutionManager: replacement or variable is invalid");
    }
  }
  else
    SC_LOG_WARNING("EraseSolutionManager: solution node does not have substitutions at rrel_2");
}

void EraseSolutionManager::eraseConnectors(ScAddr const & source, ScType const & connectorType, ScAddr const & target) const
{
  auto const & connectorsIterator = context->CreateIterator3(source, connectorType, target);
  while (connectorsIterator->Next())
    safeEraseElement(connectorsIterator->Get(1));
}
}  // namespace solutionModule
