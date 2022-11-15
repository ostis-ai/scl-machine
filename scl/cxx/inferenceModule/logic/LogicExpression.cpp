/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "LogicExpression.hpp"
#include "utils/ReplacementsUtils.hpp"
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

AndExpressionNode::AndExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

AndExpressionNode::AndExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVectorType & operands)
  : AndExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult AndExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;
  operatorResult.value = true;

  for (auto & operand : operands)
  {
    auto checkResult = operand->check(params);
    operatorResult.formulaTemplate = checkResult.formulaTemplate;
    if (checkResult.hasSearchResult)
      operatorResult.templateSearchResult = checkResult.templateSearchResult;

    if (!checkResult.value)
    {
      operatorResult.value = false;
      return operatorResult;
    }
  }

  return operatorResult;
}

LogicFormulaResult AndExpressionNode::compute() const
{
  LogicFormulaResult result;
  result.value = false;
  LogicFormulaResult fail = {false, {}};
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
    LogicFormulaResult lastResult = operand->compute();
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
  for (auto const & genAtom : formulasToGenerate)  // atoms which should be generated are processed here
  {
    LogicFormulaResult lastResult = genAtom->generate(result.replacements);
    if (!lastResult.value)
      return fail;
    result.replacements = ReplacementsUtils::intersectReplacements(result.replacements, lastResult.replacements);
    if (result.replacements.empty())
      return fail;
  }
  return result;
}

OrExpressionNode::OrExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

OrExpressionNode::OrExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVectorType & operands)
  : OrExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult OrExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;
  operatorResult.value = false;

  for (auto & operand : operands)
  {
    auto checkResult = operand->check(params);
    operatorResult.formulaTemplate = checkResult.formulaTemplate;

    if (checkResult.hasSearchResult)
      operatorResult.templateSearchResult = checkResult.templateSearchResult;

    if (checkResult.value)
    {
      operatorResult.value = true;
      return operatorResult;
    }
  }

  return operatorResult;
}

