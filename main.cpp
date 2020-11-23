#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <ctime>
#include <string>
#include "Explosion.h"
#include <fstream>
#include "Particle.h"

using namespace std;
using namespace sf;

struct barrier{
    int life;
    RectangleShape b;
};
struct powerUp{
    int life;
    RectangleShape p;
};
struct explode{
    RectangleShape particles[30];
    double velocity[30];
    double angle[30];
    int life[30];
    bool dead[30];
};
//global variables used in the project
const int SCREEN_X = 640*3;
const int SCREEN_Y = 640*3;
int fontSize = SCREEN_X/10;
int wallWidth = SCREEN_X/20;
int wallHeight = SCREEN_Y/5;

Texture texture;
RenderWindow window(VideoMode(SCREEN_X, SCREEN_Y), "PONG");
RectangleShape r1;
RectangleShape r2;
RectangleShape ball;
Text text;
double ballVelocityX;
double ballVelocityY;
double velocityMax = SCREEN_X / 100;
int ballWidth = SCREEN_X / 20;
int ballHeight = SCREEN_X / 20;
int score;
int finalScore;
string gameState = "mainMenu";

vector<barrier> barriers;
vector<powerUp> powerUps;
//vector<explode> explosions;
vector<Explosion> explosions;


int generateRandomNumber(int low, int high);

void createPowerUp();

void createWall();

void move();

void setup();

bool hitBarrierX(int,int,int,int);

bool hitBarrierY(int,int,int,int);

void endGame();

void keyListeners();

void updateScore(int);

