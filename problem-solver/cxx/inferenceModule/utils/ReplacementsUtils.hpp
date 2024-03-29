/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "Types.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_template.hpp>

using ReplacementsHashes = std::unordered_map<size_t, std::vector<size_t>>;
using namespace std;

namespace inference
{
class ReplacementsUtils
{
public:
  static Replacements intersectReplacements(Replacements const & first, Replacements const & second);
  static Replacements uniteReplacements(Replacements const & first, Replacements const & second);
  static Replacements subtractReplacements(Replacements const & first, Replacements const & second);
  static Replacements removeRows(Replacements const & replacements, ScAddrHashSet & keysToRemove);
  static vector<ScTemplateParams> getReplacementsToScTemplateParams(Replacements const & replacements);
  static size_t getColumnsAmount(Replacements const & replacements);
  static void getKeySet(Replacements const & map, ScAddrHashSet & keySet);

private:
  static ScAddrHashSet getCommonKeys(ScAddrHashSet const & first, ScAddrHashSet const & second);
  static Replacements copyReplacements(Replacements const & replacements);
  static void removeDuplicateColumns(Replacements & replacements);
  static ReplacementsHashes calculateHashesForCommonKeys(
      Replacements const & replacements,
      ScAddrHashSet const & commonKeys);
};

}  // namespace inference
