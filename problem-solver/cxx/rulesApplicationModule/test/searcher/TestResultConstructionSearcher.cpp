/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "TestResultConstructionSearcher.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-memory/sc_link.hpp>

#include "keynodes/TestKeynodes.hpp"

using namespace rulesApplicationModule;
using namespace utils;

TestResultConstructionSearcher::TestResultConstructionSearcher(ScMemoryContext * context) : context(context)
{}

bool TestResultConstructionSearcher::checkSeveralSimpleEquivalenceRulesResultStructure(ScAddr const & outputStructure)
{
  ScAddr firstEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_1");
  ScAddr secondEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_2");

  ScTemplate scTemplate;
  scTemplate.Triple(
        TestKeynodes::power_supply,
        ScType::EdgeAccessVarNegPerm,
        firstEquipmentPiece
  );
  scTemplate.Triple(
        TestKeynodes::equipment_normaly_in_service,
        ScType::EdgeAccessVarPosPerm,
        firstEquipmentPiece
  );
  scTemplate.Triple(
        TestKeynodes::equipment_normaly_in_service,
        ScType::EdgeAccessVarPosPerm,
        secondEquipmentPiece
  );
  ScTemplateSearchResult result;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, result);

  return result.Size() == 1;
}

bool TestResultConstructionSearcher::checkSeveralNestedEquivalenceRulesOneTemplateToGenerateResultStructure(
      ScAddr const & outputStructure)
{
  ScAddr firstTerminal = context->HelperFindBySystemIdtf("terminal_1");
  ScAddr secondTerminal = context->HelperFindBySystemIdtf("terminal_2");
  ScAddr firstUsagePoint = context->HelperFindBySystemIdtf("usage_point_1");
  ScAddr secondUsagePoint = context->HelperFindBySystemIdtf("usage_point_2");
  ScAddr connectivityNode = context->HelperFindBySystemIdtf("connectivity_node_1");

  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        firstTerminal,
        ScType::EdgeDCommonVar,
        firstUsagePoint,
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::cim_nrel_equipment_usage_point
  );
  scTemplate.TripleWithRelation(
        firstTerminal,
        ScType::EdgeDCommonVar,
        secondUsagePoint,
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::cim_nrel_equipment_usage_point
  );
  scTemplate.TripleWithRelation(
        firstTerminal,
        ScType::EdgeDCommonVar,
        connectivityNode,
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::cim_nrel_connectivity_node
  );
  scTemplate.TripleWithRelation(
        secondTerminal,
        ScType::EdgeDCommonVar,
        connectivityNode,
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::cim_nrel_connectivity_node
  );

  ScTemplateSearchResult result;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, result);

  return result.Size() == 1;
}

bool TestResultConstructionSearcher::checkSeveralNestedEquivalenceRulesTwoTemplatesToGenerateResultStructure(
      ScAddr const & outputStructure)
{
  ScAddr firstTerminal = context->HelperFindBySystemIdtf("terminal_1");
  ScAddr secondTerminal = context->HelperFindBySystemIdtf("terminal_2");
  ScAddr firstUsagePoint = context->HelperFindBySystemIdtf("usage_point_1");
  ScAddr secondUsagePoint = context->HelperFindBySystemIdtf("usage_point_2");
  ScAddr firstConnectivityNode = context->HelperFindBySystemIdtf("node_1");

  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        firstTerminal,
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_terminal_1_usage_points_set",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_equipment_usage_points
  );
  scTemplate.Triple("_terminal_1_usage_points_set", ScType::EdgeAccessVarPosPerm, firstUsagePoint);
  scTemplate.Triple("_terminal_1_usage_points_set", ScType::EdgeAccessVarPosPerm, secondUsagePoint);
  scTemplate.TripleWithRelation(
        secondTerminal,
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_terminal_2_usage_points_set",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_equipment_usage_points
  );
  scTemplate.Triple("_terminal_2_usage_points_set", ScType::EdgeAccessVarPosPerm, secondUsagePoint);
  scTemplate.TripleWithRelation(
        secondTerminal,
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_terminal_2_connectivity_nodes_set",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_connectivity_nodes
  );
  scTemplate.Triple("_terminal_2_connectivity_nodes_set", ScType::EdgeAccessVarPosPerm, firstConnectivityNode);
  ScTemplateSearchResult result;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, result);

  return result.Size() == 1;
}

bool TestResultConstructionSearcher::checkCannotApplyRuleTestResultConstruction(ScAddr const & outputStructure)
{
  ScTemplate scTemplate;
  scTemplate.Triple(
        outputStructure,
        ScType::EdgeAccessVarPosPerm,
        ScType::Unknown
  );
  ScTemplateSearchResult result;

  return !context->HelperSearchTemplateInStruct(scTemplate, outputStructure, result);
}

