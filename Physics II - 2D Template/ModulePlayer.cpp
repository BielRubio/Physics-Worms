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

	//Initialize variables
	bulletCharge = 10;

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int speed = 5;

	//Movement
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
	//Fire Projectile
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_REPEAT) {
		if (bulletCharge < 100)
			bulletCharge++;

		float vecX = App->input->GetMouseX() - pbody->GetPosition().x;
		float vecY = App->input->GetMouseY() - pbody->GetPosition().y;

		float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
		float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

		App->renderer->DrawLine(pbody->GetPosition().x, pbody->GetPosition().y, pbody->GetPosition().x + vecNormX * bulletCharge, pbody->GetPosition().y + vecNormY * bulletCharge, 200, 200, 0, 255);
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_UP) {
		FireBullet(bulletCharge);
		bulletCharge = 10;

	}

	return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

void ModulePlayer::FireBullet(int f) {
	
	Body* bullet = App->physics->CreateCircle(pbody->GetPosition().x, pbody->GetPosition().y,10,PhysType::PROJECTILE);

	float vecX = App->input->GetMouseX() - pbody->GetPosition().x;
	float vecY = App->input->GetMouseY() - pbody->GetPosition().y;

	float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
	float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

	bullet->jumpPlayerForce = { vecNormX * f, vecNormY * f };

	LOG("%f %f",bullet->jumpPlayerForce.x, bullet->jumpPlayerForce.y);

}




