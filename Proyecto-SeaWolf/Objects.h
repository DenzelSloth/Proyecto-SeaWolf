

enum KEYS { LEFT, RIGHT, SPACE, P, SHIFT };
enum IDS { GAME, INVENTORY, PLAYER, TORPEDO ,SHIP, MINE, EXPLOSION};
enum MENU { NONE,PLAY,OPTIONS, EXIT};
enum CONFIG {OPTION1, OPTION2, OPTION3, OPTION4, BACK, NOCONFIG};

struct Menu {
	bool exit;
	bool redraw;
	ALLEGRO_COLOR selectionColor;
	int selection;
	int mouseX;
	int mouseY;
};

struct Config {	
	int extraTimePoints[4];	  // Puntos para tiempo extra.
	int selectedExtraTimePoints;  //Guarda el puntuaje para tiempo extra seleccionado.
	int selectedOption;  //Posicion dentro del arreglo de los puntuajes
	ALLEGRO_COLOR selectedColor;
};

struct Score {
	int lastScores[5];
	int scoresLenght;
	int highScore;
};

struct Game {
	int ID;
	bool done;
	bool pause;
	bool redraw;
	bool isGameOver;
	int time;      //Tiempo que ha pasado
	int gameTime;  //Game time es el tiempo restante de juego.
	int score;
	int scoreMilestone;
	int milestone;
};

struct Submarine {
  int ID;
  int x;
  int y;
  int speed;

  ALLEGRO_BITMAP* image;
  int boundx;
  int boundy;
};

struct Inventory {
	int ID;
	int x;
	int y;
	int angle;

	int torpedoCount;
	int torpedoCooldown;
	int torpedoTimestamp;

	int reloadTime;
	int reloadTimeStamp;
	int torpedoReloadTime;
	bool reloading;

	ALLEGRO_BITMAP* image;
	int boundx;
	int boundy;

	ALLEGRO_SAMPLE* reloadSound;
};

struct Torpedo
{
  int ID;
  int x;
  int y;
  bool live;
  int speed;

  ALLEGRO_BITMAP* image;
  int boundx;
  int boundy;

  ALLEGRO_SAMPLE* launchSound;
};

struct Ship {
  int ID;
  int x;
  int y;
  bool live;
  int speed;
  int points;
  
  ALLEGRO_BITMAP* image;
  int boundx;
  int boundy;
 };

struct Mine {
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int points;

	ALLEGRO_BITMAP* image;
	int boundx;
	int boundy;
};

struct Explosion {
  int x;
  int y;
  bool live;

  int maxFrame;
  int curFrame;
  int frameCount;
  int frameDelay;
  int frameWidth;
  int frameHeight;
  int animationColumns;
  int animationDirection;

  ALLEGRO_BITMAP* image;
  ALLEGRO_SAMPLE* explotionSound;
};




