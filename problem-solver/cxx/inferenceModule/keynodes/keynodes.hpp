/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/cpp/sc_addr.hpp"
#include "sc-memory/cpp/sc_object.hpp"

#include "keynodes.generated.hpp"

namespace exampleModule
{

class InferenceKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("rrel_solver_key_sc_element"), ForceCreate)
  static ScAddr rrel_solver_key_sc_element;
};

} // namespace inferenceModule
