/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <queue>
#include <vector>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

#include "generator/SolutionTreeGenerator.hpp"

using namespace std;

namespace inference
{

// TODO: Fix issue with using interface InferenceManager
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  ScAddr applyInference(
        const ScAddr & targetStatement,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet);

  ~DirectInferenceManager();

private:
  queue<ScAddr> createQueue(ScAddr const & set);

  bool useRule(ScAddr const & rule, vector<ScAddr> const & argumentList);

  bool isTargetAchieved(ScAddr const & target, vector<ScAddr> const & argumentList);

  ScTemplateParams createTemplateParams(ScAddr const & scTemplate, const vector<ScAddr> & argumentList);

  bool generateStatement(ScAddr const & statement, ScTemplateParams const & templateParams);

  ScMemoryContext * ms_context;
  SolutionTreeGenerator * solutionTreeManager;
};



}
