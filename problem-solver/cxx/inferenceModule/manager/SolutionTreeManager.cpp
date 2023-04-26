#include "SolutionTreeManager.hpp"

namespace inference
{
SolutionTreeManager::SolutionTreeManager(ScMemoryContext * context)
    : context(context)
{
  solutionTreeGenerator = std::make_unique<SolutionTreeGenerator>(context);
  solutionTreeSearcher = std::make_unique<SolutionTreeSearcher>(context);
}

bool SolutionTreeManager::addNode(ScAddr const & formula, std::vector<ScTemplateParams> const & templateParamsVector, std::set<std::string> const & varNames)
{
  bool result = true;
  for (ScTemplateParams const & templateParams : templateParamsVector)
    result &= solutionTreeGenerator->addNode(formula, templateParams, varNames);
  return result;
}

ScAddr SolutionTreeManager::createSolution(ScAddr const & outputStructure, bool targetAchieved)
{
  return solutionTreeGenerator->createSolution(outputStructure, targetAchieved);
}

bool SolutionTreeManager::checkIfSolutionNodeExists(ScAddr const & rule, ScTemplateParams const & templateParams, std::set<std::string> const & varNames)
{
  return solutionTreeSearcher->checkIfSolutionNodeExists(rule, templateParams, varNames);
}
} // inference
