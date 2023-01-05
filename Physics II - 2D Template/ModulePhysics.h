#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y 10.0f

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

//New class created in order to work with vectors instead of p2Point
class Vector {
public:
	Vector() {}
	~Vector() {}

	Vector(float x, float y) : x(x), y(y) {}; 
public:
	float x, y; 
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
	void SetVelocity(Vector speed);
	void SetPosition(p2Point<float> position); 
	void SetWidth(int width);
	void SetHeigth(int heigth);
	void SetMass(unsigned int mass); 

	void OnCollision(Body* body2);

private:

	p2Point<float> position;
	Vector speed; 
	int width, height;
	int radius;
	float mass = 1.0; 
	unsigned int coefElastic; 

	
	friend class ModulePhysics;
public:
	PhysType type;
	Shape shape;
	BodyType btype; 

	//Forces
	Vector gravityForce;
	Vector bounceForce;
	Vector frictionForce;
	Vector dragForce;
	Vector liftForce;
	Vector totalForce;

};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	//Creates a rectangle shaped body (Don't set w or h as negative)
	Body* CreateRectangle(int x, int y, int w, int h, PhysType type);
	//Creates a circle shaped body
	Body* CreateCircle(int x, int y, int radius, PhysType type);
	void DestroyBody(Body* body); 
	void DebugKeys(); 
	void Integrator();
	void CheckCollisions();
	void CollisionSolver(Body* b1, Body* b2);

private:

	bool debug;

	enum class INTEGRATION_METHOD {
		BW_EULER,
		FW_EULER,
		VERLET
	};

	INTEGRATION_METHOD integMethod;

	enum class COL_SOLVER_METHOD {
		TP_NORM_VEC,
		ITERATE_CONTACT_POINT
	};

	COL_SOLVER_METHOD colSolMethod;

	p2List<Body*> bodyList;
};