int main(){
    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "ERROR";
    }

    ifstream highScoreReader ("HighScore.txt");
    if(highScoreReader.fail()){
        cout << "Could not open file reader" << endl;
    }
    int currentHighScore;
    highScoreReader >> currentHighScore;
    highScoreReader.close();

    Text highScoreText;
    string highScoreTextString = "Best: " + to_string(currentHighScore);
    highScoreText.setString(highScoreTextString);
    highScoreText.setFont(font);
    highScoreText.setFillColor(Color::White);
    highScoreText.setCharacterSize(SCREEN_X/20);
    int lengthHighScore = highScoreTextString.length();
    //cout << lengthHighScore;
    highScoreText.setPosition(SCREEN_X/2 - lengthHighScore/2.0 * SCREEN_X/40,SCREEN_Y/4 - SCREEN_Y/40);

    RectangleShape startButton;
    startButton.setSize(Vector2f(SCREEN_X/4,SCREEN_Y/10));
    startButton.setPosition(SCREEN_X/2 - startButton.getSize().x/2, SCREEN_Y/2 - startButton.getSize().y/2);
    startButton.setFillColor(Color::Green);
    startButton.setOutlineThickness(5);
    startButton.setOutlineColor(Color::White);
    Text startButtonText;
    string startButtonTextString = "PLAY";
    startButtonText.setString(startButtonTextString);
    startButtonText.setFont(font);
    startButtonText.setFillColor(Color::Black);
    startButtonText.setCharacterSize(SCREEN_X/20);
    int length = startButtonTextString.length();
    //cout << length;
    startButtonText.setPosition(SCREEN_X/2 - length/2.0 * SCREEN_X/40,SCREEN_Y/2 - SCREEN_Y/40);

    RectangleShape playAgainButton;
    playAgainButton.setSize(Vector2f(SCREEN_X/4,SCREEN_Y/10));
    playAgainButton.setPosition(SCREEN_X/2 - startButton.getSize().x/2, SCREEN_Y/2 - startButton.getSize().y/2);
    playAgainButton.setFillColor(Color::Green);
    playAgainButton.setOutlineThickness(5);
    playAgainButton.setOutlineColor(Color::White);
    Text playAgainButtonText;
    string playAgainButtonTextString = "PLAY AGAIN";
    playAgainButtonText.setString(playAgainButtonTextString);
    playAgainButtonText.setFont(font);
    playAgainButtonText.setFillColor(Color::Black);
    playAgainButtonText.setCharacterSize(SCREEN_X/30);
    int l = playAgainButtonTextString.length();
    playAgainButtonText.setPosition(SCREEN_X/2 - l/2.0 * SCREEN_X/60,SCREEN_Y/2 - SCREEN_Y/60);


    //initializes all the values of stuff
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);


    text.setCharacterSize(fontSize);
    text.setFont(font);
    text.setPosition(SCREEN_X/2 - fontSize /2, 100 + fontSize / 2);

    while (window.isOpen()){
        //stuff every graphics thing has to make it show
        Event event;

        while (window.pollEvent(event)){

            if (event.type == Event::Closed){
                window.close();
            } else if (event.type == Event::MouseButtonPressed){
                Vector2i position = Mouse::getPosition(window);
                //cout << position.x << endl;
                //cout << position.y << endl;
                if(position.x >= startButton.getPosition().x && position.x <= startButton.getPosition().x + startButton.getSize().x){
                    if(position.y >= startButton.getPosition().y && position.y <= startButton.getPosition().y + startButton.getSize().y){
                        setup();
                        gameState = "game";
                    }
                }
            }
        }
        if(gameState == "mainMenu"){
            window.draw(startButton);
            window.draw(startButtonText);
            window.draw(highScoreText);
        } else if(gameState == "game"){
            keyListeners();
            move();
            //determines what key is being pressed
            if(hitBarrierX(r2.getPosition().x, r2.getPosition().y, r2.getSize().x, r2.getSize().y)){
                ballVelocityX *= -1;
                explosions.push_back(Explosion(ball.getPosition().x,ball.getPosition().y,90,270));
                if(ballVelocityX > 0 && ballVelocityX < velocityMax){
                    ballVelocityX += SCREEN_X/2000.0;
                } else if(ballVelocityX > -velocityMax){
                    ballVelocityX -= SCREEN_X/2000.0;
                }
                if(ballVelocityY > 0 && ballVelocityY < velocityMax){
                    ballVelocityY += SCREEN_Y/2000.0;
                } else if(ballVelocityY > -velocityMax){
                    ballVelocityY -= SCREEN_Y/2000.0;
                }
                //cout << "SPEED: " << ballVelocityX << " " << ballVelocityY << endl;
                createWall();
                if(score == 0 || score % 25 == 0){
                    createPowerUp();
                }
                score++;
                updateScore(1);
            }
            if(hitBarrierY(r2.getPosition().x, r2.getPosition().y, r2.getSize().x, r2.getSize().y))
            {
                ballVelocityY *= -1;
                endGame();
            }
            if(hitBarrierX(r1.getPosition().x, r1.getPosition().y, r1.getSize().x, r1.getSize().y))
            {
                ballVelocityX *= -1;
                explosions.push_back(Explosion(ball.getPosition().x,ball.getPosition().y,270,450));
                if(ballVelocityX > 0 && ballVelocityX < velocityMax){
                    ballVelocityX += SCREEN_X/2000.0;
                } else if(ballVelocityX > -velocityMax){
                    ballVelocityX -= SCREEN_X/2000.0;
                }
                if(ballVelocityY > 0 && ballVelocityY < velocityMax){
                    ballVelocityY += SCREEN_Y/2000.0;
                } else if(ballVelocityY > -velocityMax){
                    ballVelocityY -= SCREEN_Y/2000.0;
                }
                //cout << "SPEED: " << ballVelocityX << " " << ballVelocityY << endl;
                createWall();
                if(score == 0 || score % 25 == 0){
                    createPowerUp();
                }
                score++;
                updateScore(1);
            }
            if(hitBarrierY(r1.getPosition().x, r1.getPosition().y, r1.getSize().x, r1.getSize().y))
            {
                ballVelocityY *= -1;
                endGame();
            }
            if(ball.getPosition().y <= 0 || ball.getPosition().y + ball.getSize().y >= SCREEN_Y){
                ballVelocityY *= -1;
            }
            if(ball.getPosition().x <= 0 || ball.getPosition().x + ball.getSize().x >= SCREEN_X){
                endGame();
            }
            for(int i = 0; i < barriers.size(); i++){
                if(hitBarrierX(barriers.at(i).b.getPosition().x, barriers.at(i).b.getPosition().y, barriers.at(i).b.getSize().x, barriers.at(i).b.getSize().y)){
                    ballVelocityX *= -1;
                }
                if(hitBarrierY(barriers.at(i).b.getPosition().x, barriers.at(i).b.getPosition().y, barriers.at(i).b.getSize().x, barriers.at(i).b.getSize().y)){
                    ballVelocityY *= -1;
                }
            }
            for(int i = 0; i < powerUps.size(); i++){
                if(hitBarrierX(powerUps.at(i).p.getPosition().x, powerUps.at(i).p.getPosition().y, powerUps.at(i).p.getSize().x, powerUps.at(i).p.getSize().y)){
                    updateScore(10);
                    powerUps.erase(powerUps.begin() + i );
                    createPowerUp();
                }
                if(hitBarrierY(powerUps.at(i).p.getPosition().x, powerUps.at(i).p.getPosition().y, powerUps.at(i).p.getSize().x, powerUps.at(i).p.getSize().y)){
                    updateScore(10);
                    powerUps.erase(powerUps.begin() + i );
                    createPowerUp();
                }
            }


            string scoreString = to_string(finalScore);
            text.setString(scoreString);
            text.setFillColor(sf::Color::White);
            r1.move(0,ballVelocityY);
            r2.move(0,ballVelocityY);
            for(int i = 0; i < explosions.size(); i++){
                explosions.at(i).updateExplosion();
            }

            for(int i = 0; i < powerUps.size(); i++){
                window.draw(powerUps.at(i).p);
            }
            window.draw(r2);
            window.draw(r1);
            window.draw(ball);

            for(int i = 0; i < explosions.size(); i++){
                for(int j = 0; j < explosions.at(i).size; j++){
                    if(!explosions.at(i).particles.at(j).dead)
                        window.draw(explosions.at(i).particles.at(j).particle);
                }
            }

            for(int i = 0; i < barriers.size(); i++){
                window.draw(barriers.at(i).b);
            }
            window.draw(text);
        } else if(gameState == "gameOver"){
            window.draw(playAgainButton);
            window.draw(playAgainButtonText);
            window.draw(highScoreText);
        }

        //animate stuff
        window.display();
        window.clear();
    }
}
void createPowerUp(){
    int xPosition = generateRandomNumber(SCREEN_X/5,SCREEN_X-SCREEN_X/5);
    int yPosition = generateRandomNumber(0,SCREEN_Y-SCREEN_Y/10);
    int sizeX = SCREEN_X/15;
    int sizeY = SCREEN_X/15;
    RectangleShape tempPowerUp;
    tempPowerUp.setPosition(Vector2f(xPosition,yPosition));
    tempPowerUp.setSize(Vector2f(sizeX,sizeY));
    int color[3] = {generateRandomNumber(0,100),generateRandomNumber(150,255), generateRandomNumber(0,100)};
    tempPowerUp.setFillColor(Color(color[0],color[1],color[2]));
    tempPowerUp.setOutlineThickness(5);
    tempPowerUp.setOutlineColor(Color::White);
    int life = 1800;
    powerUps.push_back({life, tempPowerUp});
}
void createWall(){
    int xPosition = generateRandomNumber(SCREEN_X/5,SCREEN_X-SCREEN_X/5);
    int yPosition = generateRandomNumber(0,SCREEN_Y-SCREEN_Y/10);
    int sizeX = ball.getSize().x;
    int lowerBound = SCREEN_Y/20;
    int upperBound = SCREEN_Y/10;
    int sizeY = generateRandomNumber(lowerBound,upperBound);
    RectangleShape tempWall;
    tempWall.setPosition(Vector2f(xPosition,yPosition));
    tempWall.setSize(Vector2f(sizeX,sizeY));
    tempWall.setOutlineThickness(5);
    tempWall.setOutlineColor(Color::Red);
    barriers.push_back({600,tempWall});
}
void endGame(){
    cout << "FINAL SCORE: " << finalScore << endl;
    gameState = "gameOver";
    ifstream highScoreReader ("HighScore.txt");
    if(highScoreReader.fail()){
        cout << "Could not open file reader" << endl;
    }
    int currentHighScore;
    highScoreReader >> currentHighScore;
    highScoreReader.close();

    if(finalScore > currentHighScore){
        ofstream highScoreWriter ("HighScore.txt");
        if(highScoreWriter.fail()){
            cout << "Could not open file writer" << endl;
        }
        highScoreWriter << finalScore;
        highScoreWriter.close();
    }


}


