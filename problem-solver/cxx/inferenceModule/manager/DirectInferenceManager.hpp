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
class DirectInferenceManager
{
public:
  explicit DirectInferenceManager(ScMemoryContext * ms_context);

  ScAddr & applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet);

  ~DirectInferenceManager() = default;

private:
  queue<ScAddr> createQueue(ScAddr const & set);

  //TODO: Move to utils
  void addToQueue(vector<ScAddr> const & elementList, queue<ScAddr> & queue);
  ScAddr & useRule(ScAddr const & rule, vector<ScAddr> const & argumentList);
  bool isTargetAchieved(ScAddr const & targetTemplate, vector<ScAddr> const & argumentList);

  ScMemoryContext * ms_context;
};



}