//
// Created by Kurtis Quant on 11/22/20.
//
#include "Explosion.h"
#include "Particle.h"
#include <vector>
using namespace std;
using namespace sf;

//constructor
Explosion::Explosion(int xPos, int yPos, int angleLow, int angleHigh, int size, int screenSize){
    //sets the variables in the class to the variables in the parameter
    this -> size = size;

    //creates multiple particles depending on the size parameter
    for(int i = 0; i < size; i++){
        //starting velocity of the particles
        double velocity = 1 + rand() % 200 / 20.0;

        //assigns an angle to the particle
        int angle;
        if(i % 2 == 0){
            angle = generateRandomNumber(angleLow,(angleLow+angleHigh)/2);
        } else {
            angle = generateRandomNumber((angleLow+angleHigh)/2,angleHigh);
        }

        //gives the particle a life time
        int life = 15 + rand() % 30;

        //give the particle a size
        int particleSize = generateRandomNumber(screenSize / 150, screenSize / 75);

        //pushes back a particle into the particles vector
        particles.push_back(Particle(velocity, angle, life, xPos, yPos, particleSize));
    }
}

//generates a random number between low and high parameteres
int Explosion::generateRandomNumber(int low, int high){
    int random = low + rand() % (high - low);
    return random;
}

//returns the size of the explosion
int Explosion::getSize(){
    return particles.size();
}
//update the particles in the explosion
void Explosion::updateExplosion(){
    //update every particle in the particles vector
    for(int i = 0; i < size; i++){
        particles.at(i).update();
    }
}