#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include<allegro5\allegro_native_dialog.h>
#include "Objects.h"

/** Variables Constantes **/
const int WIDTH = 1024;  //Ancho de la pantalla.
const int HEIGHT = 768;  //Alto de la pantalla.
const int FPS = 60;      //Frames por segundo - Se usa para medir tiempos.

const int DEFAULT_GAME_TIME = 60;   //Tiempo de juego (En segundos).
const int DEFAULT_EXTRA_TIME = 30;   //Tiempo extra al llegar a la meta de puntos (En segundos).
const int NUM_TORPEDOS = 5;         //Cantidad Maxima de Torpedos.
const int TORPEDO_COOLDOWN = 5;     //Enfriamento entre disparos de torpedo (En segundos).
const int RELOAD_TIME = 5;          //Tiempo de regarga de torpedos (En segundos).
const int NUM_SHIPS = 2;            //Numero maximo de barcos en pantalla.
const int NUM_MINES = 2;            //Numero maximo de Minas en pantalla.
const int NUM_EXPLOSIONS = NUM_TORPEDOS + NUM_SHIPS + NUM_MINES; //Numero maximo de explosiones en pantalla.
const bool SHOW_HITBOXES = false;   //Muestra los límites de textura para colisiones.

bool keys[5] = { false, false, false, false }; //Arreglo para determinar estados de teclas. 

/** Variables Globales / Publicas **/
ALLEGRO_DISPLAY* DISPLAY; //Objeto de la pantalla.
Config CONFIG;            //Objeto para almacenar las configuaciones 
Score SCORES;             //Objeto para guardar las puntuaciones

/** Prototipo de Metodos  / Declaracion de funciones **/
int game();
int menu();
int config();

/** Metodo de incializacion de objetos **/
void initConfig(Config& config);
void initScore(Score& score);
void initGame(Game& game);
void initSubmarine(Submarine& submarine, ALLEGRO_BITMAP* image);
void initTorpedos(Torpedo torpedo[], int size, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* launchSound);
void initInventory(Inventory& inventory, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* reloadSound);
void initShip(Ship ships[], int size);
void initExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* explosionSound);
void initMines(Mine mines[], int size, ALLEGRO_BITMAP* mineImage);
/** Metodo de incializacion de objetos **/





void pauseGame(Game& game);
void drawSubmarine(Submarine& submarine);
void moveSubmarineLeft(Submarine& submarine);
void moveSubmarineRight(Submarine& submarine);
void reloadSubmarine(Inventory& inventory, Game& game);
void updateInventory(Torpedo torpedo[], int size, Inventory& inventory, Game& game);
void drawInventory(Torpedo torpedo[], int size, Inventory& inventory, ALLEGRO_FONT* font);
void drawTorpedos(Torpedo torpedo[], int size);
void fireTorpedo(Torpedo torpedo[], int size, Submarine& submarine, Inventory& inventory, Game& game);
void updateTorpedos(Torpedo torpedo[], int size);
void collideTorpedosShips(Torpedo torpedo[], int sizeTorpedos, Ship ships[], int sizeShips, Submarine& submarine,
    Game& game, Explosion explosions[], int eSize);
void collideTorpedosMines(Torpedo torpedo[], int sizeTorpedos, Mine mines[], int sizeMines, Submarine& submarine,
    Game& game, Explosion explosions[], int eSize);
void drawShip(Ship ships[], int size);
void startShip(Ship ships[], int size, ALLEGRO_BITMAP* shipImages[], ALLEGRO_SAMPLE* hornSound);
void updateShips(Ship ships[], int size);
void drawMines(Mine mines[], int size);
void startMine(Mine mines[], int size);
void updateMines(Mine mines[], int size);
void drawExplosions(Explosion explosions[], int size);
void startExplosions(Explosion explosion[], int size, int x, int y);
void updateExplosions(Explosion explosions[], int size);

/** Metodo main **/
int main(){
    DISPLAY = NULL;
    if (!al_init())
        return -1;

    /*Preparacion de modulos Allegro*/
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    /*Preparacion de modulos Allegro*/

    DISPLAY = al_create_display(WIDTH, HEIGHT);

    if (!DISPLAY)
        return -1;

    al_set_window_title(DISPLAY, "Menu Principal");

    initConfig(CONFIG);
    initScore(SCORES);
    menu();

    al_uninstall_mouse();
    al_uninstall_keyboard();
    al_destroy_display(DISPLAY);
    al_uninstall_audio();

    return 0;
}

