/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "InferenceManagerBuilderAbstract.hpp"

using namespace inference;

InferenceManagerBuilderAbstract::InferenceManagerBuilderAbstract(ScMemoryContext * context) : context(context)
{
  inferenceManager = std::make_unique<InferenceManagerGeneral>(context);
}

InferenceManagerBuilderAbstract & InferenceManagerBuilderAbstract::setTemplateManager(std::unique_ptr<TemplateManagerAbstract> manager)
{
  templateManager = std::move(manager);
  return *this;
}

std::unique_ptr<InferenceManagerGeneral> InferenceManagerBuilderAbstract::build()
{
  return std::move(inferenceManager);
}

void InferenceManagerBuilderAbstract::reset()
{
  inferenceManager.reset();
}
