#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

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
enum class BodyType {
	DYNAMIC,
	STATIC,
	UNKNOWN
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
	//Getters
	p2Point<float> GetPosition() { return position; }
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
	unsigned int GetMass() { return mass; }
	//Setters
	void SetVelocity(p2Point<float> speed);
	void SetPosition(p2Point<float> position); 
	void SetWidth(int width);
	void SetHeigth(int heigth);

	void OnCollision(Body* body2);

private:

	p2Point<float> position;
	p2Point<float> speed; 
	int width, height;
	int radius;
	float mass; 
	unsigned int coefElastic; 

	
	friend class ModulePhysics;
public:
	PhysType type;
	Shape shape;
	BodyType btype; 

	//Forces
	p2Point<float> gravityForce;
	p2Point<float> bounceForce;
	p2Point<float> frictionForce;
	p2Point<float> dragForce;
	p2Point<float> liftForce;
	p2Point<float> totalForce;

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
	void DestroyBody(Body* body); 
	//Col solver
	//Physics Integrator

private:

	bool debug;

	p2List<Body*> bodyList;
};