#pragma once

#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "generator/SolutionTreeGenerator.hpp"
#include "searcher/SolutionTreeSearcher.hpp"

namespace inference
{

class SolutionTreeManager
{
public:
  explicit SolutionTreeManager(ScMemoryContext * context);

  bool addNode(ScAddr const & formula, std::vector<ScTemplateParams> const & templateParamsVector, std::set<std::string> const & varNames);

  ScAddr createSolution(ScAddr const & outputStructure, bool targetAchieved);

  bool checkIfSolutionNodeExists(ScAddr const & formula, ScTemplateParams const & templateParams, std::set<std::string> const & varNames);

private:
  ScMemoryContext * context;
  std::unique_ptr<SolutionTreeGenerator> solutionTreeGenerator;
  std::unique_ptr<SolutionTreeSearcher> solutionTreeSearcher;

};

} // inference