int menu(void) {

    Menu menu;
    menu.exit = false;
    menu.redraw = false;
    menu.selection = NONE;
    menu.selectionColor = al_map_rgb(255, 255, 255);

    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_FONT* font48 = NULL;

    ALLEGRO_BITMAP* menuImage = NULL;
    ALLEGRO_SAMPLE* clicSound = NULL;
    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    menuImage = al_load_bitmap("Resources/MenuImage.png");
    font48 = al_load_font("Resources/PixelFont.ttf", 48, 0);

    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(DISPLAY));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (!menu.exit) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        al_draw_bitmap(menuImage, 0, 0, 0);
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            menu.redraw = true;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            menu.mouseX = ev.mouse.x;
            menu.mouseY = ev.mouse.y;

            //std::cout << "Mouse X, " << menu.mouseX << "  - MouseY, " << menu.mouseY << "\n";

            if (menu.mouseX >= 35 && menu.mouseX <= 485 && menu.mouseY >= 450 && menu.mouseY <= 531) {
                menu.selection = PLAY;                             
                if (ev.mouse.button & 1)
                    game();
            }
            else if (menu.mouseX >= 36 && menu.mouseX <= 484 && menu.mouseY >= 578 && menu.mouseY <= 660) {
                menu.selection = OPTIONS;
                if (ev.mouse.button & 1) {                  
                    config();
                    al_flush_event_queue(event_queue);
                }
                    
            }
            else if (menu.mouseX >= 900 && menu.mouseX <= 1000 && menu.mouseY >= 10 && menu.mouseY <= 100) {
                menu.selection = EXIT;
                   if (ev.mouse.button & 1)
                        return 1;
            }
            else{
                menu.selection = NONE;
            }

        }

        if (menu.redraw && al_is_event_queue_empty(event_queue)) {
            al_draw_bitmap(menuImage, 0, 0, 0);

            switch (menu.selection) {
                case PLAY:
                    al_draw_rectangle(37, 450, 485, 532, menu.selectionColor, 3);
                    break;
                case OPTIONS:
                    al_draw_rectangle(36, 578, 484, 660, menu.selectionColor, 3);
                break;
                case EXIT:
                    al_draw_rectangle(910, 10, 1000, 100, menu.selectionColor, 3);
                    break;
            }

            al_flip_display();           
        }
    }

    al_destroy_bitmap(menuImage);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font48);

    al_stop_samples();


    return 0;

}

