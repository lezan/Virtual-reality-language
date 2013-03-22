
// Dark GDK - The Game Creators - www.thegamecreators.com
#include "header.h"

void DarkGDK (void) {

	bool isPaused = false;
	int volumeStartingSound=0;
	int counterVolumeStartingSound=0;

	srand(dbTimer());
	dbRandomize(dbTimer());

	int partita=0;

	dbSetDir ( "..\\..\\media\\" );
	
	//dbSetDisplayMode(1280,800, 32);
	dbSetDisplayModeVSync(1280,800,32,false);
	dbSetWindowSize(1280,800);
	dbSetWindowTitle("FPS Mentale");
	dbSetWindowPosition(0,0);

	// initial application set up	
	dbAutoCamOff();
	dbSyncOn         ( );				// turn sync on
	dbSyncRate       (0);			// set sync rate to 60
	dbSetCameraRange ( 0, 0.5f, 30000 );	// set the camera range
	dbBackdropOff    ();				// switch backdrop off - no need to clear screen
	
	dbLoadImage("menu\\presentazione.jpg",imagePresentation);
	dbPasteImage(imagePresentation,0,0);
	dbWait(5000);
	dbDeleteImage(imagePresentation);

	dbSync();
	dbSync();

	dbLoadImage("menu\\loading.jpg",imageLoading);
	dbPasteImage(imageLoading,0,0);
	dbDeleteImage(imageLoading);

	dbSync();
	dbSync();

	if(dbFileExist("sound\\start.wav")==1) {
		dbLoadSound("sound\\start.wav",soundStart);
		dbLoopSound(soundStart);
		dbSetSoundVolume(soundStart,100);
		volumeStartingSound=dbSoundVolume(soundStart);
	}
	else {
		return;
	}

	dbLoadImage("menu\\starting.jpg",imageStarting);

	dbLoadImage("menu\\pause.jpg",imagepause);
	dbSprite(spritePause,0,0,imagepause);
	dbSetSpriteAlpha(spritePause,100);
	dbHideSprite(spritePause);
	
	SC_Start();
	
	setupScene();
	makeCharacter();
	tempoCaricamento=dbTimer();
	makeEnemies();
	tempoCaricamento=dbTimer()-tempoCaricamento;
	makeBullets();
	makeBulletMarkers();
	makeClip();
	makeMedikit();
	makeWeapon();
	makeExplosion();
	
	if(bciOn==true) {
		thinkgear=new bci();
		thinkgear->getConnection();
		thinkgear->enableBlinkDetection();
		thinkgear->checkgAvailableFog();
	}

	dbPasteImage(imageStarting,0,0);
	dbDeleteImage(imageStarting);
	dbSync();
	
	dbSuspendForKey();	

	ptimer=dbTimer();	
	
	// loop round until escape key is pressed
	while ( LoopGDK ( ) ) {
	
		if ( dbEscapeKey ( ) ) {
			thinkgear->disconnected();
			return;
		}

		dbHideMouse();

		partita=checkGame();

		if(partita==1 || partita==2) {
			int t=dbSoundPlaying(soundDeathEnemy);
			int y=dbObjectFrame(mobs[lastEnemeyId].id);
			int u=dbSoundPlaying(soundDeathPlayer);
			//removeAllObjcets();
			if(partita==1 /*&& dbSoundPlaying(soundDeathPlayer)!=1*/ ) { //partita persa
				dbFlushVideoMemory();
				dbLoadImage("menu\\gameOver.jpg",imageGameOver);
				dbPasteImage(imageGameOver,0,0);				
			}
			else if(partita==2 && dbSoundPlaying(soundDeathEnemy)!=1 && checkEnemyStateAnimation(mobs[lastEnemeyId-firstEnemies].currentAnimation)!=ENEMY_DEAD) { //partita vinta	
				dbFlushVideoMemory();
				dbLoadImage("menu\\youWin.jpg",imageYouWin);
				dbPasteImage(imageYouWin,0,0);
							
			}
			if(dbReturnKey()==1 ) {
				return;
			}
			if(dbSpaceKey()==1 ) {
 				resetGame();
			}
		}
		else {

			if(dbSoundVolume(soundStart)==0) {
				dbStopSound(soundStart);
				dbDeleteSound(soundStart);
			}
			else {
				counterVolumeStartingSound+=1;
				if(counterVolumeStartingSound==1) {
					volumeStartingSound--;
				}
				else if(counterVolumeStartingSound==21) {
					volumeStartingSound--;
				}
				else if(counterVolumeStartingSound==41) {
					volumeStartingSound--;
				}
				else if(counterVolumeStartingSound==61) {
					volumeStartingSound--;
				}
				else if(counterVolumeStartingSound==81) {
					volumeStartingSound--;
					counterVolumeStartingSound=0;
				}
				dbSetSoundVolume(soundStart,volumeStartingSound);
			}

			if (dbKeyState(25) && (dbTimer() > ptimer + 500)) {
				ptimer = dbTimer();
				if (!isPaused) {		// If we are not paused
					isPaused = true;	// then pause the game...
				}
				else {		// But if we are
					isPaused = false;	// Un-pause the game
				}
			}

			if (isPaused) {
				dbShowSprite(3);
			} else {

				if(bciOn==true) {
					thinkgear->readData();
					thinkgear->updateFog();
				}

				dbHideSprite(3);

				printInfo();

				if(firstRunning==true && ground==1 && player.currentWeapon==1) {

					dbPositionSound(soundSelectElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
					dbPlaySound(soundSelectElite);

					player.mode=0;
					firstRunning=false;
					controllerAnimation.select=true;
				}

				updateCamera(dbMouseMoveX(),dbMouseMoveY());
				
				positionWeapon();

				moveEnemies();
				updateAnimationEnemy();
				moveCharacter();
				updateAnimationWeapon();
				
				updateBullets();
				updateExplosion();
				
				if(waterOn==true) {
					WaterEffect->UpdateWater();
				}
				
				removeMobs();
				resetBullets();

			}
			
			showFPS   ( );

			//dbSprite(1,dbMouseX()-16,dbMouseY()-16,3);
			dbSprite(spriteNoScope,dbScreenWidth()/2-16,dbScreenHeight()/2-16,imageCrosshairNoScope);
		}
		dbSync ( );
	}
}

// SETUP OBJECTS //
void setupScene(void) {

	// load images for the terrain
	dbLoadImage ( "mappa\\1\\texture.jpg", imageTexture );	// diffuse texture
	dbLoadImage ( "mappa\\1\\detail.jpg",  imageDetail );	// detail texture

	dbLoadImage	("player\\crosshair.png", imageCrosshairNoScope );
	dbLoadImage("player\\crosshairScoope2.png",imageCrosshairScope);
	
	if(blitzOn==true) {

		dbLoadImage("mappa\\1\\map.bmp",imageHeightmap);

		BT_Main BTMain;

		Terrain = BTMain.MakeTerrain();

		Terrain->SetHeightmap(imageHeightmap);
		Terrain->SetTexture(imageTexture);
		Terrain->SetDetail(imageDetail);

		Terrain->SetScale(3.0f);
		Terrain->SetYScale(0.6f);
		Terrain->SetSplit(8);
		Terrain->SetDetailTile(3.0f);

		Terrain->SetLOD(3); //3 LOD Levels.
		Terrain->SetLODDistance(1,1000.0f); //LOD Distances start at one and go up to 1 less the LOD level count
		Terrain->SetLODDistance(2,2000.0f);

		Terrain->SetSmoothing(1);

		/*Terrain->SetEnvironment(4);
		int EM_Grass=Terrain->AddEnvironment(dbRgb(0,128,0));
		int EM_Rock=Terrain->AddEnvironment(dbRgb(192,192,192));
		int EM_Sand=Terrain->AddEnvironment(dbRgb(128,64,0));*/

		Terrain->Build(fakeTerrain,true);

		Terrain->MakeObject(2,terrain);

		BTMain.EnableAutoRender(true);

		//dbPositionObject(terrain,0,0,0);
		//dbPositionObject(Terrain->GetObjectID(),0,0,0);
	}
	else {

		// create the terrain
		dbSetupTerrain        ( );										// set up terrain library
		dbMakeObjectTerrain   ( terrain );									// make a new terrain
		dbSetTerrainHeightMap ( terrain, "mappa\\1\\map.bmp" );					// set the terrain height map
		dbSetTerrainScale     ( terrain, 3.0,0.6,3.0/*19.535, 3.5, 19.535*/ );						// set the scale
		dbSetTerrainSplit     ( terrain, 16 );								// set the split value
		dbSetTerrainTiling    ( terrain, 4 );									// set the detail tiling
		dbSetTerrainLight     ( terrain, 1.0f, -0.25f, 0.0f, 1, 1, 0.78f, 0.5f );	// set the light
		dbSetTerrainTexture   ( terrain, imageTexture, imageDetail );				// set the textures
		dbBuildTerrain        ( terrain );								// build the terrain
		dbPositionObject(terrain,0,0,0);
	}

	SC_SetupComplexObject( terrain, groupWorld, 0 );

	// create our skybox
	dbLoadObject       ( "skybox\\3\\skybox.x", sky );		// load the skybox model
	dbSetObjectLight   ( sky, 0 );						// turn lighting off
	dbSetObjectTexture ( sky, imageDetail, 1 );					// set texture properties
	dbPositionObject   ( sky, 1000, 2000, 4000 );		// position the skybox
	dbScaleObject      ( sky, 30000, 30000, 30000 );	// and finally scale
	dbSetObjectCull(sky,0);

	SC_SetupComplexObject(sky, groupWorld, 2 );
	
	if(waterOn==true) {
		WaterEffect=new waterEffect(water,fxWater,500,512,16.286);
	}

	dbLoadSound("sound\\playerFireBullet.wav",soundFirePlayer);
	dbSetSoundVolume(soundFirePlayer,80);
	dbLoadSound("sound\\enemyFireBullet.wav",soundFireBulletEnemy);
	dbSetSoundVolume(soundFireBulletEnemy,80);
	dbLoadSound("sound\\enemyFireParticle.wav",soundFireParticleEnemey);
	dbSetSoundVolume(soundFireParticleEnemey,50);
	dbLoadSound("sound\\hitPlayer.wav",soundHitPlayer);
	dbSetSoundVolume(soundHitPlayer,100);
	dbLoadSound("sound\\hitEnemy.wav",soundHitEnemy);
	dbSetSoundVolume(soundHitEnemy,60);
	dbLoadSound("sound\\explosion.wav",soundExplosion);
	dbSetSoundVolume(soundExplosion,60);
	dbLoadSound("sound\\enemy.wav",soundEnemy);
	dbSetSoundVolume(soundEnemy,80);
	dbLoadSound("sound\\deathEnemy.wav",soundDeathEnemy);
	dbSetSoundVolume(soundDeathEnemy,80);
	dbLoadSound("sound\\deathPlayer.wav",soundDeathPlayer);
	dbSetSoundVolume(soundDeathPlayer,80);
	dbLoadSound("sound\\movementPlayer.wav",soundMovementPlayer);
	dbSetSoundVolume(soundMovementPlayer,80);
	dbLoadSound("sound\\sea.wav",soundSea);
	dbLoadSound("sound\\jumpPlayer.wav",soundJumpPlayer);
	dbSetSoundVolume(soundJumpPlayer,70);

	dbLoadSound("weapons\\elite\\dryfire.wav",soundDryFireElite);
	dbSetSoundVolume(soundDryFireElite,80);
	dbLoadSound("weapons\\elite\\fire.wav",soundFireElite);
	dbSetSoundVolume(soundFireElite,80);
	dbLoadSound("weapons\\elite\\putaway.wav",soundPutAwayElite);
	dbSetSoundVolume(soundPutAwayElite,80);
	dbLoadSound("weapons\\elite\\reload.wav",soundReloadElite);
	dbSetSoundVolume(soundReloadElite,80);
	dbLoadSound("weapons\\elite\\select.wav",soundSelectElite);
	dbSetSoundVolume(soundSelectElite,80);
}

void makeCharacter(void) {

	float posX=0;
	float posZ=0;
	float posY=0;

	dbMakeObjectSphere(character, radius*2.0f );
	dbHideObject(character);

	if(blitzOn==false) {
		posX=randFloatRange(0,dbObjectSizeX(terrain,1));
		posZ=randFloatRange(0,dbObjectSizeZ(terrain,1));
		posY=dbGetTerrainGroundHeight(terrain,posX,posZ);
	}
	else {
		posX=randFloatRange(0,Terrain->GetTerrainSize());
		posZ=randFloatRange(0,Terrain->GetTerrainSize());
		posY=Terrain->GetGroundHeight(posX,posZ);
	}
	dbPositionObject(character, posX, posY, posZ );
	SC_SetupObject(character, 0, 1 );

	dbMakeObjectSphere	(sphereCam, 20 );//make the object that follows the player, the camera follows this object
	dbScaleObject		(sphereCam, .0001, .0001, .0001 );//scale camera out of view .0001, .0001, .0001
	dbPositionObject	(sphereCam, dbObjectPositionX(character), dbObjectPositionY(character)+15, dbObjectPositionZ(character) );//position it where it goes
	
	//setup elite
	dbLoadObject("weapons\\elite\\HUD.x",weapon);
	dbFixObjectPivot(weapon);
	dbScaleObject(weapon,75,75,75);
	dbHideObject(weapon);
	dbDisableObjectZDepth(weapon);

	//setup m4
	//dbLoadObject("weapons\\m4\\m4.dbo",weapon);
	//dbZRotateObject(weapon,270);
	//dbScaleObject(weapon,150, 150, 150);

	player.health=100;
	player.vx=0;
	player.vy=0;
	player.vz=0;
	player.id=character;
	player.speedFire=speedFirePlayer;
	player.damage=95;
	player.mode=1;
	player.nCaricatori=3;
	player.currentBullets=15;
	player.currentWeapon=0;

	controllerAnimation.select=false;
	controllerAnimation.idle=false;
	controllerAnimation.movement=false;
	controllerAnimation.fire=false;
	controllerAnimation.reload=false;
	controllerAnimation.putaway=false;

	inventory.weapon1=true;
	inventory.weapon2=false;
}

void makeEnemies(void) {

	float posX=0;
	float posZ=0;
	float posY=0;

	dbLoadObject("enemies//H-Alien Mutant-Idle.dbo",firstEnemies);
	dbAppendObject("enemies//H-Alien Mutant-Attack1.dbo",firstEnemies,dbTotalObjectFrames(firstEnemies)+1);
	dbAppendObject("enemies//H-Alien Mutant-Impact.dbo",firstEnemies,dbTotalObjectFrames(firstEnemies)+1);
	dbAppendObject("enemies//H-Alien Mutant-Move.dbo",firstEnemies,dbTotalObjectFrames(firstEnemies)+1);
	dbAppendObject("enemies//H-Alien Mutant-Static.dbo",firstEnemies,dbTotalObjectFrames(firstEnemies)+1);
	dbAppendObject("enemies//H-Alien Mutant-Die.dbo",firstEnemies,dbTotalObjectFrames(firstEnemies)+1);

	dbLoadImage("enemies//alien2.dds",imageTextureEnemies);
	dbTextureObject(firstEnemies,imageTextureEnemies);

	//srand(dbTimer());
	//dbRandomize(dbTimer());

	for(int i=0; i<numEnemies; i++) {
		mobs[i].id=i+firstEnemies;
		mobs[i].health=100;
		mobs[i].mode=0;
		//mobs[i].vxEnemies=0;
		//mobs[i].vyEnemies=0;
		//mobs[i].vzEnemies=0;
		mobs[i].velocityEnemies=0.2f;
		mobs[i].timeIdle=0;
		mobs[i].timeMove=0;
		mobs[i].speedFire=speedFireEnemy;
		//srand(dbTimer());
		mobs[i].accuracyX=randFloatRange(-10,10);
		mobs[i].accuracyY=randFloatRange(-10,10);
		mobs[i].accuracyZ=randFloatRange(-10,10);
		mobs[i].timerUpdateAnimation=0;
		mobs[i].currentAnimation=0;
		mobs[i].hit=0;
		mobs[i].speedFireParticle=0;
		mobs[i].timerSoundEnemy=500;
		mobs[i].timerHealth=0;
		mobs[i].recoveryHealth=false;
		mobs[i].controlDirection=false;

		if(i>0) {
			dbCloneObject(i+firstEnemies,firstEnemies);
		}
		else {
			dbTurnObjectLeft(i+firstEnemies, 180);
		}
		dbFixObjectPivot(i+firstEnemies);
		dbScaleObject(i+firstEnemies,1000,1000,1000);
		if(blitzOn==false) {
			posX=randFloatRange(0,dbObjectSizeX(terrain,1));
			posZ=randFloatRange(0,dbObjectSizeZ(terrain,1));
			posY=dbGetTerrainGroundHeight(terrain,posX,posZ);
		}
		else {
			posX=randFloatRange(0,Terrain->GetTerrainSize());
			posZ=randFloatRange(0,Terrain->GetTerrainSize());
			posY=Terrain->GetGroundHeight(posX,posZ);
		}
		dbPositionObject(i+firstEnemies, posX, posY, posZ );
		SC_SetupObject( i+firstEnemies, groupEnemy, 1 );

		controllerEnemy[i].idle=false;
		controllerEnemy[i].patrol=false;
		controllerEnemy[i].chase=false;
		controllerEnemy[i].fire=false;
		controllerEnemy[i].getAway=false;
		controllerEnemy[i].impact=false;
		controllerEnemy[i].dead=false;
	}
}
void makeBullets(void) {

	dbMakeObjectSphere(firstBullet,1);
	
	//dbLoadObject("weapons\\m4\\bulletOne.dbo",firstGraphicBullet);

	for(int i=0; i<maxBullets; i++) {
		bullets[i].id=i+firstBullet;
		bullets[i].life=1000;
		bullets[i].oldX=0;
		bullets[i].oldY=0;
		bullets[i].oldZ=0;
		bullets[i].coordX=0;
		bullets[i].coordY=0;
		bullets[i].coordZ=0;
		bullets[i].targetId=0;
		bullets[i].sourceId=0;
		bullets[i].active=false;
		bullets[i].damage=0;
		bullets[i].type=0;
		bullets[i].velocity=10;
		bullets[i].velocityY=bullets[i].velocity*dbSIN(tiltAngle);
		//bullets[i].graphicId=i+firstGraphicBullet;
		bullets[i].explosion=false;

		if(i>0) {
			dbCloneObject(i+firstBullet,firstBullet);
			//dbInstanceObject(i+firstBullet,firstBullet);
			//dbCloneObject(i+firstGraphicBullet,firstGraphicBullet);
		}
		dbColorObject	   ( i+firstBullet, dbRgb(255, 0, 0) );
		dbHideObject(i+firstBullet);
		//dbHideObject(i+firstGraphicBullet);
		SC_SetupObject(i+firstBullet,groupBullets,0);
	}
}

void makeBulletMarkers(void) {

	dbMakeObjectPlain(firstBulletMarker,0.5,0.5 );

	for( int i=0; i<maxBulletMarker; i++ )	{
		if(i>0) {
			dbCloneObject(i+firstBulletMarker,firstBulletMarker);
		}
		dbColorObject(i+firstBulletMarker,dbRGB(0,0,0));
		dbSetObjectLight( i+firstBulletMarker,0 );
		dbHideObject( i+firstBulletMarker );
		SC_SetupObject(i+firstBulletMarker,groupBulletMarkers,0);
	}
}

void makeClip(void) {
	
	dbLoadObject("weapons\\elite\\Elite_ammo.X",firstBulletClip);
	dbScaleObject(firstBulletClip,75,75,75);
	//srand(dbTimer());
	for(int i=0; i<maxBulletClip; i++) {

		clip[i].id=firstBulletClip+i;
		clip[i].state=0; //sono a terra e visibili
		clip[i].nBullets=15;
		clip[i].type=1;

		if(blitzOn==false) {
			clip[i].positionX=randFloatRange(0,dbObjectSizeX(terrain,1));
			clip[i].positionZ=randFloatRange(0,dbObjectSizeZ(terrain,1));
			clip[i].positionY=dbGetTerrainGroundHeight(terrain,clip[i].positionX,clip[i].positionZ);
		}
		else {
			clip[i].positionX=randFloatRange(0,Terrain->GetTerrainSize());
			clip[i].positionZ=randFloatRange(0,Terrain->GetTerrainSize());
			clip[i].positionY=Terrain->GetGroundHeight(clip[i].positionX,clip[i].positionZ);
		}
		
		if(i>0) {
			dbInstanceObject(firstBulletClip+i,firstBulletClip);
			//dbCloneObject(firstBulletClip+i,firstBulletClip);
		}

		dbPositionObject(i+firstBulletClip,clip[i].positionX,clip[i].positionY,clip[i].positionZ);

		clip[i].ray=dbObjectSizeX(i+firstBulletClip,1);

		//SC_SetupObject(i+firstBulletClip,0,2);
	}
}
void makeMedikit(void) {
	
	dbLoadObject("misc\\army.x",firstMedikit);
	dbScaleObject(firstMedikit,30,30,30);

	dbLoadImage("misc\\army_d2.dds",imageTextureMedikit);
	dbTextureObject(firstMedikit,imageTextureMedikit);
	
	for(int i=0; i<maxMedikit; i++) {

		medikit[i].id=firstMedikit+i;
		medikit[i].state=0; //sono a terra e visibili

		if(blitzOn==false) {
			medikit[i].positionX=randFloatRange(0,dbObjectSizeX(terrain,1));
			medikit[i].positionZ=randFloatRange(0,dbObjectSizeZ(terrain,1));
			medikit[i].positionY=dbGetTerrainGroundHeight(terrain,medikit[i].positionX,medikit[i].positionZ);
		}
		else {
			medikit[i].positionX=randFloatRange(0,Terrain->GetTerrainSize());
			medikit[i].positionZ=randFloatRange(0,Terrain->GetTerrainSize());
			medikit[i].positionY=Terrain->GetGroundHeight(medikit[i].positionX,medikit[i].positionZ);
		}
		
		if(i>0) {
			dbInstanceObject(firstMedikit+i,firstMedikit);
			//dbCloneObject(firstMedikit+i,firstMedikit);
		}

		dbScaleObject(i+firstMedikit,30,30,30);

		dbPositionObject(i+firstMedikit,medikit[i].positionX,medikit[i].positionY,medikit[i].positionZ);

		medikit[i].ray=dbObjectSizeX(i+firstMedikit,1);

		//SC_SetupObject(i+firstBulletmedikit,0,2);
	}
}
void makeWeapon(void) {
	
	dbLoadObject("weapons\\elite\\Elite.X",firstWeapon);
	dbScaleObject(firstWeapon,80,80,80);

	/*dbLoadImage("misc\\army_d2.dds",imageTextureMedikit);
	dbTextureObject(firstMedikit,imageTextureMedikit);*/
	
	for(int i=0; i<maxWeapon; i++) {

		weapons[i].id=firstWeapon+i;
		weapons[i].state=0; //sono a terra e visibili
		weapons[i].type=1;

		if(blitzOn==false) {
			weapons[i].positionX=randFloatRange(0,dbObjectSizeX(terrain,1));
			weapons[i].positionZ=randFloatRange(0,dbObjectSizeZ(terrain,1));
			weapons[i].positionY=dbGetTerrainGroundHeight(terrain,weapons[i].positionX,weapons[i].positionZ);
		}
		else {
			weapons[i].positionX=randFloatRange(0,Terrain->GetTerrainSize());
			weapons[i].positionZ=randFloatRange(0,Terrain->GetTerrainSize());
			weapons[i].positionY=Terrain->GetGroundHeight(weapons[i].positionX,weapons[i].positionZ);
		}
		
		if(i>0) {
			dbInstanceObject(firstWeapon+i,firstWeapon);
			//dbCloneObject(firstWeapon+i,firstWeapon);
		}

		dbScaleObject(i+firstWeapon,80,80,80);

		dbPositionObject(i+firstWeapon,weapons[i].positionX,weapons[i].positionY,weapons[i].positionZ);

		weapons[i].ray=dbObjectSizeX(i+firstWeapon,1);

		//SC_SetupObject(i+firstBulletmedikit,0,2);
	}
}


void makeExplosion(void) {

	dbLoadImage("misc\\fire.bmp",imageExplosion);

	dbMakeObjectSphere(firstExplosion,20);
	dbTextureObject(firstExplosion,imageExplosion);

	for(int i=0; i<maxExplosion; i++) {
		if(i>0) {
			dbInstanceObject(i+firstExplosion,firstExplosion);
		}
		dbGhostObjectOn(i+firstExplosion);
		dbHideObject(i+firstExplosion);
		explosion[i].id=i+firstExplosion;
		explosion[i].state=true;
		explosion[i].x=0;
		explosion[i].y=0;
		explosion[i].z=0;
		explosion[i].timer=timerExplosion;
		explosion[i].eScale=20*(30-explosion[i].timer);
	}
}
// MOVEMENT //
void inputHandler(void) {

	float angy = dbObjectAngleY(character);
	player.vx = 0;
	player.vz = 0;
	
	if(player.currentWeapon==1 && !(checkPlayerStateAnimation(player.currentAnimation)==SELECT) && !(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT) && !(checkPlayerStateAnimation(player.currentAnimation)==FIRE) && !(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && !(checkPlayerStateAnimation(player.currentAnimation)==PUTAWAY)) {
		player.mode=1;
		controllerAnimation.idle=true;
		player.currentAnimation=MIN_IDLE;
	}

	if (dbKeyState(32) == 1 ) {
		if(dbSoundPlaying(soundMovementPlayer)==0) {
			dbPositionSound(soundMovementPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
			dbPlaySound(soundMovementPlayer);
		}
		player.vx = player.vx + dbCos(angy); 
		player.vz = player.vz - dbSin(angy);
		if(!(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT) {
			player.mode=2;
			controllerAnimation.movement=true;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)) {
				player.currentAnimation=MIN_MOVEMENT;
			}
		}
	}
	if (dbKeyState(30) == 1 ) {
		if(dbSoundPlaying(soundMovementPlayer)==0) {
			dbPositionSound(soundMovementPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
			dbPlaySound(soundMovementPlayer);
		}
		player.vx = player.vx - dbCos(angy); 
		player.vz = player.vz + dbSin(angy);
		if(!(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT) {
			player.mode=2;
			controllerAnimation.movement=true;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)) {
				player.currentAnimation=MIN_MOVEMENT;
			}
		}
	}
	if (dbKeyState(31) == 1 ) {
		if(dbSoundPlaying(soundMovementPlayer)==0) {
			dbPositionSound(soundMovementPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
			dbPlaySound(soundMovementPlayer);
		}
		player.vx = player.vx - dbSin(angy); 
		player.vz = player.vz - dbCos(angy);
		if(!(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT) {
			player.mode=2;
			controllerAnimation.movement=true;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)) {
				player.currentAnimation=MIN_MOVEMENT;
			}
		}
	}
	if (dbKeyState(17) == 1 ) {
		if(dbSoundPlaying(soundMovementPlayer)==0) {
			dbPositionSound(soundMovementPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
			dbPlaySound(soundMovementPlayer);
		}
		player.vx = player.vx + dbSin(angy); 
		player.vz = player.vz + dbCos(angy);
		if(!(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT) {
			player.mode=2;
			controllerAnimation.movement=true;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)) {
				player.currentAnimation=MIN_MOVEMENT;
			}
		}
	}

	if(dbShiftKey()==1 && (dbKeyState(17) == 1 || dbKeyState(31) == 1 || dbKeyState(30) == 1 || dbKeyState(32) == 1 )) {
		player.vx=player.vx*1.8;
		player.vz=player.vz*1.8;

		if(!(checkPlayerStateAnimation(player.currentAnimation)==RELOAD) && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT /*|| !(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)*/) {
			player.mode=6;
			controllerAnimation.movement=true;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==MOVEMENT)) {
				player.currentAnimation=MIN_MOVEMENT;
			}
		}
	}

	if(dbKeyState(19)==1 && player.currentWeapon==1 && reloadTimer==0 && player.nCaricatori>0 && checkPlayerStateAnimation(player.currentAnimation)!=RELOAD && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT ) { //tasto "R", serve per ricare l'arma
		dbPositionSound(soundReloadElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
		dbPlaySound(soundReloadElite);
		player.nCaricatori--;
		player.currentBullets=15;
		player.mode=4;
		controllerAnimation.reload=true;
		reloadTimer=120;
		player.currentAnimation=MIN_RELOAD;
	}
	else if(reloadTimer>0) {
		reloadTimer--;
	}

	if(bciOn==true) { 
		if(thinkgear->checkBlinkDetection() && player.currentWeapon==1 && reloadTimer==0 && player.nCaricatori>0 && checkPlayerStateAnimation(player.currentAnimation)!=RELOAD && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT ) { //tasto "R", serve per ricare l'arma
			dbPositionSound(soundReloadElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
			dbPlaySound(soundReloadElite);
			player.nCaricatori--;
			player.currentBullets=15;
			player.mode=4;
			controllerAnimation.reload=true;
			reloadTimer=120;
			player.currentAnimation=MIN_RELOAD;
		}
		else if(reloadTimer>0) {
			reloadTimer--;
		}
	}

	if(player.currentBullets==0 && player.nCaricatori==0 && player.currentWeapon==1) {
		putAwayOn=true;
	}

	if(putAwayOn==true) {
		
		if(!(checkPlayerStateAnimation(player.currentAnimation)==PUTAWAY)) {
			dbPositionSound(soundPutAwayElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
			dbPlaySound(soundPutAwayElite);
			player.mode=5;
			player.currentWeapon=0;
			controllerAnimation.putaway=true;
			player.currentAnimation=MIN_PUTAWAY;
		}

	}

	/*if(player.currentWeapon==0 && player.nCaricatori>0) {
		player.currentWeapon=1;
		dbPositionSound(soundSelectElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
		dbPlaySound(soundSelectElite);
		player.mode=0;
		controllerAnimation.select=true;
		player.currentAnimation=MIN_SELECT;
	}*/

	if(dbKeyState(2)==1) {//l'utente ha premuto il tasto uno e vuole cambiare l'arma attuale con quella del tasto 1 (elite singola)

		if(player.currentWeapon!=1 && inventory.weapon1==true && player.nCaricatori>0) {
			player.mode=0;
			controllerAnimation.select=true;
			player.currentWeapon=1;
			if(!(checkPlayerStateAnimation(player.currentAnimation)==SELECT)) {
				player.currentAnimation=MIN_SELECT;
			}
		}
	}

	if(dbKeyState(11)==1 && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY && checkPlayerStateAnimation(player.currentAnimation)!=SELECT && checkPlayerStateAnimation(player.currentAnimation)!=FIRE && checkPlayerStateAnimation(player.currentAnimation)!=RELOAD) { //l'utente ha premuto il tasto 0 e vuole disarmarsi qualunque sia l'arma da lui posseduta
		
		if(player.currentWeapon==1) {
			dbPositionSound(soundPutAwayElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
			dbPlaySound(soundPutAwayElite);
			player.mode=5;
			player.currentWeapon=0;
			controllerAnimation.putaway=true;
			player.currentAnimation=MIN_PUTAWAY;
		}
		else if(player.currentWeapon==2) {
		}

	}

	if (debug==true) {
		if(dbKeyState(22)==1) { //super velocità per il debug
			player.vx=player.vx*10;
			player.vz=player.vz*10;
		}

		if(dbKeyState(21)==1) { //cambia l'angolo del mob per il debug
			dbYRotateObject(firstEnemies,dbWrapValue(dbObjectAngleY(firstEnemies)+1));
		}

		if(dbKeyState(20)==1) { //aumenta la velocità dei mob per il debug
			mobs[0].velocityEnemies+=0.01;
		}
		if(dbKeyState(34)==1) { //diminuisce la velocità dei mob per il debug
			mobs[0].velocityEnemies-=0.01;
		}
	}

	if ( ground == 1 ) {
		if ( dbSpaceKey() == 1 && jumptimer == 0 ) {
			if(dbSoundPlaying(soundJumpPlayer)==0) {
				dbPositionSound(soundJumpPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
				dbPlaySound(soundJumpPlayer);
			}
			player.vy = player.vy + 3.0f;
			jumptimer = 20;
		}
	}

	if(mouseFullClick(1) && player.speedFire==0 && checkPlayerStateAnimation(player.currentAnimation)!=RELOAD && player.currentWeapon!=0) {
		if(player.currentBullets>0) {
			if(player.currentAnimation>=75 || player.currentAnimation<=154) {
				player.speedFire=speedFirePlayer;
				shootBullets();
				if(player.currentWeapon==3) {
					dbPositionSound(soundFirePlayer,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
					dbPlaySound(soundFirePlayer);
				}
				else {
					dbPositionSound(soundFireElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
					dbPlaySound(soundFireElite);
				}
				player.mode=3;
				controllerAnimation.fire=true;
				if(!(checkPlayerStateAnimation(player.currentAnimation)==FIRE)) {
					player.currentAnimation=MIN_FIRE;
				}
				player.currentBullets--;
			}
		}
		else {
			dbPositionSound(soundDryFireElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
			dbPlaySound(soundDryFireElite);
		}
	}
	else if(player.speedFire>0) {
		player.speedFire--;
	}
	
	if(bciOn==true) {
		if((thinkgear->getLevelMeditation()>50) && player.speedFire==0 && checkPlayerStateAnimation(player.currentAnimation)!=RELOAD && player.currentWeapon!=0) {
			if(player.currentBullets>0) {
				if(player.currentAnimation>=75 || player.currentAnimation<=154) {
					player.speedFire=speedFirePlayer;
					shootBullets();
					if(player.currentWeapon==3) {
						dbPositionSound(soundFirePlayer,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
						dbPlaySound(soundFirePlayer);
					}
					else {
						dbPositionSound(soundFireElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
						dbPlaySound(soundFireElite);
					}
					player.mode=3;
					controllerAnimation.fire=true;
					if(!(checkPlayerStateAnimation(player.currentAnimation)==FIRE)) {
						player.currentAnimation=MIN_FIRE;
					}
					player.currentBullets--;
				}
			}
			else {
				dbPositionSound(soundDryFireElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
				dbPlaySound(soundDryFireElite);
			}
		}
		else if(player.speedFire>0) {
			player.speedFire--;
		}
	}

	if(mouseFullClick(2) && ztimer < dbTimer() && view==0) {
		zoom=1-zoom;
		ztimer = dbTimer() + 300;
	}

	if(view==0 && zoom==1 ) {   //prima persona e zoom attivo, allora zoomma
		if(dbSpriteVisible(1)==1) {
			dbHideSprite(1);
		}
		dbSprite(spriteScope,dbMouseX()-300,dbMouseY()-300,imageCrosshairScope);
		dbSetSpriteAlpha(2,100);
		dbScaleSprite(2,200);
		if(dbSpriteVisible(2)==0) {
			dbShowSprite(2);
		}
		dbSetCameraFOV(25.0f);
		
		/*if(dbMouseClick()==3 || dbMouseClick()==4 || dbMouseClick()==5) {
			dbMoveCameraUp(0,dbMouseMoveZ()/120);
		}*/

	}
	else if(view==0 && zoom==0) {  //prima persona e zoom inattivo, allora non zoommare
		if(dbSpriteVisible(2)==1) {
			dbHideSprite(2);
		}
		if(dbSpriteVisible(1)==0) {
			dbShowSprite(1);
		}
		dbSetCameraFOV(61.95/*80.0f*/);
	}

	/*if( strcmp(dbInKey(), "z") == 0 && ztimer < dbTimer()) {
		ztimer = dbTimer() + 300;
		zoom=1-zoom;
	}*/

	if ( strcmp(dbInKey(), "v") == 0 && vtimer < dbTimer() && zoom!=1) {
		vtimer = dbTimer() + 300;
		view = 1 - view;
	}

	SC_UpdateObject(character);
}
void moveEnemies(void) {
	for( int i = firstEnemies; i < firstEnemies+numEnemies; i++ ) {
		if(dbObjectExist(i)==1) {
			
			if(mobs[i-firstEnemies].timerSoundEnemy==0) {
				dbPlaySound(soundEnemy);
				mobs[i-firstEnemies].timerSoundEnemy=1000;
			}
			else {
				mobs[i-firstEnemies].timerSoundEnemy--;
			}

			float oldx = dbObjectPositionX(i);
			float oldy = dbObjectPositionY(i);
			float oldz = dbObjectPositionZ(i);

			if(distanceBetween2Points(dbObjectPositionX(i),dbObjectPositionY(i),dbObjectPositionZ(i),dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character))<=maxDistance) {
				mobs[i-firstEnemies].mode=2; //insegue
				if(distanceBetween2Points(dbObjectPositionX(i),dbObjectPositionY(i),dbObjectPositionZ(i),dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character))<=maxAttack) {
					mobs[i-firstEnemies].mode=3; //attacca
				}
			}
			else {
				if(mobs[i-firstEnemies].timeIdle==0) {
					mobs[i-firstEnemies].mode=1; //vai in mode normal e comincia a muoverti
					mobs[i-firstEnemies].timeIdle=(int)300*randFloatRange(0.0,1.0);
				}
				else {
					if(mobs[i-firstEnemies].timeMove==0) {
						mobs[i-firstEnemies].mode=0; //stai in idle mode, cioè fermo sul posto
					}				
				}
			}

			if(mobs[i-firstEnemies].health<=20) {
				mobs[i-firstEnemies].mode=4; //mode 4 significa che la vita del mob è minore di 20 e quindi fugge
				//dbYRotateObject(mobs[i-firstEnemies].id,dbObjectAngleY(character)); //fugge nella direzione opposto al player
				if(60>distanceBetween2Points(dbObjectPositionX(mobs[i-firstEnemies].id),dbObjectPositionY(mobs[i-firstEnemies].id),dbObjectPositionZ(mobs[i-firstEnemies].id),dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character))) {
					mobs[i-firstEnemies].controlDirection=true;
				}
				mobs[i-firstEnemies].recoveryHealth=true;
			}

			if(mobs[i-firstEnemies].timerHealth==0 && mobs[i-firstEnemies].health<=50 && mobs[i-firstEnemies].recoveryHealth==true ) {
				mobs[i-firstEnemies].health++;
				if(mobs[i-firstEnemies].health<=20) {
					mobs[i-firstEnemies].timerHealth=150;
				}
				else {
					mobs[i-firstEnemies].timerHealth=300;
				}
			}
			else if(mobs[i-firstEnemies].timerHealth>0) {
				mobs[i-firstEnemies].timerHealth--;				
			}
			else if(mobs[i-firstEnemies].health>50) {
				mobs[i-firstEnemies].recoveryHealth=false;
			}
			else if(mobs[i-firstEnemies].health>20) {
				mobs[i-firstEnemies].controlDirection=false;
			}

			if (dbObjectPositionX(mobs[i-firstEnemies].id)<0) {
				dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
			}

			if (dbObjectPositionZ(mobs[i-firstEnemies].id)<0) {
				dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
			}

			if(blitzOn==false) {
				if (dbObjectPositionX(mobs[i-firstEnemies].id)>(int)dbObjectSizeX(terrain,1)) {
					dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
				}

				if (dbObjectPositionZ(mobs[i-firstEnemies].id)>(int)dbObjectSizeZ(terrain,1)) {
					dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
				}
			}
			else {
				if (dbObjectPositionX(mobs[i-firstEnemies].id)>((int)Terrain->GetTerrainSize())) {
					dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
				}

				if (dbObjectPositionZ(mobs[i-firstEnemies].id)>((int)Terrain->GetTerrainSize())) {
					dbYRotateObject(i,dbWrapValue(dbObjectAngleY(mobs[i-firstEnemies].id)+180*randFloatRange(0.0,1.0)));
				}
			}

			switch(mobs[i-firstEnemies].mode) {
				case 0:
					dbMoveObject(i,0);
					//sta fermo
					break;
				case 1:
					if(mobs[i-firstEnemies].timeMove==0) {
						srand(dbTimer());
						dbYRotateObject(i,dbWrapValue(dbObjectAngleY(i) + rand()%360));
						mobs[i-firstEnemies].timeMove=(int)800*randFloatRange(0.0,1.0);
					}
					dbMoveObject(i,mobs[i-firstEnemies].velocityEnemies);
					//lancia particelle laser a 360° e si muove
					if(mobs[i-firstEnemies].speedFireParticle==0) {
						dbPositionSound(soundFireParticleEnemey,dbObjectPositionX(mobs[i-firstEnemies].id),dbObjectPositionY(mobs[i-firstEnemies].id),dbObjectPositionZ(mobs[i-firstEnemies].id));
						dbPlaySound(soundFireParticleEnemey);
						mobsAttack(mobs[i-firstEnemies].id,1);
						mobs[i-firstEnemies].speedFireParticle=speedFireEnemy;
					}
					else {
						mobs[i-firstEnemies].speedFireParticle--;
					}
					break;
				case 2:
					dbPointObject(i, dbObjectPositionX(character), dbObjectPositionY(character), dbObjectPositionZ(character) );
					dbMoveObject(i,mobs[i-firstEnemies].velocityEnemies );
					//insegue senza attaccare 
					break;
				case 3:
					dbPointObject(i, dbObjectPositionX(character), dbObjectPositionY(character), dbObjectPositionZ(character) );
					dbMoveObject(i,mobs[i-firstEnemies].velocityEnemies );
					if(mobs[i-firstEnemies].speedFire==0) {
						mobs[i-firstEnemies].speedFire=speedFireEnemy;
						srand(dbTimer());
						mobs[i-firstEnemies].accuracyX=randFloatRange(-10,10);
						mobs[i-firstEnemies].accuracyY=randFloatRange(-10,10);
						mobs[i-firstEnemies].accuracyZ=randFloatRange(-10,10);
						dbPositionSound(soundFireBulletEnemy,dbObjectPositionX(mobs[i-firstEnemies].id),dbObjectPositionY(mobs[i-firstEnemies].id),dbObjectPositionZ(mobs[i-firstEnemies].id));
						dbPlaySound(soundFireBulletEnemy);
						mobsAttack(mobs[i-firstEnemies].id,0);
					}
					else {
						mobs[i-firstEnemies].speedFire--;
					}
					break;
				case 4:
					if(mobs[i-firstEnemies].controlDirection==true) {
						dbYRotateObject(i,dbObjectAngleY(character));
						mobs[i-firstEnemies].controlDirection=false;
					}
					dbMoveObject(i,mobs[i-firstEnemies].velocityEnemies*2);
					break;
				/*default:
					dbMoveObject(i,mobs[i-firstEnemies].velocityEnemies );*/
			}

			if(mobs[i-firstEnemies].timeMove>0) {
				mobs[i-firstEnemies].timeMove--;
			}
			if(mobs[i-firstEnemies].timeIdle>0) {
				mobs[i-firstEnemies].timeIdle--;
			}

			float x = dbObjectPositionX(i);
			float y = dbObjectPositionY(i);
			float z = dbObjectPositionZ(i);

			float fHeight;
			
			if(blitzOn==false) {
				fHeight = dbGetTerrainGroundHeight( terrain, dbObjectPositionX(i), dbObjectPositionZ(i) );
			}
			else {
				fHeight = Terrain->GetGroundHeight(dbObjectPositionX(i), dbObjectPositionZ(i));
			}
			dbPositionObject( i, dbObjectPositionX(i), fHeight + 2.0f, dbObjectPositionZ(i) );

			//float y = dbObjectPositionY(i);

			//little spheres collide with all (0)
			int collide = SC_SphereSlide( 0, oldx,oldy,oldz, x,y,z, littleRadius, i );
			//int collide = SC_SphereSlideGroup( 0, oldx,oldy,oldz, x,y,z, littleRadius, i );


			if ( collide > 0 ) {

				float x1 = SC_GetCollisionSlideX();
				float y1 = SC_GetCollisionSlideY();
				float z1 = SC_GetCollisionSlideZ();
				dbPositionObject( i, SC_GetCollisionSlideX(),SC_GetCollisionSlideY(),SC_GetCollisionSlideZ() );
			}
			SC_UpdateObject( i );
		}
	}
}

void moveCharacter(void) {

	float oldx = dbObjectPositionX(character);
	float oldy = dbObjectPositionY(character);
	float oldz = dbObjectPositionZ(character);

	inputHandler();

	if ( player.vy == 0 && jumptimer == 0 ) {
		player.vy = player.vy + 10*gravity; 
	}
	else {
		player.vy = player.vy + gravity;
	}

	float x = oldx + player.vx;
	float y = oldy + player.vy;
	float z = oldz + player.vz;

	int collide = SC_SphereCastGroup( groupWorld, oldx, oldy, oldz, oldx, oldy+player.vy, oldz, radius, 0 );

	if ( collide > 0 ) {
		float ny = SC_GetCollisionNormalY();
		if ( dbAbs(ny) > slope ) {
			oldy = SC_GetStaticCollisionY();
		}
		else {
			x = x - oldx; 
			z = z - oldz;
			oldx = SC_GetCollisionSlideX();
			oldy = SC_GetCollisionSlideY();
			oldz = SC_GetCollisionSlideZ();
			x = x + oldx; 
			z = z + oldz;
		}
		
		if ( ny > slope ) {
			ground = 1;
			player.vy = 0;
		}
		else {
			ground = 0;
			if ( ny < -slope ) {
				player.vy = gravity;
			}
		}
	}
	else {
		oldy = oldy + player.vy;
		ground = 0;
	}

	if ( ground == 1 && jumptimer > 0 ) {
		jumptimer--;
	}

	collide = SC_SphereSlideGroup( groupWorld, oldx, oldy, oldz, x, oldy, z, radius, 0 );
	
	if ( collide > 0 ) {
		x = SC_GetCollisionSlideX();
		oldy = SC_GetCollisionSlideY();
		z = SC_GetCollisionSlideZ();
		player.vx = 0;
		player.vz = 0;
	}

	dbPositionObject( character, x, oldy, z );
	
	checkPicking();

	SC_UpdateObject(character);
}


// ANIMATIONS //
void updateAnimationEnemy(void) {
	for(int i=0; i<numEnemies; i++) {
		if(dbObjectExist(mobs[i].id)==1) {
			if(mobs[i].hit==0) { //il mob non sta subbendo danno
				if(mobs[i].health>0) {
					if(mobs[i].mode==0) {	//il mob sta in idle, cioè non si muove, sta fermo sul posto
						movementAnimation(0,23,80,i);
					}
					else if(mobs[i].mode==1) { //se il mob sta in pattugliamento o sta inseguendo ma senza attaccare
						movementAnimation(67,84,40,i);
					}
					else if( mobs[i].mode==2) {
						movementAnimation(67,84,25,i);
					}
					else if(mobs[i].mode==3) { //il mob sta attaccando
						if(player.health>0) {
							movementAnimation(27,49,40,i);
						}
					}
					else if( mobs[i].mode==4) {
						movementAnimation(67,84,60,i);
					}
				}			
				else { //il mob è morto, fai l'animazione della morte
					if(controllerEnemy[i].dead==true) {
						movementAnimation(87,120,34,i);
					}
					if(player.currentAnimation==ENEMY_CAP_DEAD) {
						controllerEnemy[i].dead=false;
					}
				}
			}
			else if(mobs[i].hit==1) {//il mob sta subbendo danno
				movementAnimation(50,60,34,i);
				if(mobs[i].currentAnimation==60) {
					mobs[i].hit=0;
				}
			}
		}
	}
}
void updateAnimationWeapon(void) {
	if(player.mode==0) {
		
		if(controllerAnimation.select==true) {
			movementAnimation(1,30,80,-1); //estrai l'arma
		}

		if(player.currentAnimation==CAP_SELECT) {
			controllerAnimation.select=false;
		}
	}
	else if(player.mode==1) { //idle
		
		if(controllerAnimation.idle==true) {
			movementAnimation(31,49,40,-1);
		}

		if(player.currentAnimation==CAP_IDLE) {
			controllerAnimation.idle=false;
		}
	}
	else if(player.mode==2) { //movimento
		
		if(controllerAnimation.movement==true) {
			movementAnimation(51,65,40,-1);
		}
		if(player.currentAnimation==CAP_MOVEMENT) {
			controllerAnimation.movement=false;
		}
	}
	else if(player.mode==3) { //spara
		
		if(controllerAnimation.fire==true) {
			movementAnimation(66,72,25,-1);
		}
		if(player.currentAnimation==CAP_FIRE) {
			controllerAnimation.fire=false;
		}
	}
	else if(player.mode==4) { //ricarica arma
		
		if(controllerAnimation.reload==true) {
			movementAnimation(75,154,60,-1);
		}
		if(player.currentAnimation==CAP_RELOAD) {
			controllerAnimation.reload=false;			
		}
	}
	else if(player.mode==5) { //riponi arma
		/*if(player.currentAnimation==CAP_PUTAWAY+1) {
			controllerAnimation.putaway=false;
			//putAwayOn=false;
		}*/
		if(controllerAnimation.putaway==true) {
			movementAnimation(155,170,60,-1);
		}
		if(player.currentAnimation==CAP_PUTAWAY) {
			controllerAnimation.putaway=false;
			putAwayOn=false;
		}
	}
	else if(player.mode==6) {

		if(controllerAnimation.movement==true) {
			movementAnimation(51,65,25,-1);
		}

		if(player.currentAnimation==CAP_MOVEMENT) {
			controllerAnimation.movement=false;
		}
	}
}
void movementAnimation(int startAnimation,int finalAnimation, int timer, int index) {
	if(index==-1) { //update player
		if(dbTimer() - player.timerUpdateAnimation >= timer) {
			if(player.currentAnimation < startAnimation || player.currentAnimation > finalAnimation ) {
				player.currentAnimation=startAnimation;
			}
			dbSetObjectFrame(weapon,player.currentAnimation);
			player.currentAnimation +=1;
			player.timerUpdateAnimation=dbTimer();
		}
	}
	else { //update animazione mob
		if(dbTimer() - mobs[index].timerUpdateAnimation >= timer) {
			if(mobs[index].currentAnimation < startAnimation || mobs[index].currentAnimation > finalAnimation ) {
				mobs[index].currentAnimation=startAnimation;
			}
			dbSetObjectFrame(mobs[index].id,mobs[index].currentAnimation);
			mobs[index].currentAnimation +=1;
			mobs[index].timerUpdateAnimation=dbTimer();
		}
	}
}
stateAnimation checkPlayerStateAnimation(int frame) {
	if(frame<1 || frame>170) {
		return ERRORE;
	}
	else {
		if(frame<CAP_SELECT) {
			return SELECT;
		}
		else if(frame>=MIN_IDLE && frame<CAP_IDLE) {
			return IDLE;
		}
		else if(frame>=MIN_MOVEMENT && frame<CAP_MOVEMENT) {
			return MOVEMENT;
		}
		else if(frame>=MIN_FIRE && frame<CAP_FIRE) {
			return FIRE;
		}
		else if(frame>=MIN_RELOAD && frame<CAP_RELOAD) {
			return RELOAD;
		}
		else if(frame>=MIN_PUTAWAY && frame<CAP_PUTAWAY) {
			return PUTAWAY;
		}
	}
	return ERRORE;
}


stateAnimationEnemy checkEnemyStateAnimation(int frame) {
	if(frame<0 || frame>120) {
		return ENEMY_ERRORE;
	}
	else {
		if(frame<ENEMY_CAP_IDLE) {
			return ENEMY_IDLE;
		}
		else if(frame>=ENEMY_MIN_FIRE && frame<ENEMY_CAP_FIRE) {
			return ENEMY_FIRE;
		}
		else if(frame>=ENEMY_MIN_IMPACT && frame<ENEMY_CAP_IMPACT) {
			return ENEMY_IMPACT;
		}
		else if(frame>=ENEMY_MIN_MOVEMENT && frame<ENEMY_CAP_MOVEMENT) {
			return ENEMY_MOVEMENT;
		}
		else if(frame>=ENEMY_MIN_DEAD && frame<ENEMY_CAP_DEAD) {
			return ENEMY_DEAD;
		}
	}
	return ENEMY_ERRORE;
}
// PICKING //
void checkPicking(void) {

	/*for(int i=0; i<maxBulletClip; i++) {
		if(dbObjectExist(i+firstBulletClip)==1 && clip[i].state==0) {
			if(dbObjectPositionX(character)>=clip[i].positionX+20 && dbObjectPositionX(character)<=clip[i].positionX+clip[i].ray+20 && dbObjectPositionZ(character)>=clip[i].positionZ+20 && dbObjectPositionZ(character)<=clip[i].positionZ+clip[i].ray+20) {
				pickBulletClip(clip[i].id);
			}
		}
	}

	for(int i=0; i<maxMedikit; i++) {
		if(dbObjectExist(i+firstMedikit)==1 && medikit[i].state==0) {
			if(dbObjectPositionX(character)>=medikit[i].positionX+20 && dbObjectPositionX(character)<=medikit[i].positionX+medikit[i].ray+20 && dbObjectPositionZ(character)>=medikit[i].positionZ+20 && dbObjectPositionZ(character)<=medikit[i].positionZ+medikit[i].ray+20) {
				pickMedikit(medikit[i].id);
			}
		}
	}

	for(int i=0; i<maxWeapon ; i++) {
		if(dbObjectExist(i+firstWeapon)==1 && weapons[i].state==0) {
			if(dbObjectPositionX(character)>=weapons[i].positionX+20 && dbObjectPositionX(character)<=weapons[i].positionX+weapons[i].ray+20 && dbObjectPositionZ(character)>=weapons[i].positionZ+20 && dbObjectPositionZ(character)<=weapons[i].positionZ+weapons[i].ray+20) {
				pickWeapon(weapons[i].id);
			}
		}
	}*/

	for(int i=0; i<maxBulletClip; i++) {
		if(dbObjectExist(i+firstBulletClip)==1 && clip[i].state==0) {
			if(distanceBetween2Points(dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character),dbObjectPositionX(clip[i].id),dbObjectPositionY(clip[i].id),dbObjectPositionZ(clip[i].id))<10) {
				pickBulletClip(clip[i].id);
			}
		}
	}

	for(int i=0; i<maxMedikit; i++) {
		if(dbObjectExist(i+firstMedikit)==1 && medikit[i].state==0) {
			if(distanceBetween2Points(dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character),dbObjectPositionX(medikit[i].id),dbObjectPositionY(medikit[i].id),dbObjectPositionZ(medikit[i].id))<10) {
				pickMedikit(medikit[i].id);
			}
		}
	}

	for(int i=0; i<maxWeapon ; i++) {
		if(dbObjectExist(i+firstWeapon)==1 && weapons[i].state==0) {
			if(distanceBetween2Points(dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character),dbObjectPositionX(weapons[i].id),dbObjectPositionY(weapons[i].id),dbObjectPositionZ(weapons[i].id))<10) {
				pickWeapon(weapons[i].id);
			}
		}
	}
}
void pickBulletClip(int idClip) {
	if(inventory.weapon1==true) {
		if(player.nCaricatori<maxPlayerClip) {
			if(clip[idClip-firstBulletClip].type==1) {
				player.nCaricatori++;
				dbHideObject(idClip);
				clip[idClip-firstBulletClip].state=1;
			}
		}
	}
	
}
void pickMedikit(int idMedikit) {
	if(player.health<100) {
		if(player.health+healthUp>100) {
			player.health=100;
		}
		else {
			player.health+=healthUp;
		}
		dbHideObject(idMedikit);
		medikit[idMedikit-firstMedikit].state=1;
	}	
}
void pickWeapon(int idWeapon) {
	if(player.currentWeapon==0 && weapons[idWeapon-firstWeapon].type==1 && inventory.weapon1==false) {
		inventory.weapon1=true;
		player.currentWeapon=1;
		player.nCaricatori=3;
		player.currentBullets=15;
		dbHideObject(idWeapon);
		weapons[idWeapon-firstWeapon].state=1;
		dbPositionSound(soundSelectElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
		dbPlaySound(soundSelectElite);
		player.mode=0;
		controllerAnimation.select=true;
		player.currentAnimation=MIN_SELECT;
	}
	else if(player.currentWeapon==0 && weapons[idWeapon-firstWeapon].type==1 && inventory.weapon1==true && player.nCaricatori!=3) {
		player.currentWeapon=1;
		if(player.nCaricatori==0) {
			player.nCaricatori=2;
		}
		else if(player.nCaricatori==1) {
			player.nCaricatori=3;
		}
		dbHideObject(idWeapon);
		weapons[idWeapon-firstWeapon].state=1;
		dbPositionSound(soundSelectElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
		dbPlaySound(soundSelectElite);
		player.mode=0;
		controllerAnimation.select=true;
		player.currentAnimation=MIN_SELECT;		
	}
	else if(player.currentWeapon==1 && weapons[idWeapon-firstWeapon].type==1 && inventory.weapon1==true) {
		if(player.nCaricatori!=3) {
			if(player.nCaricatori==0 || player.nCaricatori==1) {
				player.nCaricatori+=2;
			}
			else if(player.nCaricatori==2) {
				player.nCaricatori=maxPlayerClip;
			}
			dbHideObject(idWeapon);
			weapons[idWeapon-firstWeapon].state=1;
			dbPositionSound(soundSelectElite,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));
			dbPlaySound(soundSelectElite);
		}
	}
	/*else if(player.currentWeapon==0 && weapons[idWeapon-firstWeapon].type==2) {
		player.currentWeapon=2;
		dbHideObject(idWeapon);
		weapons[idWeapon-firstWeapon].state=1;
	}*/
}

// BULLETS //
void shootBullets(void) {
	int bulletId=0;
	int targetId=0;

	for(int i=0; i<maxBullets; i++) {
		if(bullets[i].active==false) {
			bulletId=bullets[i].id;
			break;
		}
	}
	
	bullets[bulletId-firstBullet].active=true;

	dbYRotateObject(bulletId,dbObjectAngleY(character));
	dbPositionObject(bulletId,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));

	/*dbYRotateObject(bulletId-firstBullet+firstGraphicBullet,dbObjectAngleY(character));
	dbPositionObject(bulletId-firstBullet+firstGraphicBullet,dbObjectPositionX(weapon),dbObjectPositionY(weapon),dbObjectPositionZ(weapon));*/

	//dbShowObject(bulletId);
	//dbShowObject(bulletId-firstBullet+firstGraphicBullet);

	bullets[bulletId-firstBullet].oldX=dbObjectPositionX(bulletId);
	bullets[bulletId-firstBullet].oldY=dbObjectPositionY(bulletId);
	bullets[bulletId-firstBullet].oldZ=dbObjectPositionZ(bulletId);

	//targetId=dbPickObject(dbMouseX(),dbMouseY(),0,firstEnemies+numEnemies);
	targetId=dbPickObject(dbSpriteX(1)+16,dbSpriteY(1)+16,0,firstEnemies+numEnemies);

	if(targetId!=0) {

		float distance=dbGetPickDistance();

		//dbPickScreen(dbMouseX(), dbMouseY(), distance);
		dbPickScreen(dbSpriteX(1)+16,dbSpriteY(1)+16, distance);
		
		float mult = dbABS ( dbCameraPositionY() / dbGetPickVectorY() );

		float provax,provay,provaz;
		provax=dbGetPickVectorX();
		provay=dbGetPickVectorY();
		provaz=dbGetPickVectorZ();
		
		bullets[bulletId-firstBullet].coordX = dbGetPickVectorX()*mult + dbCameraPositionX();
		bullets[bulletId-firstBullet].coordY = dbGetPickVectorY()*mult + dbCameraPositionY();
		bullets[bulletId-firstBullet].coordZ = dbGetPickVectorZ()*mult + dbCameraPositionZ();
		
		if(targetId==terrain) {
		}
		else if(targetId>=firstEnemies && targetId<=firstEnemies+numEnemies) {
			//mobs[targetId-firstEnemies].hit=1;
			if(mobs[targetId-firstEnemies].health>0) {
				//makeExplosion();
				bullets[bulletId-firstBullet].sourceId=character;
				bullets[bulletId-firstBullet].targetId=targetId;
				bullets[bulletId-firstBullet].coordX = dbObjectPositionX(targetId);
				bullets[bulletId-firstBullet].coordY = dbObjectPositionY(targetId);
				bullets[bulletId-firstBullet].coordZ = dbObjectPositionZ(targetId);
				takeDamage(bullets[bulletId-firstBullet].id);

			}
			if(mobs[targetId-firstEnemies].health<=0) {
				dbPositionSound(soundDeathEnemy,dbObjectPositionX(mobs[targetId-firstEnemies].id),dbObjectPositionY(mobs[targetId-firstEnemies].id),dbObjectPositionZ(mobs[targetId-firstEnemies].id));
				dbPlaySound(soundDeathEnemy);

				controllerEnemy[targetId-firstEnemies].dead=true;
				mobs[targetId-firstEnemies].currentAnimation=ENEMY_MIN_DEAD;

				//dbHideObject(mobs[targetId-firstEnemies].id);
				//resetBullet(bulletId-firstBullet);
				//removeMob(targetId-firstEnemies);
			}
		}
	}
	else {
		resetBullet(bulletId-firstBullet);
	}	
}
void mobsAttack(int sourceId,int type) {

	if(type==0) { //pallottola singola
		int bulletId=0;
		int collide=0;

		float oldx=0;
		float oldy=0;
		float oldz=0;

		float x=0;
		float y=0;
		float z=0;

		for(int i=0; i<maxBullets; i++) {
			if(bullets[i].active==false) {
				bulletId=bullets[i].id;
				break;
			}
		}
		
		bullets[bulletId-firstBullet].active=true;

		dbYRotateObject(bulletId,dbObjectAngleY(sourceId));
		dbPositionObject(bulletId,dbObjectPositionX(sourceId),dbObjectPositionY(sourceId),dbObjectPositionZ(sourceId)+littleRadius+1);	
		
		/*dbYRotateObject(bulletId-firstBullet+firstGraphicBullet,dbObjectAngleY(sourceId));
		dbPositionObject(bulletId-firstBullet+firstGraphicBullet,dbObjectPositionX(sourceId),dbObjectPositionY(sourceId),dbObjectPositionZ(sourceId)+littleRadius+1);*/
		
		//dbShowObject(bulletId);
		//dbShowObject(bulletId-firstBullet+firstGraphicBullet);

		bullets[bulletId-firstBullet].oldX=dbObjectPositionX(bulletId);
		bullets[bulletId-firstBullet].oldY=dbObjectPositionY(bulletId);
		bullets[bulletId-firstBullet].oldZ=dbObjectPositionZ(bulletId);

		oldx=dbObjectPositionX(bulletId);
		oldy=dbObjectPositionY(bulletId);
		oldz=dbObjectPositionZ(bulletId);

		bullets[bulletId-firstBullet].coordX=dbObjectPositionX(character)+mobs[sourceId-firstEnemies].accuracyX;
		bullets[bulletId-firstBullet].coordY=dbObjectPositionY(character)+mobs[sourceId-firstEnemies].accuracyY;
		bullets[bulletId-firstBullet].coordZ=dbObjectPositionZ(character)+mobs[sourceId-firstEnemies].accuracyZ;

		x=dbObjectPositionX(character)+mobs[sourceId-firstEnemies].accuracyX;
		y=dbObjectPositionY(character)+mobs[sourceId-firstEnemies].accuracyY;
		z=dbObjectPositionZ(character)+mobs[sourceId-firstEnemies].accuracyZ;

		bullets[bulletId-firstBullet].sourceId=sourceId;
		bullets[bulletId-firstBullet].targetId=character;

		collide=SC_RayCast(0,oldx,oldy,oldz,x,y,z,0);

		if(collide>0) {
			if(collide==terrain) {
				//makeExplosion()
			}
			else if(collide==character) {
				//makeExplosion()

				bullets[bulletId-firstBullet].sourceId=sourceId;
				bullets[bulletId-firstBullet].targetId=character;

				takeDamage(bullets[bulletId-firstBullet].id);
			}
		}
		else {
			resetBullet(bulletId-firstBullet);
		}
	}
	else { //pallottole multiple
		int bulletId[4]={ firstBullet, firstBullet, firstBullet, firstBullet };
		int collide[4]={0,0,0,0};

		float oldX[4]={0,0,0,0};
		float oldY[4]={0,0,0,0};
		float oldZ[4]={0,0,0,0};

		float x[4]={0,0,0,0};
		float y[4]={0,0,0,0};
		float z[4]={0,0,0,0};

		float distance[4]={0.0,0.0,0.0,0.0};
		
		for(int j=0; j<4; j++) {
			for(int i=0; i<maxBullets; i++) {
				if(bullets[i].active==false) {
					bullets[i].active=true;
					bulletId[j]=bullets[i].id;
					break;
				}
			}
		}
		
		for(int i=0; i<4; i++) {
			switch(i) {
				case 0:
					dbYRotateObject(bulletId[i],dbWrapValue(dbObjectAngleY(sourceId)+45));
					break;
				case 1:
					dbYRotateObject(bulletId[i],dbWrapValue(dbObjectAngleY(sourceId)+135));
					break;
				case 2:
					dbYRotateObject(bulletId[i],dbWrapValue(dbObjectAngleY(sourceId)+225));
					break;
				case 3:
					dbYRotateObject(bulletId[i],dbWrapValue(dbObjectAngleY(sourceId)+315));
					break;
				default:
					break;
			}

			bullets[bulletId[i]-firstBullet].type=1;

			bullets[bulletId[i]-firstBullet].velocity *=7;

			dbPositionObject(bulletId[i],dbObjectPositionX(sourceId),dbObjectPositionY(sourceId),dbObjectPositionZ(sourceId));	

			//dbShowObject(bulletId[i]);

			bullets[bulletId[i]-firstBullet].oldX=dbObjectPositionX(bulletId[i]);
			bullets[bulletId[i]-firstBullet].oldY=dbObjectPositionY(bulletId[i]);
			bullets[bulletId[i]-firstBullet].oldZ=dbObjectPositionZ(bulletId[i]);

			oldX[i]=dbObjectPositionX(bulletId[i]);
			oldY[i]=dbObjectPositionY(bulletId[i]);
			oldZ[i]=dbObjectPositionZ(bulletId[i]);

			//distance[i]=(bullets[bulletId[i]-firstBullet].velocity)*dbSIN(45)*gravity*((bullets[bulletId[i]-firstBullet].velocity)*dbSIN(45)+dbSQRT(pow((bullets[bulletId[i]-firstBullet].velocity)*dbSIN(45),2)+2*gravity*oldY[i]));
			distance[i]=bullets[bulletId[i]-firstBullet].velocity*dbSIN(tiltAngle)*dbCOS(swivelAngle)*(1/gravity)*( bullets[bulletId[i]-firstBullet].velocity*dbSIN(tiltAngle) + dbSQRT(pow(bullets[bulletId[i]-firstBullet].velocity*dbSIN(tiltAngle),2) + 2*gravity+oldY[i]) );

			bullets[bulletId[i]-firstBullet].coordX=distance[i]*dbCOS(swivelAngle)*dbCos(dbObjectAngleY(bulletId[i]))+oldX[i];
			bullets[bulletId[i]-firstBullet].coordZ=distance[i]*dbSIN(swivelAngle)*dbSin(dbObjectAngleY(bulletId[i]))+oldZ[i];
			if(blitzOn==false) {
				bullets[bulletId[i]-firstBullet].coordY=dbGetTerrainGroundHeight(terrain,bullets[bulletId[i]-firstBullet].coordX,bullets[bulletId[i]-firstBullet].coordZ);
			}
			else {
				bullets[bulletId[i]-firstBullet].coordY=Terrain->GetGroundHeight(bullets[bulletId[i]-firstBullet].coordX,bullets[bulletId[i]-firstBullet].coordZ);
			}

			x[i]=bullets[bulletId[i]-firstBullet].coordX;
			y[i]=bullets[bulletId[i]-firstBullet].coordY;
			z[i]=bullets[bulletId[i]-firstBullet].coordZ;

			collide[i]=SC_RayCast(0,oldX[i],oldY[i],oldZ[i],x[i],y[i],z[i],sourceId);

			if(collide[i]>0) {
				if(collide[i]==terrain) {
					//makeExplosion() 
				}
				else if(collide[i]==character) {
					//makeExplosion()

					bullets[bulletId[i]-firstBullet].sourceId=sourceId;
					bullets[bulletId[i]-firstBullet].targetId=character;

					takeDamage(bullets[bulletId[i]-firstBullet].id);
				}
			}
			else {
				resetBullet(bulletId[i]-firstBullet);
			}
		}
	}
}

void updateBullets(void) {
	int collide=0;
	for(int i=0; i<maxBullets; i++) {
		if(bullets[i].active==true) {
			
			if(bullets[i].sourceId==character) {
				collide=SC_RayCast(0,bullets[i].oldX,bullets[i].oldY,bullets[i].oldZ,bullets[i].coordX,bullets[i].coordY,bullets[i].coordZ,character);
			}
			else {
				collide=SC_RayCast(0,bullets[i].oldX,bullets[i].oldY,bullets[i].oldZ,bullets[i].coordX,bullets[i].coordY,bullets[i].coordZ,bullets[i].sourceId);
			}

			if(bullets[i].life>0) {

				bullets[i].life--;

				dbPointObject(bullets[i].id,bullets[i].coordX,bullets[i].coordY,bullets[i].coordZ);

				if(collide>0) {
					
					if(collide==terrain) {

						float newx = SC_GetStaticCollisionX();
						float newy = SC_GetStaticCollisionY();
						float newz = SC_GetStaticCollisionZ();

						float normx = SC_GetCollisionNormalX();
						float normy = SC_GetCollisionNormalY();
						float normz = SC_GetCollisionNormalZ();					
					
						//dbPositionObject(bullets[i].id,newx,newy,newz);
						dbPointObject( hole, newx + normx, newy + normy, newz + normz );
						
						dbPositionObject( hole, newx,newy,newz);
						//dbPositionObject( hole, newx + normx/100.0f, newy + normy/100.0f, newz + normz/100.0f );
						//dbPointObject( hole, newx + normx, newy + normy, newz + normz );
						dbShowObject(hole);

						if(bullets[i].type==0 && bullets[i].explosion==false) {
			
							bullets[i].explosion=true;

							int freeExplosion=takeFreeExplosion();
							//dbPointObject(explosion[freeExplosion].id, newx + normx, newy + normy, newz + normz );
							explosion[freeExplosion].x=/*newz*/dbObjectPositionX(hole);
							explosion[freeExplosion].y=/*newy*/dbObjectPositionY(hole);
							explosion[freeExplosion].z=/*newz*/dbObjectPositionZ(hole);
							
							int freeExplosion2=takeFreeExplosion();
							//dbPointObject(explosion[freeExplosion2].id, newx + normx, newy + normy, newz + normz );
							explosion[freeExplosion2].x=/*newz*/dbObjectPositionX(hole);
							explosion[freeExplosion2].y=/*newy*/dbObjectPositionY(hole);
							explosion[freeExplosion2].z=/*newz*/dbObjectPositionZ(hole);

							dbPositionObject(explosion[freeExplosion].id,explosion[freeExplosion].x,explosion[freeExplosion].y,explosion[freeExplosion].z);
							dbShowObject(explosion[freeExplosion].id);

							dbPositionObject(explosion[freeExplosion2].id,explosion[freeExplosion2].x,explosion[freeExplosion2].y,explosion[freeExplosion2].z);
							dbShowObject(explosion[freeExplosion2].id);
						}

						hole++;
						if(hole>firstBulletMarker+maxBulletMarker-1) {
							hole=firstBulletMarker;
						}
					}
					else if(collide==character || (collide>=firstEnemies && collide<firstEnemies+numEnemies)) {
						dbHideObject(bullets[i].id);
						//dbHideObject(bullets[i].graphicId);
						resetBullet(i);
					}					
				}

				if(bullets[i].type==0) { //proiettile singolo

					dbMoveObject(bullets[i].id,bullets[i].velocity);

				}
				else if(bullets[i].type==1) { //particelle

					bullets[i].velocityY=bullets[i].velocity*dbSIN(tiltAngle);
					
					dbMoveObjectUp(bullets[i].id,bullets[i].velocityY/*dbSIN(45)*/);
					dbMoveObject(bullets[i].id,bullets[i].velocity*dbCOS(tiltAngle)/*dbSIN(45)*/);

					bullets[i].velocityY -= gravity;
				}

				SC_UpdateObject(bullets[i].id);
			}
			else {

				dbHideObject(bullets[i].id);
				//dbHideObject(bullets[i].graphicId);
				resetBullet(i);
			}
		}
	}
}
void updateExplosion() {
	
	for(int i=0; i<maxExplosion; i+=2) {
		if(explosion[i].state==false) {

			explosion[i].eScale=20*(30-explosion[i].timer);
			explosion[i+1].eScale=20*(30-explosion[i+1].timer);

			dbScaleObject(i+firstExplosion,explosion[i].eScale,explosion[i].eScale,explosion[i].eScale);
			dbYRotateObject(i+firstExplosion,dbWrapValue(explosion[i].timer*37));

			dbScaleObject(1+i+firstExplosion,explosion[i+1].eScale,explosion[i+1].eScale,explosion[i+1].eScale);
			dbYRotateObject(1+i+firstExplosion,dbWrapValue(explosion[i+1].timer*37));

			explosion[i].timer--;
			explosion[i+1].timer--;

			if(explosion[i].timer==0 && explosion[i+1].timer==0) {
				dbHideObject(explosion[i].id);
				dbHideObject(explosion[i+1].id);
				resetExplosion(i,i+1);
			}

			if(explosion[i].timer==18 && explosion[i+1].timer==18) {
				dbPositionSound(soundExplosion,explosion[i].x,explosion[i].y,explosion[i].z);
				dbPlaySound(soundExplosion);
			}

			if(explosion[i].timer<15 && explosion[i+1].timer<15) {
				dbPositionSound(soundExplosion,dbCameraPositionX(),dbCameraPositionY(),dbCameraPositionZ());
			}
		}
	}	
}
int takeFreeExplosion(void) {
	for(int i=0; i<maxExplosion; i++) {
		if(explosion[i].state==true) {
			explosion[i].state=false;
			return i;
		}
	}	
}
void takeDamage(int bulletId) {
	float distanzaPercorribile=bullets[bulletId-firstBullet].velocity*bullets[bulletId-firstBullet].life;
	float distance=distanceBetween2Points(bullets[bulletId-firstBullet].oldX,bullets[bulletId-firstBullet].oldY,bullets[bulletId-firstBullet].oldZ,bullets[bulletId-firstBullet].coordX,bullets[bulletId-firstBullet].coordY,bullets[bulletId-firstBullet].coordZ);
	if(distance<distanzaPercorribile) {
		if(bullets[bulletId-firstBullet].sourceId==character) {
			mobs[bullets[bulletId-firstBullet].targetId-firstEnemies].hit=1;
			dbPositionSound(soundHitEnemy,dbObjectPositionX(bullets[bulletId-firstBullet].targetId),dbObjectPositionY(bullets[bulletId-firstBullet].targetId),dbObjectPositionZ(bullets[bulletId-firstBullet].targetId));
			dbPlaySound(soundHitEnemy);
			mobs[bullets[bulletId-firstBullet].targetId-firstEnemies].health-=player.damage;
		}
		else if(bullets[bulletId-firstBullet].sourceId>=firstEnemies && bullets[bulletId-firstBullet].sourceId<=firstEnemies+numEnemies) {
			
			dbPositionSound(soundHitPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
			dbPlaySound(soundHitPlayer);

			if(bullets[bulletId-firstBullet].type==0) {			
				player.health-=damageBullet;
			}
			else if(bullets[bulletId-firstBullet].type==1) {
				player.health-=damageParticle;
			}
		}
	}
}
void resetBullet(int bulletIndex) {
	bullets[bulletIndex].id=firstBullet+bulletIndex;
	bullets[bulletIndex].active=false;
	bullets[bulletIndex].coordX=0;
	bullets[bulletIndex].coordY=0;
	bullets[bulletIndex].coordZ=0;
	bullets[bulletIndex].oldX=0;
	bullets[bulletIndex].oldY=0;
	bullets[bulletIndex].oldZ=0;
	bullets[bulletIndex].damage=10;
	bullets[bulletIndex].life=100;
	bullets[bulletIndex].sourceId=0;
	bullets[bulletIndex].targetId=0;
	bullets[bulletIndex].type=0;
	bullets[bulletIndex].velocity=2;
	bullets[bulletIndex].velocityY=bullets[bulletIndex].velocity*dbSIN(tiltAngle);
	//bullets[bulletIndex].graphicId=firstGraphicBullet+bulletIndex;
	bullets[bulletIndex].explosion=false;

	dbHideObject(bulletIndex+firstBullet);
	//dbHideObject(bulletIndex+firstGraphicBullet);
}
void resetBullets(void) {
	for(int i=0; i<maxBullets; i++) {
		if(bullets[i].active==false || bullets[i].life<=0) {
			resetBullet(i);
		}
	}
}
void resetExplosion(int explosionIndex,int explosionIndex2) {
	explosion[explosionIndex].eScale=0;
	explosion[explosionIndex].state=true;
	explosion[explosionIndex].timer=timerExplosion;
	explosion[explosionIndex].x=0;
	explosion[explosionIndex].y=0;
	explosion[explosionIndex].z=0;

	explosion[explosionIndex2].eScale=0;
	explosion[explosionIndex2].state=true;
	explosion[explosionIndex2].timer=timerExplosion;
	explosion[explosionIndex2].x=0;
	explosion[explosionIndex2].y=0;
	explosion[explosionIndex2].z=0;
}
// CAMERA //
void updateCamera(int MouseDiffX, int MouseDiffY) {

	static float CamSpeed = 0.07;
	float RotX;
	float RotY;
	RotX = dbObjectAngleX(sphereCam);
	RotY = dbObjectAngleY(sphereCam);
			
	RotX = dbWrapValue(RotX + float(MouseDiffY) * CamSpeed); //- because when u move the mouse frwd u want him to look up
	RotY = dbWrapValue(RotY + float(MouseDiffX) * CamSpeed);
			
	//keeps camera from turning upside down and also keeps movement smooth

	dbPositionObject ( sphereCam, dbObjectPositionX(character), dbObjectPositionY(character) + 15, dbObjectPositionZ(character));
	//move the object that follows the character
	dbPositionCamera (dbObjectPositionX(sphereCam), dbObjectPositionY(sphereCam), dbObjectPositionZ(sphereCam) );//move the camera to the object that follows the character
	// code below makes sure the camera is within boundaries
	// limits downwards point
	/*if(RotX < 18.0) {
		dbRotateObject(sphereCam, RotX, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -RotX, RotY, 0);//allows for player to look up and down with the mouse	 
	}
	else if (RotX > 18.0 && RotX < 60.0) {
		dbRotateObject(sphereCam, 18.0, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -18.0, RotY, 0);//allows for player to look up and down with the mouse	
	}
	// limits upwards point
	if(RotX > 345.0 && RotX < 360.0) {
		dbRotateObject(sphereCam, RotX, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -RotX, RotY, 0);//allows for player to look up and down with the mouse	
	}
	else if (RotX > 200.0 && RotX < 345.0) {	
		dbRotateObject(sphereCam, 345.0, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -345.0, RotY, 0);//allows for player to look up and down with the mouse	
	}*/
	if(RotX < 0.0) {
		dbRotateObject(sphereCam, 0.0, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -0.0, RotY, 0);//allows for player to look up and down with the mouse	 
	}
	else if (RotX >= 0.0 && RotX < 90.0) {
		dbRotateObject(sphereCam, RotX, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -RotX, RotY, 0);//allows for player to look up and down with the mouse	
	}
	// limits upwards point
	if(RotX >360.0) {
		dbRotateObject(sphereCam, 360.0, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -360.0, RotY, 0);//allows for player to look up and down with the mouse	
	}
	else if (RotX > 270.0 && RotX < 360.0) {	
		dbRotateObject(sphereCam, RotX, RotY, 0);//allows for player to look up and down with the mouse
		dbRotateCamera	 ( dbObjectAngleX(sphereCam), dbObjectAngleY(sphereCam), dbObjectAngleZ(sphereCam) );//rotate the camera to the object that follows the character
		dbRotateObject(character, -RotX, RotY, 0);//allows for player to look up and down with the mouse	
	}
}
// WEAPON //
void positionWeapon(void) {

	//setup m4 e forse altre armi?
	/*dbPositionObject(weapon,dbCameraPositionX(), dbCameraPositionY()-10,dbCameraPositionZ());
	dbRotateObject(weapon,-dbCameraAngleX(), dbCameraAngleY()-180,dbCameraAngleZ());
	dbMoveObject(weapon, -16);
	dbMoveObjectRight(weapon, -7);*/

	/*if(firstRunning!=true) {
		
		if(player.currentWeapon==1) {
			//setup elite con braccia
			dbPositionObject(weapon,dbCameraPositionX(), dbCameraPositionY()-5,dbCameraPositionZ());
			dbRotateObject(weapon,-dbCameraAngleX(), dbCameraAngleY()-180,dbCameraAngleZ());
			dbMoveObject(weapon, -8);
			dbMoveObjectRight(weapon, -7);
		}

	}*/

	if(player.currentWeapon==0 && checkPlayerStateAnimation(player.currentAnimation)!=PUTAWAY) {
		dbHideObject(weapon);
	}
	else if(player.currentWeapon==1) {
		dbShowObject(weapon);
	}
	//setup elite con braccia
	dbPositionObject(weapon,dbCameraPositionX(), dbCameraPositionY()-5,dbCameraPositionZ());
	dbRotateObject(weapon,-dbCameraAngleX(), dbCameraAngleY()-180,dbCameraAngleZ());
	if(dbCameraAngleX()>80 && dbCameraAngleX()<=90) {
		dbMoveObject(weapon,2);
	}
	else if(dbCameraAngleX()>70 && dbCameraAngleX()<=80) {
		dbMoveObject(weapon,1);
	}
	else if(dbCameraAngleX()>60 && dbCameraAngleX()<=70) {
		dbMoveObject(weapon,0);
	}
	else if(dbCameraAngleX()>50 && dbCameraAngleX()<=60) {
		dbMoveObject(weapon,-1);
	}
	else if(dbCameraAngleX()>40 && dbCameraAngleX()<=50) {
		dbMoveObject(weapon,-5);
	}
	else if(dbCameraAngleX()>30 && dbCameraAngleX()<=40) {
		dbMoveObject(weapon,-7);
	}
	else {
		dbMoveObject(weapon, -8);
	}
	//dbMoveObjectRight(weapon, -2);
}

// GAME // 
int checkGame() {
	int n=0;

	if(player.health <= 0) {
		//gameOver
		dbPositionSound(soundDeathPlayer,dbObjectPositionX(character),dbObjectPositionY(character),dbObjectPositionZ(character));
		dbPlaySound(soundDeathPlayer);
		return 1;
	}
	else {
		for(int i=0; i<numEnemies; i++) {
			if(mobs[i].health <=0) {
				n++;
			}
			//else {
			if(n+1==numEnemies && dbObjectExist(mobs[i].id)) {
					lastEnemeyId=mobs[i].id;
				}
			//}
		}
		if(n==numEnemies) {
			//partita vinta
			return 2;
		}
	}
	//partita ancora in corso
	return 0;
}
void resetGame() {
	removeAllObjcets();
	setupScene();
	makeCharacter();
	makeEnemies();
	makeBullets();
	makeBulletMarkers();
	makeClip();
	makeMedikit();
	makeWeapon();
	makeExplosion();
}


// UTILITY //
bool mouseFullClick(int type) {
	bool buttonClick=false;
	if(dbMouseClick()==type) {
		while(dbMouseClick()==type);
		buttonClick=true;
	}
	return buttonClick;
}
float distanceBetween2Points(float x, float y, float z, float x2, float y2, float z2) {
	float distance = sqrtf(pow((x2-x),2) + pow((y2-y),2) + pow((z2-z),2));
	return distance;
}

void printInfo(void) {

	char healthChar[40];
	char healthEnemy[40];
	char posWeapon[40];
	char positionChar[40];
	char terrainXSize[40];
	char terrainZSize[40];
	char positionEnemy[40];

	char poly[40];

	char tCaricamento[40];

	char bulletsInfo[40];

	char idFakeTerrain[40];
	char idTerrain[40];

	wsprintf(healthChar,"Health char: %d",player.health);
	dbText(0,0,healthChar);

	wsprintf(healthEnemy,"Health enemy: %d",mobs[0].health);
	dbText(0,30,healthEnemy);

	wsprintf(posWeapon,"Weapon. X: %d, Y: %d, Z: %d",(int)dbObjectPositionX(weapon),(int)dbObjectPositionY(weapon),(int)dbObjectPositionZ(weapon));
	dbText(0,60,posWeapon);

	wsprintf(positionChar,"Char. X: %d, Y: %d, Z: %d",(int)dbObjectPositionX(character),(int)dbObjectPositionY(character),(int)dbObjectPositionZ(character));
	dbText(0,90,positionChar);
	
	if(blitzOn==true) {
		wsprintf(terrainXSize,"Terrain X: %d",(int)Terrain->GetTerrainSize());
		dbText(0,120,terrainXSize);

		wsprintf(terrainZSize,"Terrain Z: %d",(int)Terrain->GetTerrainSize());
		dbText(0,150,terrainZSize);
	}
	else {
		wsprintf(terrainXSize,"Terrain X: %d",(int)dbObjectSizeX(terrain,1));
		dbText(0,120,terrainXSize);

		wsprintf(terrainZSize,"Terrain Z: %d",(int)dbObjectSizeZ(terrain,1));
		dbText(0,150,terrainZSize);
	}

	wsprintf(positionEnemy,"Enemy. X: %d, Y: %d, Z: %d",(int)dbObjectPositionX(10),(int)dbObjectPositionY(10),(int)dbObjectPositionZ(10));
	dbText(0,180,positionEnemy);

	wsprintf(poly,"Poly: %d",dbStatistic(1));
	dbText(0,210,poly);

	wsprintf(tCaricamento,"Tempo carimento enemies: %d",tempoCaricamento);
	dbText(0,240,tCaricamento);

	wsprintf(bulletsInfo,"Bullet: %d | %d",player.currentBullets,player.nCaricatori);
	dbText(0,270,bulletsInfo);

	/*wsprintf(idFakeTerrain,"Id fake terrain: %d",fakeTerrain);
	dbText(0,300,idFakeTerrain);

	unsigned long t=Terrain->GetObjectID();
	unsigned long f=Terrain->GetID();

	wsprintf(idTerrain,"Id terrain: %d | %lu | %lu",terrain,Terrain->GetID(),Terrain->GetObjectID());
	dbText(0,330,idTerrain);*/
	
	if(bciOn==true) {
		thinkgear->printBciInfo();
	}
}

void showFPS(void) {
	char szFPS [ 256 ] = "";
	strcpy ( szFPS, "fps = " );
	strcat ( szFPS, dbStr ( dbScreenFPS ( ) ) );
	dbText ( dbScreenWidth ( ) - 20 - dbTextWidth ( szFPS ), dbScreenHeight ( ) - 40, szFPS );
}
float randFloatRange(float a, float b) {
	//srand(dbTimer());
	return ((b-a)*((float)rand()/RAND_MAX))+a;
}
void removeMob(int mobIndex) {
	SC_RemoveObject(mobs[mobIndex].id);
	dbDeleteObject(mobs[mobIndex].id);
}
void removeMobs(void) {
	for(int i=0; i<numEnemies; i++) {
		if(dbObjectExist(mobs[i].id)==1) {
			if(mobs[i].health<=0 && checkEnemyStateAnimation(mobs[i].currentAnimation)!=ENEMY_DEAD && dbSoundPlaying(soundDeathEnemy)==0 ) {
				dbHideObject(mobs[i].id);
				SC_RemoveObject(mobs[i].id);
				dbDeleteObject(mobs[i].id);
			}
		}
	}
}


void removeAllObjcets() {

	//sistemarla per eliminare solo quello che c'è, e non fare cicli di for a vuoto

	//remove mobs
	removeMobs();

	//remove player
	if(dbObjectExist(character)==1 && player.health<=0) {
		SC_RemoveObject(character);
		dbDeleteObject(character);
	}

	//remove bullets
	for(int i=0; i<maxBullets; i++) {
		if(dbObjectExist(bullets[i].id)==1) {
			SC_RemoveObject(bullets[i].id);
			dbDeleteObject(bullets[i].id);
			/*if(dbObjectExist(bullets[i].graphicId)==1) {
				SC_RemoveObject(bullets[i].graphicId);
				dbDeleteObject(bullets[i].graphicId);
			}*/
		}	
	}
	
	//remove all existent objects
	for(int i=0; i<12000; i++) {
		if(dbObjectExist(i)==1) {
			if(SC_CollisionStatus(i)==1) {
				SC_RemoveObject(i);
			}
			dbDeleteObject(i);
		}
	}
}


