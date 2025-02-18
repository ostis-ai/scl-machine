/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "inference/containers_utils.hpp"

namespace inference
{
void ContainersUtils::AddToQueue(ScAddrVector const & elementList, std::queue<ScAddr> & targetQueue)
{
  for (ScAddr const & element : elementList)
  {
    targetQueue.push(element);
  }
}
}  // namespace inference
