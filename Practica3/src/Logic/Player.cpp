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

float Player::getPosX() {
	return posX;
}

float Player::getPosY() {
	return posY;
}

float Player::getAngle() {
	return angle;
}