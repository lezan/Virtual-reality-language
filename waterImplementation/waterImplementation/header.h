#pragma once

// include Dark GDK header file
#include "DarkGDK.h"
#include "math.h"
#include "SC_Collision.h"
#include <BlitzTerrain.h>
#include "bci.h"
#include "waterEffect.h"

#define CAP_SELECT 30
#define CAP_IDLE 49
#define CAP_MOVEMENT 65
#define CAP_FIRE 72
#define CAP_RELOAD 154
#define CAP_PUTAWAY 170

#define MIN_SELECT 1
#define MIN_IDLE 31
#define MIN_MOVEMENT 51
#define MIN_FIRE 66
#define MIN_RELOAD 75
#define MIN_PUTAWAY 155

#define ENEMY_CAP_IDLE 0
#define ENEMY_CAP_MOVEMENT 67
//#define ENEMY_CAP_CHASE 67
#define ENEMY_CAP_FIRE 27
//#define ENEMY_CAP_GETAWAY 67
#define ENEMY_CAP_IMPACT 50
#define ENEMY_CAP_DEAD 87

#define ENEMY_MIN_IDLE 23
#define ENEMY_MIN_MOVEMENT 84
//#define ENEMY_MIN_CHASE 84
#define ENEMY_MIN_FIRE 49
//#define ENEMY_MIN_GETAWAY 84
#define ENEMY_MIN_IMPACT 60
#define ENEMY_MIN_DEAD 120

bool debug=1;
bool blitzOn=false;
bool bciOn=false;
bool waterOn=true;

bool firstRunning=true;
bool putAwayOn=false;

float radius = 7.0f;
float littleRadius = 2.0f;

float gravity = -0.5f;
float slope = 0.5f; 
int ground = 0;
int groundEnemies = 1;
int jumptimer = 0; //Timer for jumping

int tempoCaricamento=0; 

int view = 0; //First personal shoot(1) or third(0)
int vtimer=0; //Timer of view

int ztimer=0; //Timer of zoom
int zoom=0; //Zoom is on (1) or off(0)
float oldAngle=0.0f;

int ptimer=0; //Timer of pause

int reloadTimer=0; //Timer of reload

float maxDistance=150; //Max distance of following
float maxAttack=80; //Max distance for attack's enemies

const int speedFireEnemy=100; //Speed fire of enemy (weapon speed fire)
const int speedFirePlayer=10; //Speed fire of player (weapon speed fire)

const int numEnemies=2; //Number of enemies in game
const int firstEnemies=10; //Id of first enemy
const int firstBullet=firstEnemies+numEnemies; //Id of first bullet
const int maxBullets=1000; //Max bullets on game

const int firstGraphicBullet=firstBullet+maxBullets; //Id of first graphic bullet

const int firstBulletMarker=firstGraphicBullet+1; //Id of first marker
const int maxBulletMarker=40; //Max number of marker
int hole=firstBulletMarker; //Current marker

const int firstBulletClip=firstBulletMarker+maxBulletMarker; //Id of first clip
const int maxBulletClip=5; //Max number of ground's clip
const int maxPlayerClip=3; //Max number of clip for player

const int firstMedikit=firstBulletClip+maxBulletClip; //Id of first medikit
const int maxMedikit=5; //Max number of medikits
const int healthUp=30; //Restore life of medikits

const int firstWeapon=firstMedikit+maxMedikit; //Id of first ground's weapon
const int maxWeapon=3; //Max number of groun's weapons

const int firstExplosion=firstWeapon+maxWeapon;
const int maxExplosion=40;
const int timerExplosion=30;

int damageBullet=1; //Damage of bullets
int damageParticle=1; //Damage of particles (enemies)

const int swivelAngle=45; //theta
const int tiltAngle=45; //phi

const int waterHeigth=95; //Heigth of water

const int terrain=1; //Id of terrain object
const int character=2; //Id of character object
const int sky=3; //Id of sky object
const int weapon=4; //Id of weapon object
const int sphereCam=5; //Id of spherecam object
const int fakeTerrain=6; //Id of fake terrain object
const int water=7; //Id of water object

//Id of images
const int imageTexture=1;
const int imageDetail=2;
const int imageCrosshairNoScope=3;
const int imageCrosshairScope=4;
const int imageGameOver=5;
const int imageYouWin=6;
const int imagePresentation=7;
const int imageLoading=8;
const int imageStarting=9;
const int imagepause=10;
const int imageTextureEnemies=11;
const int imageHeightmap=12;
const int imageTextureMedikit=13;
const int imageWavesWater=14;
const int imageFresnel=15;
const int imageWaterMask=16;
const int imageExplosion=17;

