//
// Created by Kurtis Quant on 11/22/20.
//

#ifndef SFML_TEMPLATE_EXPLOSION_H
#define SFML_TEMPLATE_EXPLOSION_H
#include "Particle.h"
#include <vector>
using namespace std;

class Explosion {
public:
    vector<Particle> particles;

    Explosion(int, int, int, int, int, int);

    int getSize();

    void updateExplosion();

    int generateRandomNumber(int, int);

private:
    int size;
};



#endif //SFML_TEMPLATE_EXPLOSION_H
