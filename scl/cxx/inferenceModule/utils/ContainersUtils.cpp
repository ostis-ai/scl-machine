/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "ContainersUtils.hpp"

using namespace std;

namespace inference
{

void ContainersUtils::addToQueue(vector<ScAddr> const & elementList, queue<ScAddr> & targetQueue)
{
  for (auto element : elementList)
  {
    targetQueue.push(element);
  }
}
}  // namespace inference
