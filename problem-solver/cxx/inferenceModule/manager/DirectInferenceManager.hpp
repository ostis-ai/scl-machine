/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "manager/InferenceManager.hpp"

namespace inference
{

class DirectInferenceManager : public InferenceManager
{
public:
  SolutionTreeNode & applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet) override;
};

}
