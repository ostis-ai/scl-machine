/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "TemplateSearcher.hpp"

#include <memory>
#include <sc-memory/sc_addr.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>
#include <sc-agents-common/keynodes/coreKeynodes.hpp>
#include <utility>
#include <algorithm>

#include "keynodes/InferenceKeynodes.hpp"

using namespace inference;

TemplateSearcher::TemplateSearcher(ScMemoryContext * context)
{
  this->context = context;
  params = {};
  inputStructure = {};
}

vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplate(
      const ScAddr & templateAddr,
      const ScTemplateParams & templateParams)
{
  searchWithoutContentResult = std::make_unique<ScTemplateSearchResult>();
  vector<ScTemplateSearchResultItem> searchResult;
  ScTemplate searchTemplate;
  if (context->HelperBuildTemplate(searchTemplate, templateAddr, templateParams))
  {
    if (context->HelperCheckEdge(
          InferenceKeynodes::concept_template_with_links,
          templateAddr,
          ScType::EdgeAccessConstPosPerm))
    {
      searchResult = searchTemplateWithContent(searchTemplate, templateAddr);
    }
    else
    {
      context->HelperSearchTemplateInStruct(searchTemplate, inputStructure, *searchWithoutContentResult);
      for (size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
      {
        searchResult.push_back((*searchWithoutContentResult)[searchItemIndex]);
      }
    }
  }
  else { throw runtime_error("Template is not built."); }

  return searchResult;
}

std::vector<ScTemplateSearchResultItem> TemplateSearcher::searchTemplateWithContent(
      const ScTemplate & searchTemplate,
      const ScAddr & templateAddr)
{
  context->HelperSearchTemplateInStruct(searchTemplate, inputStructure, *searchWithoutContentResult);
  std::map<std::string, std::string> linksContentMap = getTemplateKeyLinksContent(templateAddr);
  vector<ScTemplateSearchResultItem> searchWithContentResult;
  for(size_t searchItemIndex = 0; searchItemIndex < searchWithoutContentResult->Size(); searchItemIndex++)
  {
    bool contentIsIdentical = true;

    ScTemplateSearchResultItem searchResultItem = (*searchWithoutContentResult)[searchItemIndex];
    for (auto const & linkIdContentPair: linksContentMap)
    {
      ScAddr linkAddr = searchResultItem[linkIdContentPair.first];
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
      {
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      }
      else
      {
        stringContent = "";
      }
      if (stringContent != linkIdContentPair.second)
      {
        contentIsIdentical = false;
        break;
      }
    }
    if (contentIsIdentical)
    {
      searchWithContentResult.push_back(searchResultItem);
    }
  }

  return searchWithContentResult;
}

std::map<std::string, std::string> TemplateSearcher::getTemplateKeyLinksContent(const ScAddr &templateAddr)
{
  std::string link_alias = "_link";

  std::map<std::string, std::string> linksContent;
  ScTemplate scTemplate;
  scTemplate.TripleWithRelation(
        templateAddr,
        ScType::EdgeAccessVarPosPerm,
        ScType::Link >> link_alias,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
  ScTemplateSearchResult searchResult;
  context->HelperSearchTemplateInStruct(scTemplate, inputStructure, searchResult);
  for(size_t i = 0; i < searchResult.Size(); i++)
  {
    ScAddr linkAddr = searchResult[i][link_alias];
    if (utils::CommonUtils::checkType(context, linkAddr, ScType::LinkVar))
    {
      std::string stringContent;
      ScStreamPtr linkContentStream = context->GetLinkContent(linkAddr);
      if (linkContentStream != nullptr)
      {
        ScStreamConverter::StreamToString(linkContentStream, stringContent);
      }
      else
      {
        stringContent = "";
      }
      linksContent.emplace(context->HelperGetSystemIdtf(linkAddr), stringContent);
    }
  }
  return linksContent;
}

std::set<std::string> TemplateSearcher::getVarNames(ScAddr const & structure)
{
  std::set<std::string> identifiers;
  auto const & collectVarIdtfs = [this, structure] (
        ScType const & varType,
        set<std::string> & identifiers)
  {
      ScIterator3Ptr variablesIter3 = context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, varType);
      while (variablesIter3->Next())
      {
        auto const variableSystemIdtf = context->HelperGetSystemIdtf(variablesIter3->Get(2));
        if (!variableSystemIdtf.empty())
          identifiers.insert(variableSystemIdtf);
      }
  };
  collectVarIdtfs(ScType::NodeVar, identifiers);
  collectVarIdtfs(ScType::LinkVar, identifiers);
  return identifiers;
}

const ScAddrVector & TemplateSearcher::getParams() const
{
  return params;
}

void TemplateSearcher::addParam(ScAddr param)
{
  params.push_back(param);
}

bool TemplateSearcher::addParamIfNotPresent(ScAddr param)
{
  if (std::find(params.begin(), params.end(), param) == std::end(params))
  {
    params.push_back(param);
    return true;
  }
  return false;
}

map<string, vector<ScAddr>> TemplateSearcher::searchTemplate(
      const ScAddr & templateAddr,
      const vector<ScTemplateParams> & scTemplateParamsVector)
{
  map<string, vector<ScAddr>> result;
  auto varNames = getVarNames(templateAddr);
  for (auto const & scTemplateParams : scTemplateParamsVector)
  {
    auto searchResults = searchTemplate(templateAddr, scTemplateParams);
    SC_LOG_DEBUG("In searchResults " + to_string(searchResults.size()) + " element(s)")
    for (auto const & searchResult : searchResults)
    {
      SC_LOG_DEBUG("In searchResult " + to_string(searchResult.Size()) + " element(s)")
      for (int i = 0; i < searchResult.Size(); ++i)
      {
        if (utils::CommonUtils::checkType(context, searchResult[i], ScType::Node))
          SC_LOG_DEBUG("Node " + context->HelperGetSystemIdtf(searchResult[i]))
        else
          SC_LOG_DEBUG("Edge")
      }
      if (searchResult.Has("test_argument"))
        SC_LOG_DEBUG("searchResult has test_argument")
      for (auto const & varName : varNames)
      {
        SC_LOG_DEBUG("For var " + varName + " contains returned " + to_string(searchResult.Has(varName)))
        result[varName].push_back(searchResult[varName]);
      }
    }
  }

  return result;
}

void TemplateSearcher::setInputStructure(const ScAddr & inputStructure)
{
  TemplateSearcher::inputStructure = inputStructure;
}

std::map<std::string, std::vector<ScAddr>> TemplateSearcher::searchTemplate(const ScAddr & templateAddr)
{

  map<string, vector<ScAddr>> result;
  auto varNames = getVarNames(templateAddr);
  ScTemplateParams blankParams;
  auto searchResults = searchTemplate(templateAddr, blankParams);
  SC_LOG_DEBUG("In searchResults " + to_string(searchResults.size()) + " element(s)")
  for (auto const & searchResult : searchResults)
  {
    SC_LOG_DEBUG("In searchResult " + to_string(searchResult.Size()) + " element(s)")
    for (int i = 0; i < searchResult.Size(); ++i)
    {
      if (utils::CommonUtils::checkType(context, searchResult[i], ScType::Node))
      SC_LOG_DEBUG("Node " + context->HelperGetSystemIdtf(searchResult[i]))
      else
      SC_LOG_DEBUG("Edge")
    }
    if (searchResult.Has("test_argument"))
    SC_LOG_DEBUG("searchResult has test_argument")
    for (auto const & varName : varNames)
    {
      SC_LOG_DEBUG("For var " + varName + " contains returned " + to_string(searchResult.Has(varName)))
      result[varName].push_back(searchResult[varName]);
    }
  }

  return result;
}