int generateRandomNumber(int low, int high){
    int random = low + rand()%(high-low);
    return random;
}
void keyListeners(){
    if (Keyboard::isKeyPressed(Keyboard::W)){
        if(r1.getPosition().y > 0){
            r1.move(0.0, -SCREEN_Y/100);
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::S)){
        if(r1.getPosition().y < SCREEN_Y - wallHeight){
            r1.move(0.0, SCREEN_Y/100);
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Up)){
        if(r2.getPosition().y > 0){
            r2.move(0.0, -SCREEN_Y/100);
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Down)){
        if(r2.getPosition().y < SCREEN_Y-wallHeight){
            r2.move(0.0, SCREEN_Y/100);
        }
    }
}
bool hitBarrierX(int xPos, int yPos, int xSize, int ySize){
    if(ball.getPosition().x + ball.getSize().x + ballVelocityX > xPos &&
       ball.getPosition().x + ballVelocityX < xPos + xSize &&
       ball.getPosition().y + ball.getSize().y > yPos &&
       ball.getPosition().y < yPos + ySize)
    {
        return true;
    }
    return false;
}
bool hitBarrierY(int xPos, int yPos, int xSize, int ySize){
    if(ball.getPosition().x + ball.getSize().x > xPos &&
       ball.getPosition().x < xPos + xSize &&
       ball.getPosition().y + ball.getSize().y + ballVelocityY > yPos &&
       ball.getPosition().y + ballVelocityY < yPos + ySize)
    {
        return true;
    }
    return false;
}
void move(){
    //changes the position by the velocity
    ball.move(ballVelocityX,ballVelocityY);
    int loopSize = barriers.size();
    for(int i = 0; i < loopSize; i++){
        barriers.at(i).life--;
        if(barriers.at(i).life <= 0){
            barriers.erase(barriers.begin() + i);
            loopSize = barriers.size();
            i = 0;
        }
    }
}

