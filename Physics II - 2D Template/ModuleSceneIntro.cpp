#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include <time.h>



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
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

	//Water
	water = App->physics->CreateRectangle( 700, App->renderer->camera.h - 200, App->renderer->camera.w - 700,200 , PhysType::WATER);
	water->SetBodyType(BodyType::STATIC);

	target = App->physics->CreateCircle(100,500, 40, PhysType::TARGET);
	target->SetBodyType(BodyType::STATIC);

	return ret;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_STATE::KEY_DOWN) {
		ChangeTargetPos();
	}

	return UPDATE_CONTINUE;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleSceneIntro::ChangeTargetPos() {

	//srand(time(NULL));

	int rX = rand() % SCREEN_WIDTH;
	int rY = rand() % (SCREEN_HEIGHT - 600) + 200;

	LOG("XPOS: %i", rX);
	LOG("YPOS: %i", rY);
	p2Point<float> newPos;
	newPos.x = rX;
	newPos.y = rY;

	target->SetPosition(newPos);
	targetScore++;
}
