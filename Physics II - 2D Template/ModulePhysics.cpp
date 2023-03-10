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
	terrain->atmosDensity = 1.0f; 
	terrain->wind = {10.0f, 5.0f};

	water->waterDensity = 50.0f; 
	water->waterDrag = { -1.0f, 0.0f };

	allowGravity = true; 
	allowAero = true; 
	allowHydro = true; 
	allowFriction = true; 
	showState = false; 



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
	static char teleChar[256];

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
	case ModulePhysics::COL_SOLVER_METHOD::BACK_TO_LAST_POINT:
		sprintf_s(colChar, 256, "Back to last point");
		break;
	default:
		break;
	}

	static char title[256];
	sprintf_s(title, 256, "Integ. Method (F1): %s | Debug Draw (F2): %s | Col. Solving Scheme (F3): %s | FPS : %f (F4 add, F5 substract, F6 30/60)", integChar, debugChar,colChar, App->FPS);
	App->window->SetTitle(title);

	//Apply forces to all bodies
	if (bodyList.getFirst() != nullptr) {
		Integrator();
	}
	CheckCollisions();

	if (bodyList.getFirst() != nullptr && colSolMethod == ModulePhysics::COL_SOLVER_METHOD::BACK_TO_LAST_POINT) {
		for (p2List_item<Body*>* bodyNode = bodyList.getFirst(); bodyNode != nullptr; bodyNode = bodyNode->next) {

			Body* body = bodyNode->data;

			// Save last position
			body->LastPosition = body->position;

		}
	}


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

				RGBAlpha[0] = 82;
				RGBAlpha[1] = 54;
				RGBAlpha[2] = 20;
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
			case (PhysType::WATER):

				RGBAlpha[0] = 53;
				RGBAlpha[1] = 109;
				RGBAlpha[2] = 160;
				RGBAlpha[3] = 170;
				break;
			case (PhysType::TELE_PROJECTILE):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 255;
				RGBAlpha[2] = 255;
				RGBAlpha[3] = 255;
				break;
			}

			if (body->GetShape() == Shape::RECTANGLE) {
				SDL_Rect rect = { body->GetPosition().x,body->GetPosition().y,body->GetWidth(),body->GetHeight()};

				App->renderer->DrawQuad(rect, RGBAlpha[0], RGBAlpha[1], RGBAlpha[2], RGBAlpha[3]);
				
			}

			if (body->GetShape() == Shape::CIRCLE) {

				if (body->GetType() == PhysType::TARGET) {
					App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius(), 255, 0, 0,255);
					App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius()-10, 255, 255, 255,255);
					App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius()-20, 255, 0, 0, 255);
					App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius() - 30, 255, 255, 255, 255);
				}
				else {
					App->renderer->DrawCircle(body->GetPosition().x, body->GetPosition().y, body->GetRadius(), RGBAlpha[0], RGBAlpha[1], RGBAlpha[2], RGBAlpha[3]);
				}
				
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

	delete terrain;
	terrain = nullptr;

	delete water;
	water = nullptr;

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

bool ModulePhysics::CheckCollisions(Body* b1, Body* b2) {

	bool returnBool = (b1 == nullptr || b2 == nullptr) ? false : true;

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
					if (returnBool)
						return true;
					else
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
					if (returnBool)
						return true;
					else {
						CollisionSolver(body1, body2);
					}
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
					if (returnBool)
						return true;
					else
						CollisionSolver(body1, body2);
				}
			}

		}
	}

	return false;
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
			colSolMethod = COL_SOLVER_METHOD::BACK_TO_LAST_POINT;
			break;
		case COL_SOLVER_METHOD::BACK_TO_LAST_POINT:
			colSolMethod = COL_SOLVER_METHOD::TP_NORM_VEC;
			break;
		default:
			break;
		}
	}

	//Delta time
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_STATE::KEY_REPEAT) {
		if (App->FPS != 1) {
			App->FPS++;
			App->frameDelay = 1 / App->FPS;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_STATE::KEY_REPEAT) {
		App->FPS--;
		App->frameDelay = 1 / App->FPS;
	}
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_STATE::KEY_DOWN) {
		if (fps30 == true) {
			App->FPS = 60;
			App->frameDelay = 1 / App->FPS;
			fps30 = false;
		}
		else {
			App->FPS = 30;
			App->frameDelay = 1 / App->FPS;
			fps30 = true;
		}
	}
	//Show state
	if(App->input->GetKey(SDL_SCANCODE_F10) == KEY_STATE::KEY_DOWN)
		showState = (!showState) ? true : false;

	//Forces desactivation
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_STATE::KEY_DOWN) 
		allowGravity = (!allowGravity) ? true : false;
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_STATE::KEY_DOWN)
		allowAero = (!allowAero) ? true : false;
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_STATE::KEY_DOWN)
		allowHydro = (!allowHydro) ? true : false;
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_STATE::KEY_DOWN)
		allowFriction = (!allowFriction) ? true : false;

}