LogicFormulaResult OrExpressionNode::compute() const
{
  LogicFormulaResult result;
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
    LogicFormulaResult lastResult = operand->compute();
    SC_LOG_DEBUG("One of operands in disjunction returned " + to_string(lastResult.value));
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  if (result.replacements.empty())
    return {false, {}};
  for (auto const & atom : formulasWithoutConstants)
  {
    LogicFormulaResult lastResult = atom->find(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  for (auto const & genAtom : formulasToGenerate)
  {
    LogicFormulaResult lastResult = genAtom->generate(result.replacements);
    result.value |= lastResult.value;
    result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, lastResult.replacements);
  }
  return result;
}

NotExpressionNode::NotExpressionNode(std::unique_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

NotExpressionNode::NotExpressionNode(ScMemoryContext * context, std::unique_ptr<LogicExpressionNode> op)
  : NotExpressionNode(std::move(op))
{
  this->context = context;
}

LogicExpressionResult NotExpressionNode::check(ScTemplateParams params) const
{
  if (operands.size() != 1)
  {
    SC_LOG_ERROR("Negation should have 1 operand but it has " + to_string(operands.size()));
    return {false, false, {nullptr, nullptr}, ScAddr()};
  }
  auto checkResult = operands[0]->check(params);
  checkResult.value = !checkResult.value;

  return checkResult;
}

LogicFormulaResult NotExpressionNode::compute() const
{
  const LogicFormulaResult & formulaResult = operands[0]->compute();
  SC_LOG_DEBUG("Sub formula in negation returned " + to_string(formulaResult.value));
  return {!formulaResult.value, {}};
}

ImplicationExpressionNode::ImplicationExpressionNode(OperandsVectorType & operands)
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
  auto ifResult = operands[0]->check(params);
  auto checkResult = operands[1]->check(params);
}

LogicFormulaResult ImplicationExpressionNode::compute() const
{
  LogicFormulaResult result;
  result.value = false;
  FormulaClassifier formulaClassifier(context);

  auto leftAtom = dynamic_cast<TemplateExpressionNode *>(operands[0].get());
  bool isLeftGenerated = (leftAtom) && formulaClassifier.isFormulaToGenerate(leftAtom->getFormulaTemplate());

  auto rightAtom = dynamic_cast<TemplateExpressionNode *>(operands[1].get());
  bool isRightGenerated = (rightAtom) && formulaClassifier.isFormulaToGenerate(rightAtom->getFormulaTemplate());

  LogicFormulaResult leftResult;
  LogicFormulaResult rightResult;

  if (!isLeftGenerated)
  {
    SC_LOG_DEBUG("If part shouldn't be generated");
    leftResult = operands[0]->compute();
    rightResult = (isRightGenerated ? rightAtom->generate(leftResult.replacements) : operands[1]->compute());
  }
  else
  {
    if (isRightGenerated)
    {
      SC_LOG_DEBUG("Then part should be generated");
      return {true, {}};
    }
    else
    {
      SC_LOG_DEBUG("Then part shouldn't be generated");
      rightResult = operands[1]->compute();
      leftResult = leftAtom->generate(rightResult.replacements);
    }
  }

  result.value = !leftResult.value || rightResult.value;
  if (rightResult.value)
    result.replacements = ReplacementsUtils::uniteReplacements(leftResult.replacements, rightResult.replacements);
  return result;
}

ImplicationExpressionNode::ImplicationExpressionNode(
    ScMemoryContext * context,
    OperatorLogicExpressionNode::OperandsVectorType & operands)
  : ImplicationExpressionNode(operands)
{
  this->context = context;
}

EquivalenceExpressionNode::EquivalenceExpressionNode(OperandsVectorType & operands)
{
  for (auto & operand : operands)
    this->operands.emplace_back(std::move(operand));
}

LogicExpressionResult EquivalenceExpressionNode::check(ScTemplateParams params) const
{
}

LogicFormulaResult EquivalenceExpressionNode::compute() const
{
  LogicFormulaResult result;
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
    subFormulaResults.push_back(operand->compute());
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
    leftResult = operands[0]->compute();
    rightResult = (isRightGenerated ? rightAtom->generate(leftResult.replacements) : operands[1]->compute());
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
      rightResult = operands[1]->compute();
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
    OperatorLogicExpressionNode::OperandsVectorType & operands)
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
  std::string result = (!searchResult.empty() ? "right" : "wrong");
  SC_LOG_DEBUG("Statement " + context->HelperGetSystemIdtf(formulaTemplate) + " " + result);

  if (!searchResult.empty())
  {
    return {true, true, searchResult[0], formulaTemplate};
  }
  else
  {
    return {false, false, {nullptr, nullptr}, formulaTemplate};
  }
}

LogicFormulaResult TemplateExpressionNode::compute() const
{
  LogicFormulaResult result;
  auto const & idtf = context->HelperGetSystemIdtf(formulaTemplate);
  SC_LOG_DEBUG("Checking atom " + idtf);
  result.replacements = templateSearcher->searchTemplate(formulaTemplate);
  result.value = !result.replacements.empty();
  std::string ending = (result.value ? " true" : " false");
  SC_LOG_DEBUG("Compute Statement " + idtf + ending);

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
    SC_LOG_DEBUG("Template " + context->HelperGetSystemIdtf(formulaTemplate) + " is not generated");
    return compute();
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
      SC_LOG_DEBUG("Template " + context->HelperGetSystemIdtf(formulaTemplate) + " is generated");
      bool outputIsValid = outputStructure.IsValid();
      for (auto i = 0; i < generationResult.Size(); ++i)
      {
        templateSearcher->addParamIfNotPresent(generationResult[i]);
        if (outputIsValid)
          context->CreateEdge(ScType::EdgeAccessConstPosPerm, outputStructure, generationResult[i]);
      }
    }
  }
  return compute();
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentList)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentList(std::move(argumentList))
{
}

