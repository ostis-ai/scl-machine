/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory_headers.hpp>

namespace solutionModule
{
class EraseSolutionManager
{
public:
  explicit EraseSolutionManager(ScMemoryContext * context, utils::ScLogger * logger);

  void eraseSolution(ScAddr const & solution) const;

private:
  ScMemoryContext * context;
  utils::ScLogger * logger;

  void safeEraseElement(ScAddr const & element) const;

  void eraseRuleAndSubstitutionsPairs(ScAddrList const & ruleAndSubstitutionPairs) const;

  void eraseSubstitutions(ScAddr const & substitutions) const;

  ScAddrList getListFromSet(ScAddr const & set) const;

  void eraseConnectors(ScAddr const & source, ScType const & connectorType, ScAddr const & target) const;
};

}  // namespace solutionModule
