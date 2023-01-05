#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include <cmath>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	integMethod = INTEGRATION_METHOD::BW_EULER;

	colSolMethod = COL_SOLVER_METHOD::TP_NORM_VEC;

	p2Point<float> terrainPos; 
	terrainPos.x = 0;
	terrainPos.y = App->renderer->camera.h - 200;
	CreateTerrain(terrainPos);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	
	//Debug features input 
	DebugKeys();

	return UPDATE_CONTINUE;
}

//
update_status ModulePhysics::Update()
{
	//Set title
	static char integChar[256];
	static char debugChar[256];
	static char colChar[256];

	switch (integMethod)
	{
	case ModulePhysics::INTEGRATION_METHOD::BW_EULER:
		sprintf_s(integChar, 256, "BW_Euler");
		break;
	case ModulePhysics::INTEGRATION_METHOD::FW_EULER:
		sprintf_s(integChar, 256, "FW_Euler");
		break;
	case ModulePhysics::INTEGRATION_METHOD::VERLET:
		sprintf_s(integChar, 256, "Verlet");
		break;
	default:
		break;
	}

	(debug) ? sprintf_s(debugChar, 256, "On") : sprintf_s(debugChar, 256, "Off");

	switch (colSolMethod)
	{
	case ModulePhysics::COL_SOLVER_METHOD::TP_NORM_VEC:
		sprintf_s(colChar, 256, "TP to surface");
		break;
	case ModulePhysics::COL_SOLVER_METHOD::ITERATE_CONTACT_POINT:
		sprintf_s(colChar, 256, "Iterate 'till contact point");
		break;
	default:
		break;
	}

	static char title[256];
	sprintf_s(title, 256, "| Integ. Method (F1): %s | Debug Draw (F2): %s | Col. Solving Scheme (F3): %s |", integChar, debugChar,colChar);
	App->window->SetTitle(title);

	//Apply forces to all bodies
	if (bodyList.getFirst() != nullptr) {
		Integrator();
	}

	CheckCollisions();

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(!debug)
		return UPDATE_CONTINUE;

	if (bodyList.getFirst() != nullptr) {
		for (p2List_item<Body*>* bodyNode = bodyList.getFirst(); bodyNode != nullptr; bodyNode = bodyNode->next) {

			Body* body = bodyNode->data;

			//Color for every PhysType
			Uint8 RGBAlpha[4] = { 0,0,0,0 };
			switch (body->GetType()) {
			case (PhysType::TERRAIN):

				RGBAlpha[0] = 255;
				RGBAlpha[1] = 0;
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 255;
				break;
			case (PhysType::ENTITY):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 255;
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 255;
				break;
			case (PhysType::PROJECTILE):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 0;
				RGBAlpha[2] = 255;
				RGBAlpha[3] = 255;
				break;
			}

			if (body->GetShape() == Shape::RECTANGLE) {
				SDL_Rect rect = { body->GetPosition().x,body->GetPosition().y,body->GetWidth(),body->GetHeight()};

				App->renderer->DrawQuad(rect, RGBAlpha[0], RGBAlpha[1], RGBAlpha[2], RGBAlpha[3]);
				
			}

			if (body->GetShape() == Shape::CIRCLE) {

				App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius(), RGBAlpha[0], RGBAlpha[1], RGBAlpha[2], RGBAlpha[3]);
			}
		}
	}
	

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics items");

	bodyList.clear();

	return true;
}

void ModulePhysics::DestroyBody(Body* body) {
	p2List_item<Body*>* bList; 
	for (bList = bodyList.getFirst(); bList != NULL; bList->next) {
		if (bList->data = body) {
			bodyList.del(bList); 
			delete body; 
			body->~Body(); 
			return; 
		}
	}
}