int config(void) {

    Menu menu;
    menu.exit = false;
    menu.redraw = false;
    menu.selection = NOCONFIG;   
    menu.selectionColor = al_map_rgb(255, 255, 255);

    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_FONT* font28 = NULL;
    ALLEGRO_FONT* font48 = NULL;
    ALLEGRO_BITMAP* configImage = NULL;

    configImage = al_load_bitmap("Resources/OptionsMenuImage.png");
    font28 = al_load_font("Resources/PixelFont.ttf", 28, 0);
    font48 = al_load_font("Resources/PixelFont.ttf", 48, 0);
    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / FPS);

    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(DISPLAY));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (!menu.exit) {
        ALLEGRO_EVENT ev;

        //Espera a detectar eventos.
        al_wait_for_event(event_queue, &ev);      
      
        //Procesamiento de eventos
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            menu.redraw = true;
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            menu.mouseX = ev.mouse.x;
            menu.mouseY = ev.mouse.y;

            if (menu.mouseX >= 640 && menu.mouseX <= 948 && menu.mouseY >= 344 && menu.mouseY <= 415) {
                menu.selection = OPTION1;
                if (ev.mouse.button & 1){
                    CONFIG.selectedOption = menu.selection;
                    CONFIG.selectedExtraTimePoints = CONFIG.extraTimePoints[menu.selection];
                }
                    
            }
            else if (menu.mouseX >= 640 && menu.mouseX <= 948 && menu.mouseY >= 435 && menu.mouseY <= 509) {
                menu.selection = OPTION2;
                if (ev.mouse.button & 1) {
                    CONFIG.selectedOption = menu.selection;
                    CONFIG.selectedExtraTimePoints = CONFIG.extraTimePoints[menu.selection];
                }
            }
            else if (menu.mouseX >= 640 && menu.mouseX <= 948 && menu.mouseY >= 531 && menu.mouseY <= 600) {
                menu.selection = OPTION3;
                if (ev.mouse.button & 3) {
                    CONFIG.selectedOption = menu.selection;
                    CONFIG.selectedExtraTimePoints = CONFIG.extraTimePoints[menu.selection];
                }
            }
            else if (menu.mouseX >= 640 && menu.mouseX <= 948 && menu.mouseY >= 628 && menu.mouseY <= 695) {
                menu.selection = OPTION4;
                if (ev.mouse.button & 1) {
                    CONFIG.selectedOption = menu.selection;
                    CONFIG.selectedExtraTimePoints = CONFIG.extraTimePoints[menu.selection];
                }
            }
            else if (menu.mouseX >= 909 && menu.mouseX <= 1000 && menu.mouseY >= 11 && menu.mouseY <= 97) {
                menu.selection = BACK;
                if (ev.mouse.button & 1) {
                    al_flush_event_queue(event_queue);
                    menu.exit = true;
                    al_rest(1);
                }                    
            }
            else {
                menu.selection = NOCONFIG;
            }
        }

        //Evento que detecta el cierre de la ventana (windows).
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            menu.exit = true;

        if (menu.redraw && al_is_event_queue_empty(event_queue)) {
            //Dibuja el fonto
            al_draw_bitmap(configImage, 0, 0, 0);

            //Ciclo que dibuja las ultimas puntuaciones,Si no hay escribe un texto
            if (SCORES.scoresLenght == 0) {
                al_draw_textf(font28, al_map_rgb(0, 0, 0),
                    120, 400, 0, "No hay");
                al_draw_textf(font28, al_map_rgb(0, 0, 0),
                    120, 400 + 28, 0, "puntuaciones");
                al_draw_textf(font28, al_map_rgb(0, 0, 0),
                    120, 400 + 56, 0, "recientes.");
            }
            else {
                for (int i = 0; i < SCORES.scoresLenght; i++) {
                    al_draw_textf(font28, al_map_rgb(0, 0, 0),
                        120, 300 + (35*i), 0, "%i", SCORES.lastScores[i]);
                }
            }

            //Dibuja los puntos para tiempo extra en los botones.
            for (int i = 0; i < 4; i++) {
                al_draw_textf(font28, al_map_rgb(0, 0, 0),
                    665, 355 + (100 * i), 0, "%i", CONFIG.extraTimePoints[i]);
            }

            //Dibuja recuadro rojo en la opcion seleccionada
            switch (CONFIG.selectedOption) {
            case OPTION1:
                al_draw_rectangle(640, 344, 948, 415, CONFIG.selectedColor, 3);
                break;
            case OPTION2:
                al_draw_rectangle(640, 435, 948, 509, CONFIG.selectedColor, 3);
                break;
            case OPTION3:
                al_draw_rectangle(640, 531, 948, 600, CONFIG.selectedColor, 3);
                break;
            case OPTION4:
                al_draw_rectangle(640, 628, 948, 695, CONFIG.selectedColor, 3);
                break;
            case BACK:
                al_draw_rectangle(909, 11, 1000, 97, CONFIG.selectedColor, 3);
                break;
            }

            //Dibuja rectangulo blanco cuando pasas el mouse
            switch (menu.selection) {
            case OPTION1:
                al_draw_rectangle(640, 344, 948, 415, menu.selectionColor, 3);
                break;
            case OPTION2:
                al_draw_rectangle(640, 435, 948, 509, menu.selectionColor, 3);
                break;
            case OPTION3:
                al_draw_rectangle(640, 531, 948, 600, menu.selectionColor, 3);
                break;
            case OPTION4:
                al_draw_rectangle(640, 628, 948, 695, menu.selectionColor, 3);
                break;
            case BACK:
                al_draw_rectangle(909, 11, 1000, 97, menu.selectionColor, 3);
                break;
            }

            al_flip_display();
        }
    }

    al_destroy_bitmap(configImage);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font48);
    al_destroy_font(font28);   
    al_stop_samples();
  
    return 0;
}

