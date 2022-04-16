/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "TestKeynodes.generated.hpp"

namespace rulesApplicationModule
{

class TestKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_check_connection"), ForceCreate)
  static ScAddr action_check_connection;

  SC_PROPERTY(Keynode("nrel_connection"), ForceCreate)
  static ScAddr nrel_connection;

  SC_PROPERTY(Keynode("nrel_connector"), ForceCreate)
  static ScAddr nrel_connector;

  SC_PROPERTY(Keynode("concept_equipment"), ForceCreate)
  static ScAddr concept_equipment;

  SC_PROPERTY(Keynode("concept_topological_node"), ForceCreate)
  static ScAddr concept_topological_node;

  SC_PROPERTY(Keynode("nrel_connected_elements"), ForceCreate)
  static ScAddr nrel_connected_elements;

  SC_PROPERTY(Keynode("equipment_normaly_in_service"), ForceCreate)
  static ScAddr equipment_normaly_in_service;

  SC_PROPERTY(Keynode("power_supply"), ForceCreate)
  static ScAddr power_supply;

  SC_PROPERTY(Keynode("nrel_equipment_usage_points"), ForceCreate)
  static ScAddr nrel_equipment_usage_points;

  SC_PROPERTY(Keynode("cim_nrel_equipment_usage_point"), ForceCreate)
  static ScAddr cim_nrel_equipment_usage_point;

  SC_PROPERTY(Keynode("nrel_connectivity_nodes"), ForceCreate)
  static ScAddr nrel_connectivity_nodes;

  SC_PROPERTY(Keynode("cim_nrel_connectivity_node"), ForceCreate)
  static ScAddr cim_nrel_connectivity_node;

  SC_PROPERTY(Keynode("nrel_is_power_supply"), ForceCreate)
  static ScAddr nrel_is_power_supply;

  SC_PROPERTY(Keynode("nrel_equipment_normaly_in_service"), ForceCreate)
  static ScAddr nrel_equipment_normaly_in_service;

  SC_PROPERTY(Keynode("nrel_test_idtf"), ForceCreate)
  static ScAddr nrel_test_idtf;
};

} // namespace rulesApplicationModule
