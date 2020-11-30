#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include "Explosion.h"
#include <fstream>

using namespace std;
using namespace sf;

//barrier struct is a wall object in the game
struct barrier{
    int life;
    RectangleShape b;
};

//power up struct is a coin object in the game
struct powerUp{
    Sprite coin;
};

//global variables
Font font;

//constant int variables do not change
const int SCREEN_X = 640;
const int SCREEN_Y = 640;
const int WALL_WIDTH  = SCREEN_X / 20;
const int WALL_HEIGHT = SCREEN_Y / 5;
const int BALL_WIDTH  = SCREEN_X / 20;
const int BALL_HEIGHT = SCREEN_X / 20;

//int variables
int fontSize = SCREEN_X / 10;
int score = 0;
int finalScore = 0;
int highScore = 0;
int gameTime = 0;

//constant double variable does not change
const double VELOCITY_MAX = SCREEN_X / 100;

//double variables
double ballVelocityX = 1.0;
double ballVelocityY = 1.0;

//render window will have the graphics stuff stores on it
RenderWindow window(VideoMode(SCREEN_X, SCREEN_Y), "PONG");

//all the rectangle shapes
RectangleShape leftWall;
RectangleShape rightWall;
RectangleShape ball;
RectangleShape startButton;
RectangleShape playAgainButton;
RectangleShape topWall;
RectangleShape bottomWall;

//stores the current game state or screen
string gameState = "mainMenu";

//contains the text that will be on the screen
Text text;
Text highScoreText;
Text startButtonText;
Text playAgainButtonText;

//contains the textures for the coin
Texture textures[9];

//the vectors that will store the game objects
vector<barrier> barriers;
vector<powerUp> powerUps;
vector<Explosion> explosions;

//functions used in main.cpp
void checkHit();

void createBarrier();

void createEndScreen();

void createMainMenu();

void createPowerUp();

void draw();

void endGame();

int generateRandomNumber(int low, int high);

bool hitBarrierX(int,int,int,int);

bool hitBarrierY(int,int,int,int);

void hitWall(int,int);

void keyListeners();

void loadFont();

void loadHighScore();

void loadTextures();

void move();

void setup();

void updateBarriers();

void updatePowerUp();

void updateScore(int);

