#include "MijnLookAt.h"

glm::mat4x4 MijnLookAt(glm::vec3 &position, glm::vec3 &target, glm::vec3 &up_world)
{
	glm::mat4x4 matleft, matright;
	glm::vec3 right, direction, up;

	direction = glm::normalize(position - target);
	right = glm::normalize(glm::cross(glm::normalize(up_world), direction));
	up = glm::normalize(glm::cross(direction, right));

	matleft[0][0] = right.x;
	matleft[1][0] = right.y;
	matleft[2][0] = right.z;

	matleft[0][1] = up.x;
	matleft[1][1] = up.y;
	matleft[2][1] = up.z;

	matleft[0][2] = direction.x;
	matleft[1][2] = direction.y;
	matleft[2][2] = direction.z;

	matright[3][0] = -position.x;
	matright[3][1] = -position.y;
	matright[3][2] = -position.z;

	return  matleft * matright;
}