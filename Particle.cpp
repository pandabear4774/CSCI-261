//
// Created by Kurtis Quant on 11/22/20.
//

#include "Particle.h"
#include <SFML/Graphics.hpp>

using namespace std;

Particle::Particle(double velocity, int angle, int life, int x, int y){
    this -> velocity = velocity;
    this -> angle = (angle + 360) % 360;
    this -> life = life;
    dead = false;
    particle.setPosition(x,y);
    particle.setSize(Vector2f(10,10));
}
double Particle::getVelocity(){
    return velocity;
}
void Particle::setVelocity(double velocity){
    this -> velocity = velocity;
}
int Particle::getAngle(){
    return angle;
}
void Particle::setAngle(int angle){
    this -> angle = angle;
}
int Particle::getLife(){
    return life;
}
void Particle::setLife(int life){
    this -> life = life;
}
bool Particle::getDead(){
    return dead;
}
void Particle::setDead(bool dead){
    this -> dead = dead;
}
void Particle::update(){
    if(!dead){
        double changeX = velocity * cos(angle * M_PI  / 180);
        if(angle > 90 && angle < 270){
            changeX = -abs(changeX);
        } else {
            changeX = abs(changeX);
        }
        double changeY = velocity * sin(angle * M_PI / 180);
        if(angle > 0 && angle < 180){
            changeY = abs(changeY);
        } else {
            changeY = -abs(changeY);
        }
        particle.move(changeX,changeY);
        life--;
        if(life <= 0){
            setDead(true);
        }
    }
}