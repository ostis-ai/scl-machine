/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "ComparatorByNesting.hpp"

using namespace std;
using namespace rulesApplicationModule;

bool ComparatorByNesting::operator()(pair<ScAddr, std::size_t> const & a, pair<ScAddr, std::size_t> const & b) const
{
  return a.second < b.second;
}
