#ifndef FOUR_PARTICLE_BOND_H
#define FOUR_PARTICLE_BOND_H
#include "Bond.hpp"

namespace Bond {

  class FourParticleBond : public Bond {
  public:
    FourParticleBond() : Bond(3) {}
    virtual ~FourParticleBond() = default;

    // new virtual methods for three particle bond calculation implemented
    // in concrete classes
    // force calculation
    virtual int calc_bonded_four_particle_force(Particle *p1, Particle *p2, Particle *p3, 
					       Particle *p4, double force[3], double force2[3], 
					       double force3[3], double force4[3]) const=0;
    //energy calculation
    virtual int calc_bonded_four_particle_energy(Particle *p1, Particle *p2, Particle *p3, 
						 Particle *p4, double *_energy) const=0;

    // write forces to particles
    // there is a default implementation of this function
    // but it can be overwritten, because there are forces, which
    // sum up the forces in a different way
    virtual void write_force_to_particle(Particle *p1, Particle *p2, Particle *p3, 
					 Particle *p4, double force[3], double force2[3],
					 double force3[3], double force4[3]) const;

    // general bond calculation functions of abstract class
    // p1: particle, bl_id: id number of bond in bl.e
    // return value: 0: ok, 1: bond broken, 2: return from "add_bonded_force" in forces_inline.cpp
    int add_bonded_force(Particle *p1, int bl_id) override;
    int add_bonded_energy(Particle *p1, int bl_id) override;


  };

}

#endif
