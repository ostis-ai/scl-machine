/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-memory/cpp/sc_addr.hpp>

class AddrComparator {
public:
  bool operator()(const ScAddr& a, const ScAddr& b) const;
};
