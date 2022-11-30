#pragma once

#include <map>
#include <set>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_template.hpp>

using Replacements = std::map<std::string, ScAddrVector>;
using namespace std;

namespace inference
{
class ReplacementsUtils
{
public:
  static Replacements intersectReplacements(Replacements const & first, Replacements const & second);
  static Replacements uniteReplacements(Replacements const & first, Replacements const & second);
  static vector<ScTemplateParams> getReplacementsToScTemplateParams(Replacements const & replacements);
  static size_t getColumnsAmount(Replacements const & replacements);
  static size_t getRowsAmount(Replacements const & replacements);

private:
  static set<string> getKeySet(Replacements const & map);
  static set<string> getCommonKeys(set<string> const & first, set<string> const & second);
  static Replacements copyReplacements(Replacements const & replacements);
};

}  // namespace inference
