#ifndef HARMONIC_BOND_CLASS_H
#define HARMONIC_BOND_CLASS_H
#include"Bond.hpp"
/*
The definition of the concrete classes.
only possible to inherit public from abstact class!
*/

// Harmonic bond
namespace Bond {
class Harmonic : public Bond {
public: 
  Harmonic(double k_i, double r_i, double r_cut_i) : m_k{k_i}, m_r{r_i}, m_r_cut{r_cut_i}{}
    // Member function
  int add_bonded_force(Particle *p1, Particle *p2, double dx[3], double force[3]) const override;
  int add_bonded_energy(Particle *p1, Particle *p2, double dx[3], double *_energy) const override;
  //bond parameters
private:
  double m_k;
  double m_r;
  double m_r_cut;
};
}
#endif