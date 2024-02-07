#include "Camera.h"


Camera::Camera(float eyeX = 20.0f, float eyeY = 2.5f, float eyeZ = 20.0f, float centerX = 0.0f, float centerY = 2.5f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
	eye = Vector(eyeX, eyeY, eyeZ);
	center = Vector(centerX, centerY, centerZ);
	up = Vector(upX, upY, upZ);
	isFirst = true;
}

void Camera::moveX(float d) {
	Vector right = up.crossProd(center - eye).unit();
	eye = eye + right * d;
	center = center + right * d;
}

void Camera::moveY(float d) {
	eye = eye + up.unit() * d;
	center = center + up.unit() * d;
}

void Camera::moveZ(float d) {
	Vector view = (center - eye).unit();
	eye = eye + view * d;
	center = center + view * d;
}

void Camera::rotateX(float a) {
	Vector view = (center - eye).unit();
	Vector right = up.crossProd(view).unit();
	view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
	up = view.crossProd(right);
	center = eye + view;
}

void Camera::rotateY(float a) {
	Vector view = (center - eye).unit();
	Vector right = up.crossProd(view).unit();
	view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
	right = view.crossProd(up);
	center = eye + view;
}

void Camera::look(Player player) {
	eye.x = player.pos.x + sin(DEG2RAD(player.ang)) * 0.6;
	eye.z = player.pos.z + cos(DEG2RAD(player.ang)) * 0.6;
	center.x = player.pos.x + 2 * sin(DEG2RAD(player.ang));
	center.z = player.pos.z + 2 * cos(DEG2RAD(player.ang));
	if (!isFirst) {
		gluLookAt(
			eye.x - 10 * sin(DEG2RAD(player.ang)), eye.y + 2, eye.z - 10 * cos(DEG2RAD(player.ang)),
			center.x + 10 * sin(DEG2RAD(player.ang)), center.y - 3, center.z + 10 * cos(DEG2RAD(player.ang)),
			up.x, up.y, up.z
		);
	}
	else {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}

	/*gluLookAt(
		10, 10, 0,
		0, 0, 0,
		up.x, up.y, up.z
	);*/

}
