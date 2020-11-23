//
// Created by Kurtis Quant on 11/22/20.
//

#ifndef SFML_TEMPLATE_PARTICLE_H
#define SFML_TEMPLATE_PARTICLE_H
#include <SFML/Graphics.hpp>
using namespace sf;

class Particle {
private:

public:
    RectangleShape particle;
    double velocity;
    int angle;
    int life;
    bool dead;


    Particle(double,int,int,int,int);

    double getVelocity();
    void setVelocity(double);

    int getAngle();
    void setAngle(int);

    int getLife();
    void setLife(int);

    bool getDead();
    void setDead(bool);

    void update();
};


#endif //SFML_TEMPLATE_PARTICLE_H
