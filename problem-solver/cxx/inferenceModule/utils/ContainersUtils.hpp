/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <string>
#include <vector>
#include <queue>

using namespace std;

namespace inference
{

class ContainersUtils
{
public:

  template<typename T>
  static void addToQueue(vector<T> const & elementList, queue<T> & queue);
};

}