bool TestResultConstructionSearcher::checkRulesWithLinkInConditionResultStructure(ScAddr const & outputStructure)
{
  ScAddr firstEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_1");
  ScAddr secondEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_2");

  ScTemplate firstTemplate;
  firstTemplate.Triple(
        TestKeynodes::power_supply,
        ScType::EdgeAccessVarNegPerm,
        firstEquipmentPiece
  );
  firstTemplate.Triple(
        TestKeynodes::equipment_normaly_in_service,
        ScType::EdgeAccessVarPosPerm,
        firstEquipmentPiece
  );
  ScTemplateSearchResult firstResult;
  context->HelperSearchTemplateInStruct(firstTemplate, outputStructure, firstResult);

  ScTemplate secondTemplate;
  secondTemplate.Triple(
        TestKeynodes::equipment_normaly_in_service,
        ScType::EdgeAccessVarPosPerm,
        secondEquipmentPiece
  );
  ScTemplateSearchResult secondResult;
  context->HelperSearchTemplateInStruct(secondTemplate, outputStructure, secondResult);

  return secondResult.IsEmpty() && firstResult.Size() == 1;
}

bool TestResultConstructionSearcher::checkRulesWithLinkInResultResultStructure(ScAddr const & outputStructure)
{
  ScAddr firstEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_1");
  ScAddr secondEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_2");

  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        firstEquipmentPiece,
        ScType::EdgeDCommonVar,
        ScType::LinkVar >> "_first_link",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_equipment_normaly_in_service
  );
  scTemplate.TripleWithRelation(
        firstEquipmentPiece,
        ScType::EdgeDCommonVar,
        ScType::LinkVar >> "_second_link",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_is_power_supply
  );
  scTemplate.TripleWithRelation(
        secondEquipmentPiece,
        ScType::EdgeDCommonVar,
        ScType::LinkVar >> "_third_link",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_is_power_supply
  );
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, searchResult);

  bool checkResult = false;

  if (searchResult.Size() == 1)
  {
    ScLink firstLink = ScLink(*context, searchResult[0]["_first_link"]);
    checkResult = firstLink.GetAsString() == "true";
    ScLink secondLink = ScLink(*context, searchResult[0]["_second_link"]);
    checkResult = checkResult && (secondLink.GetAsString() == "false");
    ScLink thirdLink = ScLink(*context, searchResult[0]["_third_link"]);
    checkResult = checkResult && (thirdLink.GetAsString() == "false");
  }

  return checkResult;
}

bool TestResultConstructionSearcher::checkRulesWithSameResultResultStructure(ScAddr const & outputStructure)
{
  ScAddr firstTerminal = context->HelperFindBySystemIdtf("terminal_1");
  ScAddr firstUsagePoint = context->HelperFindBySystemIdtf("usage_point_1");

  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        firstTerminal,
        ScType::EdgeDCommonVar,
        ScType::NodeVar >> "_terminal_1_usage_points_set",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_equipment_usage_points
  );
  scTemplate.Triple("_terminal_1_usage_points_set", ScType::EdgeAccessVarPosPerm, firstUsagePoint);
  ScTemplateSearchResult result;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, result);

  return result.Size() == 1;
}

bool TestResultConstructionSearcher::checkRuleWithLinkResultStructure(ScAddr const & outputStructure)
{
  ScAddr firstEquipmentPiece = context->HelperFindBySystemIdtf("equpment_piece_1");

  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        firstEquipmentPiece,
        ScType::EdgeDCommonVar,
        ScType::LinkVar >> "_first_link",
        ScType::EdgeAccessVarPosPerm,
        TestKeynodes::nrel_test_idtf
  );
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, searchResult);

  if (searchResult.Size() == 1)
  {
    ScLink firstLink = ScLink(*context, searchResult[0]["_first_link"]);
    return firstLink.GetAsString() == "some link content";
  }
  return false;
}


bool TestResultConstructionSearcher::checkActionApplicationResultStructure(
      ScAddr const & outputStructure,
      bool const & successfulCall)
{
  ScAddr firstEquipmentPiece = context->HelperFindBySystemIdtf("first_equipment_piece");
  ScAddr secondEquipmentPiece = context->HelperFindBySystemIdtf("second_equipment_piece");

  ScTemplate scTemplate;
  scTemplate.Triple(
        TestKeynodes::concept_topological_node,
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "_topological_node"
  );
  scTemplate.Triple(
        "_topological_node",
        ScType::EdgeAccessVarPosPerm,
        firstEquipmentPiece
  );
  scTemplate.Triple(
        "_topological_node",
        ScType::EdgeAccessVarPosPerm,
        secondEquipmentPiece
  );

  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplateInStruct(scTemplate, outputStructure, searchResult);

  if (successfulCall)
    return searchResult.Size() == 1;
  else
    return searchResult.Size() == 0;
}
