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
    ScAddrHashSet const & variables)
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
  solutionNodeTemplate.Quintuple(
      solution_node_alias,
      ScType::EdgeAccessVarPosPerm,
      rule,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_1);
  solutionNodeTemplate.Quintuple(
      solution_node_alias,
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVar >> solutions_set_alias,
      ScType::EdgeAccessVarPosPerm,
      scAgentsCommon::CoreKeynodes::rrel_2);
  for (ScAddr const & variable : variables)
  {
    ScAddr replacement;
    templateParams.Get(variable, replacement);
    if (replacement.IsValid())
    {
      std::string pair_alias = replacement_for_alias + to_string(variable.Hash());
      solutionNodeTemplate.Triple(solutions_set_alias, ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> pair_alias);
      solutionNodeTemplate.Quintuple(
          pair_alias,
          ScType::EdgeAccessVarPosPerm,
          replacement,
          ScType::EdgeAccessVarPosPerm,
          scAgentsCommon::CoreKeynodes::rrel_1);
      solutionNodeTemplate.Quintuple(
          pair_alias,
          ScType::EdgeAccessVarPosPerm,
          variable,
          ScType::EdgeAccessVarPosPerm,
          scAgentsCommon::CoreKeynodes::rrel_2);
      solutionNodeTemplate.Triple(variable, ScType::EdgeAccessVarPosTemp, replacement);
    }
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound,
          "SolutionTreeSearcher: rule " << context->HelperGetSystemIdtf(rule) << " has var "
                                        << context->HelperGetSystemIdtf(variable)
                                        << " but templateParams don't have replacement for this var");
  }
  context->HelperSearchTemplate(solutionNodeTemplate, searchResult);
  return !searchResult.IsEmpty();
}
}  // namespace inference
