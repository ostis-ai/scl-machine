/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace rulesApplicationModule
{

class RuleConstructionsSearcher
{
public:
  explicit RuleConstructionsSearcher(ScMemoryContext * context);

  std::vector<std::string> getVariablesNodesSystemIdentifiers(ScAddr const & structure);

  bool isContainsConstants(ScAddr const & structure);

  bool isKeyElement(ScAddr const & structure, ScAddr const & element);

  ScAddrVector getNoroleRelations(ScAddr const & structure);

  std::vector<std::pair<std::string, std::string>>  getConnectedByRelationVariablesIdentifiesrs(
        ScAddr const & relation,
        ScAddr const & statement);

private:
  ScMemoryContext * context;
};

} // namespace rulesApplicationModule
