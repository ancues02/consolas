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
	void setSpeed(float sp);

	unsigned int getSize() const;
	void setSize(unsigned int s);

	unsigned int getBarSize() const;
	void setBarSize(unsigned int s);

private:
	float posX = 0, posY = 0;
	float angle = 0;
	float speed = 3; // tiles por segundo
	unsigned int playerSize = 5;
	unsigned int playerBarSize = 25;
};