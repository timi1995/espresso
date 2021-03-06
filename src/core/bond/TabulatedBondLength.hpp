#ifndef TABULATED_BOND_LENGTH_BOND_CLASS_H
#define TABULATED_BOND_LENGTH_BOND_CLASS_H
#include "PairBond.hpp"
#include "Tabulated.hpp"
#include "CutoffBond.hpp"

namespace Bond {
  class TabulatedBondLength : public PairBond, public Tabulated, public CutoffBond {
  public: 
    //constructor
    //using Tabulated::Tabulated; //inherit constructor
    TabulatedBondLength(double min, double max, std::vector<double> const &energy,
			std::vector<double> const &force) : 
      Tabulated{min, max, energy, force, TabulatedBondedInteraction::TAB_BOND_LENGTH},
      CutoffBond(max)
    {m_bondtype = BondType::BONDED_IA_TABULATED;}
    // Member function
    int calc_bonded_pair_force(Particle *p1, Particle *p2, double dx[3], 
			      double force[3]) const override;
    int calc_bonded_pair_energy(Particle *p1, Particle *p2, 
			       double dx[3], double *_energy) const override;
    
    boost::any get_bond_parameters_from_bond() const override;
    
  };
}

#endif