void ModulePhysics::Integrator() {

	p2List_item<Body*>* bList;
	for (bList = bodyList.getFirst(); bList != nullptr; bList = bList->next) {
		if (bList->data->btype != BodyType::STATIC) {
			//Gravity Force
			if (allowGravity) {
				float mass = bList->data->GetMass();
				Vector gravity = Vector(GRAVITY_X, GRAVITY_Y);
				bList->data->gravityForce = Vector(mass * gravity.x, mass * gravity.y);
			}
			

			//LOG("Gravity force: %f, %f", bList->data->gravityForce.x, bList->data->gravityForce.y);
			if (bList->data->IsOnWater) {
				if (allowHydro) {
					//Drag force on water
					Vector vel = { bList->data->speed.x - water->waterDrag.x, bList->data->speed.y - water->waterDrag.y };
					float modVel = modulus(vel.x, vel.y);
					Vector unitaryDrag = { vel.x / modVel, vel.y / modVel };
					float dragModulus = modVel * bList->data->hydroDrag;
					bList->data->dragForce.x = -unitaryDrag.x * dragModulus;
					bList->data->dragForce.y = -unitaryDrag.y * dragModulus;
				}
			}
			else {
				if (allowAero) {
					Vector vel = { bList->data->speed.x - terrain->wind.x, bList->data->speed.y - terrain->wind.y };
					float modVel = modulus(vel.x, vel.y);
					Vector unitaryDrag = { vel.x / modVel, vel.y / modVel };
					float dragModulus = 0.5f * terrain->atmosDensity * modVel * modVel * bList->data->dragC;
					if (bList->data->GetVelocity().x != 0) {
						bList->data->dragForce.x = -unitaryDrag.x * dragModulus;
						bList->data->dragForce.y = -unitaryDrag.y * dragModulus;
					}
				}
				//LOG("DragForce: %f %f", bList->data->dragForce.x, bList->data->dragForce.y);
			}
			//Friction force
			if (allowFriction) {
				if (bList->data->applyFriction) {
					if (bList->data->GetVelocity().x < 0) {
						bList->data->frictionForce.x = bList->data->gravityForce.y * terrain->frictionC;
					}
					else if (bList->data->GetVelocity().x > 0) {
						bList->data->frictionForce.x = bList->data->gravityForce.y * -terrain->frictionC;
					}
				}
			}
			//LOG("Friction: %f", bList->data->frictionForce.x);

			//Addition of all the forces in order to calculate the acceleration

			float totalX = bList->data->gravityForce.x + bList->data->dragForce.x + bList->data->frictionForce.x + bList->data->jumpPlayerForce.x + bList->data->buoyancyForce.x;

			float totalY = bList->data->gravityForce.y + bList->data->dragForce.y + bList->data->frictionForce.y + bList->data->jumpPlayerForce.y + bList->data->buoyancyForce.y;

			//LOG("Total force: %f, %f", totalX, totalY);

			Vector acceleration = Vector(totalX / bList->data->mass, totalY / bList->data->mass);
			p2Point<float> currentPos = bList->data->GetPosition();

			Vector velocity = bList->data->GetVelocity(); 
			//LOG("Pos before moving: %f, %f", currentPos.x, currentPos.y);
			//LOG("Velocity before moving: %f, %f", velocity.x, velocity.y);
			//LOG("Acceleration before moving: %f, %f", acceleration.x, acceleration.y);


			switch (integMethod) {
			case(INTEGRATION_METHOD::BW_EULER):
				currentPos.x += velocity.x * App->frameDelay; 
				currentPos.y += velocity.y * App->frameDelay;

				velocity.x += acceleration.x * App->frameDelay;
				velocity.y += acceleration.y * App->frameDelay;
				//LOG("AAAAAA: %f, %f", acceleration.y * App->frameDelay, acceleration.y, App->frameDelay);
			case(INTEGRATION_METHOD::FW_EULER):
				velocity.x += acceleration.x * App->frameDelay;
				velocity.y += acceleration.y * App->frameDelay;

				currentPos.x += velocity.x * App->frameDelay;
				currentPos.y += velocity.y * App->frameDelay;
			case(INTEGRATION_METHOD::VERLET):
				currentPos.x += velocity.x * App->frameDelay + 0.5 * acceleration.x * App->frameDelay * App->frameDelay;
				currentPos.y += velocity.y * App->frameDelay + 0.5 * acceleration.y * App->frameDelay * App->frameDelay;

				velocity.x += acceleration.x * App->frameDelay;
				velocity.y += acceleration.y * App->frameDelay;
			default: 
				break; 
			}


			bList->data->SetPosition(currentPos);

			bList->data->SetVelocity(velocity); 

			bList->data->jumpPlayerForce.x = 0; 
			bList->data->jumpPlayerForce.y = 0; 
			bList->data->frictionForce.x = 0; 
			bList->data->applyFriction = false;
			bList->data->IsOnWater = false; 
			bList->data->buoyancyForce = { 0,0 };
		}
	}
}

