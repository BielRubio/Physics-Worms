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
	
	//Debug features input 

	return UPDATE_CONTINUE;
}

//
update_status ModulePhysics::Update()
{

	CheckCollisions();

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

					//Collsion detected
					LOG("Rectangles Colliding")
				}
			}

			//If both bodies are cirles
			if (body1->shape == Shape::CIRCLE && body2->shape == Shape::CIRCLE) {

				float dx = body1->GetPosition().x - body2->GetPosition().x;
				float dy = body1->GetPosition().y - body2->GetPosition().y;
				double distance = sqrt(dx * dx + dy * dy);

				if (distance < (body1->GetRadius() + body2->GetRadius())) {

					//Collsion detected
					LOG("Circles Colliding")
				}
				else {
					LOG("not");
				}

			}

			//If one body is a rectangle and the other a circle
			if (body1->shape != body2->shape) {

			}

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

