//
// Created by Kurtis Quant on 11/22/20.
//

#ifndef SFML_TEMPLATE_PARTICLE_H
#define SFML_TEMPLATE_PARTICLE_H
#include <SFML/Graphics.hpp>
using namespace sf;

class Particle {
private:
    double velocity;
    int angle;
    int life;
    bool dead;
public:
    RectangleShape particle;

    Particle(double, int, int, int, int, int);

    bool getDead();

    void update();
};


#endif //SFML_TEMPLATE_PARTICLE_H
