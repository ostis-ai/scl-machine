#pragma once

#include <map>
#include <set>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_template.hpp>

using namespace std;

namespace inference
{

class ReplacementsUtils
{
public:
  static map<string, ScAddrVector> intersectReplacements(
      map<string, ScAddrVector> const & first,
      map<string, ScAddrVector> const & second);
  static map<string, ScAddrVector> uniteReplacements(
      map<string, ScAddrVector> const & first,
      map<string, ScAddrVector> const & second);
  static vector<ScTemplateParams> getReplacementsToScTemplateParams(map<string, ScAddrVector> const & replacements);
  static size_t getColumnsAmount(map<string, ScAddrVector> const & replacements);
  static size_t getRowsAmount(map<string, ScAddrVector> const & replacements);

private:
  static set<string> getKeySet(map<string, ScAddrVector> const & map);
  static set<string> getCommonKeys(set<string> const & first, set<string> const & second);
  static map<string, ScAddrVector> copyReplacements(map<string, ScAddrVector> const & replacements);
};

}  // namespace inference