void ModulePhysics::CollisionSolver(Body* b1, Body* b2) {

	if (b1->GetType() == PhysType::TARGET && (b2->GetType() == PhysType::PROJECTILE || b2->GetType() == PhysType::TELE_PROJECTILE)) {
		b2->bulletDamage *= 2;
		App->scene_intro->ChangeTargetPos();
		return;
	}
	if (b1->GetType() == PhysType::ENTITY && (b2->GetType() == PhysType::PROJECTILE || b2->GetType() == PhysType::TELE_PROJECTILE)) {
		if (b1 != b2->whoShotMe) {
			LOG("Wyh");
			App->player->HitPlayer(b1, b2);
			App->player->TurnFinished = true;
			App->player->turnTime = 1;
		}
		return;
	}
	if (b1->GetType() == PhysType::WATER) {
		if (allowHydro) {
			b2->IsOnWater = true;
			float waterTopLevel = water->waterBody->GetPosition().y + water->waterBody->GetHeight();

			float h = 2.0f * b2->radius;
			float surf = h * (waterTopLevel - b2->GetPosition().y);
			if ((b2->GetPosition().y + b2->radius) < waterTopLevel) {
				surf = h * h;
			}
			surf *= 0.004;
			LOG("Surf %f", surf);
			double buoyancyModulus = water->waterDensity * 10 * surf * 0.025;
			b2->buoyancyForce.x = 0;
			b2->buoyancyForce.y = -buoyancyModulus;
			LOG("Buoyancy: %f", buoyancyModulus);
		}
		return; 
	}

	switch (colSolMethod)
	{
	case COL_SOLVER_METHOD::TP_NORM_VEC:

		if (b1->GetType() == PhysType::WATER && b2->GetType() != PhysType::WATER) {
			//Apply buyancy force
			break;
		}

		if (b1->btype == BodyType::STATIC && b2->btype != BodyType::STATIC) {
			
			if (b2->GetShape() != Shape::CIRCLE) {

				float dY = b1->GetPosition().y - b2->GetPosition().y;

				p2Point<float> newPos;
				newPos.x = b2->GetPosition().x;
				newPos.y = b2->GetPosition().y - (b2->GetHeight() - dY);

				b2->SetPosition(newPos);
			}
			else if (b2->GetShape() == Shape::CIRCLE) {

				float dY = b1->GetPosition().y - b2->GetPosition().y;

				p2Point<float> newPos;
				newPos.x = b2->GetPosition().x;
				newPos.y = b2->GetPosition().y - (b2->GetRadius() - dY);

				b2->SetPosition(newPos);

				b2->speed.y = -b2->GetVelocity().y * b1->coefRest;
			}
			
		}

		if (b1->btype == BodyType::DYNAMIC && b2->btype == BodyType::DYNAMIC) {

			//Impulse each object on the contrary direction
		}

		break;
	case COL_SOLVER_METHOD::ITERATE_CONTACT_POINT:

		if (b1->GetType() == PhysType::WATER || b2->GetType() == PhysType::WATER) {
			//Apply buyancy force
			break;
		}

		if (b1->btype == BodyType::STATIC && b2->btype != BodyType::STATIC) {

			if (b2->GetVelocity().x == 0 && b2->GetVelocity().y == 0) 
				break;

			float normX = b2->GetVelocity().x / (sqrt(pow(b2->GetVelocity().x, 2) + pow(b2->GetVelocity().y, 2)));
			float normY = b2->GetVelocity().y / (sqrt(pow(b2->GetVelocity().x, 2) + pow(b2->GetVelocity().y, 2)));

			while (CheckCollisions(b1,b2)) {
		
				p2Point<float> newPos;
				newPos.x = b2->GetPosition().x - normX;
				newPos.y = b2->GetPosition().y - normY;

				b2->SetPosition(newPos);
			}
		}

		break;
	case COL_SOLVER_METHOD::BACK_TO_LAST_POINT:

		if (b1->GetType() == PhysType::WATER || b2->GetType() == PhysType::WATER) {
			//Apply buyancy force
			break;
		}

		if (b1->btype == BodyType::DYNAMIC && b2->btype == BodyType::DYNAMIC) {
			b1->position = b1->LastPosition;
			//b2->position = b2->LastPosition;

		}
		if (b1->btype == BodyType::DYNAMIC && b2->btype == BodyType::STATIC) {
			b1->position = b1->LastPosition;

		}
		if (b1->btype == BodyType::STATIC && b2->btype == BodyType::DYNAMIC) {
			b2->position = b2->LastPosition;

		}

		break;
	default:
		break;
	}

}

