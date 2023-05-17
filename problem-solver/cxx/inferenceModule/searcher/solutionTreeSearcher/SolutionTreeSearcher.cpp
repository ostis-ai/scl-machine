#include "SolutionTreeSearcher.hpp"
#include "keynodes/InferenceKeynodes.hpp"

namespace inference
{
SolutionTreeSearcher::SolutionTreeSearcher(ScMemoryContext * context)
  : context(context)
{
}

bool SolutionTreeSearcher::checkIfSolutionNodeExists(
    ScAddr const & rule,
    ScTemplateParams const & templateParams,
    std::set<std::string> const & varNames)
{
  ScTemplate solutionNodeTemplate;
  ScTemplateSearchResult searchResult;
  std::string const & solution_alias = "_solution";
  std::string const & solution_node_alias = "_solution_node";
  std::string const & solutions_set_alias = "_solutions_set";
  std::string const & replacement_for_alias = "_replacement_for";
  solutionNodeTemplate.Triple(
      InferenceKeynodes::concept_solution, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> solution_alias);
  solutionNodeTemplate.Triple(solution_alias, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> solution_node_alias);
  solutionNodeTemplate.TripleWithRelation(
      solution_node_alias,
      ScType::EdgeAccessVarPosPerm,
      rule,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_1);
  solutionNodeTemplate.TripleWithRelation(
      solution_node_alias,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> solutions_set_alias,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_2);
  for (std::string const & varName : varNames)
  {
    ScAddr replacement;
    templateParams.Get(varName, replacement);
    if (replacement.IsValid())
    {
      ScAddr const & varNode = context->HelperFindBySystemIdtf(varName);
      std::string pair_alias = replacement_for_alias + varName;
      solutionNodeTemplate.Triple(solutions_set_alias, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> pair_alias);
      solutionNodeTemplate.TripleWithRelation(
          pair_alias,
          ScType::EdgeAccessVarPosPerm,
          replacement,
          ScType::EdgeAccessVarPosPerm,
          scAgentsCommon::CoreKeynodes::rrel_1);
      solutionNodeTemplate.TripleWithRelation(
          pair_alias,
          ScType::EdgeAccessVarPosPerm,
          varNode,
          ScType::EdgeAccessVarPosPerm,
          scAgentsCommon::CoreKeynodes::rrel_2);
      solutionNodeTemplate.Triple(varName, ScType::EdgeAccessVarPosTemp, replacement);
    }
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound,
          "SolutionTreeSearcher: rule " << context->HelperGetSystemIdtf(rule) << " has var " << varName
                                        << " but templateParams don't have replacement for this var");
  }
  context->HelperSearchTemplate(solutionNodeTemplate, searchResult);
  return !searchResult.IsEmpty();
}
}  // namespace inference
