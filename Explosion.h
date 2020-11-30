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
    int size;
    int xPos;
    int yPos;

    Explosion(int, int, int, int, int, int);
    void createExplosion(int, int, int);

    void updateExplosion();

    int generateRandomNumber(int, int);

private:


};



#endif //SFML_TEMPLATE_EXPLOSION_H
