#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y 10.0f

#define PIXELS_PER_METER (20.0f) // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER) // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) std::floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)


enum class PhysType {

	UNKNOWN = 0,
	TERRAIN,
	ENTITY,
	PROJECTILE,
	WATER,
	TARGET,
	TELE_PROJECTILE
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
			//LOG("This object shape doesn't have radius");
		}

		return r;
	}
	PhysType GetType() { return type; }
	Shape GetShape() { return shape; }
	unsigned int GetMass() { return mass; }
	Vector GetVelocity() { return speed; }
	//Setters
	void SetVelocity(Vector speed);
	void SetPosition(p2Point<float> position); 
	void SetWidth(int width);
	void SetHeigth(int heigth);
	void SetMass(unsigned int mass); 
	void SetBodyType(BodyType bt);

	void OnCollision(Body* body2);

private:

	p2Point<float> position, LastPosition;
	Vector speed;
	int width, height;
	float radius = 1;
	float mass = 1.0; 
	unsigned int coefElastic; 
	float dragC = 0.005; 
	float hydroDrag = 1.0f;
	bool IsOnWater = false; 
	bool applyFriction = true; 
	float coefRest = 1;

	
	friend class ModulePhysics;
public:
	PhysType type;
	Shape shape;
	BodyType btype; 

	//Forces
	Vector gravityForce = {0,0};
	Vector buoyancyForce = { 0,0 };
	Vector frictionForce = { 0,0 };
	Vector dragForce = { 0,0 };
	Vector liftForce = { 0,0 };
	Vector totalForce = { 0,0 };
	Vector jumpPlayerForce = { 0,0 };

	//Bullet damage 
	int bulletDamage = 7;
	Body* whoShotMe;

};

class Terrain {
public:
	Terrain() {}
	~Terrain() {}

	Terrain(Vector gravity, float friction, Body* terrainBody) : gravity(gravity), frictionC(friction), terrainBody(terrainBody) {}

public:
	Body* terrainBody;

	Vector gravity = Vector(GRAVITY_X, GRAVITY_Y);

	float atmosDensity; 

	Vector wind; 

	float frictionC;


};
class Water {
public: 
	Water() {}
	~Water() {}

	Water(Vector gravity, float waterDensity, Body* waterBody) : gravity(gravity), waterDensity(waterDensity), waterBody(waterBody) {}
public:
	Body* waterBody; 
	Vector gravity = Vector(GRAVITY_X, GRAVITY_Y);

	float waterDensity; 

	Vector waterDrag = { 0,0 }; 
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
	bool CheckCollisions(Body* b1 = nullptr, Body* b2 = nullptr);
	void CollisionSolver(Body* b1, Body* b2);
	void CreateTerrain(p2Point<float> pos);

public:
	bool showState;
	bool allowGravity;
	bool allowAero;
	bool allowFriction;
	bool allowHydro;
private:

	bool debug;
	
	
	bool fps30 = true;

	enum class INTEGRATION_METHOD {
		BW_EULER,
		FW_EULER,
		VERLET
	};

	INTEGRATION_METHOD integMethod;

	enum class COL_SOLVER_METHOD {
		TP_NORM_VEC,
		ITERATE_CONTACT_POINT,
		BACK_TO_LAST_POINT
	};

	COL_SOLVER_METHOD colSolMethod;

	p2List<Body*> bodyList;

	Terrain* terrain; 

	Water* water; 
};

float modulus(float vx, float vy);