int main(){
    //loads in coin texture
    loadTextures();

    //loads in arial font
    loadFont();

    //loads in previous high score
    loadHighScore();

    //creates the main menu and death menu screens
    createMainMenu();
    createEndScreen();

    //sets framerate
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(true);

    while (window.isOpen()){
        //stuff every graphics thing has to make it show
        Event event;

        while (window.pollEvent(event)){
            //closes window when red x is clicked
            if (event.type == Event::Closed){
                window.close();
            } else if (event.type == Event::MouseButtonPressed){
                Vector2i position = Mouse::getPosition(window);
                //checks to see if the start or play again button was clicked
                if(position.x >= startButton.getPosition().x && position.x <= startButton.getPosition().x + startButton.getSize().x){
                    if(position.y >= startButton.getPosition().y && position.y <= startButton.getPosition().y + startButton.getSize().y){
                        cout << gameState << endl;
                        if(gameState == "mainMenu" || gameState == "gameOver"){
                            setup();
                            gameState = "game";
                        }
                    }
                }
            }
        }
        //checks to see what the current game state is and then displays the screen for that game state
        if(gameState == "mainMenu"){
            window.draw(startButton);
            window.draw(startButtonText);
            window.draw(highScoreText);
        } else if(gameState == "game"){
            //moves the left and right walls
            keyListeners();

            //moves the ball
            move();

            //updates the barrier life span
            updateBarriers();

            //increments the game time
            gameTime++;

            //determines if the ball is going to hit a barrier or a wall
            checkHit();

            //makes new power ups and checks if a power up was hit by the ball
            updatePowerUp();

            //updates the score board
            string scoreString = to_string(finalScore);
            text.setString(scoreString);

            //updates the explosion particles
            for(int i = 0; i < explosions.size(); i++){
                explosions.at(i).updateExplosion();
            }

            //draws everything on the screen
            draw();
        } else if(gameState == "gameOver"){
            //draws the end screen
            window.draw(playAgainButton);
            window.draw(playAgainButtonText);
            window.draw(highScoreText);
        }

        //animate stuff
        window.display();
        window.clear();
    }
}
void checkHit(){
    //is the ball out of the screen
    if(ball.getPosition().x <= 0 || ball.getPosition().x + ball.getSize().x >= SCREEN_X){
        //go to the end screen
        endGame();
    }

    //if ball hit the top or bottom on the left or right wall end the game
    if(hitBarrierY(rightWall.getPosition().x, rightWall.getPosition().y, rightWall.getSize().x, rightWall.getSize().y) ||
       hitBarrierY( leftWall.getPosition().x,  leftWall.getPosition().y,  leftWall.getSize().x,  leftWall.getSize().y)){
        //bounce
        ballVelocityY *= -1;

        //end the game
        endGame();
    }

    //if the ball hit the left or right of the left or right wall make the ball bounce
    if(hitBarrierX(rightWall.getPosition().x, rightWall.getPosition().y, rightWall.getSize().x, rightWall.getSize().y)){
        //bounce and change ball speed
        hitWall(90,270);
    }
    if(hitBarrierX(leftWall.getPosition().x, leftWall.getPosition().y, leftWall.getSize().x, leftWall.getSize().y)){
        //bounce and change ball speed
        hitWall(270,450);
    }

    //if the ball hit the top of the screen
    if(ball.getPosition().y <= SCREEN_Y/50){
        //bounce
        ballVelocityY *= -1;

        //impact effect
        int explosionSize = generateRandomNumber(10,20);
        explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y, 0, 180, explosionSize, SCREEN_Y));
    }

    //if the ball hit the bottom of the screen
    if(ball.getPosition().y + ball.getSize().y >= SCREEN_Y - SCREEN_Y / 50){
        //bounce
        ballVelocityY *= -1;

        //generate impact effect
        int explosionSize = generateRandomNumber(10,20);
        explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y, 180, 360, explosionSize, SCREEN_Y));
    }

    //did it hit a barrier
    for(int i = 0; i < barriers.size(); i++){
        //checks if ball will hit the left or right side of a barrier
        if(hitBarrierX(barriers.at(i).b.getPosition().x, barriers.at(i).b.getPosition().y, barriers.at(i).b.getSize().x, barriers.at(i).b.getSize().y)){
            //bounce
            ballVelocityX *= -1;

            //impact effect
            int explosionSize = generateRandomNumber(10,20);
            if(ballVelocityX > 0){
                explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y, 270, 450, explosionSize, SCREEN_Y));
            } else {
                explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y,  90, 270, explosionSize, SCREEN_Y));
            }
        }
        //checks if ball will hit the top or bottom of a barrier
        if(hitBarrierY(barriers.at(i).b.getPosition().x, barriers.at(i).b.getPosition().y, barriers.at(i).b.getSize().x, barriers.at(i).b.getSize().y)){
            //bounce
            ballVelocityY *= -1;

            //impact effect
            int explosionSize = generateRandomNumber(10,20);
            if(ballVelocityY > 0){
                explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y, 0, 180, explosionSize, SCREEN_Y));
            } else {
                explosions.push_back(Explosion(ball.getPosition().x, ball.getPosition().y, 180, 360, explosionSize, SCREEN_Y));
            }
        }
    }

    //did it hit a power up
    for(int i = 0; i < powerUps.size(); i++){
        //checks every individual power up to see if it will be hit
        if(hitBarrierX(powerUps.at(i).coin.getPosition().x, powerUps.at(i).coin.getPosition().y, SCREEN_X / 15, SCREEN_Y / 15) ||
           hitBarrierY(powerUps.at(i).coin.getPosition().x, powerUps.at(i).coin.getPosition().y, SCREEN_X / 15, SCREEN_Y / 15)){
            //increase the score
            updateScore(10);

            //destroy the power up
            powerUps.erase(powerUps.begin() + i);

            //make a new power up
            createPowerUp();
        }
    }
}
void createBarrier(){
    //generate the size of the barrier
    int lowerBound = SCREEN_Y / 20;
    int upperBound = SCREEN_Y / 10;
    int sizeX = ball.getSize().x;
    int sizeY = generateRandomNumber(lowerBound,upperBound);

    //generate the position of the barrier
    int xPosition = generateRandomNumber(SCREEN_X / 5,SCREEN_X - SCREEN_X / 5);
    int yPosition = generateRandomNumber(SCREEN_Y / 50,SCREEN_Y - SCREEN_Y / 50 - sizeY);

    //create a temporary barrier object which will be passed into the barrier vector later on
    RectangleShape tempWall;
    tempWall.setPosition(Vector2f(xPosition, yPosition));
    tempWall.setSize(Vector2f(sizeX, sizeY));
    tempWall.setOutlineThickness(5);
    tempWall.setOutlineColor(Color::Red);

    //pass the temporary barrier object into the barrier vector
    barriers.push_back({600, tempWall});
}

