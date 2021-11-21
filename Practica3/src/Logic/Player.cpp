#include "Player.h"
#include <algorithm>

Player::Player(float X, float Y, float A) : posX(X), posY(Y), angle(A) { }		

void Player::setPosX(float pX) {
	posX = pX;
}

void Player::setPosY(float pY) {
	posY = pY;
}

void Player::calculateAngle(float dX, float dY) {
	if (dX != 0 || dY != 0) angle = atan2(dY, dX);
}

float Player::getSpeed() const
{
	return speed;
}

void Player::setSpeed(float sp)
{
	speed = sp;
}

unsigned int Player::getSize() const
{
	return playerSize;
}

void Player::setSize(unsigned int s)
{
	playerSize = s;
}

unsigned int Player::getBarSize() const
{
	return playerBarSize;
}

void Player::setBarSize(unsigned int s)
{
	playerBarSize = s;
}

float Player::getPosX() const {
	return posX;
}

float Player::getPosY() const {
	return posY;
}

float Player::getAngle() const {
	return angle;
}