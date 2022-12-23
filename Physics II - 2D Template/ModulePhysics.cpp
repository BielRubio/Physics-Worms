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
		for (p2List_item<Body*>* bodyNode = bodyList.getFirst(); bodyNode->next != nullptr; bodyNode = bodyNode->next) {

			Body* body = bodyNode->data;

			//Color for every PhysType
			Uint8 RGBAlpha[4] = { 0,0,0,0 };
			switch (body->GetType()) {
			case (PhysType::TERRAIN):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 0;
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 0;
				break;
			case (PhysType::ENTITY):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 0;
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 0;
				break;
			case (PhysType::PROJECTILE):

				RGBAlpha[0] = 0;
				RGBAlpha[1] = 0;
				RGBAlpha[2] = 0;
				RGBAlpha[3] = 0;
				break;
			}

			if (body->GetShape() == Shape::RECTANGLE) {
				SDL_Rect rect = body->GetBounds();

				App->renderer->DrawQuad(rect, RGBAlpha[0], RGBAlpha[1], RGBAlpha[2], RGBAlpha[3]);
			}

			if (body->GetShape() == Shape::CIRCLE) {

				//App->renderer->DrawCircle(, colorAlpha[0], colorAlpha[1], colorAlpha[2], colorAlpha[3]);
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

Body* ModulePhysics::CreateRectangle() {

	Body* body = new Body();
	
	bodyList.add(body);

	return body;
}
Body* ModulePhysics::CreateCircle() {

	Body* body = new Body();

	bodyList.add(body);

	return body;
}
