/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicExpression.hpp"
#include "utils/ReplacementsUtils.hpp"
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

ConjunctionExpressionNode::ConjunctionExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

ConjunctionExpressionNode::ConjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : ConjunctionExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult ConjunctionExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult conjunctionResult;
  conjunctionResult.value = true;

  for (auto & operand : operands)
  {
    auto operandResult = operand->check(params);
    conjunctionResult.formulaTemplate = operandResult.formulaTemplate;
    if (operandResult.hasSearchResult)
      conjunctionResult.templateSearchResult = operandResult.templateSearchResult;

    if (!operandResult.value)
    {
      conjunctionResult.value = false;
      return conjunctionResult;
    }
  }

  return conjunctionResult;
}

LogicFormulaResult ConjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;
  LogicFormulaResult fail = {false, false, {}};
  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;
  FormulaClassifier formulaClassifier(context);
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula without constants in conjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in conjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult = operand->compute(result);
    if (!lastResult.value)
      return fail;
    if (!result.value)  // this is true only when processing the first operand
      result = lastResult;
    else
    {
      result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
      if (result.replacements.empty())
        return fail;
    }
  }
  for (auto const & atom : formulasWithoutConstants)  // atoms without constants are processed here
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    if (!lastResult.value)
      return fail;
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
      return fail;
  }
  for (auto const & formulaToGenerate : formulasToGenerate)  // atoms which should be generated are processed here
  {
    LogicFormulaResult lastResult = formulaToGenerate->generate(result.replacements);
    if (!lastResult.value)
      return fail;
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
      return fail;
  }
  return result;
}

DisjunctionExpressionNode::DisjunctionExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

DisjunctionExpressionNode::DisjunctionExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : DisjunctionExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult DisjunctionExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult disjunctionResult;
  disjunctionResult.value = false;

  for (auto & operand : operands)
  {
    auto operandResult = operand->check(params);
    disjunctionResult.formulaTemplate = operandResult.formulaTemplate;

    if (operandResult.hasSearchResult)
      disjunctionResult.templateSearchResult = operandResult.templateSearchResult;

    if (operandResult.value)
    {
      disjunctionResult.value = true;
      return disjunctionResult;
    }
  }

  return disjunctionResult;
}

LogicFormulaResult DisjunctionExpressionNode::compute(LogicFormulaResult & result) const
{
  LogicFormulaResult fail = {false, false, {}};
  result.value = false;
  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;
  FormulaClassifier formulaClassifier(context);
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula without constants in disjunction");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in disjunction");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult;
    operand->compute(lastResult);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  if (result.replacements.empty())
    return fail;
  for (auto const & atom : formulasWithoutConstants)
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  for (auto const & formulaToGenerate : formulasToGenerate)
  {
    LogicFormulaResult lastResult = formulaToGenerate->generate(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  return result;
}

NegationExpressionNode::NegationExpressionNode(std::unique_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

NegationExpressionNode::NegationExpressionNode(ScMemoryContext * context, std::unique_ptr<LogicExpressionNode> op)
  : NegationExpressionNode(std::move(op))
{
  this->context = context;
}

LogicExpressionResult NegationExpressionNode::check(ScTemplateParams params) const
{
  if (operands.size() != 1)
  {
    SC_LOG_ERROR("Negation should have 1 operand but it has " + to_string(operands.size()));
    return {false, false, {nullptr, nullptr}, ScAddr()};
  }
  auto operandResult = operands[0]->check(params);
  operandResult.value = !operandResult.value;

  return operandResult;
}

LogicFormulaResult NegationExpressionNode::compute(LogicFormulaResult & result) const
{
  const LogicFormulaResult & formulaResult = operands[0]->compute(result);
  std::string formulaValue = (formulaResult.value ? "true" : "false");
  SC_LOG_DEBUG("Sub formula in negation returned " + formulaValue);
  return {!formulaResult.value, formulaResult.isGenerated, formulaResult.replacements};
}

ImplicationExpressionNode::ImplicationExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

LogicExpressionResult ImplicationExpressionNode::check(ScTemplateParams params) const
{
  if (operands.size() != 2)
  {
    SC_LOG_ERROR("Implication should have 2 operands but it has " + to_string(operands.size()));
    return {false, false, {nullptr, nullptr}, ScAddr()};
  }
  auto premiseResult = operands[0]->check(params);
  auto conclusionResult = operands[1]->check(params);
}

LogicFormulaResult ImplicationExpressionNode::compute(LogicFormulaResult & result) const
{
  result.value = false;
  FormulaClassifier formulaClassifier(context);

  auto premiseAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (premiseAtom) && formulaClassifier.isFormulaToGenerate(premiseAtom->getFormulaTemplate());

  auto conclusionAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (conclusionAtom) && formulaClassifier.isFormulaToGenerate(conclusionAtom->getFormulaTemplate());

  LogicFormulaResult premiseResult;
  LogicFormulaResult conclusionResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("Premise shouldn't be generated");
    premiseResult = operands[0]->compute(result);
    conclusionResult = (isRightGenerated ? conclusionAtom->generate(premiseResult.replacements) : operands[1]->compute(result));
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("Conclusion should be generated");
      return {true, true, {}};
    }
    else
    {
      SC_LOG_DEBUG("Conclusion shouldn't be generated");
      conclusionResult = operands[1]->compute(result);
      premiseResult = premiseAtom->generate(conclusionResult.replacements);
    }
  }

  result.value = !premiseResult.value || conclusionResult.value;
  result.isGenerated = premiseResult.isGenerated || conclusionResult.isGenerated;
  if (conclusionResult.value)
    result.replacements = ReplacementsUtils::uniteReplacements(premiseResult.replacements, conclusionResult.replacements);
  return result;
}

