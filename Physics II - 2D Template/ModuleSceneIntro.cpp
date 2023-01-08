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

	target = App->physics->CreateCircle(App->renderer->camera.w / 2,300, 40, PhysType::TARGET);
	target->SetBodyType(BodyType::STATIC);

	return ret;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_STATE::KEY_DOWN) {
		ChangeTargetPos();
	}

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_STATE::KEY_DOWN) {
		App->player->Disable();
		App->physics->Disable();
	}
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_STATE::KEY_DOWN) {
		App->player->Enable();
		App->physics->Enable();
	}

	return UPDATE_CONTINUE;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	delete target;
	target = nullptr;

	return true;
}

void ModuleSceneIntro::ChangeTargetPos() {

	//srand((unsigned)time(NULL));

	int rX = rand() % SCREEN_WIDTH;
	int rY = rand() % (SCREEN_HEIGHT - 600) + 200;

	LOG("XPOS: %i", rX);
	LOG("YPOS: %i", rY);
	p2Point<float> newPos;
	newPos.x = rX;
	newPos.y = rY;

	target->SetPosition(newPos);
}
