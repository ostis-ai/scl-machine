/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <vector>
#include <queue>

#include <sc-memory/sc_addr.hpp>

namespace inference
{
class ContainersUtils
{
public:
  static void addToQueue(ScAddrVector const & elementList, std::queue<ScAddr> & targetQueue);
};

}  // namespace inference
