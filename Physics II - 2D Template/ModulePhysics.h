#pragma once
#include "Module.h"
#include "Globals.h"

enum class PhysType {

	UNKNOWN = 0,
	TERRAIN,
	ENTITY,
	PROJECTILE
};

enum class Shape {

	UNKNOWN = 0,
	RECTANGLE,
	CIRCLE
};

class Body {
public:

	Body() {}
	//Rectangle constructor
	Body(int x_, int y_, int w, int h, PhysType t) : width(w), height(h), type(t) 
	{
		position.x = x_;
		position.y = y_;
		shape = (Shape)1;
	}
	//Circle constructor
	Body(int x_, int y_, int r, PhysType t) : radius(r), type(t)
	{
		position.x = x_;
		position.y = y_;
		shape = (Shape)2;
	}
	~Body() {}

	p2Point<int> GetPosition() { return position; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetRadius() 
	{	
		int r;

		if (this->shape == Shape::CIRCLE) {
			r = radius;
		}
		else {
			r = NULL;
			LOG("This object shape doesn't have radius");
		}

		return r;
	}
	PhysType GetType() { return type; }
	Shape GetShape() { return shape; }

private:

	p2Point<int> position;
	int width, height;
	int radius;
	PhysType type;
	Shape shape;
	
	friend class ModulePhysics;
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	Body* CreateRectangle(int x, int y, int w, int h, PhysType type);
	Body* CreateCircle(int x, int y, int radius, PhysType type);

	//Col solver
	//Physics Integrator

private:

	bool debug;

	p2List<Body*> bodyList;
};