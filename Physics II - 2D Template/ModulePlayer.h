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

public:
	Body* pbody; 

	bool alive = true; 

	bool destroyBullet; 

	int bulletCharge;

	bool teleBullets;

	Body* lastTeleBullet;
};