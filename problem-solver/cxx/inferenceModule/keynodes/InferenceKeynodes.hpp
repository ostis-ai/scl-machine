/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once
#include "sc-memory/sc_keynodes.hpp"

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_object.hpp>

namespace inference
{
class InferenceKeynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_direct_inference{"action_direct_inference"};

  static inline ScKeynode const concept_solution{"concept_solution"};

  static inline ScKeynode const concept_success_solution{"concept_success_solution"};

  static inline ScKeynode const concept_template_with_links{"concept_template_with_links"};

  static inline ScKeynode const concept_template_for_generation{"concept_template_for_generation"};

  static inline ScKeynode const atomic_logical_formula{"atomic_logical_formula"};

  static inline ScKeynode const nrel_disjunction{"nrel_disjunction"};

  static inline ScKeynode const nrel_conjunction{"nrel_conjunction"};

  static inline ScKeynode const nrel_negation{"nrel_negation"};

  static inline ScKeynode const nrel_implication{"nrel_implication"};

  static inline ScKeynode const nrel_equivalence{"nrel_equivalence"};

  static inline ScKeynode const rrel_if{"rrel_if"};

  static inline ScKeynode const rrel_then{"rrel_then"};

  static inline ScKeynode const nrel_output_structure{"nrel_output_structure"};
};

}  // namespace inference
