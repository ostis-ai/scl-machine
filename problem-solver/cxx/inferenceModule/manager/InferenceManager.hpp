/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/sc_addr.hpp>

namespace inference
{

class InferenceManager
{
public:
  virtual ScAddr & applyInference(
        const ScAddr & targetTemplate,
        const ScAddr & ruleSet,
        const ScAddr & argumentSet) = 0;

  virtual ~InferenceManager() = 0;
};

}