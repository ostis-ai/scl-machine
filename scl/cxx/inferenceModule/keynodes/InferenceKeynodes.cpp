/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "InferenceKeynodes.hpp"

#include <sc-memory/sc_memory.hpp>

namespace inference
{

ScAddr InferenceKeynodes::action_use_logic_rule;
ScAddr InferenceKeynodes::action_reverse_inference;
ScAddr InferenceKeynodes::action_direct_inference;
ScAddr InferenceKeynodes::concept_solution;
ScAddr InferenceKeynodes::concept_success_solution;
ScAddr InferenceKeynodes::rrel_solver_key_sc_element;
ScAddr InferenceKeynodes::concept_template_with_links;
ScAddr InferenceKeynodes::concept_template_for_generation;
ScAddr InferenceKeynodes::concept_satisfiable_formula;
ScAddr InferenceKeynodes::nrel_basic_sequence;
ScAddr InferenceKeynodes::atomic_logical_formula;
ScAddr InferenceKeynodes::nrel_disjunction;
ScAddr InferenceKeynodes::nrel_conjunction;
ScAddr InferenceKeynodes::nrel_negation;
ScAddr InferenceKeynodes::nrel_implication;
ScAddr InferenceKeynodes::nrel_equivalence;
ScAddr InferenceKeynodes::rrel_main_key_sc_element;

}
