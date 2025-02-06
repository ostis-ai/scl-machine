/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_template.hpp>

#include <inference/types.hpp>

using ReplacementsHashes = std::unordered_map<size_t, std::vector<size_t>>;

namespace inference
{
class ReplacementsUtils
{
public:
  static void intersectReplacements(
      Replacements const & first,
      Replacements const & second,
      Replacements & intersection);
  static void uniteReplacements(Replacements const & first, Replacements const & second, Replacements & unionResult);
  static void subtractReplacements(Replacements const & first, Replacements const & second, Replacements & difference);
  static Replacements removeRows(Replacements const & replacements, ScAddrUnorderedSet & keysToRemove);
  static void getReplacementsToScTemplateParams(
      Replacements const & replacements,
      std::vector<ScTemplateParams> & templateParams);
  static size_t getColumnsAmount(Replacements const & replacements);
  static void getKeySet(Replacements const & map, ScAddrUnorderedSet & keySet);

private:
  static void getCommonKeys(
      ScAddrUnorderedSet const & first,
      ScAddrUnorderedSet const & second,
      ScAddrUnorderedSet & commonKeys);
  static Replacements copyReplacements(Replacements const & replacements);
  static void removeDuplicateColumns(Replacements & replacements);
  static void calculateHashesForCommonKeys(
      Replacements const & replacements,
      ScAddrUnorderedSet const & commonKeys,
      ReplacementsHashes & hashes);
};

}  // namespace inference
