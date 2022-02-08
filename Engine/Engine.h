#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <math.h>
#include <iostream>

class Timer
{
public:

    Timer();

    Timer(float s);

    void set_seconds(float s);

    void start();

    bool morethanseconds();

private:

    Uint32 startedticks, ticks;

    float seconds;

    bool started;
};

class Texture
{
public:
    Texture();

    ~Texture();

    bool loadFromFile(std::string path);

    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

    void SetColor(Uint8 red, Uint8 green, Uint8 blue);

    void setBlendMode(SDL_BlendMode blending);

    void setAlpha(Uint8 alpha);

    void free();

    void render(int x, int y, SDL_Rect* clip = NULL);

    int getWidth();

    int getHeight();

    void to_input(SDL_Rect& col);

private:
    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;

};

class Tilemap
{
public:

    Texture elements[1064];

    int ground[40][24];

    int objects[40][24];

    SDL_Rect collider[40][24];

    Tilemap();

    bool load();

    void set();

    void show(int id);

    void free();

    bool loadFromfile(std::string path, int l);

};
class Player
{
public:
    SDL_Rect up[3], down[3], left[3], right[3], Collider;

    Texture player_texture;

    int frame, lastx, lasty, velocity, map;

    int health, strenght;

    bool ismoving, keyboard_active;

    std::string nick, id, save;

    Player(int pozx, int pozy);

    bool load();

    void handleEvents(SDL_Event& e);

    void move(SDL_Rect& wall);

    void render();

    ~Player();

};

class NPC
{
public:
    virtual bool load() = 0;
    virtual void render() = 0;
    bool ismoving;
    int lastx, lasty;
    SDL_Rect Collider, up[3], down[3], left[3], right[3];
    Texture npc_texture;
    int frame;
};

class Start_men : public NPC
{
public:
    Start_men(int x, int y);

    bool load();

    void render();

private:
    Texture npc_texture;

    int frame;
};
class Animation
{
private:
    Timer timer;
    char direction;
    bool animation;
    float seconds;
public:
    Animation(float s, char d = 's');

    void start();

    void check();

    void anim(NPC* npc);

    void anim(Player* player);
};

class Dialog
{
    Texture back;
    std::string texts[100];
    Texture text;
    int i;
    int page, max_pages;
    int pixels[5];
    Timer next;
    bool view;
    bool to_hide;
public:
    Dialog(std::string path);

    void next_page(SDL_Event& e);

    void start();


    void draw();

};
class Input
{
public:
    Input(int x, int y);

    void inputtext(SDL_Event& e);

    void inside(int x, int y, SDL_Event& e);

    void render();

    SDL_Rect Collider;

    Texture texttexture;

    bool write, renderText;

    std::string text;

    SDL_Point mPosition;

};

class Button
{
public:
    Button(std::string pathin, std::string pathout);

    void setPosistion(int x, int y);

    int handleEvent(SDL_Event* e);

    void render();

    ~Button();

private:
    SDL_Point mPosition;

    Texture mousein, mouseuot;

    int BUTTON_HEIGHT, BUTTON_WIDTH;

    bool inside;

};

class Eq
{
    Texture eq;
    Texture nickname;
    Texture hp;
    Texture power;
    bool show;
public:
    Eq(Player *p);
    void draw();
    void check_show(SDL_Event e);
    ~Eq();
};

class Fight
{
public:
    Fight(Tilemap* t, Player* p);
    bool fight(Player* p, NPC* npc);

private:
    Texture back;
    Texture ui;
};

bool checkCollision(SDL_Rect& a, SDL_Rect& b);
bool init();
void close(Tilemap* t, Player* p);
bool checkCollision(SDL_Rect& a, SDL_Rect& b);
int loadMedia(Tilemap* t);
void first(Tilemap* t, Player* p);
void menu(Tilemap* t, Player* p);
void play_menu(Tilemap* t, Player* p);
void new_game_menu(Tilemap* t, Player* p);
void load_game_menu(Tilemap* t, Player* p);
void about_menu(Tilemap* t, Player* p);