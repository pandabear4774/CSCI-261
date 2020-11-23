//
// Created by Kurtis Quant on 11/22/20.
//

#include "Explosion.h"
#include "Particle.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
using namespace std;
using namespace sf;

Explosion::Explosion(int xPos, int yPos, int angleLow, int angleHigh){
    this -> xPos = xPos;
    this -> yPos = yPos;

    size = generateRandomNumber(10,20);
    for(int i = 0; i < size; i++){
        double velocity = 1 + rand() % 200 / 20.0;

        int angle;
        if(i%2 == 0){
            angle = generateRandomNumber(angleLow,(angleLow+angleHigh)/2);
        } else {
            angle = generateRandomNumber((angleLow+angleHigh)/2,angleHigh);
        }

        int life = 15 + rand()%30;

        particles.push_back(Particle(velocity,angle,life,xPos,yPos));
    }
}
int Explosion::generateRandomNumber(int low, int high){
    int random = low + rand()%(high-low);
    return random;
}

void Explosion::updateExplosion(){
    for(int i = 0; i < size; i++){
        particles.at(i).update();
    }
}