#include "AddrComparator.hpp"

bool AddrComparator::operator()(const ScAddr& a, const ScAddr& b) const {
  return (a.GetRealAddr().seg != b.GetRealAddr().seg) ? (a.GetRealAddr().seg < b.GetRealAddr().seg) : (a.GetRealAddr().offset < b.GetRealAddr().offset);
}