void createEndScreen(){
    //creates the play again button
    playAgainButton.setSize(Vector2f(SCREEN_X / 4,SCREEN_Y / 10));
    playAgainButton.setPosition(SCREEN_X / 2 - startButton.getSize().x / 2, SCREEN_Y / 2 - startButton.getSize().y / 2);
    playAgainButton.setFillColor(Color::Green);
    playAgainButton.setOutlineThickness(5);
    playAgainButton.setOutlineColor(Color::White);

    //creates the text for the play again button
    string playAgainButtonTextString = "PLAY AGAIN";
    playAgainButtonText.setString(playAgainButtonTextString);
    playAgainButtonText.setFont(font);
    playAgainButtonText.setFillColor(Color::Black);
    playAgainButtonText.setCharacterSize(SCREEN_X / 30);
    int l = playAgainButtonTextString.length();
    playAgainButtonText.setPosition(SCREEN_X / 2 - l / 2.0 * SCREEN_X / 60,SCREEN_Y / 2 - SCREEN_Y / 60);
}

void createMainMenu(){
    //creates the high score Text which will be displayed at the top of the main menu and death menu
    string highScoreTextString = "Best: " + to_string(highScore);
    highScoreText.setString(highScoreTextString);
    highScoreText.setFont(font);
    highScoreText.setFillColor(Color::White);
    highScoreText.setCharacterSize(SCREEN_X/20);
    int lengthHighScore = highScoreTextString.length();
    highScoreText.setPosition(SCREEN_X / 2 - lengthHighScore / 2.0 * SCREEN_X / 40,SCREEN_Y / 4 - SCREEN_Y / 40);

    //creates the start button for the game
    startButton.setSize(Vector2f(SCREEN_X/4,SCREEN_Y/10));
    startButton.setPosition(SCREEN_X / 2 - startButton.getSize().x / 2, SCREEN_Y / 2 - startButton.getSize().y / 2);
    startButton.setFillColor(Color::Green);
    startButton.setOutlineThickness(5);
    startButton.setOutlineColor(Color::White);
    string startButtonTextString = "PLAY";
    startButtonText.setString(startButtonTextString);
    startButtonText.setFont(font);
    startButtonText.setFillColor(Color::Black);
    startButtonText.setCharacterSize(SCREEN_X / 20);
    int length = startButtonTextString.length();
    startButtonText.setPosition(SCREEN_X / 2 - length / 2.0 * SCREEN_X / 40,SCREEN_Y / 2 - SCREEN_Y / 40);
}
void createPowerUp(){
    //generates the location of the power up
    int xPosition = generateRandomNumber(SCREEN_X / 5,SCREEN_X - SCREEN_X / 5);
    int yPosition = generateRandomNumber(0,SCREEN_Y - SCREEN_Y / 10);

    //creates a temporary power up sprite which will be pushed back into the power up vector
    Sprite tempPowerUp;
    tempPowerUp.setPosition(Vector2f(xPosition, yPosition));
    tempPowerUp.setScale(Vector2f(SCREEN_X / 256,SCREEN_Y / 256));
    tempPowerUp.setTexture(textures[0]);

    //put the temporary power up sprite into the vector
    powerUps.push_back({tempPowerUp});
}

void draw(){
    //draws the explosions
    for(int i = 0; i < explosions.size(); i++){
        for(int j = 0; j < explosions.at(i).size; j++){
            if(!explosions.at(i).particles.at(j).dead)
                window.draw(explosions.at(i).particles.at(j).particle);
        }
    }

    //draws the barriers
    for(int i = 0; i < barriers.size(); i++){
        window.draw(barriers.at(i).b);
    }

    //draws the power ups
    for(int i = 0; i < powerUps.size(); i++){
        window.draw(powerUps.at(i).coin);
    }

    //draws the walls
    window.draw(rightWall);
    window.draw(leftWall);
    window.draw(topWall);
    window.draw(bottomWall);

    //draws the ball
    window.draw(ball);

    //draws the current score text
    window.draw(text);
}

