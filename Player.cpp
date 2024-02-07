#include "Player.h"


Player::Player() {
	pos.x = 65;
	pos.y = 0;
	pos.z = 30;
	ang = -90;
	lives = 3;
	gold = 0;
}

void Player::draw() {

	//playerModel.pos.y = pos.y;

	playerModel.pos.x = pos.x;
	playerModel.pos.z = pos.z;
	playerModel.rot.y = 180 + ang;
	playerModel.scale = 0.025;
	this->playerModel.Draw();
}

void Player::rotateLeft() {
	ang += 3;
}

void Player::rotateRight() {
	ang -= 3;
}

void Player::moveForward() {
	pos.x += sin(ang * 3.14 / 180);
	pos.z += cos(ang * 3.14 / 180);
}

void Player::moveBackward() {
	pos.x -= sin(ang * 3.14 / 180);
	pos.z -= cos(ang * 3.14 / 180);
}

void Player::moveLeft() {
	ang += 90;
	moveForward();
	ang -= 90;
}

void Player::moveRight() {
	ang -= 90;
	moveForward();
	ang += 90;
}
