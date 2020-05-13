/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/cpp/sc_addr.hpp>
#include <sc-memory/cpp/sc_object.hpp>

#include "InferenceKeynodes.generated.hpp"

namespace inference
{

class InferenceKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("question_use_logic_rule"), ForceCreate)
  static ScAddr question_use_logic_rule;

  SC_PROPERTY(Keynode("question_reverse_inference"), ForceCreate)
  static ScAddr question_reverse_inference;

  SC_PROPERTY(Keynode("question_direct_inference"), ForceCreate)
  static ScAddr question_direct_inference;

  SC_PROPERTY(Keynode("solution"), ForceCreate)
  static ScAddr solution;

  SC_PROPERTY(Keynode("success_solution"), ForceCreate)
  static ScAddr success_solution;

  SC_PROPERTY(Keynode("rrel_solver_key_sc_element"), ForceCreate)
  static ScAddr rrel_solver_key_sc_element;

};

}