/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "LogicExpression.hpp"
#include "utils/ReplacementsUtils.hpp"
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

AndExpressionNode::AndExpressionNode(OperandsVectorType &operands)
{
  for (auto &operand: operands)
    this->operands.emplace_back(std::move(operand));
}

AndExpressionNode::AndExpressionNode(ScMemoryContext *context,
                                     OperatorLogicExpressionNode::OperandsVectorType & operands):
      AndExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult AndExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;
  operatorResult.value = true;

  for (auto &operand: operands)
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

LogicFormulaResult AndExpressionNode::compute(ScTemplateParams params) const
{
  LogicFormulaResult result;
  result.value = false;
  vector<LogicExpressionNode *> formulasWithoutConstants;
  vector<LogicExpressionNode *> formulasToGenerate;
  FormulaClassifier formulaClassifier(context);
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (!formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult = operand->compute(params);
    if (!lastResult.value || lastResult.replacements.empty())
      return {false, {}};
    if (!result.value) // this is true only when processing the first operand
      result = lastResult;
    else
    {
      result.replacements = ReplacementsUtils::intersectionOfReplacements(result.replacements, lastResult.replacements);
      if (result.replacements.empty())
      {
        result.value = false;
        return result;
      }
    }
  }
  // search in noConst and generate
}

OrExpressionNode::OrExpressionNode(OperandsVectorType &operands)
{
  for (auto &operand: operands)
    this->operands.emplace_back(std::move(operand));
}

OrExpressionNode::OrExpressionNode(ScMemoryContext *context, OperatorLogicExpressionNode::OperandsVectorType & operands):
      OrExpressionNode(operands)
{
  this->context = context;
}

