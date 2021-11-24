#pragma once

class Player {
public:
	Player(float X, float Y, float A);

	float getPosX() const;
	float getPosY() const;
	void setPosX(float pX);
	void setPosY(float pY);

	float getAngle() const;
	void setAngle(float angle);
	void calculateAngle(float dX, float dY);

	float getSpeed() const;

	unsigned int getSize() const;

	unsigned int getBarSize() const;

private:
	const float PLAYERSPEED = 3; // tiles por segundo
	const unsigned int PLAYERSIZE = 5;
	const unsigned int PLAYERBARSIZE = 25;

	float posX = 0, posY = 0;
	float angle = 0;
};