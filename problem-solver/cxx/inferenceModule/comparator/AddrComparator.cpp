/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "AddrComparator.hpp"

bool AddrComparator::operator()(const ScAddr & a, const ScAddr & b) const
{
  return (a.GetRealAddr().seg != b.GetRealAddr().seg) ? (a.GetRealAddr().seg < b.GetRealAddr().seg)
                                                      : (a.GetRealAddr().offset < b.GetRealAddr().offset);
}