LogicExpression::LogicExpression(
    ScMemoryContext * context,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddrVector argumentList,
    ScAddr outputStructure)
  : context(context)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , argumentList(std::move(argumentList))
  , outputStructure(outputStructure)
{
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const & node)
{
  auto resolveOperandsForTuple = [this](ScAddr const & node) {
    ScIterator3Ptr operands = context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::Unknown);

    OperatorLogicExpressionNode::OperandsVectorType operandsVector;

    while (operands->Next())
    {
      if (!operands->Get(2).IsValid())
        continue;
      std::unique_ptr<LogicExpressionNode> op = build(operands->Get(2));
      operandsVector.emplace_back(std::move(op));
    }
    SC_LOG_DEBUG(
        "[Amount of operands in " + context->HelperGetSystemIdtf(node) + "]: " + to_string(operandsVector.size()));

    return operandsVector;
  };
  auto resolveOperandsForEdge = [this](ScAddr const & edge) {
    ScAddr begin;
    ScAddr end;
    context->GetEdgeInfo(edge, begin, end);
    OperatorLogicExpressionNode::OperandsVectorType operandsVector;

    if (begin.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> op1 = build(begin);
      operandsVector.emplace_back(std::move(op1));
    }
    if (end.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> op2 = build(end);
      operandsVector.emplace_back(std::move(op2));
    }
    return operandsVector;
  };
  auto resolveOperandsForImplicationTuple = [this](ScAddr const & node) {
    ScAddr rrel_if = context->HelperFindBySystemIdtf("rrel_if");
    ScAddr rrel_then = context->HelperFindBySystemIdtf("rrel_then");
    ScAddr begin = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_if);
    ScAddr end = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_then);

    OperatorLogicExpressionNode::OperandsVectorType operandsVector;
    if (begin.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> op1 = build(begin);
      operandsVector.emplace_back(std::move(op1));
    }
    if (end.IsValid())
    {
      std::unique_ptr<LogicExpressionNode> op2 = build(end);
      operandsVector.emplace_back(std::move(op2));
    }
    return operandsVector;
  };

  ScIterator3Ptr atomicFormulaIter3 =
      context->Iterator3(InferenceKeynodes::atomic_logical_formula, ScType::EdgeAccessConstPosPerm, node);

  if (atomicFormulaIter3->Next())
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

  ScIterator3Ptr conjunctionIter3 =
      context->Iterator3(InferenceKeynodes::nrel_conjunction, ScType::EdgeAccessConstPosPerm, node);

  if (conjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction tuple");
    auto operands = resolveOperandsForTuple(node);
    if (!operands.empty())
      return std::make_unique<AndExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Conjunction must have operands");
  }

  ScIterator3Ptr disjunctionIter3 =
      context->Iterator3(InferenceKeynodes::nrel_disjunction, ScType::EdgeAccessConstPosPerm, node);

  if (disjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction tuple");
    auto operands = resolveOperandsForTuple(node);
    if (!operands.empty())
      return std::make_unique<OrExpressionNode>(context, operands);
    else
      SC_THROW_EXCEPTION(utils::ScException, "Disjunction must have operands");
  }

  ScIterator3Ptr negationIter3 =
      context->Iterator3(InferenceKeynodes::nrel_negation, ScType::EdgeAccessConstPosPerm, node);

  if (negationIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a negation tuple");
    auto operands = resolveOperandsForTuple(node);
    if (operands.size() == 1)
      return std::make_unique<NotExpressionNode>(context, std::move(operands[0]));
    else
      SC_THROW_EXCEPTION(
          utils::ScException, "There is " + to_string(operands.size()) + " operands in negation, but should be one");
  }

  ScIterator3Ptr implicationIter3 =
      context->Iterator3(InferenceKeynodes::nrel_implication, ScType::EdgeAccessConstPosPerm, node);

  if (implicationIter3->Next())
  {
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeDCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication edge");
      auto operands = resolveOperandsForEdge(node);
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
  ScIterator3Ptr equivalenceIter3 =
      context->Iterator3(InferenceKeynodes::nrel_equivalence, ScType::EdgeAccessConstPosPerm, node);

  if (equivalenceIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence");
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeUCommon))
    {
      SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence edge");
      auto operands = resolveOperandsForEdge(node);
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
      auto operands = resolveOperandsForTuple(node);
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