int game(void) {

    /* Objetos / Clases */
    //Game es la Clase,  y game es e objeto
    Game game;
    Inventory inventory;
    Submarine submarine;
    Torpedo torpedos[NUM_TORPEDOS];
    Ship ships[NUM_SHIPS];
    Mine mines[NUM_MINES];
    Explosion explosions[NUM_EXPLOSIONS];
    
    ALLEGRO_EVENT_QUEUE* event_queue = NULL; //Cola de eventos
    ALLEGRO_TIMER* timer = NULL;  //Reloj
    ALLEGRO_FONT* font28 = NULL;  //Fuente pequena
    ALLEGRO_FONT* font48 = NULL;  //Fuente Grande

    /*Declara las imagenes a usar*/
    ALLEGRO_BITMAP* backgroundImage = NULL;
    ALLEGRO_BITMAP* submarineImage = NULL;
    ALLEGRO_BITMAP* reloadImage = NULL;
    ALLEGRO_BITMAP* rocketImage = NULL;
    ALLEGRO_BITMAP* mineImage = NULL; 
    ALLEGRO_BITMAP* shipImages[4];
    ALLEGRO_BITMAP* expImage = NULL;

    /*Declara los sonidos a usar*/
    ALLEGRO_SAMPLE* launchSound = NULL;
    ALLEGRO_SAMPLE* reloadSound = NULL;
    ALLEGRO_SAMPLE* explosionSound = NULL;
    ALLEGRO_SAMPLE* hornSound = NULL;

    /*Definiciones*/

    event_queue = al_create_event_queue();  //Definicion de la cola de eventos
    timer = al_create_timer(1.0 / FPS);     //Definicion de el reloj

    /*Define las imagenes*/
    //C:\XXXX\XXXX\Desktop\Proyecto-SeaWolf\Proyecto-SeaWolf\Resources   -- Ruta Absoluta
    //Resources/BackgroundImage.png   -- Rutas relativas  
    backgroundImage = al_load_bitmap("Resources/BackgroundImage.png");
    submarineImage = al_load_bitmap("Resources/CrosshairSprite.png");
    reloadImage = al_load_bitmap("Resources/reloadSprite.png");
    rocketImage = al_load_bitmap("Resources/RocketSprite.png");
    mineImage = al_load_bitmap("Resources/WaterMine.png");
    shipImages[0] = al_load_bitmap("Resources/ContainerShipSprite.png");
    shipImages[1] = al_load_bitmap("Resources/CargoShipSprite.png");
    shipImages[2] = al_load_bitmap("Resources/BuqueSprite.png");
    shipImages[3] = al_load_bitmap("Resources/BotePTSprite.png");
    expImage = al_load_bitmap("Resources/PixelExplotionSpriteFramesImage.png");

    /*Guarda espacios en la memoria de la tarjeta de sonido para que se puedran reproducir todos*/
    al_reserve_samples(16);

    /*Carga los sonidos*/
    launchSound = al_load_sample("Resources/SonarSound.ogg");
    reloadSound = al_load_sample("Resources/LoadSound.ogg");
    explosionSound = al_load_sample("Resources/ExplosionSound.ogg");
    hornSound = al_load_sample("Resources/BotePTSound.ogg");

    /*Trae numeros aleatorios segun los segundos que se executo (time)*/
    srand(time(NULL));

    /*Inicializa los objetos*/
    initGame(game);
    initSubmarine(submarine, submarineImage);
    initInventory(inventory, reloadImage, reloadSound);
    initTorpedos(torpedos, NUM_TORPEDOS, rocketImage, launchSound);
    initMines(mines, NUM_MINES, mineImage);
    initShip(ships, NUM_SHIPS);
    initExplosions(explosions, NUM_EXPLOSIONS, expImage, explosionSound);

    font28 = al_load_font("Resources/PixelFont.ttf", 28, 0);
    font48 = al_load_font("Resources/PixelFont.ttf", 48, 0);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(DISPLAY));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_start_timer(timer);
    while (!game.done) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if (game.pause) {
            al_draw_textf(font28, al_map_rgb(255, 0, 0),
                WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Juego Pausado");
            al_flip_display();
            al_stop_timer(timer);
        }
        else {
            al_start_timer(timer);
        }
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            
            game.redraw = true;
            game.time = al_get_timer_count(timer);
            if (keys[LEFT])
                moveSubmarineLeft(submarine);
            else if (keys[RIGHT])
                moveSubmarineRight(submarine);

            if (!game.isGameOver) {
                startShip(ships, NUM_SHIPS, shipImages, hornSound);
                startMine(mines, NUM_MINES);  
                updateMines(mines, NUM_MINES);
                updateShips(ships, NUM_SHIPS);    
                updateTorpedos(torpedos, NUM_TORPEDOS);
                updateInventory(torpedos, NUM_TORPEDOS, inventory, game);
                collideTorpedosMines(torpedos, NUM_TORPEDOS, mines, NUM_MINES, submarine, game, explosions, NUM_EXPLOSIONS);
                collideTorpedosShips(torpedos, NUM_TORPEDOS, ships, NUM_SHIPS, submarine, game, explosions, NUM_EXPLOSIONS);
                updateExplosions(explosions, NUM_EXPLOSIONS);
                if (game.score >= (CONFIG.selectedExtraTimePoints * game.scoreMilestone) && CONFIG.selectedOption > 0) {
                    game.gameTime += (DEFAULT_EXTRA_TIME*100);
                    game.scoreMilestone++;
                }
                 if ((game.gameTime - game.time) <= 0)
                    game.isGameOver = true;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_P:
                case ALLEGRO_KEY_ESCAPE:
                    keys[P] = true;
                    pauseGame(game);
                    break;
                case ALLEGRO_KEY_A:
                case ALLEGRO_KEY_LEFT:
                    keys[LEFT] = true;
                    break;
                case ALLEGRO_KEY_D:
                case ALLEGRO_KEY_RIGHT:
                    keys[RIGHT] = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                    keys[SPACE] = true;        
                    fireTorpedo(torpedos, NUM_TORPEDOS, submarine, inventory, game);
                    break;
            
                case ALLEGRO_KEY_LSHIFT:
                case ALLEGRO_KEY_RSHIFT:
                    keys[SHIFT] = true;
                    reloadSubmarine(inventory, game);
                    break;              
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
            case ALLEGRO_KEY_P:
                keys[P] = false;
                break;
            case ALLEGRO_KEY_A:
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = false;
                break;
            case ALLEGRO_KEY_D:
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = false;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
            case ALLEGRO_KEY_LSHIFT:
            case ALLEGRO_KEY_RSHIFT:
                keys[SHIFT] = false;
                break;

            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
            game.done = true;

        if (game.redraw && al_is_event_queue_empty(event_queue)) {
            game.redraw = false;
            if (!game.isGameOver) {     
                drawInventory(torpedos, NUM_TORPEDOS, inventory,font28);
                drawTorpedos(torpedos, NUM_TORPEDOS);
                drawShip(ships, NUM_SHIPS);
                drawMines(mines, NUM_MINES);
                drawExplosions(explosions, NUM_EXPLOSIONS);
                drawSubmarine(submarine);

                al_draw_textf(font28, al_map_rgb(255, 0, 0),
                    WIDTH - 120, HEIGHT - 78, ALLEGRO_ALIGN_CENTRE, "Puntuacion Top");
                al_draw_textf(font48, al_map_rgb(0, 0, 0),
                    WIDTH-120, HEIGHT - 50, ALLEGRO_ALIGN_CENTRE, "%i", SCORES.highScore);

                al_draw_textf(font28, al_map_rgb(255, 0, 0),
                    120, HEIGHT - 178, ALLEGRO_ALIGN_CENTRE, "Puntos", game.score);
                al_draw_textf(font48, al_map_rgb(0, 0, 0),
                    120, HEIGHT - 150, ALLEGRO_ALIGN_CENTRE, "%i", game.score);

                al_draw_textf(font28, al_map_rgb(255, 0, 0),
                    120, HEIGHT - 78, ALLEGRO_ALIGN_CENTRE, "Tiempo Restante");
                al_draw_textf(font48, al_map_rgb(0, 0, 0),
                    120, HEIGHT-50, ALLEGRO_ALIGN_CENTRE, "%i", ((game.gameTime - game.time) / 100));
            }
            else {
                al_clear_to_color(al_map_rgb(0,0, 0));                
                            
                if (SCORES.scoresLenght < 5) {
                    SCORES.lastScores[SCORES.scoresLenght] = game.score;
                    SCORES.scoresLenght++;
                }else{
                    SCORES.scoresLenght = 0;
                    SCORES.lastScores[SCORES.scoresLenght] = game.score;
                }

                for (int i = 0; i < SCORES.scoresLenght; i++) {
                    if (game.score > SCORES.lastScores[i]) {
                        SCORES.highScore = game.score;
                    }   
                    else {
                        SCORES.highScore = SCORES.lastScores[i];
                    }
                }



                al_draw_textf(font28, al_map_rgb(0, 255, 255),
                    WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "Tiempo agotado, Puntos: %i", game.score);
                al_flip_display();
                al_rest(3);
                game.done = true;
                
            }

            al_flip_display();
            al_draw_bitmap(backgroundImage, 0, 0, 0);             
        }
    }

    al_destroy_bitmap(backgroundImage);
    al_destroy_bitmap(submarineImage);
    al_destroy_bitmap(reloadImage);
    al_destroy_bitmap(rocketImage);
    al_destroy_bitmap(mineImage);
    al_destroy_bitmap(shipImages[0]);
    al_destroy_bitmap(shipImages[1]);
    al_destroy_bitmap(shipImages[2]);
    al_destroy_bitmap(shipImages[3]);
    al_destroy_bitmap(expImage);

    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_font(font28);
    al_destroy_font(font48);

    al_stop_samples();
    al_destroy_sample(launchSound);
    al_destroy_sample(explosionSound);
    al_destroy_sample(reloadSound);
    al_destroy_sample(hornSound);
    
    
   
    return 0;

}

