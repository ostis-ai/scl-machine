/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/cpp/kpm/sc_agent.hpp>

using namespace std;

namespace inference
{

// TODO: Fix issue with using interface InferenceManager
class SolutionTreeGenerator
{
public:
  explicit SolutionTreeGenerator(ScMemoryContext * ms_context);

  ~SolutionTreeGenerator() = default;

  bool addNode(ScAddr const & rule, ScTemplateParams const & templateParams);

  ScAddr createSolution(bool targetAchieved);

private:
  ScAddr createSolutionNode(ScAddr const & rule, ScTemplateParams const & templateParams);

  ScMemoryContext * ms_context;
  ScAddr lastNode;
};

}
