/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <iostream>

#include <sc-memory/cpp/sc_addr.hpp>

#include "DirectInferenceManager.hpp"
#include "model/SolutionTreeNode.hpp"

using namespace std;
using namespace inference;

DirectInferenceManager::DirectInferenceManager(ScMemoryContext * ms_context) : ms_context(ms_context)
{
}

SolutionTreeNode & DirectInferenceManager::applyInference(
      const ScAddr & targetTemplate,
      const ScAddr & ruleSet,
      const ScAddr & argumentSet)
{

  auto * treeNode = new SolutionTreeNode();
  cout << this->ms_context->HelperGetSystemIdtf(targetTemplate) << endl;

  return *treeNode;
}