void initConfig(Config& config) {
    config.extraTimePoints[0] = 0;
    config.extraTimePoints[1] = 1000;
    config.extraTimePoints[2] = 1500;
    config.extraTimePoints[3] = 2000;
    config.selectedExtraTimePoints = 1;
    config.selectedExtraTimePoints = config.extraTimePoints[OPTION1];
    config.selectedColor = al_map_rgb(255, 0, 0);
}

void initScore(Score& score) {
    int scoresLenght = 0;
}

void initGame(Game& game) {
    game.ID = GAME;
    game.done = false;
    game.pause = false;
    game.redraw = true;
    game.isGameOver = false;
    game.score = 0;
    game.gameTime = DEFAULT_GAME_TIME * 100; //Convierte los segundos a milisegundos.
    game.scoreMilestone = CONFIG.selectedOption;
    game.milestone = 1;
}

void pauseGame(Game& game) {
    if (game.pause)
        game.pause = false;
    else
        game.pause = true;
}

void initSubmarine(Submarine& submarine, ALLEGRO_BITMAP* image) {
    submarine.ID = PLAYER;
    submarine.x = WIDTH / 2;
    submarine.y = HEIGHT / 2;
    submarine.speed = 10;  
    submarine.image = image;
    submarine.boundx = al_get_bitmap_width(submarine.image);
    submarine.boundy = al_get_bitmap_height(submarine.image);   
}

void drawSubmarine(Submarine& submarine) { 
    al_draw_bitmap(submarine.image, submarine.x , submarine.y, 0);
}

