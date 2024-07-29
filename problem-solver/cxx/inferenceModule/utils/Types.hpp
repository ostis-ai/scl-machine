#pragma once

#include <unordered_map>
#include <unordered_set>

#include <sc-memory/sc_addr.hpp>

namespace inference
{
using Replacements = std::unordered_map<ScAddr, ScAddrVector, ScAddrHashFunc>;
}  // namespace inference
