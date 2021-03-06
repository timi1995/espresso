/*
  Copyright (C) 2010,2011,2012,2013,2014,2015,2016 The ESPResSo project
  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
    Max-Planck-Institute for Polymer Research, Theory Group

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file tab.cpp
 *
 *  Implementation of \ref tab.hpp
 */
#include "tab.hpp"

#ifdef TABULATED
#include "communication.hpp"
#include "utils/make_unique.hpp" //for creating a unique ptr to a bond class object
#include "bond/TabulatedBondedInteraction.hpp" //for TabulatedBondedInteraction

int tabulated_set_params(int part_type_a, int part_type_b, double min,
                         double max, std::vector<double> const &energy,
                         std::vector<double> const &force) {
  
  auto data = get_ia_param_safe(part_type_a, part_type_b);
  assert(max >= min);
  assert((max == min) || force.size() > 1);
  assert(force.size() == energy.size());

  data->TAB.maxval = max;
  data->TAB.minval = min;
  data->TAB.invstepsize = static_cast<double>(force.size() - 1) / (max - min);

  data->TAB.force_tab = force;
  data->TAB.energy_tab = energy;

  mpi_bcast_ia_params(part_type_a, part_type_b);

  return 0;
}

int tabulated_bonded_set_params(int bond_type,
                                TabulatedBondedInteraction tab_type, double min,
                                double max, std::vector<double> const &energy,
                                std::vector<double> const &force) {
  if (bond_type < 0)
    return 1;

  assert(max >= min);
  assert((max == min) || force.size() > 1);
  assert(force.size() == energy.size());

  switch(tab_type){
  case TAB_BOND_LENGTH:
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondLength>
				    (min, max, energy, force));
    break;
  case TAB_BOND_ANGLE:
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondAngle>
				    (min, max, energy, force));
    break;
  case TAB_BOND_DIHEDRAL:
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondDihedral>
				    (min, max, energy, force));
    break;
  default:
    runtimeError("Unsupported tabulated bond type.");
    return 1;
  };

  return ES_OK;

}

#endif
