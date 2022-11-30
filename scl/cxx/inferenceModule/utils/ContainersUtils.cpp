/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ContainersUtils.hpp"

namespace inference
{
void ContainersUtils::addToQueue(ScAddrVector const & elementList, std::queue<ScAddr> & targetQueue)
{
  for (ScAddr const & element : elementList)
  {
    targetQueue.push(element);
  }
}
}  // namespace inference