ImplicationExpressionNode::ImplicationExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : ImplicationExpressionNode(operands)
{
  this->context = context;
}

EquivalenceExpressionNode::EquivalenceExpressionNode(OperandsVector & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

LogicExpressionResult EquivalenceExpressionNode::check(ScTemplateParams params) const
{
}

LogicFormulaResult EquivalenceExpressionNode::compute(LogicFormulaResult & result) const
{
  vector<LogicFormulaResult> subFormulaResults;
  result.value = false;
  FormulaClassifier formulaClassifier(context);

  vector<TemplateExpressionNode *> formulasWithoutConstants;
  vector<TemplateExpressionNode *> formulasToGenerate;
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula without constants in equivalence");
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        SC_LOG_DEBUG("Found formula to generate in equivalence");
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    subFormulaResults.push_back(operand->compute(result));
  }
  SC_LOG_DEBUG("Processed " + to_string(subFormulaResults.size()) + " formulas in equivalence");
  if (subFormulaResults.empty())
  {
    SC_LOG_ERROR("All sub formulas in equivalence are either don't have constants or supposed to be generated");
    throw std::exception();
  }

  if (!formulasWithoutConstants.empty())
  {
    SC_LOG_DEBUG("Processing formula without constants");
    auto formulaWithoutConstants = formulasWithoutConstants[0];
    subFormulaResults.push_back(formulaWithoutConstants->find(subFormulaResults[0].replacements));
  }

  if (!formulasToGenerate.empty())
  {
    SC_LOG_DEBUG("Processing formula to generate");
    auto formulaToGenerate = formulasToGenerate[0];
    subFormulaResults.push_back(formulaToGenerate->generate(subFormulaResults[0].replacements));
  }
  result.value = subFormulaResults[0].value == subFormulaResults[1].value;
  if (result.value)
    result.replacements =
        ReplacementsUtils::intersectReplacements(subFormulaResults[0].replacements, subFormulaResults[1].replacements);
  return result;

  auto leftAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (leftAtom) && formulaClassifier.isFormulaToGenerate(leftAtom->getFormulaTemplate());

  auto rightAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (rightAtom) && formulaClassifier.isFormulaToGenerate(rightAtom->getFormulaTemplate());

  bool leftHasConstants = (leftAtom) && formulaClassifier.isFormulaWithConst(leftAtom->getFormulaTemplate());
  bool rightHasConstants = (rightAtom) && formulaClassifier.isFormulaWithConst(rightAtom->getFormulaTemplate());

  SC_LOG_DEBUG("Left has constants = " + to_string(leftHasConstants));
  SC_LOG_DEBUG("Right has constants = " + to_string(rightHasConstants));

  LogicFormulaResult leftResult;
  LogicFormulaResult rightResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("*** Left part of equivalence shouldn't be generated");
    leftResult = operands[0]->compute(result);
    rightResult = (isRightGenerated ? rightAtom->generate(leftResult.replacements) : operands[1]->compute(result));
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("*** Right part should be generated");
      return {true, {}};
    }
    else
    {
      SC_LOG_DEBUG("*** Right part shouldn't be generated");
      rightResult = operands[1]->compute(result);
      leftResult = leftAtom->generate(rightResult.replacements);
    }
  }

  result.value = leftResult.value == rightResult.value;
  if (rightResult.value)
    result.replacements = ReplacementsUtils::intersectReplacements(leftResult.replacements, rightResult.replacements);
  return result;
}