//called when the game has ended
void endGame(){

    //prints the final score to the console
    cout << "FINAL SCORE: " << finalScore << endl;

    //changes the game state so that the end screen pops up
    gameState = "gameOver";

    //compares the current score to the high score
    ifstream highScoreReader ("HighScore.txt");
    if(highScoreReader.fail()){
        cout << "Could not open file reader" << endl;
    }
    int currentHighScore;
    highScoreReader >> currentHighScore;
    cout << "Current High Score: " << currentHighScore << endl;
    highScoreReader.close();

    //if the current score = greater than the high score then it will save the current score to the high score file
    if(finalScore > currentHighScore){
        currentHighScore = finalScore;

        ofstream highScoreWriter ("HighScore.txt");
        if(highScoreWriter.fail()){
            cout << "Could not open file writer" << endl;
        }
        highScoreWriter << finalScore;
        highScoreWriter.close();
    }

    string highScoreTextString = "Best: " + to_string(currentHighScore);
    highScoreText.setString(highScoreTextString);
}

//generates a random number between the low and high parameters
int generateRandomNumber(int low, int high){
    int random = low + rand() % (high - low);
    return random;
}

//checks if the ball hit the left or right side of a rectangle of position xPos and yPos with size xSize and ySize
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

//checks if the ball hit the top or bottom side of a rectangle of position xPos and yPos with size xSize and ySize
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

//called when the ball hits the left or the right wall
void hitWall(int lowAngle, int highAngle){
    //bounce
    ballVelocityX *= -1;

    //impact effect
    int explosionSize = generateRandomNumber(10,20);
    explosions.push_back(Explosion(ball.getPosition().x,ball.getPosition().y,lowAngle,highAngle, explosionSize, SCREEN_Y));

    //increases the ball's velocity as long as the velocity is less than the max velocity
    if(ballVelocityX > 0 && ballVelocityX < VELOCITY_MAX){
        ballVelocityX += SCREEN_X / 2000.0;
    } else if(ballVelocityX > -VELOCITY_MAX){
        ballVelocityX -= SCREEN_X / 2000.0;
    }
    if(ballVelocityY > 0 && ballVelocityY < VELOCITY_MAX){
        ballVelocityY += SCREEN_Y / 2000.0;
    } else if(ballVelocityY > -VELOCITY_MAX){
        ballVelocityY -= SCREEN_Y / 2000.0;
    }

    //adds another barrier to the barrier vector
    createBarrier();

    //creates a power up depending on the current score
    if(score == 0 || score % 25 == 0){
        createPowerUp();
    }

    //increases the current score
    score++;
    updateScore(1);
}

//checks to see if a key is pressed
void keyListeners(){
    //W key pressed then move the left wall up
    if (Keyboard::isKeyPressed(Keyboard::W)){
        if(leftWall.getPosition().y - 2 * abs(ballVelocityY) >= SCREEN_Y / 50){
            leftWall.move(0.0, -2 * abs(ballVelocityY));
        }
    }

    //S key pressed then move th left wall down
    if (Keyboard::isKeyPressed(Keyboard::S)){
        if(leftWall.getPosition().y + 2 * abs(ballVelocityY) <= SCREEN_Y - WALL_HEIGHT - SCREEN_Y / 50){
            leftWall.move(0.0, 2 * abs(ballVelocityY));
        }
    }

    //up key pressed then move the right wall up
    if (Keyboard::isKeyPressed(Keyboard::Up)){
        if(rightWall.getPosition().y - 2 * abs(ballVelocityY) >= SCREEN_Y / 50){
            rightWall.move(0.0, -2 * abs(ballVelocityY));
        }
    }

    //down key pressed then move the right wall down
    if (Keyboard::isKeyPressed(Keyboard::Down)){
        if(rightWall.getPosition().y + 2 * abs(ballVelocityY) <= SCREEN_Y - WALL_HEIGHT - SCREEN_Y / 50){
            rightWall.move(0.0, 2 * abs(ballVelocityY));
        }
    }
}

void loadFont(){
    //open font file
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "ERROR loading font";
    }

    //set the text of the in game score to the current font and change the settings of the Text
    text.setCharacterSize(SCREEN_X / 20);
    text.setFont(font);
    text.setFillColor(Color::Green);
    text.setPosition(SCREEN_X / 2 - fontSize / 2, 100 + fontSize / 2);
}

//open the high score file and put the contents into the highScore int
void loadHighScore(){
    ifstream highScoreReader ("HighScore.txt");
    if(highScoreReader.fail()){
        cout << "Could not open file reader" << endl;
    }
    highScoreReader >> highScore;
    highScoreReader.close();
}

