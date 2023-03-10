#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleFonts.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	pbody = App->physics->CreateRectangle(70,500,30,60,PhysType::ENTITY);
	pbody2 = App->physics->CreateRectangle(App->renderer->camera.w - 100, 500, 30, 60, PhysType::ENTITY);

	//Initialize variables
	bulletCharge = 10;
	char lookupTable[] = { "abcdefghijklmnopqrstuvwxyz0123456789: " };
	WhiteFont = App->fonts->Load("../Assets/FontWhiteDef.png", lookupTable, 1);

	player1Turn = true;

	turnTime = 30;

	Time = 0;
	timer = 0;

	health1 = 100;
	health2 = 100;

	TurnFinished = false;
	CanPlayerMove = true;

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int speed = 5;

	//Movement
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT && TurnFinished == false && CanPlayerMove == true) {

		if (player1Turn) {
			p2Point<float> newPos;
			newPos.x = pbody->GetPosition().x - speed;
			newPos.y = pbody->GetPosition().y;

			pbody->SetPosition(newPos);
		}
		else {
			p2Point<float> newPos;
			newPos.x = pbody2->GetPosition().x - speed;
			newPos.y = pbody2->GetPosition().y;

			pbody2->SetPosition(newPos);
		}
		
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT && TurnFinished == false && CanPlayerMove == true) {
		if (player1Turn) {
			p2Point<float> newPos;
			newPos.x = pbody->GetPosition().x + speed;
			newPos.y = pbody->GetPosition().y;

			pbody->SetPosition(newPos);
		}
		else {
			p2Point<float> newPos;
			newPos.x = pbody2->GetPosition().x + speed;
			newPos.y = pbody2->GetPosition().y;

			pbody2->SetPosition(newPos);
		}
		
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_STATE::KEY_DOWN && TurnFinished == false && CanPlayerMove == true) {
		if (player1Turn) {
			pbody->jumpPlayerForce = { 0,-1000 };
		}
		else {
			pbody2->jumpPlayerForce = { 0,-1000 };
		}
	}

	//Fire Projectile
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_REPEAT && TurnFinished == false) {
		CanPlayerMove == false;
		if (player1Turn) {
			if (bulletCharge < 100)
				bulletCharge++;

			float vecX = App->input->GetMouseX() - pbody->GetPosition().x;
			float vecY = App->input->GetMouseY() - pbody->GetPosition().y;

			float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
			float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

			App->renderer->DrawLine(pbody->GetPosition().x, pbody->GetPosition().y, pbody->GetPosition().x + vecNormX * bulletCharge, pbody->GetPosition().y + vecNormY * bulletCharge, 200, 200, 0, 255);
		}
		else {
			if (bulletCharge < 100)
				bulletCharge++;

			float vecX = App->input->GetMouseX() - pbody2->GetPosition().x;
			float vecY = App->input->GetMouseY() - pbody2->GetPosition().y;

			float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
			float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

			App->renderer->DrawLine(pbody2->GetPosition().x, pbody2->GetPosition().y, pbody2->GetPosition().x + vecNormX * bulletCharge, pbody2->GetPosition().y + vecNormY * bulletCharge, 200, 200, 0, 255);
		}

	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_UP && TurnFinished == false) {
		FireBullet(bulletCharge);
		bulletCharge = 10;
		turnTime = 5;
		TurnFinished = true;
		CanPlayerMove == false;

	}
	//Fire tele-projectile
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_STATE::KEY_DOWN && TurnFinished == false) {
		CanPlayerMove == false;
		if (player1Turn) {
			lastTeleBullet = App->physics->CreateCircle(pbody->GetPosition().x, pbody->GetPosition().y, 10, PhysType::TELE_PROJECTILE);

			lastTeleBullet->SetMass(1);
			lastTeleBullet->whoShotMe = pbody;

			float vecX = App->input->GetMouseX() - pbody->GetPosition().x;
			float vecY = App->input->GetMouseY() - pbody->GetPosition().y;

			float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
			float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

			lastTeleBullet->jumpPlayerForce = { vecNormX * 100, vecNormY * 100 };
		}
		else {
			lastTeleBullet = App->physics->CreateCircle(pbody2->GetPosition().x, pbody2->GetPosition().y, 10, PhysType::TELE_PROJECTILE);

			lastTeleBullet->SetMass(1);
			lastTeleBullet->whoShotMe = pbody2;

			float vecX = App->input->GetMouseX() - pbody2->GetPosition().x;
			float vecY = App->input->GetMouseY() - pbody2->GetPosition().y;

			float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
			float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

			lastTeleBullet->jumpPlayerForce = { vecNormX * 100, vecNormY * 100 };
		}

	}
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_STATE::KEY_REPEAT && TurnFinished == false) {
		GuideTeleBullet();
		CanPlayerMove == false;
	}
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_STATE::KEY_DOWN && TurnFinished == false) {
		turnTime = 5;
		lastTeleBullet = nullptr;
		TurnFinished = true;
		CanPlayerMove == false;
	}
	
	//End turn
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_STATE::KEY_DOWN || turnTime <= 0) {
		player1Turn = (!player1Turn) ? true : false;
		turnTime = 30;
		TurnFinished = false;
		CanPlayerMove = true;
	}

	//Manage turnTime
	timer = SDL_GetTicks();
	if (timer > Time * 1000) {
		Time++;
		turnTime--;
	}

	// Fonts
	char charAux[12];
	sprintf_s(charAux, "%d", health1);
	App->fonts->BlitText(144, 0, WhiteFont, charAux);
	App->fonts->BlitText(0, 0, WhiteFont, "player 1 hp:");
	App->fonts->BlitText(846, 0, WhiteFont, "player 2 hp:");
	if (player1Turn == true) {
		App->fonts->BlitText(470, 0, WhiteFont, "p1:");
	}
	else {
		App->fonts->BlitText(470, 0, WhiteFont, "p2:");
	}
	sprintf_s(charAux, "%d", health2);
	App->fonts->BlitText(990, 0, WhiteFont, charAux);
	sprintf_s(charAux, "%d", turnTime);
	App->fonts->BlitText(506, 0, WhiteFont, charAux);
	if (App->physics->showState) {
		App->fonts->BlitText(0, 50, WhiteFont, "gravity: ");
		if (App->physics->allowGravity == true) {
			App->fonts->BlitText(95, 50, WhiteFont, "on");
		}
		else {
			App->fonts->BlitText(95, 50, WhiteFont, "off");
		}
		App->fonts->BlitText(0, 75, WhiteFont, "aerodinamics: ");
		if (App->physics->allowAero == true) {
			App->fonts->BlitText(155, 75, WhiteFont, "on");
		}
		else {
			App->fonts->BlitText(155, 75, WhiteFont, "off");
		}
		App->fonts->BlitText(0, 100, WhiteFont, "friction: ");
		if (App->physics->allowFriction == true) {
			App->fonts->BlitText(105, 100, WhiteFont, "on");
		}
		else {
			App->fonts->BlitText(105, 100, WhiteFont, "off");
		}
		App->fonts->BlitText(0, 125, WhiteFont, "hydrodinamics: ");
		if (App->physics->allowHydro == true) {
			App->fonts->BlitText(165, 125, WhiteFont, "on");
		}
		else {
			App->fonts->BlitText(165, 125, WhiteFont, "off");
		}
		sprintf_s(charAux, "x:%.0f y:%.0f", pbody->GetPosition().x, pbody->GetPosition().y);
		App->fonts->BlitText(310, 0, WhiteFont, charAux);
		App->fonts->BlitText(200,0,WhiteFont,"position: ");
		sprintf_s(charAux, "x:%.0f y:%.0f", pbody2->GetPosition().x, pbody2->GetPosition().y);
		App->fonts->BlitText(660, 0, WhiteFont, charAux);
		App->fonts->BlitText(550, 0, WhiteFont, "position: ");

	}

	return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading players");

	delete pbody;
	pbody = nullptr;

	delete pbody2;
	pbody2 = nullptr;

	if (lastTeleBullet != nullptr) {
		delete lastTeleBullet;
		lastTeleBullet = nullptr;
	}
	

	return true;
}

