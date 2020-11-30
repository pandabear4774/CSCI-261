//
// Created by Kurtis Quant on 11/22/20.
//

#include "Particle.h"
#include <SFML/Graphics.hpp>

using namespace std;

//constructor
Particle::Particle(double velocity, int angle, int life, int x, int y, int size){
    //sets the class variables to the parameter variables
    this -> velocity = velocity;
    this -> angle = (angle + 360) % 360;
    this -> life = life;

    //particles start alive
    dead = false;

    //sets the position and the size of the particles
    particle.setPosition(x,y);
    particle.setSize(Vector2f(size,size));
}

//updates the particle
void Particle::update(){
    //checks to see if it's dead
    if(!dead){
        //calculates the X Velocity
        double changeX = velocity * cos(angle * M_PI  / 180);
        if(angle > 90 && angle < 270){
            changeX = -abs(changeX);
        } else {
            changeX = abs(changeX);
        }

        //calculates the Y Velocity
        double changeY = velocity * sin(angle * M_PI / 180);
        if(angle > 0 && angle < 180){
            changeY = abs(changeY);
        } else {
            changeY = -abs(changeY);
        }

        //move the particle by the x and y velocity
        particle.move(changeX,changeY);

        //decrease the life
        life--;

        //if it's life is over then the particle is dead
        if(life <= 0){
            dead = true;
        }
    }
}