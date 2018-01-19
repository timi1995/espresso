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

  make_bond_type_exist(bond_type);

  /* set types */
  bonded_ia_params[bond_type].type = BONDED_IA_TABULATED;
  bonded_ia_params[bond_type].p.tab.type = tab_type;
  bonded_ia_params[bond_type].p.tab.pot = new TabulatedPotential;
  auto tab_pot = bonded_ia_params[bond_type].p.tab.pot;

  /* set number of interaction partners */
  if (tab_type == TAB_BOND_LENGTH) {
    tab_pot->minval = min;
    tab_pot->maxval = max;
    bonded_ia_params[bond_type].num = 1;
  } else if (tab_type == TAB_BOND_ANGLE) {
    tab_pot->minval = 0.0;
    tab_pot->maxval = PI + ROUND_ERROR_PREC;
    bonded_ia_params[bond_type].num = 2;
  } else if (tab_type == TAB_BOND_DIHEDRAL) {
    tab_pot->minval = 0.0;
    tab_pot->maxval = 2.0 * PI + ROUND_ERROR_PREC;
    bonded_ia_params[bond_type].num = 3;
  } else {
    runtimeError("Unsupported tabulated bond type.");
    return 1;
  }

  tab_pot->invstepsize = static_cast<double>(force.size() - 1) / (max - min);

  tab_pot->force_tab = force;
  tab_pot->energy_tab = energy;

  mpi_bcast_ia_params(bond_type, -1);


  /* Check interval for angle and dihedral potentials.  With adding
     ROUND_ERROR_PREC to the upper boundary we make sure, that during
     the calculation we do not leave the defined table!
  */
  if(tab_type == TAB_BOND_ANGLE ) {
    if( input_minval != 0.0 || 
	std::abs(input_maxval-PI) > 1e-5 ) {
      fclose(fp);
     return 6;
    }
    input_maxval = PI+ROUND_ERROR_PREC;
  }
  /* check interval for angle and dihedral potentials */
  if(tab_type == TAB_BOND_DIHEDRAL ) {
    if( input_minval != 0.0 || 
	std::abs(input_maxval-(2*PI)) > 1e-5 ) {
      fclose(fp);
      return 6;
    }
    input_maxval = (2*PI)+ROUND_ERROR_PREC;
  }

  /* calculate dependent parameters */
  input_invstepsize = (double)(size-1)/(input_maxval-input_minval);

  /* allocate force and energy tables */
  input_f = (double*)Utils::malloc(size*sizeof(double));
  input_e = (double*)Utils::malloc(size*sizeof(double));

  /* Read in the new force and energy table data */
  for (i =0 ; i < size ; i++) {
    if (fscanf(fp,"%lf %lf %lf", &dummr,
	       &input_f[i],
	       &input_e[i]) != 3){ 

      return 5;
    }
  }

  fclose(fp);
  /*End of NEW CODE */


  // this structure is just for testing the code with old iaparams stuff
  switch(tab_type){
  case TAB_BOND_LENGTH:    
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondLength>
		     (Bond::TabulatedBondedInteraction::TAB_BOND_LENGTH, input_filename,
		      input_minval, input_maxval, input_npoints, input_invstepsize, input_f, 
		      input_e));
  case TAB_BOND_ANGLE:    
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondAngle>
		     (Bond::TabulatedBondedInteraction::TAB_BOND_ANGLE, input_filename,
		      input_minval, input_maxval, input_npoints, input_invstepsize, input_f, 
		      input_e));
  case TAB_BOND_DIHEDRAL:    
    bond_container.set_bond_by_type(bond_type, Utils::make_unique<Bond::TabulatedBondDihedral>
		     (Bond::TabulatedBondedInteraction::TAB_BOND_DIHEDRAL, input_filename,
		      input_minval, input_maxval, input_npoints, input_invstepsize, input_f, 
		      input_e));
  default:
    runtimeError("Unsupported tabulated bond type.");
    return 1;
  };

  mpi_bcast_ia_params(bond_type, -1); 
  return ES_OK;

}

#endif
