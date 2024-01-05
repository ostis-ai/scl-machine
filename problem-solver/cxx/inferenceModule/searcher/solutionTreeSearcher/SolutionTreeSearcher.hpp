#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "utils/ReplacementsUtils.hpp"

namespace inference
{
class SolutionTreeSearcher
{
public:
  explicit SolutionTreeSearcher(ScMemoryContext * context);

  bool checkIfSolutionNodeExists(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      ScAddrHashSet const & variables);

private:
  ScMemoryContext * context;
};

}  // namespace inference