void ModulePhysics::CreateTerrain(p2Point<float> pos) {
	Body* terrain = new Body(); 

	terrain->SetPosition(pos); 
	terrain->SetVelocity(Vector(0, 0)); 

	terrain->SetWidth(300); 
	terrain->SetHeigth(SCREEN_HEIGHT - terrain->position.y); 

	terrain->SetMass(10000000000); //Value?

	terrain->btype = BodyType::STATIC; 
	terrain->shape = Shape::RECTANGLE; 
	terrain->type = PhysType::TERRAIN; 

	terrain->coefElastic = 0.5;
		
	bodyList.add(terrain);

	this->terrain = new Terrain(Vector(GRAVITY_X, GRAVITY_Y), 0, terrain);

	Body* water = new Body(); 
	p2Point<float> waterPos; 
	waterPos.x = terrain->GetPosition().x + terrain->GetWidth();
	waterPos.y = App->renderer->camera.h - 200;

	water->SetPosition(waterPos); 
	water->SetVelocity(Vector(0, 0));

	water->SetWidth(SCREEN_WIDTH - 600);
	water->SetHeigth(SCREEN_HEIGHT - water->position.y);

	water->btype = BodyType::STATIC;
	water->shape = Shape::RECTANGLE;
	water->type = PhysType::WATER;

	bodyList.add(water); 

	this->water = new Water(Vector(GRAVITY_X, GRAVITY_Y), 50.0f, water);

	Body* terrain2 = new Body();

	p2Point<float> terrPos;
	terrPos.x = terrain->GetPosition().x + water->GetWidth() + terrain->GetWidth();
	terrPos.y = App->renderer->camera.h - 200;

	terrain2->SetPosition(terrPos);
	terrain2->SetVelocity(Vector(0, 0));

	terrain2->SetWidth(400);
	terrain2->SetHeigth(SCREEN_HEIGHT - terrain2->position.y);

	terrain2->SetMass(10000000000); //Value?

	terrain2->btype = BodyType::STATIC;
	terrain2->shape = Shape::RECTANGLE;
	terrain2->type = PhysType::TERRAIN;

	terrain2->coefElastic = 0.5;

	bodyList.add(terrain2);

	this->terrain = new Terrain(Vector(GRAVITY_X, GRAVITY_Y), 0, terrain2);

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
void  Body::SetBodyType(BodyType bt) {
	this->btype = bt;
}


Body* ModulePhysics::CreateRectangle(int x, int y, int w, int h, PhysType type) {

	Body* body = new Body(x, y, w, h, type);
	body->SetVelocity(Vector(0, 0));
	body->SetMass(10); 

	body->btype = (body->GetType() != PhysType::TERRAIN) ? BodyType::DYNAMIC : BodyType::STATIC;
	
	bodyList.add(body);

	return body;
}
Body* ModulePhysics::CreateCircle(int x, int y, int radius, PhysType type) {

	Body* body = new Body(x, y, radius, type);
	body->SetVelocity(Vector(0, 0));
	body->SetMass(10);

	body->btype = (body->GetType() != PhysType::TERRAIN) ? BodyType::DYNAMIC : BodyType::STATIC;

	bodyList.add(body);

	return body;
}

void Body::OnCollision(Body* body2) {

}

float modulus(float vx, float vy)
{
	return std::sqrt(vx * vx + vy * vy);
}

