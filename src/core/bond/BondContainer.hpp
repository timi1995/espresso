#ifndef BOND_CONTAINER_H
#define BOND_CONTAINER_H
#include <unordered_map>
#include <memory>
#include "Bond.hpp"
#include "PairBond.hpp"
#include "ThreeParticlePressureBond.hpp"
#include "OifGlobalForces.hpp"

namespace Bond {

  class BondContainer {
  public:
    //constructor
    BondContainer() = default;

    // insert bond in all bonds, energy and force bonds
    void set_bond_by_type(int type, std::unique_ptr<Bond> && bond);
    // bonds with force and energy contribution
    int force_loop(Particle *p1);
    int energy_loop(Particle *p1);
    // pair bonds
    int virial_loop(Particle *p1);
    // three body pressure bonds
    int three_body_stress_loop(Particle *p1);
    // pair bonds
    int local_stress_tensor_loop(Particle *p1, DoubleList *TensorInBin, int bins[3]
						   ,double range_start[3], double range[3]);
    //oif_global_forces
    int oif_global_loop(Particle *p1, double* partArea, double* VOL_partVol);
    int oif_global_force_loop(Particle *p1);
    
  private:
    //---member variables---
    //unordered unique prt
    std::unordered_map<int, std::unique_ptr<Bond> > m_all_bonds;
    //normale pointer mit .get
    std::unordered_map<int, Bond* > m_force_loop_bonds;
    std::unordered_map<int, Bond* > m_energy_loop_bonds;
    std::unordered_map<int, PairBond*> m_virial_loop_bonds;
    std::unordered_map<int, ThreeParticlePressureBond*> m_three_body_pressure_bonds;
    std::unordered_map<int, OifGlobalForces*> m_oif_global_forces_bonds;
    //---private functions---
    // sorts a Bond of type Bond
    //-> PairBonds, ThreeParticleBonds, etc.
    void sort_bond_into_lists(int type);
    //template function has to be in header file of class!
    //template function loops over all bonds which corresponds to an interface class 
    //that provides a specific function which has to be called
    //->InterFaceClass: class which provides the interface function
    //->InterFaceClassFunction interface_class_func: function which is called for each bond
    //->interface_class_bond_map: map that contains all interface classes
    //-> Args: additional arguments of interface function
    //InterFaceFunction must have the following two arguments: p1, bond_map_id
    template<class InterfaceClass, typename InterFaceClassFunction, typename ... Args>
    int loop_over_bond_partners(std::unordered_map<int, InterfaceClass*> interface_class_bond_map,
				InterFaceClassFunction interface_class_func,
				Particle *p1, Args &&... args)
    {

      // variables
      // bond_list_id: id of bond_map_id in p1->bl.e[]
      // bond_map_id: id of bond in bond_map
      // n_partners: number of partners, e.g. for a pair bond -> 1
      int i, bond_map_id, bond_list_id, n_partners, bond_broken;
      i = 0;

      //We are going through the bond list of particle p1
      while(i < p1->bl.n){


	//--first assign the variables for this step--
	//get the bond id in bond list
	bond_list_id = i;
	//get the bond map id
	bond_map_id = p1->bl.e[bond_list_id];

	//now the number of partners can be determined
	//try to find bond
	if(m_all_bonds.count(bond_map_id)==0){
	  runtimeErrorMsg() << "BondContainer - Loop: bond type of atom "
			    << p1->p.identity << " unknown\n";
	  return 1;
	};
    
	// if there is a type, get the number of bond partners
	n_partners = m_all_bonds[bond_map_id]->get_number_of_bond_partners();

	//try to find bond in specific map for desired interface classes
	if(interface_class_bond_map.count(bond_map_id)==0){
	  i += n_partners + 1;
	  continue;
	}
    
	//--Then call desired function in interface--
	bond_broken = (interface_class_bond_map[bond_map_id]->*interface_class_func)
	  (p1, bond_list_id, std::forward<Args>(args)...);
	// if there are no bond partners return
	if(bond_broken == 2)
	  return bond_broken;

	//--Now we are finished and have to go to the next bond--
	//in bond list: bond itself and number of partners 
	//next bond in -> m_partners + 1
	i += n_partners + 1;
    
      };

      return 0;

    }

  };

}
#endif