void moveSubmarineLeft(Submarine& submarine) {
    submarine.x -= submarine.speed;
    if ((submarine.x + submarine.boundx) < 0) 
        submarine.x = 0;
}

void moveSubmarineRight(Submarine& submarine) {
    submarine.x += submarine.speed;
    if (submarine.x > (WIDTH - submarine.boundx)) 
        submarine.x = (WIDTH - submarine.boundx);
}

void initInventory(Inventory& inventory, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* reloadSound) {
    inventory.x = 30;
    inventory.y = 100;
    inventory.angle = 0;

    inventory.image = image;
    inventory.boundx = al_get_bitmap_width(inventory.image);
    inventory.boundy = al_get_bitmap_height(inventory.image);

    inventory.torpedoCount = NUM_TORPEDOS;
    inventory.torpedoCooldown = TORPEDO_COOLDOWN * 100;
    inventory.torpedoTimestamp = 0;

    inventory.reloadTime = RELOAD_TIME * 100;
    inventory.reloadTimeStamp = 0;
    inventory.reloading = false;

    inventory.reloadSound = reloadSound;
}

void reloadSubmarine(Inventory& inventory, Game& game) {
    if (inventory.torpedoCount == 0 && !inventory.reloading){
        inventory.reloading = true;
        inventory.torpedoTimestamp = (game.time + inventory.reloadTime);
        al_play_sample(inventory.reloadSound, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void updateInventory(Torpedo torpedo[], int size, Inventory& inventory, Game& game) {

    if (inventory.reloading) {
        if (inventory.torpedoTimestamp < game.time) {
            inventory.reloading = false;
            inventory.torpedoCount = size;
        }
    }
}

void drawInventory(Torpedo torpedo[], int size, Inventory& inventory, ALLEGRO_FONT* font) {
    if (inventory.torpedoCount == 0 && !inventory.reloading) {
        al_draw_bitmap(inventory.image, inventory.x, inventory.y, 0);
        al_draw_textf(font, al_map_rgb(0, 0, 0),
            WIDTH / 2, 58, ALLEGRO_ALIGN_CENTRE, "Shift para recargar.");
    }
    else if(inventory.reloading){
        al_draw_rotated_bitmap(inventory.image,
            (inventory.boundx/2), (inventory.boundy/2), inventory.x+20, inventory.y+20, inventory.angle += ((1*3.14159)/2), 0);
        al_draw_textf(font, al_map_rgb(0, 0, 0),
            WIDTH / 2, 58, ALLEGRO_ALIGN_CENTRE, "Recargando...");
    }

    for (int i = 0; i < size; i++)
    {
        if (i >= inventory.torpedoCount) {
            al_draw_tinted_bitmap(torpedo[i].image, al_map_rgba_f(0, 0, 0, 0.5), (10 + (torpedo[i].boundx * i)) + 10, 20, 0);
        }
        else {
            al_draw_bitmap(torpedo[i].image, (10 + (torpedo[i].boundx * i)) + 10, 20, 0);
        }

    }

}

void initTorpedos(Torpedo torpedo[], int size, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* launchSound) {
    for (int i = 0; i < size; i++) {
        torpedo[i].ID = TORPEDO;
        torpedo[i].speed = 10;
        torpedo[i].live = false;
        torpedo[i].image = image;
        torpedo[i].boundx = al_get_bitmap_width(torpedo[i].image);
        torpedo[i].boundy = al_get_bitmap_height(torpedo[i].image);
        torpedo[i].launchSound = launchSound;
    }
}

void drawTorpedos(Torpedo torpedo[], int size) {
    for (int i = 0; i < size; i++) {
        if (torpedo[i].live) {
            al_draw_bitmap(torpedo[i].image, torpedo[i].x, torpedo[i].y, 0);
            if (SHOW_HITBOXES) {
                al_draw_filled_rectangle(torpedo[i].x, torpedo[i].y,
                    torpedo[i].x + torpedo[i].boundx, torpedo[i].y + torpedo[i].boundy, al_map_rgba(255, 0, 255, 100));
            }           
        }
    }
}

void fireTorpedo(Torpedo torpedo[], int size,Submarine& submarine ,Inventory& inventory, Game& game) {
    for (int i = 0; i < size; i++) {
        if (inventory.torpedoTimestamp < game.time && inventory.torpedoCount > 0 && !game.pause) {
            if (!torpedo[i].live) {
                al_play_sample(torpedo[i].launchSound, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                torpedo[i].x = (submarine.x - (submarine.boundx / 2));
                torpedo[i].y = HEIGHT;
                torpedo[i].live = true;
                inventory.torpedoCount--;
                inventory.torpedoTimestamp = (game.time + inventory.torpedoCooldown);              
                break;
            }
        }
    }
}

void updateTorpedos(Torpedo torpedo[], int size) {
    for (int i = 0; i < size; i++) {
        if (torpedo[i].live) {
            torpedo[i].y -= torpedo[i].speed;
            if (torpedo[i].y < 0- torpedo[i].boundy)
                torpedo[i].live = false;
        }
    }
}

void collideTorpedosShips(Torpedo torpedo[], int sizeTorpedos, Ship ships[], int sizeShips, Submarine& submarine,
    Game& game,Explosion explosions[], int eSize) {
    for (int i = 0; i < sizeTorpedos; i++) {
        if (torpedo[i].live) {
            for (int j = 0; j < sizeShips; j++) {
                if (ships[j].live) {
                    if ((torpedo[i].x < ships[j].x + ships[i].boundx) &&
                        (ships[j].x < torpedo[i].x + torpedo[i].boundx) &&
                        (torpedo[i].y < ships[j].y + ships[j].boundy) &&
                        (ships[j].y < torpedo[i].y + torpedo[i].boundy))   // is b under
                    {
                        torpedo[i].live = false;
                        ships[j].live = false;
                        game.score += ships[j].points;
                        startExplosions(explosions, eSize, torpedo[i].x, torpedo[i].y);
                    }
                }
            }
        }
    }
}

void collideTorpedosMines(Torpedo torpedo[], int sizeTorpedos, Mine mines[], int sizeMines, Submarine& submarine,
    Game& game, Explosion explosions[], int eSize) {
    for (int i = 0; i < sizeTorpedos; i++) {
        if (torpedo[i].live) {
            for (int j = 0; j < sizeMines; j++) {
                if (mines[j].live) {
                    if ((torpedo[i].x < mines[j].x + mines[i].boundx) &&
                        (mines[j].x < torpedo[i].x + torpedo[i].boundx) &&
                        (torpedo[i].y < mines[j].y+ mines[j].boundy) &&
                        (mines[j].y < torpedo[i].y+ torpedo[i].boundy))   // is b under
                    {
                        torpedo[i].live = false;
                        mines[j].live = false;
                        game.score += mines[j].points;
                        startExplosions(explosions, eSize, torpedo[i].x, torpedo[i].y);
                    }
                }
            }
        }
    }
}

void initShip(Ship ships[], int size) {
    for (int i = 0; i < size; i++) {
        ships[i].ID = SHIP;
        ships[i].speed = 5;
        ships[i].live = false;
        ships[i].boundx = 35;
        ships[i].boundy = 35;
        ships[i].points = 0;
    }
}

void drawShip(Ship ships[], int size) {
    for (int i = 0; i < size; i++) {
        if (ships[i].live) {
            if (ships[i].x  > WIDTH + ships[i].boundx || ships[i].x < 0 - ships[i].boundx) {
                ships[i].live = false;
            }
            if (ships[i].speed > 0) {
                al_draw_bitmap(ships[i].image, ships[i].x, ships[i].y, ALLEGRO_FLIP_HORIZONTAL);
            }
            else {
                al_draw_bitmap(ships[i].image, ships[i].x, ships[i].y, 0);
            }           
            if (SHOW_HITBOXES) {
               al_draw_filled_rectangle(ships[i].x, ships[i].y,
                  ships[i].x + ships[i].boundx, ships[i].y + ships[i].boundy, al_map_rgba(255, 0, 255, 100));
            }
          
        }
    }
}

void startShip(Ship ships[], int size,ALLEGRO_BITMAP* shipImages[], ALLEGRO_SAMPLE* hornSound) {
    for (int i = 0; i < size; i++) {
        if (!ships[i].live) {
            ships[i].live = true;     
            int randomShip = ((rand() % 4)+1);            
            switch (randomShip) {
            case 1: //Container Ship
                ships[i].image = shipImages[0];
                ships[i].boundx = al_get_bitmap_width(ships[i].image);
                ships[i].boundy = al_get_bitmap_height(ships[i].image);
                ships[i].points = 100;
                ships[i].speed = 5;
                break;
            case 2://Cargo Ship
                ships[i].image = shipImages[1];
                ships[i].boundx = al_get_bitmap_width(ships[i].image);
                ships[i].boundy = al_get_bitmap_height(ships[i].image);
                ships[i].points = 200;
                ships[i].speed = 5;
                break;
            case 3://Buque
                ships[i].image = shipImages[2];
                ships[i].boundx = al_get_bitmap_width(ships[i].image);
                ships[i].boundy = al_get_bitmap_height(ships[i].image);
                ships[i].points = 300;
                ships[i].speed = 5;
                break;
            case 4://BotePT
                ships[i].image = shipImages[3];
                ships[i].boundx = al_get_bitmap_width(ships[i].image);
                ships[i].boundy = al_get_bitmap_height(ships[i].image);
                ships[i].points = 700;
                ships[i].speed = 10;
                al_play_sample(hornSound, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                break;
            }
         
            if (rand() % 2) {//Derecha
                ships[i].speed = ships[i].speed  * 1;
                ships[i].x = 0 - ships[i].boundx;
            }
                
            else {//Izquierda
                ships[i].speed = ships[i].speed * -1;
                ships[i].x = WIDTH;
            }

            if (rand() % 2) {//Arriba
                ships[i].speed = ships[i].speed * 1;
                ships[i].x = 0 - ships[i].boundx;
                ships[i].y = HEIGHT / 6;
            }

            else {//Abajo
                ships[i].speed = ships[i].speed * -1;
                ships[i].x = WIDTH;
                ships[i].y = (HEIGHT / 6) + 100;
            }
        }
    }
}

void updateShips(Ship ships[], int size) {
    for (int i = 0; i < size; i++) {
        if (ships[i].live) {
            ships[i].x += ships[i].speed;
        }
    }
}

void initMines(Mine mines[], int size, ALLEGRO_BITMAP* mineImage) {
    for (int i = 0; i < size; i++) {
        mines[i].ID = MINE;
        mines[i].speed = 1;
        mines[i].live = false;
        mines[i].points = 0;
        mines[i].image = mineImage;
        mines[i].boundx = al_get_bitmap_width(mines[i].image);
        mines[i].boundy = al_get_bitmap_height(mines[i].image);
    }
}

void drawMines(Mine mines[], int size) {
    for (int i = 0; i < size; i++) {
        if (mines[i].live) {

            if (mines[i].x > WIDTH + mines[i].boundx || mines[i].x < 0 - mines[i].boundx) {
                mines[i].live = false;
            }

            if (mines[i].speed > 0) {
                al_draw_bitmap(mines[i].image, mines[i].x, mines[i].y, ALLEGRO_FLIP_HORIZONTAL);
            }
            else {
                al_draw_bitmap(mines[i].image, mines[i].x, mines[i].y, 0);
            }

            if (SHOW_HITBOXES) {
                al_draw_filled_rectangle(mines[i].x, mines[i].y,
                    mines[i].x + mines[i].boundx, mines[i].y + mines[i].boundy, al_map_rgba(255, 0, 255, 100));
            }

        }
    }
}

void startMine(Mine mines[], int size) {
    for (int i = 0; i < size; i++) {
        if (!mines[i].live) {
            mines[i].live = true;

            if (rand() % 2) {//Derecha
                mines[i].speed = mines[i].speed * 1;
                mines[i].x = 0 - mines[i].boundx;
            }

            else {//Izquierda
                mines[i].speed = mines[i].speed * -1;
                mines[i].x = WIDTH;
            }

            if (rand() % 2) {//Arriba
                mines[i].speed = mines[i].speed * 1;
                mines[i].x = 0 - mines[i].boundx;
                mines[i].y = HEIGHT - 200;
            }

            else {//Abajo
                mines[i].speed = mines[i].speed * -1;
                mines[i].x = WIDTH;
                mines[i].y = HEIGHT - 300;
            }   
        }
    }
}

void updateMines(Mine mines[], int size) {
    for (int i = 0; i < size; i++) {
        if (mines[i].live) {
            mines[i].x += mines[i].speed;
        }
    }
}

void initExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP* image, ALLEGRO_SAMPLE* explotionSound) {
    for (int i = 0; i < size; i++) {
        explosions[i].live = false;
        explosions[i].maxFrame = 31;
        explosions[i].curFrame = 0;
        explosions[i].frameCount = 0;
        explosions[i].frameDelay = 1;
        explosions[i].frameWidth = 128;
        explosions[i].frameHeight = 128;
        explosions[i].animationColumns = 8;
        explosions[i].animationDirection = 1;
        explosions[i].image = image;
        explosions[i].explotionSound = explotionSound;
    }
}

void drawExplosions(Explosion explosions[], int size) {
    for (int i = 0; i < size; i++) {
        if (explosions[i].live) {
            int fx = (explosions[i].curFrame % explosions[i].animationColumns) * explosions[i].frameWidth;
            int fy = (explosions[i].curFrame / explosions[i].animationColumns) * explosions[i].frameHeight;

            al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth,
                explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2,
                explosions[i].y - explosions[i].frameHeight / 2, 0);
        }
    }
}

void startExplosions(Explosion explosion[], int size, int x, int y) {
    for (int i = 0; i < size; i++) {
        if (!explosion[i].live) {
            al_play_sample(explosion[i].explotionSound, 1.0, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            explosion[i].live = true;
            explosion[i].x = x;
            explosion[i].y = y;
            break;
        }
    }
}

void updateExplosions(Explosion explosions[], int size) {
    for (int i = 0; i < size; i++) {
        if (explosions[i].live) {
            if (++explosions[i].frameCount >= explosions[i].frameDelay) {
                explosions[i].curFrame += explosions[i].animationDirection;
                if (explosions[i].curFrame >= explosions[i].maxFrame) {
                    explosions[i].curFrame = 0;
                    explosions[i].live = false;
                }
                explosions[i].frameCount = 0;
            }
        }
    }
}



