//load in all the coin textures
void loadTextures() {
    if(!textures[0].loadFromFile("goldCoin1.png")){
        cout << "Fail loading goldCoin1 file" << endl;
    }
    if(!textures[1].loadFromFile("goldCoin2.png")){
        cout << "Fail loading goldCoin2 file" << endl;
    }
    if(!textures[2].loadFromFile("goldCoin3.png")){
        cout << "Fail loading goldCoin3 file" << endl;
    }
    if(!textures[3].loadFromFile("goldCoin4.png")){
        cout << "Fail loading goldCoin4 file" << endl;
    }
    if(!textures[4].loadFromFile("goldCoin5.png")){
        cout << "Fail loading goldCoin5 file" << endl;
    }
    if(!textures[5].loadFromFile("goldCoin6.png")){
        cout << "Fail loading goldCoin6 file" << endl;
    }
    if(!textures[6].loadFromFile("goldCoin7.png")){
        cout << "Fail loading goldCoin7 file" << endl;
    }
    if(!textures[7].loadFromFile("goldCoin8.png")){
        cout << "Fail loading goldCoin8 file" << endl;
    }
    if(!textures[8].loadFromFile("goldCoin9.png")){
        cout << "Fail loading goldCoin9 file" << endl;
    }
}

//move the ball
void move(){
    //changes the position by the velocity
    ball.move(ballVelocityX,ballVelocityY);
}

//sets up the game by initializing variables
void setup(){
    //used to make random variables
    srand(time(0));
    rand();

    //current time set to 0
    gameTime = 0;

    //scores set to 0
    score = 0;
    finalScore = 0;

    //clears all the barriers, power ups, and explosion impact effects
    barriers.clear();
    powerUps.clear();
    explosions.clear();

    //resets the ball velocity
    ballVelocityX = SCREEN_X / 200;
    ballVelocityY = SCREEN_Y / 200;

    //sets the initial size of the objects
    leftWall.setSize(Vector2f(WALL_WIDTH, WALL_HEIGHT));
    rightWall.setSize(Vector2f(WALL_WIDTH, WALL_HEIGHT));
    ball.setSize(Vector2f(BALL_WIDTH,BALL_HEIGHT));
    topWall.setSize(Vector2f(SCREEN_X,SCREEN_Y / 50));
    bottomWall.setSize(Vector2f(SCREEN_X,SCREEN_Y / 50));

    //sets the initial position of the objects
    leftWall.setPosition(0, SCREEN_Y / 2 - leftWall.getSize().y / 2);
    rightWall.setPosition(SCREEN_X-WALL_WIDTH, SCREEN_Y/2 - rightWall.getSize().y / 2);
    ball.setPosition(SCREEN_X / 2,SCREEN_Y / 50);
    topWall.setPosition(0,0);
    bottomWall.setPosition(0,SCREEN_Y - SCREEN_Y / 50);

    //sets the colors of objects
    leftWall.setFillColor(Color::Red);
    rightWall.setFillColor(Color::Blue);
    ball.setFillColor(Color::Yellow);
    topWall.setFillColor(Color::White);
    bottomWall.setFillColor(Color::White);

    //sets the outline thickness of objects
    leftWall.setOutlineThickness(5);
    rightWall.setOutlineThickness(5);
    ball.setOutlineThickness(5);
    topWall.setOutlineThickness(5);
    bottomWall.setOutlineThickness(5);

    //sets the outline color of objects
    leftWall.setOutlineColor(Color::White);
    rightWall.setOutlineColor(Color::White);
    ball.setOutlineColor(Color::White);
    topWall.setOutlineColor(Color::Red);
    bottomWall.setOutlineColor(Color::Red);
}

void updateBarriers(){
    //updates the barriers with a little bit of randomness
    int loopSize = barriers.size();
    //this formula makes it so that the barriers at the beginning of the vector will have a shorter life span than the vectors at the end
    //it will help make it so that there aren't an infinite amount of barriers on the screen
    for(int i = 0; i < loopSize; i++){
        //decrease the life variable of the barrier
        barriers.at(i).life--;

        //destroys the barrier if its life is over
        if(barriers.at(i).life <= 0){
            //destroy the barrier
            barriers.erase(barriers.begin() + i);

            //restart the loop
            loopSize = barriers.size();
            i = 0;
        }
    }
}

//change the power up animation to the next texture
void updatePowerUp(){
    for(int i = 0; i < powerUps.size(); i++){
        powerUps.at(i).coin.setTexture(textures[(gameTime / 10) % 9]);
    }
}

//update the score by the scoreChange parameter
void updateScore(int scoreChange){
    finalScore += scoreChange;
    string newScore = to_string(finalScore);
    int scoreLength2 = newScore.length();
    text.setPosition(SCREEN_X / 2 - fontSize * scoreLength2 / 3, 100 + fontSize / 2);
}
