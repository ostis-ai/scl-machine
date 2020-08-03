/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "ContainersUtils.hpp"

using namespace std;

namespace inference
{

template<class T>
void ContainersUtils::addToQueue(vector<T> const & elementList, queue<T> & queue)
{
  for (auto element : elementList)
  {
    queue.push(element);
  }
}
}