EquivalenceExpressionNode::EquivalenceExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVector & operands)
  : EquivalenceExpressionNode(operands)
{
  this->context = context;
}

TemplateExpressionNode::TemplateExpressionNode(
    ScMemoryContext * context,
    ScAddr formulaTemplate,
    TemplateSearcher * templateSearcher)
  : context(context)
  , formulaTemplate(formulaTemplate)
  , templateSearcher(templateSearcher)
{
}
TemplateExpressionNode::TemplateExpressionNode(
    ScMemoryContext * context,
    ScAddr formulaTemplate,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddr outputStructure)
  : context(context)
  , formulaTemplate(formulaTemplate)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , outputStructure(outputStructure)
{
}

LogicExpressionResult TemplateExpressionNode::check(ScTemplateParams params) const
{
  auto searchResult = templateSearcher->searchTemplate(formulaTemplate, params);
  std::string result = (!searchResult.empty() ? "true" : "false");
  SC_LOG_DEBUG("Atomic logical formula " + context->HelperGetSystemIdtf(formulaTemplate) + " " + result);

  if (!searchResult.empty())
  {
    return {true, true, searchResult[0], formulaTemplate};
  }
  else
  {
    return {false, false, {nullptr, nullptr}, formulaTemplate};
  }
}

LogicFormulaResult TemplateExpressionNode::compute(LogicFormulaResult & result) const
{
  auto const & formulaIdentifier = context->HelperGetSystemIdtf(formulaTemplate);
  SC_LOG_DEBUG("Checking atomic logical formula " + formulaIdentifier);
  result.replacements = templateSearcher->searchTemplate(formulaTemplate);
  result.value = !result.replacements.empty();
  std::string formulaValue = (result.value ? " true" : " false");
  SC_LOG_DEBUG("Compute atomic logical formula " + formulaIdentifier + formulaValue);

  return result;
}

LogicFormulaResult TemplateExpressionNode::find(map<string, vector<ScAddr>> & replacements) const
{
  LogicFormulaResult result;
  auto paramsVector = ReplacementsUtils::getReplacementsToScTemplateParams(replacements);
  result.replacements = templateSearcher->searchTemplate(formulaTemplate, paramsVector);
  result.value = !result.replacements.empty();

  auto const & idtf = context->HelperGetSystemIdtf(formulaTemplate);
  std::string ending = (result.value ? " true" : " false");
  SC_LOG_DEBUG("Find Statement " + idtf + ending);
  return result;
}