const int groupWorld=1; //Id for world's group
const int groupEnemy=2; //Id for enemies's group
const int groupBullets=3; //Id for bullet's group
const int groupBulletMarkers=4; //Id for markers's group
const int groupBulletClip=5; //Id for clips's group

const int soundFirePlayer=1;
const int soundFireBulletEnemy=2;
const int soundFireParticleEnemey=3; 
const int soundHitPlayer=4;
const int soundHitEnemy=5; 
const int soundExplosion=6; 
const int soundEnemy=7;
const int soundDeathEnemy=8;
const int soundDeathPlayer=9;
const int soundMovementPlayer=10; 
const int soundStart=11;
const int soundSea=12;
const int soundJumpPlayer=13;

const int soundDryFireElite=14; //Id sound no-fire elite
const int soundFireElite=15; //Id sound fire elite
const int soundPutAwayElite=16; //Id sound put away elite
const int soundReloadElite=17; //Id sound reload elite
const int soundSelectElite=18; //Id sound select elite

const int spriteNoScope=1; //Id of sprite of no-scope
const int spriteScope=2; //Id of sprite of scope
const int spritePause=3;  //Id of sprite of pause

const int fxWater=1;

BT_Terrain* Terrain; //Objects for the setup of terrain with Blitz

waterEffect* WaterEffect;

bci* thinkgear;

int lastEnemeyId=0;

typedef struct tPlayer {
	int id; //id del giocatore
	int health; //vita del giocatore
	float vx; //velocita' lungo l'asse x del giocatore
	float vy; //velocita' lungo l'asse y del giocatore
	float vz; //velocita' lungo l'asse z del giocatore
	int speedFire; //ogni quanto viene sparato un proiettile se il giocatore sta sparando
	int damage; //entita' del danno fatto dal proiettile
	int mode; //stato in cui si trova il giocatore in riferimento all'arma (se sta caricando l'arma, se sta in idle, se sta sparando)
	int nCaricatori; //il numero di caricatori posseduti dal giocatore
	int currentBullets; //il numero di pallottole rimaste nel caricatore corrente
	int timerUpdateAnimation;
	int currentAnimation;
	int currentWeapon; //0=no weapon,1=elite,2=dual elite
} tPlayer;

tPlayer player;

typedef struct tMobs {
	int id;
	int health;
	int mode;
	//float vxEnemies;
	//float vyEnemies;
	//float vzEnemies;
	float velocityEnemies;
	int timeIdle;
	int timeMove;
	int speedFire;
	float accuracyX;
	float accuracyY;
	float accuracyZ;
	int timerUpdateAnimation;
	int currentAnimation;
	int hit;
	int speedFireParticle;
	int timerSoundEnemy;
	int timerHealth;
	bool recoveryHealth;
	bool controlDirection;
} tMobs;

tMobs mobs[numEnemies];

typedef struct tBullets {
	int id;                 //identificatore oggetto proiettile
	int life;               //tempo prima che esso sparisca
	float oldX;
	float oldY;
	float oldZ;
	float coordX;           //coordinata X del punto verso cui è stato sparato
	float coordY;			//coordinata Y del punto verso cui è stato sparato
	float coordZ;			//coordinata Z del punto verso cui è stato sparato
	int targetId;           //identificatore dell'oggetto puntato
	int sourceId;           //identificatore dell'oggetto che ha sparato il proiettile
	bool active;            //se il proiettile è attivo, quindi se è ancora in vita o è stato sparato
	int damage;             //la quantità di danno che infligge il proiettile; ci sono due tipi di proiettili
	int type;               //il tipo di proiettile. ci sono due tipi di proiettili: tante particelle o unica particella
	int velocity;
	int velocityY;
	//int graphicId;
	bool explosion;
} tBullets;

tBullets bullets[maxBullets];

typedef struct controlPlayerAnimation {
	bool select;
	bool idle;
	bool movement;
	bool fire;
	bool reload;
	bool putaway;
} controlPlayerAnimation;

controlPlayerAnimation controllerAnimation;

typedef struct controlEnemyAnimation {
	bool idle; //mode 0
	bool patrol; //mode 1
	bool chase; //mode 2
	bool fire; //mode 3
	bool getAway; //mode 4
	bool impact;
	bool dead; 
} controlEnemyAnimation;

