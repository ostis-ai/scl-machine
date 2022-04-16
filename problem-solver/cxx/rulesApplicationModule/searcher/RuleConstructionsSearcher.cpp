/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "RuleConstructionsSearcher.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>

using namespace rulesApplicationModule;
using namespace scAgentsCommon;
using namespace std;

RuleConstructionsSearcher::RuleConstructionsSearcher(ScMemoryContext * context) : context(context)
{}

vector<string> RuleConstructionsSearcher::getVariablesNodesSystemIdentifiers(ScAddr const & structure)
{
  set<string> identifiers;
  auto const & collectVarIdtfs = [this, structure] (
          ScType const & varType,
          set<string> & identifiers)
  {
    ScIterator3Ptr variablesIter3 = context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, varType);
    while (variablesIter3->Next())
    {
      string const variableSystemIdtf = context->HelperGetSystemIdtf(variablesIter3->Get(2));
      if (!variableSystemIdtf.empty() && identifiers.find(variableSystemIdtf) == identifiers.end())
        identifiers.insert(variableSystemIdtf);
    }
  };
  collectVarIdtfs(ScType::NodeVar, identifiers);
  collectVarIdtfs(ScType::LinkVar, identifiers);

  vector<string> identifiersVector(identifiers.begin(), identifiers.end());
  return identifiersVector;
}

bool RuleConstructionsSearcher::isContainsConstants(ScAddr const & structure)
{
  ScIterator3Ptr constNodesIter3 = context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  ScIterator3Ptr constLinksIter3 = context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::LinkConst);

  return constNodesIter3->Next() || constLinksIter3->Next();
}

bool RuleConstructionsSearcher::isKeyElement(ScAddr const & structure, ScAddr const & element)
{
  ScIterator5Ptr variablesIter5 = context->Iterator5(
        structure,
        ScType::EdgeAccessConstPosPerm,
        element,
        ScType::EdgeAccessConstPosPerm,
        CoreKeynodes::rrel_key_sc_element
  );

  return variablesIter5->Next();
}

ScAddrVector RuleConstructionsSearcher::getNoroleRelations(ScAddr const & structure)
{
  ScTemplate relationsTemplate;
  relationsTemplate.Triple(structure, ScType::EdgeAccessVarPosPerm, ScType::NodeVarNoRole >> "_relation");
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplate(relationsTemplate, searchResult);

  ScAddrVector relations;
  relations.reserve(searchResult.Size());
  for (size_t resultIndex = 0; resultIndex < searchResult.Size(); resultIndex++)
  {
    relations.push_back(searchResult[resultIndex]["_relation"]);
  }

  return relations;
}

vector<pair<string, string>>  RuleConstructionsSearcher::getConnectedByRelationVariablesIdentifiesrs(
      ScAddr const & relation,
      ScAddr const & statement)
{
  ScIterator5Ptr iterator5Ptr = context->Iterator5(
        relation,
        ScType::EdgeAccessVarPosPerm,
        ScType::Unknown,
        ScType::EdgeAccessConstPosPerm,
        statement
  );

  vector<pair<string, string>> connectedVariablesIdentifiers;
  while (iterator5Ptr->Next())
  {
    ScAddr firstVariable, secondVariable;
    context->GetEdgeInfo(iterator5Ptr->Get(2), firstVariable, secondVariable);
    connectedVariablesIdentifiers.emplace_back(
          context->HelperGetSystemIdtf(firstVariable),
          context->HelperGetSystemIdtf(secondVariable));
  }

  return connectedVariablesIdentifiers;
}
