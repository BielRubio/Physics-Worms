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
	App->physics->CreateRectangle( 0, App->renderer->camera.h - 200, App->renderer->camera.w,200 , PhysType::TERRAIN);
	
	App->physics->CreateCircle(300, 300, 50, PhysType::ENTITY);
	App->physics->CreateCircle(400, 300, 20, PhysType::PROJECTILE);

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

	return UPDATE_CONTINUE;
}