controlEnemyAnimation controllerEnemy[numEnemies];

typedef struct tClips {
	int id;
	int state;
	float positionX;
	float positionY;
	float positionZ;
	int ray;
	int nBullets; //il numero di pallottole di ogni caricatore, elite 15 colpi
	int type;//1=elite, 2=dual elite etc
} tClips;

tClips clip[maxBulletClip];

typedef struct tMedikits {
	int id;
	int state;
	float positionX;
	float positionY;
	float positionZ;
	int ray;
} tMedikits;

tMedikits medikit[maxMedikit];

typedef struct tWeapons {
	float positionX;
	float positionY;
	float positionZ;
	int id;
	int state;
	int type;
	int ray;
} tWeapons;

tWeapons weapons[maxWeapon];

typedef struct tInventory {
	bool weapon1;
	bool weapon2;
} tInvetory;

tInventory inventory;

typedef struct tExplosion {
	int id;
	bool state;
	float x;
	float y;
	float z;
	int timer;
	int eScale;
} tExplosion;

tExplosion explosion[maxExplosion];

enum stateAnimation { 
	SELECT, 
	IDLE, 
	MOVEMENT, 
	FIRE, 
	RELOAD, 
	PUTAWAY, 
	ERRORE
};

stateAnimation playerAnimation;

enum stateAnimationEnemy {  
	ENEMY_IDLE, 
	ENEMY_MOVEMENT, 
	ENEMY_FIRE,
	ENEMY_IMPACT,
	ENEMY_DEAD,
	ENEMY_ERRORE
};

stateAnimationEnemy enemyAnimation[numEnemies];

//Prototipi
// SETUP OBJECTS //
void setupScene(void); //Setup and build terrain and skybox
void makeCharacter (void); //Make and setup of character
void makeEnemies(void); //Make and setup of enemies
void makeBullets(void); //Make and setup of bullets
void makeBulletMarkers(void); //Make and setup of markers
void makeClip(void); //Make and setup clips
void makeMedikit(void); //Make and setup medikit
void makeWeapon(void); //Make and setup weapons
void makeExplosion(void); //Make an explosion when a bullet impact on ground
// MOVEMENT //
void inputHandler(void); //Handler of input for player
void moveEnemies(void); //Movement of enemies
void moveCharacter(void); //Movement of player
// ANIMATIONS //
void updateAnimationEnemy(void); //Setup of animations for the enemies
void updateAnimationWeapon(void); //Setup of animations for the player
void movementAnimation(int startAnimation,int finalAnimation, int timer, int mobIndex); //Play animations of player and enemies
stateAnimation checkPlayerStateAnimation(int frame); //Return current player animation's state
stateAnimationEnemy checkEnemyStateAnimation(int frame); //Return current enemy animation's state
//PICKING //
void checkPicking(void); //Check if player can pick up some object from ground
void pickBulletClip(int idClip); //Pick up clips from ground
void pickMedikit(int idMedikit); //Pick up medikits from ground
void pickWeapon(int idWeapon); //Pick up weapons from ground
// BULLETS //
void shootBullets(void); //Bullets shoot for the player
void mobsAttack(int sourceId,int type); //Bullets shoot for the enemies
void updateBullets(void); //Update the movement of bullets
void updateExplosion(void); //Update the explosion with terrain of bullets
int takeFreeExplosion(void); //Take free explosion object
void takeDamage(int bulletId); //Hanlder for the damage, decrease health of player and enemies
void resetBullet(int bulletIndex); //Make free bullet
void resetBullets(void); //Make free bullets
void resetExplosion(int explosionIndex,int explosionIndex2); //Make free explosion
// CAMERA //
void updateCamera(int MouseDiffX, int MouseDiffY); //Update position of player's camera
// WEAPON //
void positionWeapon(void); //Update the position of player's weapon
// GAME // 
int checkGame(); //Check if the game is over or not
void resetGame(); //Reset the game for another play.
// UTILITY //
bool mouseFullClick(int type); //Input handler for mouse cliks
float distanceBetween2Points(float x, float y, float z, float x2, float y2, float z2); //Return the distance from two points
void showFPS(void); //Show current FPS
float randFloatRange(float a, float b); //Return pseudo random float betweeb 'a' and 'b'
void printInfo(void); //Print some info to screen
void removeMob(int mobIndex); //Remove a dead mob from game
void removeMobs(void); //Remove dead mobs from game
void removeAllObjcets(); //Remove all objects from game