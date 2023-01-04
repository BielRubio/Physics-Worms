#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

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

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{

	if (bodyList.getFirst() != nullptr) {
		Integrator(); 
	}
	
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

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
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 0;
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
void ModulePhysics::DebugKeys() {

}

void ModulePhysics::Integrator() {

	p2List_item<Body*>* bList;
	for (bList = bodyList.getFirst(); bList != nullptr; bList = bList->next) {
		if (bList->data->btype != BodyType::STATIC) {
			//Gravity Force
			float mass = bList->data->GetMass(); 
			Vector gravity = Vector(GRAVITY_X, GRAVITY_Y); 
			bList->data->gravityForce = Vector(mass * gravity.x, mass * gravity.y);
			
		}
	}
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

	body->btype = BodyType::STATIC;
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