void ModulePhysics::CheckCollisions() {

	//Check every body on the list between each other to see if they are colliding
	for (p2List_item<Body*>* bodyNode1 = bodyList.getFirst(); bodyNode1 != nullptr; bodyNode1 = bodyNode1->next) {
		for (p2List_item<Body*>* bodyNode2 = bodyList.getFirst(); bodyNode2 != nullptr; bodyNode2 = bodyNode2->next) {

			Body* body1 = bodyNode1->data;
			Body* body2 = bodyNode2->data;

			if (body1 == body2) {
				continue;
			}
			
			//If both bodies are rectangles
			if (body1->shape == Shape::RECTANGLE && body2->shape == Shape::RECTANGLE) {

				if ((body1->GetPosition().x < body2->GetPosition().x + body2->GetWidth()) &&
					(body1->GetPosition().x + body1->GetWidth() > body2->GetPosition().x) &&
					(body1->GetPosition().y < body2->GetPosition().y + body2->GetHeight()) &&
					(body1->GetHeight() + body1->GetPosition().y > body2->GetPosition().y)) {

					//Collision detected
					CollisionSolver(body1, body2);
				}
			}

			//If both bodies are cirles
			if (body1->shape == Shape::CIRCLE && body2->shape == Shape::CIRCLE) {

				float dx = body1->GetPosition().x - body2->GetPosition().x;
				float dy = body1->GetPosition().y - body2->GetPosition().y;
				double distance = sqrt(dx * dx + dy * dy);

				if (distance < (body1->GetRadius() + body2->GetRadius())) {

					//Collision detected
					CollisionSolver(body1, body2);
				}
			}

			//If one body is a rectangle and the other a circle
			if (body1->shape != body2->shape) {

				Body* rect;
				Body* circ;

				if (body1->shape == Shape::RECTANGLE) {
					rect = body1;
					circ = body2;
				}
				else {
					rect = body2;
					circ = body1;
				}
				
				float testX = circ->GetPosition().x;
				float testY = circ->GetPosition().y;

				// which edge is closest?
				if (circ->GetPosition().x < rect->GetPosition().x)         
					testX = rect->GetPosition().x;      // test left edge
				else if (circ->GetPosition().x > rect->GetPosition().x + rect->GetWidth()) 
					testX = rect->GetPosition().x + rect->GetWidth();   // right edge
				if (circ->GetPosition().y < rect->GetPosition().y)         
					testY = rect->GetPosition().y;      // top edge
				else if (circ->GetPosition().y > rect->GetPosition().y + rect->GetHeight())
					testY = rect->GetPosition().y + rect->GetHeight();   // bottom edge

				// get distance from closest edges
				float distX = circ->GetPosition().x - testX;
				float distY = circ->GetPosition().y - testY;
				float distance = sqrt((distX * distX) + (distY * distY));

				// if the distance is less than the radius, collision!
				if (distance <= circ->GetRadius()) {

					//Collision detected
					CollisionSolver(body1, body2);
				}
			}

		}

	}
}
void ModulePhysics::DebugKeys() {

	//Integration Method
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_STATE::KEY_DOWN) {
		switch (integMethod)
		{
		case INTEGRATION_METHOD::BW_EULER:
			integMethod = INTEGRATION_METHOD::FW_EULER;
			break;
		case INTEGRATION_METHOD::FW_EULER:
			integMethod = INTEGRATION_METHOD::VERLET;
			break;
		case INTEGRATION_METHOD::VERLET:
			integMethod = INTEGRATION_METHOD::BW_EULER;
			break;
		default:
			break;
		}
	}

	//Debug Draw
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_STATE::KEY_DOWN)
		debug = (!debug) ? true : false;

	//Collision Solving Scheme
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_STATE::KEY_DOWN) {
		switch (colSolMethod)
		{
		case COL_SOLVER_METHOD::TP_NORM_VEC:
			colSolMethod = COL_SOLVER_METHOD::ITERATE_CONTACT_POINT;
			break;
		case COL_SOLVER_METHOD::ITERATE_CONTACT_POINT:
			colSolMethod = COL_SOLVER_METHOD::TP_NORM_VEC;
			break;
		default:
			break;
		}
	}
}

