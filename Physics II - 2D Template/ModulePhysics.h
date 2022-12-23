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
	Body(SDL_Rect b, PhysType t) : bounds(b), type(t) {}
	~Body() {}

	SDL_Rect GetBounds() { return bounds; }
	PhysType GetType() { return type; }
	Shape GetShape() { return shape; }

private:

	SDL_Rect bounds;
	PhysType type;
	Shape shape;

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

	Body* CreateRectangle();
	Body* CreateCircle();

private:

	bool debug;

	p2List<Body*> bodyList;
};