#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Initialize bodies

	//Ground
	App->physics->CreateRectangle( 0,App->renderer->camera.h,App->renderer->camera.w,-200 , PhysType::TERRAIN);
	App->physics->CreateRectangle(0, App->renderer->camera.h-100, App->renderer->camera.w/2, -200, PhysType::PROJECTILE);
	App->physics->CreateCircle(300, 300, 50, PhysType::ENTITY);
	circ = App->physics->CreateCircle(400, 300, 20, PhysType::PROJECTILE);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT) {

		p2Point<float> newPos;
		newPos.x = circ->GetPosition().x - 1;
		newPos.y = circ->GetPosition().y;

		circ->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT) {
		p2Point<float> newPos;
		newPos.x = circ->GetPosition().x + 1;
		newPos.y = circ->GetPosition().y;

		circ->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT) {

		p2Point<float> newPos;
		newPos.x = circ->GetPosition().x;
		newPos.y = circ->GetPosition().y - 1;

		circ->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT) {
		p2Point<float> newPos;
		newPos.x = circ->GetPosition().x;
		newPos.y = circ->GetPosition().y + 1;

		circ->SetPosition(newPos);
	}

	return UPDATE_CONTINUE;
}