void ModulePhysics::Integrator() {

	p2List_item<Body*>* bList;
	for (bList = bodyList.getFirst(); bList != nullptr; bList = bList->next) {
		if (bList->data->btype != BodyType::STATIC) {
			//Gravity Force
			float mass = bList->data->GetMass(); 
			Vector gravity = Vector(GRAVITY_X, GRAVITY_Y); 
			bList->data->gravityForce = Vector(mass * gravity.x, mass * gravity.y);

			LOG("Gravity: %f", bList->data->gravityForce.y);

			//Friction force
			if (bList->data->GetSpeed().x < 0) {
				bList->data->frictionForce.x = bList->data->gravityForce.y * terrain->frictionC;
			}
			else if (bList->data->GetSpeed().x > 0) {
				bList->data->frictionForce.x = bList->data->gravityForce.y * -terrain->frictionC;
			}
			LOG("Friction: %f", bList->data->frictionForce.x);

			//Drag force
			if (bList->data->GetSpeed().x < 0) {
				bList->data->dragForce.x = bList->data->GetSpeed().x * bList->data->GetSpeed().x * terrain->dragC;
			}
			else {
				bList->data->dragForce.x = -bList->data->GetSpeed().x * bList->data->GetSpeed().x * terrain->dragC;
			}

			if (bList->data->GetSpeed().y < 0) {
				bList->data->dragForce.y = bList->data->GetSpeed().y * bList->data->GetSpeed().y * terrain->dragC;
			}
			else {
				bList->data->dragForce.y = -bList->data->GetSpeed().y * bList->data->GetSpeed().y * terrain->dragC;
			}

			//Addition of all the forces in order to calculate the acceleration

			bList->data->totalForce.x = bList->data->gravityForce.x + bList->data->dragForce.x + bList->data->frictionForce.x + bList->data->jumpPlayerForce.x + bList->data->bounceForce.x;

			bList->data->totalForce.y = bList->data->gravityForce.y + bList->data->dragForce.y + bList->data->frictionForce.y + bList->data->jumpPlayerForce.y + bList->data->bounceForce.y;

			Vector acceleration = Vector(bList->data->totalForce.x / bList->data->mass, bList->data->totalForce.y / bList->data->mass);

			p2Point<float> currentPos = bList->data->GetPosition();

			Vector velocity = bList->data->GetSpeed(); 

			//Switch with dt

			bList->data->SetPosition(currentPos);

			bList->data->SetVelocity(velocity); 

			bList->data->jumpPlayerForce.x = 0; 
			bList->data->jumpPlayerForce.y = 0; 
			bList->data->frictionForce.x = 0; 
		}
	}
}

void ModulePhysics::CollisionSolver(Body* b1, Body* b2) {

	switch (colSolMethod)
	{
	case COL_SOLVER_METHOD::TP_NORM_VEC:

		if (b1->btype != BodyType::STATIC) {
			
			float dY = b1->GetPosition().y - b2->GetPosition().y;

			p2Point<float> newPos;
			newPos.x = b2->GetPosition().x; 
			newPos.y = b2->GetPosition().y - (b2->GetHeight() - dY);

			b2->SetPosition(newPos);
		}

		break;
	case COL_SOLVER_METHOD::ITERATE_CONTACT_POINT:


		break;
	default:
		break;
	}

}

void ModulePhysics::CreateTerrain(p2Point<float> pos) {
	Body* terrain = new Body(); 

	terrain->SetPosition(pos); 
	terrain->SetVelocity(Vector(0, 0)); 

	terrain->SetWidth(SCREEN_WIDTH); 
	terrain->SetHeigth(SCREEN_HEIGHT - terrain->position.y); 

	terrain->SetMass(100000); //Value?

	terrain->btype = BodyType::STATIC; 
	terrain->shape = Shape::RECTANGLE; 
	terrain->type = PhysType::TERRAIN; 
		
	bodyList.add(terrain);

	this->terrain = new Terrain(Vector(GRAVITY_X, GRAVITY_Y), 0, terrain); 
}

//Body class methods

void Body::SetVelocity(Vector speed) {

	this->speed = speed; 
}
void Body::SetPosition(p2Point<float> position) {

	this->position = position; 
}
void Body::SetWidth(int width) {

	this->width = width;
}
void Body::SetHeigth(int heigth) {

	this->height = heigth;
}
void Body::SetMass(unsigned int mass) {
	this->mass = mass; 
}

Body* ModulePhysics::CreateRectangle(int x, int y, int w, int h, PhysType type) {

	Body* body = new Body(x, y, w, h, type);
	body->SetVelocity(Vector(0, 0));
	body->SetMass(100); 

	body->btype = BodyType::DYNAMIC;
	bodyList.add(body);

	return body;
}
Body* ModulePhysics::CreateCircle(int x, int y, int radius, PhysType type) {

	Body* body = new Body(x, y, radius, type);
	body->SetVelocity(Vector(0, 0));
	body->SetMass(100);

	body->btype = BodyType::DYNAMIC;

	bodyList.add(body);

	return body;
}

void Body::OnCollision(Body* body2) {

}

