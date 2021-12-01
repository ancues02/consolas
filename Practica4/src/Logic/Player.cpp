#include "Player.h"
#include <algorithm>

Player::Player(float X, float Y, float A) : posX(X), posY(Y), angle(A) { }		

void Player::setPosX(float pX) {
	posX = pX;
}

void Player::setPosY(float pY) {
	posY = pY;
}

void Player::setAngle(float angly) {
	angle = angly;
}

void Player::calculateAngle(float dX, float dY) {
	if (dX != 0 || dY != 0) angle = atan2(dY, dX);
	if (angle < 0) angle += 2 * 3.1416;
}

float Player::getSpeed() const
{
	return PLAYERSPEED;
}

unsigned int Player::getSize() const
{
	return PLAYERSIZE;
}

unsigned int Player::getBarSize() const
{
	return PLAYERBARSIZE;
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