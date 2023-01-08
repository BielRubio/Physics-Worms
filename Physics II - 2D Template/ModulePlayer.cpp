#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	pbody = App->physics->CreateRectangle(0,0,30,60,PhysType::ENTITY);
	
	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int speed = 5;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT) {

		p2Point<float> newPos;
		newPos.x = pbody->GetPosition().x - speed;
		newPos.y = pbody->GetPosition().y;

		pbody->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT) {
		p2Point<float> newPos;
		newPos.x = pbody->GetPosition().x + speed;
		newPos.y = pbody->GetPosition().y;

		pbody->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT) {

		p2Point<float> newPos;
		newPos.x = pbody->GetPosition().x;
		newPos.y = pbody->GetPosition().y - speed;

		pbody->SetPosition(newPos);
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT) {
		p2Point<float> newPos;
		newPos.x = pbody->GetPosition().x;
		newPos.y = pbody->GetPosition().y + speed;

		pbody->SetPosition(newPos);
	}
	LOG("Position: %f, %f", pbody->GetPosition().x, pbody->GetPosition().y); 
	return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}





