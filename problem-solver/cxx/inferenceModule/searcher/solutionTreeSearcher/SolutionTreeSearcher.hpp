#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace inference
{
class SolutionTreeSearcher
{
public:
  explicit SolutionTreeSearcher(ScMemoryContext * context);

  bool checkIfSolutionNodeExists(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      std::set<std::string> const & varNames);

private:
  ScMemoryContext * context;
};

}  // namespace inference
