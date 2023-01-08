#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void FireBullet(int f);
	void GuideTeleBullet();
	void HitPlayer(Body* player, Body* bullet);

public:
	Body* pbody; 
	Body* pbody2;

	int health1;
	int health2;

	bool player1Turn;
	int turnTime;
	int Time = 0, timer = 0;

	bool alive = true; 

	bool destroyBullet; 

	bool TurnFinished = false;
	bool CanPlayerMove = true;

	int bulletCharge;

	Body* lastTeleBullet;

private:
	int WhiteFont;

};