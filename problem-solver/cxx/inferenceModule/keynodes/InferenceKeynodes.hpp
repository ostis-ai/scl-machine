/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>

#include "InferenceKeynodes.generated.hpp"

namespace inference
{
class InferenceKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_direct_inference"), ForceCreate)
  static ScAddr action_direct_inference;

  SC_PROPERTY(Keynode("concept_solution"), ForceCreate)
  static ScAddr concept_solution;

  SC_PROPERTY(Keynode("concept_success_solution"), ForceCreate)
  static ScAddr concept_success_solution;

  SC_PROPERTY(Keynode("concept_template_with_links"), ForceCreate)
  static ScAddr concept_template_with_links;

  SC_PROPERTY(Keynode("concept_template_for_generation"), ForceCreate)
  static ScAddr concept_template_for_generation;

  SC_PROPERTY(Keynode("atomic_logical_formula"), ForceCreate)
  static ScAddr atomic_logical_formula;

  SC_PROPERTY(Keynode("nrel_disjunction"), ForceCreate)
  static ScAddr nrel_disjunction;

  SC_PROPERTY(Keynode("nrel_conjunction"), ForceCreate)
  static ScAddr nrel_conjunction;

  SC_PROPERTY(Keynode("nrel_negation"), ForceCreate)
  static ScAddr nrel_negation;

  SC_PROPERTY(Keynode("nrel_implication"), ForceCreate)
  static ScAddr nrel_implication;

  SC_PROPERTY(Keynode("nrel_equivalence"), ForceCreate)
  static ScAddr nrel_equivalence;

  SC_PROPERTY(Keynode("rrel_if"), ForceCreate)
  static ScAddr rrel_if;

  SC_PROPERTY(Keynode("rrel_then"), ForceCreate)
  static ScAddr rrel_then;

  SC_PROPERTY(Keynode("nrel_output_structure"), ForceCreate)
  static ScAddr nrel_output_structure;
};

}  // namespace inference