void ModulePlayer::FireBullet(int f) {
	
	if (player1Turn) {
		Body* bullet = App->physics->CreateCircle(pbody->GetPosition().x, pbody->GetPosition().y, 10, PhysType::PROJECTILE);

		bullet->SetMass(1);
		bullet->whoShotMe = pbody;

		float vecX = App->input->GetMouseX() - pbody->GetPosition().x;
		float vecY = App->input->GetMouseY() - pbody->GetPosition().y;

		float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
		float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

		bullet->jumpPlayerForce = { vecNormX * f * 20, vecNormY * f * 20 };
	}
	else {
		Body* bullet = App->physics->CreateCircle(pbody2->GetPosition().x, pbody2->GetPosition().y, 10, PhysType::PROJECTILE);

		bullet->SetMass(1);
		bullet->whoShotMe = pbody2;

		float vecX = App->input->GetMouseX() - pbody2->GetPosition().x;
		float vecY = App->input->GetMouseY() - pbody2->GetPosition().y;

		float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
		float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

		bullet->jumpPlayerForce = { vecNormX * f * 20, vecNormY * f * 20 };
	}

	
}

void ModulePlayer::GuideTeleBullet() {

	float vecX = App->input->GetMouseX() - lastTeleBullet->GetPosition().x;
	float vecY = App->input->GetMouseY() - lastTeleBullet->GetPosition().y;

	float vecNormX = vecX / sqrt(pow(vecX, 2) + pow(vecY, 2));
	float vecNormY = vecY / sqrt(pow(vecX, 2) + pow(vecY, 2));

	lastTeleBullet->jumpPlayerForce = { vecNormX * 100, vecNormY * 100 };
}

void ModulePlayer::HitPlayer(Body* player, Body* bullet) {
	if (player == pbody) {
		health1 -= bullet->bulletDamage;
	}
	if (player == pbody2) {
		health2 -= bullet->bulletDamage;
	}

	delete bullet;
	bullet = nullptr;
}




