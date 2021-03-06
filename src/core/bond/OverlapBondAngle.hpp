#ifndef OVERLAP_BOND_ANGLE_BOND_CLASS_H
#define OVERLAP_BOND_ANGLE_BOND_CLASS_H
#include "ThreeParticleBond.hpp"
#include "Overlap.hpp"

namespace Bond {
  class OverlapBondAngle : public ThreeParticleBond, public Overlap {
  public:
    // constructor
    OverlapBondAngle(std::string filename, double maxval, int noverlaps,  std::vector<double> para_a,
		     std::vector<double> para_b, std::vector<double> para_c) : 
      Overlap{filename, OverlappedBondedInteraction::OVERLAP_BOND_ANGLE,maxval, noverlaps, para_a,
	para_b, para_c} 
    {m_bondtype = BondType::BONDED_IA_OVERLAPPED;}

    //force *
    int calc_bonded_three_particle_force(Particle *p1, Particle *p2, Particle *p3, double force[3], double force2[3]) const override;
    //energy *
    int calc_bonded_three_particle_energy(Particle *p1, Particle *p2, Particle *p3, 
						 double *_energy) const override;
  };
}
#endif
