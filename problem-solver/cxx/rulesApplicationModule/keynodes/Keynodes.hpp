/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "Keynodes.generated.hpp"

namespace rulesApplicationModule
{

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_deactivated"), ForceCreate)
  static ScAddr action_deactivated;

  SC_PROPERTY(Keynode("action_apply_rules_on_the_model"), ForceCreate)
  static ScAddr action_apply_rules_on_the_model;
  
  SC_PROPERTY(Keynode("nrel_equivalence"), ForceCreate)
  static ScAddr nrel_equivalence;

  SC_PROPERTY(Keynode("nrel_conjunction"), ForceCreate)
  static ScAddr nrel_conjunction;

  SC_PROPERTY(Keynode("nrel_action_possible_result"), ForceCreate)
  static ScAddr nrel_action_possible_result;

  SC_PROPERTY(Keynode("rrel_main_key_sc_element"), ForceCreate)
  static ScAddr rrel_main_key_sc_element;

  SC_PROPERTY(Keynode("concept_template_with_links"), ForceCreate)
  static ScAddr concept_template_with_links;

  SC_PROPERTY(Keynode("concept_current_model"), ForceCreate)
  static ScAddr concept_current_model;

  SC_PROPERTY(Keynode("concept_logic_statement_checking_by_action"), ForceCreate)
  static ScAddr concept_logic_statement_checking_by_action;
};

} // namespace rulesApplicationModule
