#include "SolutionTreeSearcher.hpp"
#include "inference/inference_keynodes.hpp"

namespace inference
{
SolutionTreeSearcher::SolutionTreeSearcher(ScMemoryContext * context)
  : context(context)
{
}

bool SolutionTreeSearcher::checkIfSolutionNodeExists(
    ScAddr const & rule,
    ScTemplateParams const & templateParams,
    ScAddrUnorderedSet const & variables)
{
  ScTemplate solutionNodeTemplate;
  ScTemplateSearchResult searchResult;
  std::string const & solutionAlias = "_solution";
  std::string const & solutionNodeAlias = "_solution_node";
  std::string const & solutionsSetAlias = "_solutions_set";
  std::string const & replacementForAlias = "_replacement_for";
  solutionNodeTemplate.Triple(
      InferenceKeynodes::concept_solution, ScType::VarPermPosArc, ScType::VarNode >> solutionAlias);
  solutionNodeTemplate.Triple(solutionAlias, ScType::VarPermPosArc, ScType::VarNode >> solutionNodeAlias);
  solutionNodeTemplate.Quintuple(
      solutionNodeAlias, ScType::VarPermPosArc, rule, ScType::VarPermPosArc, ScKeynodes::rrel_1);
  solutionNodeTemplate.Quintuple(
      solutionNodeAlias,
      ScType::VarPermPosArc,
      ScType::VarNode >> solutionsSetAlias,
      ScType::VarPermPosArc,
      ScKeynodes::rrel_2);
  for (ScAddr const & variable : variables)
  {
    ScAddr replacement;
    templateParams.Get(variable, replacement);
    if (replacement.IsValid())
    {
      std::string const & pairAlias = replacementForAlias + std::to_string(variable.Hash());
      solutionNodeTemplate.Triple(solutionsSetAlias, ScType::VarPermPosArc, ScType::VarNode >> pairAlias);
      solutionNodeTemplate.Quintuple(
          pairAlias, ScType::VarPermPosArc, replacement, ScType::VarPermPosArc, ScKeynodes::rrel_1);
      solutionNodeTemplate.Quintuple(
          pairAlias, ScType::VarPermPosArc, variable, ScType::VarPermPosArc, ScKeynodes::rrel_2);
      solutionNodeTemplate.Triple(variable, ScType::VarTempPosArc, replacement);
    }
    else
      SC_THROW_EXCEPTION(
          utils::ExceptionItemNotFound,
          "SolutionTreeSearcher: rule " << context->GetElementSystemIdentifier(rule) << " has var "
                                        << context->GetElementSystemIdentifier(variable)
                                        << " but templateParams don't have replacement for this var");
  }
  context->SearchByTemplate(solutionNodeTemplate, searchResult);
  return !searchResult.IsEmpty();
}
}  // namespace inference