void setup(){
    score = 0;
    finalScore = 0;
    srand(time(0));
    rand();
    barriers.clear();
    powerUps.clear();
    explosions.clear();
    ballVelocityX = SCREEN_X / 200;
    ballVelocityY = SCREEN_Y / 200;
    //sets the initial size of the objects
    r1.setSize(Vector2f(wallWidth, wallHeight));
    r2.setSize(Vector2f(wallWidth, wallHeight));
    ball.setSize(Vector2f(ballWidth,ballHeight));

    //sets the initial position of the objects
    r1.setPosition(0, SCREEN_Y/2 - r1.getSize().y/2);
    r2.setPosition(SCREEN_X-wallWidth, SCREEN_Y/2 - r2.getSize().y/2);
    ball.setPosition(SCREEN_X/2,SCREEN_Y/2);

    r1.setFillColor(Color::Red);
    r2.setFillColor(Color::Blue);
    ball.setFillColor(Color::Yellow);

    r1.setOutlineThickness(5);
    r2.setOutlineThickness(5);
    ball.setOutlineThickness(5);

    r1.setOutlineColor(Color::White);
    r2.setOutlineColor(Color::White);
    ball.setOutlineColor(Color::White);

}
void updateScore(int scoreChange){
    string oldScore = to_string(finalScore);
    int scoreLength1 = oldScore.length();
    finalScore+= scoreChange;
    string newScore = to_string(finalScore);
    int scoreLength2 = newScore.length();
    if(scoreLength2 > scoreLength1){
        fontSize = SCREEN_X/ (10 * scoreLength2);
        cout << "FONT SIZE: " << fontSize << endl;
        text.setCharacterSize(fontSize);
        text.setPosition(SCREEN_X/2 - fontSize * scoreLength2 / 2, 100 + fontSize / 2);
    }
}