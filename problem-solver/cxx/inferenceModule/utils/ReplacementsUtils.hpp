#pragma once

#include <map>
#include <sc-memory/sc_addr.hpp>
#include <set>
#include <sc-memory/sc_template.hpp>

using namespace std;
namespace inference
{


class ReplacementsUtils
{
public:
  static map<string, vector<ScAddr>> intersectReplacements(map<string, vector<ScAddr>> const & first, map<string, vector<ScAddr>> const & second);
  static map<string, vector<ScAddr>> uniteReplacements(map<string, vector<ScAddr>> const & first, map<string, vector<ScAddr>> const & second);
  static vector<ScTemplateParams> getReplacementsToScTemplateParams(map<string, vector<ScAddr>> const & replacements);
  static size_t getColumnsAmount(map<string, vector<ScAddr>> const & replacements);
  static size_t getRowsAmount(map<string, vector<ScAddr>> const & replacements);
private:
  static set<string> getKeySet(map<string, vector<ScAddr>> const & map);
  static set<string> getCommonKeys(set<string> const & first, set<string> const & second);
  static map<string, vector<ScAddr>> copyReplacements(map<string, vector<ScAddr>> const &);
};

}
