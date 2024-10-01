/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "DeleteSolutionManager.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

namespace solutionModule
{
DeleteSolutionManager::DeleteSolutionManager(ScMemoryContext * context)
  : context(context)
{
}

void DeleteSolutionManager::deleteSolution(ScAddr const & solution) const
{
  if (context->IsElement(solution) == SC_FALSE)
    SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "DeleteSolutionManager: solution is not valid");
  ScAddrList const & ruleAndSubstitutionPairs = getListFromSet(solution);
  SC_LOG_DEBUG("DeleteSolutionManager: Solution has " << ruleAndSubstitutionPairs.size() << " elements");
  safeDeleteElement(solution);
  deleteRuleAndSubstitutionsPairs(ruleAndSubstitutionPairs);
}

ScAddrList DeleteSolutionManager::getListFromSet(ScAddr const & set) const
{
  ScAddrList setElements;
  ScIterator3Ptr const & fromSetIterator =
      context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::ConstNode);
  while (fromSetIterator->Next())
    setElements.push_back(fromSetIterator->Get(2));
  return setElements;
}

void DeleteSolutionManager::safeDeleteElement(ScAddr const & element) const
{
  if (context->IsElement(element))
    context->EraseElement(element);
}

void DeleteSolutionManager::deleteRuleAndSubstitutionsPairs(ScAddrList const & ruleAndSubstitutionPairs) const
{
  for (ScAddr const & ruleAndSubstitutionPair : ruleAndSubstitutionPairs)
  {
    if (!context->IsElement(ruleAndSubstitutionPair))
      continue;
    ScAddr const & substitutions =
        utils::IteratorUtils::getAnyByOutRelation(context, ruleAndSubstitutionPair, ScKeynodes::rrel_2);
    safeDeleteElement(ruleAndSubstitutionPair);
    deleteSubstitutions(substitutions);
  }
}

void DeleteSolutionManager::deleteSubstitutions(ScAddr const & substitutions) const
{
  if (context->IsElement(substitutions))
  {
    ScAddrList const & substitutionPairs = getListFromSet(substitutions);
    safeDeleteElement(substitutions);
    for (ScAddr const & substitutionPair : substitutionPairs)
    {
      if (!context->IsElement(substitutionPair))
        continue;
      ScAddr const & replacement =
          utils::IteratorUtils::getAnyByOutRelation(context, substitutionPair, ScKeynodes::rrel_1);
      ScAddr const & variable =
          utils::IteratorUtils::getAnyByOutRelation(context, substitutionPair, ScKeynodes::rrel_2);
      safeDeleteElement(substitutionPair);
      if (context->IsElement(replacement) && context->IsElement(variable))
        deleteConnectors(variable, ScType::ConstTempPosArc, replacement);
      else
        SC_LOG_WARNING("DeleteSolutionManager: replacement or variable is invalid");
    }
  }
  else
    SC_LOG_WARNING("DeleteSolutionManager: solution node does not have substitutions at rrel_2");
}

void DeleteSolutionManager::deleteConnectors(ScAddr const & source, ScType const & connectorType, ScAddr const & target) const
{
  auto const & connectorsIterator = context->CreateIterator3(source, connectorType, target);
  while (connectorsIterator->Next())
    safeDeleteElement(connectorsIterator->Get(1));
}
}  // namespace solutionModule
