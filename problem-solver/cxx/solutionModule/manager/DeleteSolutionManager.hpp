/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_memory_headers.hpp"

namespace solutionModule
{
class DeleteSolutionManager
{
public:
  explicit DeleteSolutionManager(ScMemoryContext * context);

  void deleteSolution(ScAddr const & solution) const;
private:
  ScMemoryContext * context;

  void safeDeleteElement(ScAddr const & element) const;

  void deleteRuleAndSubstitutionsPairs(ScAddrList const & ruleAndSubstitutionPairs) const;

  void deleteSubstitutions(ScAddr const & substitutions) const;

  ScAddrList getListFromSet(ScAddr const & set) const;

  void deleteEdges(ScAddr const & source, ScType const & edge, ScAddr const & target) const;
};

}  // namespace solutionModule
