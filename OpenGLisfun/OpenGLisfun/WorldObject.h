#pragma once

#include <glm\glm.hpp>

class WorldObject
{
public:
	WorldObject(glm::mat4x4 &view, glm::mat4x4 &projection) : view(view), projection(projection)
	{
	}
	virtual ~WorldObject() {};
protected:
	const glm::mat4x4 &view; 
	const glm::mat4x4 &projection;
};