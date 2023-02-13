/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "TemplateExpressionNode.hpp"

TemplateExpressionNode::TemplateExpressionNode(
    ScMemoryContext * context,
    ScAddr const & formulaTemplate,
    TemplateSearcher * templateSearcher)
  : context(context)
  , formulaTemplate(formulaTemplate)
  , templateSearcher(templateSearcher)
{
}
TemplateExpressionNode::TemplateExpressionNode(
    ScMemoryContext * context,
    ScAddr const & formulaTemplate,
    TemplateSearcher * templateSearcher,
    TemplateManager * templateManager,
    ScAddr const & outputStructure)
  : context(context)
  , formulaTemplate(formulaTemplate)
  , templateSearcher(templateSearcher)
  , templateManager(templateManager)
  , outputStructure(outputStructure)
{
}

LogicExpressionResult TemplateExpressionNode::check(ScTemplateParams & params) const
{
  std::vector<ScTemplateSearchResultItem> searchResult = templateSearcher->searchTemplate(formulaTemplate, params);
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
  std::string const formulaIdentifier = context->HelperGetSystemIdtf(formulaTemplate);
  SC_LOG_DEBUG("Checking atomic logical formula " + formulaIdentifier);

  std::vector<ScTemplateParams> const templateParamsVector =
      templateManager->createTemplateParams(formulaTemplate, argumentVector);
  result.replacements = templateSearcher->searchTemplate(formulaTemplate, templateParamsVector);
  result.value = !result.replacements.empty();
  std::string formulaValue = (result.value ? " true" : " false");
  SC_LOG_DEBUG("Compute atomic logical formula " + formulaIdentifier + formulaValue);

  return result;
}

LogicFormulaResult TemplateExpressionNode::find(Replacements & replacements) const
{
  LogicFormulaResult result;
  std::vector<ScTemplateParams> paramsVector = ReplacementsUtils::getReplacementsToScTemplateParams(replacements);
  result.replacements = templateSearcher->searchTemplate(formulaTemplate, paramsVector);
  result.value = !result.replacements.empty();

  std::string const idtf = context->HelperGetSystemIdtf(formulaTemplate);
  std::string ending = (result.value ? " true" : " false");
  SC_LOG_DEBUG("Find Statement " + idtf + ending);

  return result;
}

LogicFormulaResult TemplateExpressionNode::generate(Replacements & replacements) const
{
  LogicFormulaResult result;
  result.isGenerated = false;
  std::vector<ScTemplateParams> paramsVector = ReplacementsUtils::getReplacementsToScTemplateParams(replacements);
  std::set<std::string> const & varNames = templateSearcher->getVarNames(formulaTemplate);
  if (paramsVector.empty())
  {
    SC_LOG_DEBUG("Atomic logical formula " + context->HelperGetSystemIdtf(formulaTemplate) + " is not generated");
    return compute(result);
  }

  for (ScTemplateParams const & scTemplateParams : paramsVector)
  {
    if (result.isGenerated)
      break;

    std::vector<ScTemplateSearchResultItem> searchResult =
        templateSearcher->searchTemplate(formulaTemplate, scTemplateParams);
    if (searchResult.empty())
    {
      ScTemplate generatedTemplate;
      context->HelperBuildTemplate(generatedTemplate, formulaTemplate, scTemplateParams);

      ScTemplateGenResult generationResult;
      ScTemplate::Result const & genTemplate = context->HelperGenTemplate(generatedTemplate, generationResult);
      if (genTemplate)
      {
        result.isGenerated = true;
        result.value = true;
        Replacements temporalReplacements;
        for (auto const & name : varNames)
        {
          ScAddrVector replacementsVector;
          bool const generationHasVar =
              generationResult.GetReplacements().find(name) != generationResult.GetReplacements().cend();
          ScAddr outResult;
          bool const paramsHaveVar = scTemplateParams.Get(name, outResult);
          if (generationHasVar)
            replacementsVector.push_back(generationResult[name]);
          else if (paramsHaveVar)
            replacementsVector.push_back(outResult);
          else
            SC_THROW_EXCEPTION(utils::ScException, "generation result and template params do not have replacement for " + name);
          temporalReplacements[name] = replacementsVector;
        }
        result.replacements = ReplacementsUtils::uniteReplacements(result.replacements, temporalReplacements);
      }

      SC_LOG_DEBUG("Atomic logical formula " + context->HelperGetSystemIdtf(formulaTemplate) + " is generated");
      for (size_t i = 0; i < generationResult.Size(); ++i)
      {
        templateSearcher->addParamIfNotPresent(generationResult[i]);
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, outputStructure, generationResult[i]);
      }
    }
  }
  return result;
}