LogicExpressionResult OrExpressionNode::check(ScTemplateParams params) const
{
  LogicExpressionResult operatorResult;
  operatorResult.value = false;

  for (auto & operand: operands)
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

LogicFormulaResult OrExpressionNode::compute(ScTemplateParams params) const
{
  LogicFormulaResult result;
  result.value = false;
  vector<LogicExpressionNode *> formulasWithoutConstants;
  vector<LogicExpressionNode *> formulasToGenerate;
  FormulaClassifier formulaClassifier(context);
  for (auto const & operand : operands)
  {
    auto atom = dynamic_cast<TemplateExpressionNode *>(operand.get());
    if (atom)
    {
      if (!formulaClassifier.isFormulaWithConst(atom->getFormulaTemplate()))
      {
        formulasWithoutConstants.push_back(atom);
        continue;
      }
      if (!formulaClassifier.isFormulaToGenerate(atom->getFormulaTemplate()))
      {
        formulasToGenerate.push_back(atom);
        continue;
      }
    }
    LogicFormulaResult lastResult = operand->compute(params);
    result.value |= lastResult.value;
    if (result.replacements.empty())
    {
      result.replacements = lastResult.replacements;
    }
    else
    {
      result.replacements = ReplacementsUtils::intersectionOfReplacements(result.replacements, lastResult.replacements);
      if (result.replacements.empty())
      {
        result.value = false;
        return result;
      }
    }
  }
  // search in noConst and generate
}

NotExpressionNode::NotExpressionNode(std::unique_ptr<LogicExpressionNode> op)
{
  operands.emplace_back(std::move(op));
}

NotExpressionNode::NotExpressionNode(ScMemoryContext *context,
                                     std::unique_ptr<LogicExpressionNode> op):
      NotExpressionNode(std::move(op))
{
  this->context = context;
}

LogicExpressionResult NotExpressionNode::check(ScTemplateParams params) const
{
  if (operands.size() != 1)
  {
    SC_LOG_ERROR("Negation should have 1 operand but it has " + to_string(operands.size()))
    return {
          false,
          false,
          { nullptr, nullptr },
          ScAddr()
    };
  }
  auto checkResult = operands[0]->check(params);
  checkResult.value = !checkResult.value;

  return checkResult;
}

LogicFormulaResult NotExpressionNode::compute(ScTemplateParams params) const
{

}

ImplicationExpressionNode::ImplicationExpressionNode(OperandsVectorType & operands)
{
  for (auto &operand: operands)
    this->operands.emplace_back(std::move(operand));
}

LogicExpressionResult ImplicationExpressionNode::check(ScTemplateParams params) const
{
  if (operands.size() != 2)
  {
    SC_LOG_ERROR("Implication should have 2 operands but it has " + to_string(operands.size()))
    return {
          false,
          false,
          { nullptr, nullptr },
          ScAddr()
    };
  }
  auto ifResult = operands[0]->check(params);
  auto checkResult = operands[1]->check(params);
}

LogicFormulaResult ImplicationExpressionNode::compute(ScTemplateParams params) const
{

}

ImplicationExpressionNode::ImplicationExpressionNode(ScMemoryContext *context,
                                                     OperatorLogicExpressionNode::OperandsVectorType & operands):
      ImplicationExpressionNode(operands)
{
  this->context = context;
}

EquivalenceExpressionNode::EquivalenceExpressionNode(OperandsVectorType & operands)
{
  for (auto &operand: operands)
    this->operands.emplace_back(std::move(operand));
}

LogicExpressionResult EquivalenceExpressionNode::check(ScTemplateParams params) const
{

}

LogicFormulaResult EquivalenceExpressionNode::compute(ScTemplateParams params) const
{

}

EquivalenceExpressionNode::EquivalenceExpressionNode(ScMemoryContext *context,
                                                     OperatorLogicExpressionNode::OperandsVectorType & operands):
      EquivalenceExpressionNode(operands)
{
  this->context = context;
}

TemplateExpressionNode::TemplateExpressionNode(
      ScMemoryContext *context,
      ScAddr formulaTemplate,
      TemplateSearcher *templateSearcher
)
      : context(context),
        formulaTemplate(formulaTemplate),
        templateSearcher(templateSearcher) {}

LogicExpressionResult TemplateExpressionNode::check(ScTemplateParams params) const
{
  auto searchResult = templateSearcher->searchTemplate(formulaTemplate, params);
  std::string result = (!searchResult.empty() ? "right" : "wrong");
  SC_LOG_DEBUG("Statement " + context->HelperGetSystemIdtf(formulaTemplate) + " " + result);

  if (!searchResult.empty())
  {
    return {
          true,
          true,
          searchResult[0],
          formulaTemplate
    };
  } else
  {
    return {
          false,
          false,
          { nullptr, nullptr },
          formulaTemplate
    };
  }
}

LogicFormulaResult TemplateExpressionNode::compute(ScTemplateParams params) const
{

}

LogicExpression::LogicExpression(
      ScMemoryContext *context,
      TemplateSearcher *templateSearcher,
      TemplateManager *templateManager,
      ScAddrVector argumentList
) : context(context),
    templateSearcher(templateSearcher),
    templateManager(templateManager),
    argumentList(std::move(argumentList))
{
}

std::unique_ptr<LogicExpressionNode> LogicExpression::build(ScAddr const &node)
{
  auto resolveOperandsForTuple = [this](ScAddr const &node)
  {
      ScIterator3Ptr operands = context->Iterator3(
            node,
            ScType::EdgeAccessConstPosPerm,
            ScType::Unknown);

      OperatorLogicExpressionNode::OperandsVectorType operandsVector;

      while (operands->Next())
      {
        if (!operands->Get(2).IsValid())
          continue;
        std::unique_ptr<LogicExpressionNode> op = build(operands->Get(2));
        operandsVector.emplace_back(std::move(op));
      }

      return operandsVector;
  };
  auto resolveOperandsForEdge = [this](ScAddr const &edge)
  {
      ScAddr begin;
      ScAddr end;
      context->GetEdgeInfo(edge, begin, end);
      OperatorLogicExpressionNode::OperandsVectorType operandsVector(2);

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
  auto resolveOperandsForImplicationTuple = [this](ScAddr const &node)
  {
      ScAddr rrel_if = context->HelperFindBySystemIdtf("rrel_if");
      ScAddr rrel_then = context->HelperFindBySystemIdtf("rrel_then");
      ScAddr begin = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_if);
      ScAddr end = utils::IteratorUtils::getAnyByOutRelation(context, node, rrel_then);

      OperatorLogicExpressionNode::OperandsVectorType operandsVector(2);
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

  ScIterator3Ptr atomicFormulaIter3 = context->Iterator3(
        InferenceKeynodes::atomic_logical_formula,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (atomicFormulaIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a template");
    std::vector<ScTemplateParams> params = templateManager->createTemplateParamsList(node, argumentList);

    if (!params.empty() && paramsSet.empty())
    {
      paramsSet = std::move(params);
    }

    return std::make_unique<TemplateExpressionNode>(context, node, templateSearcher);
  }

  ScIterator3Ptr conjunctionIter3 = context->Iterator3(
        InferenceKeynodes::nrel_conjunction,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (conjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a conjunction tuple");
    auto operands = resolveOperandsForTuple(node);
    if (!operands.empty())
      return std::make_unique<AndExpressionNode>(context, operands);
    else
    {
      SC_LOG_ERROR("Conjunction must have operands");
      throw std::exception();
    }
  }

  ScIterator3Ptr disjunctionIter3 = context->Iterator3(
        InferenceKeynodes::nrel_disjunction,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (disjunctionIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a disjunction tuple");
    auto operands = resolveOperandsForTuple(node);
    if (!operands.empty())
      return std::make_unique<OrExpressionNode>(context, operands);
    else
    {
      SC_LOG_ERROR("Disjunction must have operands");
      throw std::exception();
    }
  }

  ScIterator3Ptr negationIter3 = context->Iterator3(
        InferenceKeynodes::nrel_negation,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (negationIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is a negation tuple");
    auto operands = resolveOperandsForTuple(node);
    if (operands.size() == 1)
      return std::make_unique<NotExpressionNode>(context, std::move(operands[0]));
    else
    {
      SC_LOG_ERROR("There is " + to_string(operands.size()) + " operands in negation, but should be one");
      throw std::exception();
    }

  }

  ScIterator3Ptr implicationIter3 = context->Iterator3(
        InferenceKeynodes::nrel_implication,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (implicationIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an implication");
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeDCommon)){
      auto operands = resolveOperandsForEdge(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
      {
        SC_LOG_ERROR("There is " + to_string(operands.size()) + " operands in implication, but should be two");
        throw std::exception();
      }
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      auto operands = resolveOperandsForImplicationTuple(node);
      if (operands.size() == 2)
        return std::make_unique<ImplicationExpressionNode>(context, operands);
      else
      {
        SC_LOG_ERROR("There is " + to_string(operands.size()) + " operands in implication, but should be two");
        throw std::exception();
      }
    }
  }
  ScIterator3Ptr equivalenceIter3 = context->Iterator3(
        InferenceKeynodes::nrel_equivalence,
        ScType::EdgeAccessConstPosPerm,
        node
  );

  if (equivalenceIter3->Next())
  {
    SC_LOG_DEBUG(context->HelperGetSystemIdtf(node) + " is an equivalence");
    if (utils::CommonUtils::checkType(context, node, ScType::EdgeUCommon)){
      auto operands = resolveOperandsForEdge(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
      {
        SC_LOG_ERROR("There is " + to_string(operands.size()) + " operands in equivalence, but should be two");
        throw std::exception();
      }
    }
    if (utils::CommonUtils::checkType(context, node, ScType::NodeTuple))
    {
      auto operands = resolveOperandsForTuple(node);
      if (operands.size() == 2)
        return std::make_unique<EquivalenceExpressionNode>(context, operands);
      else
      {
        SC_LOG_ERROR("There is " + to_string(operands.size()) + " operands in equivalence, but should be two");
        throw std::exception();
      }
    }
  }

  SC_LOG_ERROR(context->HelperGetSystemIdtf(node) + " is not defined tuple");
  throw std::exception();
}
