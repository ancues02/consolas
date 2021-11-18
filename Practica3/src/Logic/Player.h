#pragma once

const unsigned int PLAYER_SIZE = 5;
const unsigned int PLAYER_BAR_SIZE = 25;//10;

class Player {
private:
	float posX = 0, posY = 0;
	float angle = 0;
public:
	Player(float X, float Y, float A);

	void setPosX(float pX);
	void setPosY(float pY);
	void calculateAngle(float dX, float dY);
	float getPosX();
	float getPosY();
	float getAngle();
};