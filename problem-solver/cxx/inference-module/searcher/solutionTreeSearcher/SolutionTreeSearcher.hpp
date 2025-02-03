#pragma once

#include "utils/Types.hpp"

#include <sc-memory/sc_memory.hpp>

namespace inference
{
class SolutionTreeSearcher
{
public:
  explicit SolutionTreeSearcher(ScMemoryContext * context);

  bool checkIfSolutionNodeExists(
      ScAddr const & formula,
      ScTemplateParams const & templateParams,
      ScAddrUnorderedSet const & variables);

private:
  ScMemoryContext * context;
};

}  // namespace inference