LogicFormulaResult TemplateExpressionNode::generate(map<string, vector<ScAddr>> & replacements) const
{
  LogicFormulaResult result;
  auto paramsVector = ReplacementsUtils::getReplacementsToScTemplateParams(replacements);
  if (paramsVector.empty())
  {
    result.isGenerated = false;
    SC_LOG_DEBUG("Atomic logical formula " + context->HelperGetSystemIdtf(formulaTemplate) + " is not generated");
    return compute(result);
  }

  for (auto const & scTemplateParams : paramsVector)
  {
    auto searchResult = templateSearcher->searchTemplate(formulaTemplate, scTemplateParams);
    if (searchResult.empty())
    {
      ScTemplate generatedTemplate;
      context->HelperBuildTemplate(generatedTemplate, formulaTemplate, scTemplateParams);

      ScTemplateGenResult generationResult;
      const ScTemplate::Result & genTemplate = context->HelperGenTemplate(generatedTemplate, generationResult);
      result.isGenerated = true;
      SC_LOG_DEBUG("Atomic logical formula " + context->HelperGetSystemIdtf(formulaTemplate) + " is generated");
      bool outputIsValid = outputStructure.IsValid();
      for (auto i = 0; i < generationResult.Size(); ++i)
      {
        templateSearcher->addParamIfNotPresent(generationResult[i]);
        if (outputIsValid)
          context->CreateEdge(ScType::EdgeAccessConstPosPerm, outputStructure, generationResult[i]);
      }
    }
  }
  return compute(result);
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentVector)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentVector(std::move(argumentVector))
{
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentVector,
    ScAddr outputStructure)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentVector(std::move(argumentVector))
  , outputStructure(outputStructure)
{
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & node)
{
  auto resolveTupleOperands = [this](ScAddr const & node) {
    ScIterator3Ptr operandsIterator = context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);

    OperatorLogicExpressionNode::OperandsVector operandsVector;

    while (operandsIterator->Next())
    {
      if (!operandsIterator->Get(2).IsValid())
        continue;
      std::unique_ptr<LogicExpressionNode> op = build(operandsIterator->Get(2));
      operandsVector.emplace_back(std::move(op));
    }
    SC_LOG_DEBUG(
        "[Amount of operands in " + context->HelperGetSystemIdtf(node) + "]: " + to_string(operandsVector.size()));

    return operandsVector;
  };
  auto resolveEdgeOperands = [this](ScAddr const & edge) {
    ScAddr begin;
    ScAddr end;
    context->GetEdgeInfo(edge, begin, end);
    OperatorLogicExpressionNode::OperandsVector operandsVector;

    if (begin.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> beginOperand = build(begin);
      operandsVector.emplace_back(std::move(beginOperand));
    }
    if (end.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> endOperand = build(end);
      operandsVector.emplace_back(std::move(endOperand));
    }
    return operandsVector;
  };
  auto resolveOperandsForImplicationTuple = [this](ScAddr const & node) {
    ScAddr rrel_if = context->HelperFindBySystemIdtf("rrel_if");
    ScAddr rrel_then = context->HelperFindBySystemIdtf("rrel_then");
    ScAddr begin = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_if);
    ScAddr end = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_then);

    OperatorLogicExpressionNode::OperandsVector operandsVector;
    if (begin.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> premiseOperand = build(begin);
      operandsVector.emplace_back(std::move(premiseOperand));
    }
    if (end.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> conclusionOperand = build(end);
      operandsVector.emplace_back(std::move(conclusionOperand));
    }
    return operandsVector;
  };

  ScIterator3Ptr atomicFormulaIterator =
      context->Iterator3(InferenceKeynodes::atomic_logical_formula, ScType::EdgeAccessConstPosPerm, node);

  if (atomicFormulaIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a template");
    std::vector<ScTemplateParams> params =
        templateManager->createTemplateParamsList(node, templateSearcher->getParams());

    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }

    return std::make_unique<TemplateExpressionNode>(context, node, templateSearcher, templateManager, outputStructure);
  }

  ScIterator3Ptr conjunctionIterator =
      context->Iterator3(InferenceKeynodes::nrel_conjunction, ScType::EdgeAccessConstPosPerm, node);

  if (conjunctionIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction tuple");
    auto operands = resolveTupleOperands(node);
    if (!operands.empty())
      return std::make_unique<ConjunctionExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Conjunction must have operands");
  }

  ScIterator3Ptr disjunctionIterator =
      context->Iterator3(InferenceKeynodes::nrel_disjunction, ScType::EdgeAccessConstPosPerm, node);

  if (disjunctionIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction tuple");
    auto operands = resolveTupleOperands(node);
    if (!operands.empty())
      return std::make_unique<DisjunctionExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Disjunction must have operands");
  }

  ScIterator3Ptr negationIterator =
      context->Iterator3(InferenceKeynodes::nrel_negation, ScType::EdgeAccessConstPosPerm, node);

  if (negationIterator->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a negation tuple");
    auto operands = resolveTupleOperands(node);
    if (operands.size() == 1)
      return std::make_unique<NegationExpressionNode>(context, std::move(operands[0]));
    else
      SC_THROW_EXCEPTION(
          utils::ScException, "There is " + to_string(operands.size()) + " operands in negation, but should be one");
  }

  ScIterator3Ptr implicationIterator =
      context->Iterator3(InferenceKeynodes::nrel_implication, ScType::EdgeAccessConstPosPerm, node);

  if (implicationIterator->Next())
  {
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeDCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication edge");
      auto operands = resolveEdgeOperands(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in implication edge, but should be two");
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication tuple");
      auto operands = resolveOperandsForImplicationTuple(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in implication tuple, but should be two");
    }
  }
  ScIterator3Ptr equivalenceIterator =
      context->Iterator3(InferenceKeynodes::nrel_equivalence, ScType::EdgeAccessConstPosPerm, node);

  if (equivalenceIterator->Next())
  {
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeUCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence edge");
      auto operands = resolveEdgeOperands(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in equivalence edge, but should be two");
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence tuple");
      auto operands = resolveTupleOperands(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
        SC_THROW_EXCEPTION(
            utils::ScException,
            "There is " + to_string(operands.size()) + " operands in equivalence tuple, but should be two");
    }
  }
  SC_THROW_EXCEPTION(utils::ScException, context->HelperGetSystemIdtf(node) + " is not defined tuple");
}
