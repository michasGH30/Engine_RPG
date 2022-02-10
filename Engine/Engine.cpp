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
#include "Engine.h"

const int screen_width = 1280;
const int screen_height = 768;

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

SDL_Color black = { 0, 0, 0 };

SDL_Color white = { 255,255,255 };

bool checkCollision(SDL_Rect& a, SDL_Rect& b)
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if (bottomA <= topB)
    {
        return false;

    }

    if (topA >= bottomB)
    {
        return false;

    }

    if (rightA <= leftB)
    {
        return false;

    }

    if (leftA >= rightB)
    {
        return false;

    }
    return true;

}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
    }
    else
    {

        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        gWindow = SDL_CreateWindow("SIMPLE RPG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created SDL_ERROR: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                int imgFlags = IMG_INIT_PNG;
                if ((!IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }

                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }

                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }

            }

        }

    }

    return success;
}

int loadMedia(Tilemap* t)
{
    int success = 1;

    gFont = TTF_OpenFont("Assets/Fonts/DroidSansMono.ttf", 28);

    if (gFont == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search font file. Please reinstall game :)", NULL);
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        success = 0;
    }

    if (t->load() == false)
    {
        success = 0;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search tilemap file. Please reinstall game :)", NULL);
        printf("Failed to load tilemap\n");
    }

    return success;
}

void close(Tilemap* t, Player* p)
{
    t->free();
    p->~Player();

    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

Timer::Timer()
{
    startedticks = 0;
    ticks = 0;
    started = false;
    seconds = 0;
}

Timer::Timer(float s)
{
    startedticks = 0;
    ticks = 0;
    started = false;
    seconds = s;
}
void Timer::set_seconds(float s)
{
    seconds = s;
}
void Timer::start()
{
    startedticks = SDL_GetTicks();
    started = true;
}
bool Timer::morethanseconds()
{
    bool more = false;

    if (started)
    {
        ticks = SDL_GetTicks() - startedticks;

        if (ticks / 1000.f >= seconds)
        {
            more = true;
        }
    }

    return more;
}

Texture::Texture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}
Texture::~Texture()
{
    free();
}

bool Texture::loadFromFile(std::string path)
{
    free();

    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());

    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}
bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    free();

    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);

    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());

    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

        if (mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;
}
void Texture::SetColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}
void Texture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}
void Texture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}
void Texture::free()
{
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}
void Texture::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = { x,y,mWidth,mHeight };

    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}
int Texture::getWidth()
{
    return mWidth;
}
int Texture::getHeight()
{
    return mHeight;
}

void Texture::to_input(SDL_Rect& col)
{
    SDL_QueryTexture(mTexture, NULL, NULL, &col.w, &col.h);
}


Tilemap::Tilemap()
{
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            ground[i][j] = 0;
            objects[i][j] = 0;
        }
    }

    set();
}

bool Tilemap::load()
{
    bool succes = true;

    for (int i = 0; i < 1064; i++)
    {
        std::string do_zmiany = std::to_string(i + 1);
        if (elements[i].loadFromFile("Assets/Tilemap/" + do_zmiany + ".png") == false)
        {
            succes = false;
        }
    }
    return succes;
}

void Tilemap::set()
{
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            ground[i][j] = 1;
            collider[i][j].x = i * 32;
            collider[i][j].y = j * 32;
            collider[i][j].h = collider[i][j].w = 32;
        }
    }

}

void Tilemap::show(int id)
{
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 24; j++)
        {

            if (id == 0)
            {
                elements[ground[i][j] - 1].render(i * 32, j * 32);
            }
            else
            {
                if (objects[i][j] != 0)
                {
                    elements[objects[i][j] - 1].render(i * 32, j * 32);
                }
            }

        }
    }
}

void Tilemap::free()
{
    for (int i = 0; i < 1064; i++)
    {
        elements[i].free();
    }
}

bool Tilemap::loadFromfile(std::string path, int l)
{
    bool loaded = true;
    std::fstream mapa;
    mapa.open(path, std::ios::in);
    if (!mapa.good())
    {
        printf("Something wrong with map level isn't working");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search map level file. Please reinstall game :)", NULL);
        loaded = false;
    }
    else
    {
        int a = 0;
        int i = 0, j = 0;
        while (mapa >> a)
        {
            if (i > 39)
            {
                i = 0;
                j++;
            }
            switch (l)
            {
            case 0:
                ground[i][j] = a;
                break;
            case 1:
                objects[i][j] = a;
                break;
            }
            
            i++;
        }
    }
    return loaded;
}
Player::Player(int pozx, int pozy)
{
    frame = 0;
    lastx = 0;
    lasty = -1;
    velocity = 5;
    ismoving = false;
    Collider.w = 21;
    Collider.h = 32;
    Collider.x = pozx;
    Collider.y = pozy;
    nick = "NICKNAME";
    id = "0";
    map = 1;
    save = "0";
    keyboard_active = true;
}

bool Player::load()
{
    bool succes = true;

    if (!player_texture.loadFromFile("Assets/Player/p" + id + ".png"))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search player texture file. Please reinstall game :)", NULL);
        succes = false;
    }
    else
    {
        down[0].x = 0;
        down[0].y = 0;
        down[0].w = 32;
        down[0].h = 32;

        down[1].x = 32;
        down[1].y = 0;
        down[1].w = 32;
        down[1].h = 32;

        down[2].x = 64;
        down[2].y = 0;
        down[2].w = 32;
        down[2].h = 32;

        left[0].x = 0;
        left[0].y = 32;
        left[0].w = 32;
        left[0].h = 32;

        left[1].x = 32;
        left[1].y = 32;
        left[1].w = 32;
        left[1].h = 32;

        left[2].x = 64;
        left[2].y = 32;
        left[2].w = 32;
        left[2].h = 32;

        right[0].x = 0;
        right[0].y = 64;
        right[0].w = 32;
        right[0].h = 32;

        right[1].x = 32;
        right[1].y = 64;
        right[1].w = 32;
        right[1].h = 32;

        right[2].x = 64;
        right[2].y = 64;
        right[2].w = 32;
        right[2].h = 32;

        up[0].x = 0;
        up[0].y = 96;
        up[0].w = 32;
        up[0].h = 32;

        up[1].x = 32;
        up[1].y = 96;
        up[1].w = 32;
        up[1].h = 32;

        up[2].x = 64;
        up[2].y = 96;
        up[2].w = 32;
        up[2].h = 32;
    }
    return succes;
}

void Player::handleEvents(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN && keyboard_active)
    {
        ismoving = true;
        switch (e.key.keysym.sym)
        {
        case SDLK_LEFT:
        case SDLK_a:
            Collider.x -= velocity;
            lasty = 0;
            lastx = -1;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            Collider.x += velocity;
            lastx = 1;
            lasty = 0;
            break;
        case SDLK_UP:
        case SDLK_w:
            Collider.y -= velocity;
            lasty = 1;
            lastx = 0;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            Collider.y += velocity;
            lastx = 0;
            lasty = -1;
        default:
            break;
        }

    }
    else if (e.type == SDL_KEYUP && keyboard_active)
    {
        ismoving = false;
    }
}

void Player::move(SDL_Rect& wall)
{
    if (checkCollision(Collider, wall) == 1)
    {
        if (lastx != 0)
        {
            if (lastx == 1)
            {
                Collider.x -= velocity;
            }
            else if (lastx == -1)
            {
                Collider.x += velocity;
            }
        }
        if (lasty != 0)
        {
            if (lasty == 1)
            {
                Collider.y += velocity;
            }
            else if (lasty == -1)
            {
                Collider.y -= velocity;
            }
        }

    }
    else
    {

        if (Collider.x < 0)
        {
            Collider.x += velocity;
        }
        else if (Collider.x + Collider.w > 1280)
        {
            Collider.x -= velocity;
        }

        if (Collider.y < 0)
        {
            Collider.y += velocity;
        }
        else if (Collider.y + Collider.h > 768)
        {
            Collider.y -= velocity;
        }
    }
}

void Player::render()
{
    SDL_Rect* current;

    if (ismoving)
    {

        if (frame / 20 >= 3)
        {
            frame = 0;
        }
        else
        {
            if (lastx == 1)
            {
                current = &right[frame / 20];
            }
            else if (lastx == -1)
            {
                current = &left[frame / 20];
            }
            else if (lasty == 1)
            {
                current = &up[frame / 20];
            }
            else
            {
                current = &down[frame / 20];
            }

            player_texture.render(Collider.x, Collider.y, current);

            frame++;
        }
    }
    else
    {
        if (lastx == 1)
        {
            current = &right[1];
        }
        else if (lastx == -1)
        {
            current = &left[1];
        }
        else if (lasty == 1)
        {
            current = &up[1];
        }
        else
        {
            current = &down[1];
        }

        player_texture.render(Collider.x, Collider.y, current);

    }

}

Player::~Player()
{
    player_texture.free();

    if (save != "0")
    {
        std::ofstream plik;

        plik.open("Assets/Saves/" + save + ".txt");

        plik << nick << std::endl << id << std::endl<<health << std::endl << strenght;
    }

}

Start_men::Start_men(int x, int y)
{
    int w = 24;
    int h = 32;
    Collider.x = x;
    Collider.y = y;
    Collider.w = 24;
    Collider.h = 32;

    down[0].x = 0;
    down[0].y = 64;
    down[0].w = w;
    down[0].h = h;

    down[1].x = 24;
    down[1].y = 64;
    down[1].w = w;
    down[1].h = h;

    down[2].x = 48;
    down[2].y = 64;
    down[2].w = w;
    down[2].h = h;

    left[0].x = 0;
    left[0].y = 96;
    left[0].w = w;
    left[0].h = h;

    left[1].x = 24;
    left[1].y = 96;
    left[1].w = w;
    left[1].h = h;

    left[2].x = 48;
    left[2].y = 96;
    left[2].w = w;
    left[2].h = h;

    right[0].x = 0;
    right[0].y = 32;
    right[0].w = w;
    right[0].h = h;

    right[1].x = 24;
    right[1].y = 32;
    right[1].w = w;
    right[1].h = h;

    right[2].x = 48;
    right[2].y = 32;
    right[2].w = w;
    right[2].h = h;

    up[0].x = 0;
    up[0].y = 24;
    up[0].w = w;
    up[0].h = h;

    up[1].x = 24;
    up[1].y = 0;
    up[1].w = w;
    up[1].h = h;

    up[2].x = 48;
    up[2].y = 0;
    up[2].w = w;
    up[2].h = h;

    frame = 0;

    ismoving = false;

    lastx = 0;
    lasty = -1;

    load();

}

bool Start_men::load()
{
    //mandar.png
    if (npc_texture.loadFromFile("Assets/oldman/oldman.png") == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search NPC texture file. Please reinstall game :)", NULL);
        printf("Failed to load old man texture\n");
        return false;
    }
    else
    {
        return true;
    }
}

void Start_men::render()
{

    SDL_Rect* current;

    if (ismoving)
    {

        if (frame / 20 >= 3)
        {
            frame = 0;
        }
        else
        {
            if (lastx == -1)
            {
                current = &left[frame / 20];
            }
            else if (lasty == 1)
            {
                current = &up[frame / 20];
            }
            else
            {
                current = &down[frame / 20];
            }

            npc_texture.render(Collider.x, Collider.y, current);

            frame++;
        }
    }
    else
    {
        if (lastx == -1)
        {
            current = &left[0];
        }
        else if (lasty == 1)
        {
            current = &up[0];
        }
        else
        {
            current = &down[0];
        }

        npc_texture.render(Collider.x, Collider.y, current);

    }

}


Animation::Animation(float s, char d)
{
    direction = d;
    seconds = s;
    timer.set_seconds(seconds);
    animation = false;
}

void Animation::start()
{
    animation = true;
    timer.start();
}

void Animation::check()
{
    if (timer.morethanseconds())
    {
        animation = false;
    }
}

void Animation::anim(NPC* npc)
{
    if (animation)
    {
        npc->ismoving = true;
        if (direction == 's')
        {
            npc->lasty = -1;
            npc->lastx = 0;
            npc->Collider.y++;
        }
        else if (direction == 'w')
        {
            npc->lasty = 0;
            npc->lastx = -1;
            npc->Collider.x--;
        }
        else if (direction == 'n')
        {
            npc->lasty = 1;
            npc->lastx = 0;
            npc->Collider.y--;
        }
        else if (direction == 'e')
        {
            npc->lasty = 0;
            npc->lastx = 1;
            npc->Collider.x++;
        }
    }
    else
    {
        npc->ismoving = false;
    }
}

void Animation::anim(Player* player)
{
    if (animation)
    {
        player->keyboard_active = false;
        if (direction == 's')
        {
            player->lasty = -1;
            player->lastx = 0;
            player->Collider.y++;
        }
        else if (direction == 'w')
        {
            player->lasty = 0;
            player->lastx = -1;
            player->Collider.x--;
        }
        else if (direction == 'n')
        {
            player->lasty = 1;
            player->lastx = 0;
            player->Collider.y--;
        }
        else if (direction == 'e')
        {
            player->lasty = 0;
            player->lastx = 1;
            player->Collider.x++;
        }
    }
    else
    {
        player->keyboard_active = true;
        player->ismoving = false;
    }
}

Dialog::Dialog(std::string path)
{
    next.set_seconds(5.0f);
    page = 0;
    back.loadFromFile("Assets/Gui/dialog.png");
    view = false;
    to_hide = false;
    i = 0;
    std::fstream plik;
    plik.open("Assets/dialog/" + path, std::ios::in);
    if (!plik.good())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search dialog file. Please reinstall game :)", NULL);
        printf("Dialog isn't working");
    }
    else
    {
        std::string linia;
        while (std::getline(plik, linia))
        {
            texts[i] = linia;
            i++;
        }

    }
    max_pages = ceil(i / 5);
    pixels[0] = 600;
    pixels[1] = 630;
    pixels[2] = 660;
    pixels[3] = 690;
    pixels[4] = 720;
}

void Dialog::next_page(SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == SDLK_RETURN)
        {
            if (page + 1 <= max_pages)
                page++;
            else
                view = false;
        }
    }
    if (next.morethanseconds())
    {
        if (to_hide)
            view = false;

        if (page + 1 <= max_pages && !to_hide)
        {
            page++;
            next.start();
        }
        else if (page + 1 > max_pages && !to_hide)
        {
            next.start();
            to_hide = true;
        }

    }
}

void Dialog::start()
{
    view = true;
    next.start();
}


void Dialog::draw()
{
    if (view)
    {
        back.render(0, 600);
        int ile = (page + 1) * 5;
        if (ile > i)
        {
            int r = ile - i;
            ile -= r;
        }
        int k = 0;
        for (int j = page * 5; j < ile; j++)
        {
            text.loadFromRenderedText(texts[j], white);
            text.render(0, pixels[k]);
            k++;
        }
    }
}

Input::Input(int x, int y)
{
    Collider.x = x;
    Collider.y = y;
    write = false;
    renderText = false;
    text = "some text";
    Collider.w = 100;
    Collider.h = 50;
    mPosition.x = 0;
    mPosition.y = 0;
}

void Input::inputtext(SDL_Event& e)
{

    if (e.type == SDL_TEXTINPUT && write == true)
    {
        text += e.text.text;

    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0 && write == true)
    {
        text = text.substr(0, text.length() - 1);

    }

}

void Input::inside(int x, int y, SDL_Event& e)
{
    mPosition.x = x;
    mPosition.y = y;
    bool yes = true;
    if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
    {
        if (Collider.x > mPosition.x)
        {
            yes = false;
        }
        else if (Collider.x + Collider.w < mPosition.x)
        {
            yes = false;
        }
        else if (Collider.y > mPosition.y)
        {
            yes = false;
        }
        else if (Collider.y + Collider.h < mPosition.y)
        {
            yes = false;
        }

        if (yes)
        {
            switch (e.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                write = true;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (e.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                write = false;
                break;
            default:
                break;
            }
        }

    }

}

void Input::render()
{

    texttexture.to_input(Collider);

    if (text != "")
    {
        texttexture.loadFromRenderedText(text.c_str(), white);
    }
    else
    {
        texttexture.loadFromRenderedText(" ", white);
    }

    texttexture.render(Collider.x + 10, Collider.y + 10);

    Collider.w += 20;
    Collider.h += 20;

    if (write)
    {
        SDL_SetRenderDrawColor(gRenderer, 232, 28, 255, 255);
    }
    else
    {

        SDL_SetRenderDrawColor(gRenderer, 227, 117, 240, 255);
    }

    SDL_RenderDrawRect(gRenderer, &Collider);

}


Button::Button(std::string pathin, std::string pathout)
{
    mPosition.x = 0;
    mPosition.y = 0;
    BUTTON_WIDTH = 200;
    BUTTON_HEIGHT = 100;
    if (mousein.loadFromFile(pathin) == false || mouseuot.loadFromFile(pathout) == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search someone of button texture. Please reinstall game :)", NULL);
        exit(0);
    }

    inside = false;
}

void Button::setPosistion(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

int Button::handleEvent(SDL_Event* e)
{
    int klik = 0;

    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        inside = true;
        if (x < mPosition.x)
        {
            inside = false;
        }
        else if (x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }
        else if (y < mPosition.y)
        {
            inside = false;
        }
        else if (y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        if (inside)
        {

            switch (e->type)
            {
            case SDL_MOUSEBUTTONDOWN:
                klik = 1;
                break;
            default:
                break;
            }
        }

    }
    return klik;
}

void Button::render()
{
    if (inside)
    {
        mouseuot.render(mPosition.x, mPosition.y);
    }
    else
    {
        mousein.render(mPosition.x, mPosition.y);
    }
}

Button::~Button()
{
    mousein.free();
    mouseuot.free();
}

Eq::Eq(Player *p)
{
    show = false;
    if (!eq.loadFromFile("Assets/UIEQ.png"))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search UI image file. Please reinstall game :)", NULL);
        printf("EQ UI graphics load failed");
    }
        

    nickname.loadFromRenderedText("Nazwa: "+p->nick, white);
    hp.loadFromRenderedText("Punkty zycia: "+std::to_string(p->health), white);
    power.loadFromRenderedText("Sila: "+std::to_string(p->strenght), white);
}

Eq::~Eq()
{
    eq.free();
    nickname.free();
}

void Eq::draw()
{
    if (show)
    {
        eq.render(0, 0);
        nickname.render(5, 5);
        hp.render(5, 45);
        power.render(5, 85);
    }
}

void Eq::check_show(SDL_Event e)
{
  
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == SDLK_e)
        {
            if (show)
                show = false;
            else if (!show)
                show = true;
        }
    }
        
}

Fight::Fight(Tilemap* t, Player* p)
{
    if (!back.loadFromFile("Assets/fight/fight_back.png"))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search fight window background texture file. Please reinstall game :)", NULL);
        close(t, p);
    }
    if (!ui.loadFromFile("Assets/fight/fight_ui.png"))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search fight window background texture file. Please reinstall game :)", NULL);
        close(t, p);
    }
}

bool Fight::fight(Player* p, NPC* npc, Tilemap* t)
{
    SDL_Event e;

    Texture bg;

    bg.loadFromFile("Assets/fight/fight_back.png");

    Texture ui;

    ui.loadFromFile("Assets/fight/fight_ui.png");

    Texture hp, str;
    hp.loadFromRenderedText("Twoje punkty zycia: " + std::to_string(p->health), white);
    str.loadFromRenderedText("Twoja sila: " + std::to_string(p->strenght), white);

    p->Collider.x = 600;
    p->Collider.y = 400;
    p->ismoving = false;
    p->lastx = 0;
    p->lasty = 1;
    
    npc->Collider.x = 600;
    npc->Collider.y = 200;

    Button buttons[4] =
    { 
     Button("Assets/fight/fight_attack_out.png","Assets/fight/fight_attack_in.png"),
     Button("Assets/fight/fight_cast_out.png","Assets/fight/fight_cast_in.png"),
     Button("Assets/fight/fight_back_out.png","Assets/fight/fight_back_in.png"),
     Button("Assets/fight/fight_fireball_out.png","Assets/fight/fight_fireball_in.png")
    };

    buttons[0].setPosistion(0, 468);
    buttons[1].setPosistion(0, 568);
    buttons[2].setPosistion(0, 668);
    buttons[3].setPosistion(0, 468);

    bool cast_visible = false;

    bool run = true;

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }
            buttons[2].handleEvent(&e);
            if (!cast_visible)
            {
                buttons[0].handleEvent(&e);
                if (buttons[1].handleEvent(&e) == 1)
                    cast_visible = true;
            }
            else
            {
                if (buttons[3].handleEvent(&e) == 1)
                    cast_visible = false;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        bg.render(0, 0);

        ui.render(0, 468);

        hp.render(210, 480);
        str.render(210, 520);

        buttons[2].render();
        if (!cast_visible)
        {
            buttons[0].render();
            buttons[1].render();
        }
        else
        {
            buttons[3].render();
        }

        p->render();

        npc->render();

        SDL_RenderPresent(gRenderer);
    }
}
void first(Tilemap* t, Player* p)
{
    p->Collider.x = p->Collider.y = 40;
    NPC* npc = new Start_men(1000, 250);
    Fight f(t, p);
    f.fight(p, npc, t);
    SDL_Event e;

    Dialog xd("prolog.txt");

    xd.start();

    bool run = true;

    t->set();

    t->loadFromfile("Assets/m1.txt",0);
    t->loadFromfile("Assets/m1_coll.txt",1);

    Animation a(1.5f, 's');
    Animation b(1.0f, 'w');

    Eq eq(p);

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t,p);
                exit(0);
            }
            p->handleEvents(e);

            xd.next_page(e);

            eq.check_show(e);

        }

        for (int i = 0; i < 40; i++)
        {
            for (int j = 0; j < 24; j++)
            {
                if (t->objects[i][j] != 0 && t->objects[i][j] != 6)
                    p->move(t->collider[i][j]);
                else if (t->objects[i][j] == 6 && checkCollision(p->Collider, t->collider[i][j]))
                {
                    a.start();
                    b.start();
                    p->Collider.x -= 32;
                }
            }
        }

        p->move(npc->Collider);

        b.check();
        b.anim(npc);

        a.check();
        a.anim(p);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);
        
        t->show(0);
        t->show(1);

        p->render();

        npc->render();

        xd.draw();

        eq.draw();

        SDL_RenderPresent(gRenderer);

    }
}

void play_menu(Tilemap* t, Player* p)
{
    Texture background;

    background.loadFromFile("Assets/Gui/blink_bg.png");

    Button new_game("Assets/Gui/new_game_out.png", "Assets/Gui/new_game_in.png");

    new_game.setPosistion(540, 100);

    Button load_game("Assets/Gui/load_game_out.png", "Assets/Gui/load_game_in.png");

    load_game.setPosistion(540, 250);

    Button ret("Assets/Gui/return_out.png", "Assets/Gui/return_in.png");

    ret.setPosistion(540, 500);

    bool run = true;

    SDL_Event e;

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }

            if (new_game.handleEvent(&e) == 1)
            {
                run = false;
                new_game_menu(t,p);
            }

            if (load_game.handleEvent(&e) == 1)
            {
                run = false;
                load_game_menu(t,p);
            }

            if (ret.handleEvent(&e) == 1)
            {
                run = false;
                menu(t,p);
            }

        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        background.render(0, 0);

        new_game.render();

        load_game.render();

        ret.render();

        SDL_RenderPresent(gRenderer);

    }
}


void menu(Tilemap* t, Player* p)
{
    bool run = true;

    SDL_Event e;

    Texture background;

    background.loadFromFile("Assets/Gui/main_menu.png");

    Button play("Assets/Gui/play_out.png", "Assets/Gui/play_in.png");

    Button about("Assets/Gui/about_out.png", "Assets/Gui/about_in.png");

    Button quit("Assets/Gui/quit_out.png", "Assets/Gui/quit_in.png");

    play.setPosistion(540, 200);

    about.setPosistion(540, 350);

    quit.setPosistion(540, 500);

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }

            if (play.handleEvent(&e) == 1)
            {
                run = false;
                play_menu(t,p);
            }

            if (about.handleEvent(&e) == 1)
            {
                run = false;
                about_menu(t,p);
            }

            if (quit.handleEvent(&e) == 1)
            {
                run = false;
                close(t, p);
                exit(0);
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        background.render(0, 0);

        play.render();

        about.render();

        quit.render();

        SDL_RenderPresent(gRenderer);

    }

    background.free();

}

void new_game_menu(Tilemap* t, Player* p)
{
    int anim = 1;

    std::string save_choose = "0";

    std::string outfit_choose = "0";

    bool run = true;

    SDL_Event e;

    Texture background;

    background.loadFromFile("Assets/Gui/blink_bg.png");

    Player new_game_players[4] = { Player(284,60),Player(514,60),Player(744,60),Player(974,60) };//200 416 632 848

    new_game_players[0].id = "1";

    new_game_players[1].id = "2";

    new_game_players[2].id = "3";

    new_game_players[3].id = "4";

    for (int i = 0; i < 4; i++)
    {
        new_game_players[i].load();
        new_game_players[i].ismoving = true;
    }

    Button buttons[9] =
    { Button("Assets/Gui/confirm_out.png","Assets/Gui/confirm_in.png"),
     Button("Assets/Gui/confirm_out.png","Assets/Gui/confirm_in.png"),
     Button("Assets/Gui/confirm_out.png","Assets/Gui/confirm_in.png"),
     Button("Assets/Gui/confirm_out.png","Assets/Gui/confirm_in.png"),
     Button("Assets/Gui/1_out.png","Assets/Gui/1_in.png"),
     Button("Assets/Gui/2_out.png","Assets/Gui/2_in.png"),
     Button("Assets/Gui/3_out.png","Assets/Gui/3_in.png"),
     Button("Assets/Gui/play_out.png","Assets/Gui/play_in.png"),
     Button("Assets/Gui/return_out.png","Assets/Gui/return_in.png")
    };

    buttons[0].setPosistion(195, 105);
    buttons[1].setPosistion(425, 105);
    buttons[2].setPosistion(655, 105);
    buttons[3].setPosistion(885, 105);


    buttons[4].setPosistion(300, 230);
    buttons[5].setPosistion(540, 230);
    buttons[6].setPosistion(780, 230);

    buttons[7].setPosistion(540, 520);

    buttons[8].setPosistion(540, 645);

    Texture outfit, press_enter, save, info;

    outfit.loadFromRenderedText("Wybrales  " + outfit_choose + "  wyglad", white);

    save.loadFromRenderedText("Wybrales  " + save_choose + "  zapis", white);

    press_enter.loadFromRenderedText("Podaj nick:", white);

    info.loadFromRenderedText(" ", white);

    Input nick(550, 450);

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {

            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }

            if (buttons[0].handleEvent(&e) == 1)
            {
                outfit_choose = "1";
                outfit.loadFromRenderedText("Wybrales  " + outfit_choose + "  wyglad", white);
            }

            if (buttons[1].handleEvent(&e) == 1)
            {
                outfit_choose = "2";
                outfit.loadFromRenderedText("Wybrales  " + outfit_choose + "  wyglad", white);
            }

            if (buttons[2].handleEvent(&e) == 1)
            {
                outfit_choose = "3";
                outfit.loadFromRenderedText("Wybrales  " + outfit_choose + "  wyglad", white);
            }

            if (buttons[3].handleEvent(&e) == 1)
            {
                outfit_choose = "4";
                outfit.loadFromRenderedText("Wybrales  " + outfit_choose + "  wyglad", white);
            }

            if (buttons[4].handleEvent(&e) == 1)
            {
                save_choose = "1";
                save.loadFromRenderedText("Wybrales  " + save_choose + "  zapis", white);
            }

            if (buttons[5].handleEvent(&e) == 1)
            {
                save_choose = "2";
                save.loadFromRenderedText("Wybrales  " + save_choose + "  zapis", white);
            }

            if (buttons[6].handleEvent(&e) == 1)
            {
                save_choose = "3";
                save.loadFromRenderedText("Wybrales  " + save_choose + "  zapis", white);
            }

            int x, y;

            SDL_GetMouseState(&x, &y);

            nick.inside(x, y, e);

            if (nick.write)
            {
                SDL_StartTextInput();
            }
            else
            {
                SDL_StopTextInput();
            }

            nick.inputtext(e);

            if (buttons[7].handleEvent(&e) == 1)
            {
                if (outfit_choose == "0" && save_choose == "0" && (nick.text == "some text" || nick.text.length() == 0))
                {
                    info.loadFromRenderedText("Nie wybrales wygladu, numeru zapisu, ani nazwy gracza zmien to :)", white);
                }
                else if (outfit_choose == "0" && save_choose == "0")
                {
                    info.loadFromRenderedText("Nie wybrales wygladu i numeru zapisu zmien to :)", white);
                }
                else if (outfit_choose == "0" && (nick.text == "some text" || nick.text.length() == 0))
                {
                    info.loadFromRenderedText("Nie wybrales wygladu i nazwy gracza zmien to :)", white);
                }
                else if (save_choose == "0" && (nick.text == "some text" || nick.text.length() == 0))
                {
                    info.loadFromRenderedText("Nie wybrales numeru zapisu i nazwy gracza zmien to :)", white);
                }
                else if (outfit_choose == "0")
                {
                    info.loadFromRenderedText("Nie wybrales wygladu zmien to :)", white);
                }
                else if (save_choose == "0")
                {
                    info.loadFromRenderedText("Nie wybrales numeru zapisu zmien to :)", white);
                }
                else if (nick.text == "some text" || nick.text.length() == 0)
                {
                    info.loadFromRenderedText("Nie wybrales nazwy gracza zmien to :)", white);
                }
                else
                {
                    p->id = outfit_choose;
                    p->save = save_choose;
                    p->nick = nick.text;
                    run = false;
                }
            }

            if (buttons[8].handleEvent(&e) == 1)
            {
                run = false;
                play_menu(t,p);
            }

        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        background.render(0, 0);

        nick.render();

        for (int i = 0; i < 9; i++)
            buttons[i].render();

        for (int i = 0; i < 4; i++)
        {
            new_game_players[i].render();

            new_game_players[i].frame++;

            if (new_game_players[0].frame / 20 >= 3)
            {
                new_game_players[i].frame = 0;

                if (anim <= 4)
                {
                    anim++;
                }
                else
                {
                    anim = 1;
                }
            }
            if (anim == 1)
            {
                new_game_players[i].lastx = -1;
                new_game_players[i].lasty = 0;
            }
            else if (anim == 2)
            {
                new_game_players[i].lastx = 0;
                new_game_players[i].lasty = -1;
            }
            else if (anim == 3)
            {
                new_game_players[i].lastx = 1;
                new_game_players[i].lasty = 0;
            }
            else if (anim == 4)
            {
                new_game_players[i].lastx = 0;
                new_game_players[i].lasty = 1;
            }

        }

        outfit.render(515, 10);

        save.render(515, 340);

        press_enter.render(540, 400);

        info.render(300, 500);

        SDL_RenderPresent(gRenderer);

    }

}

void load_game_menu(Tilemap* t, Player* p)
{
    std::fstream plik;

    std::string file_number = "0";

    std::string dane;

    bool run = true;

    SDL_Event e;

    Texture background;

    background.loadFromFile("Assets/Gui/blink_bg.png");

    Button one("Assets/Gui/1_out.png", "Assets/Gui/1_in.png");

    one.setPosistion(240, 100);

    Button two("Assets/Gui/2_out.png", "Assets/Gui/2_in.png");

    two.setPosistion(540, 100);

    Button three("Assets/Gui/3_out.png", "Assets/Gui/3_in.png");

    three.setPosistion(840, 100);

    Button confirm("Assets/Gui/confirm_out.png", "Assets/Gui/confirm_in.png");

    confirm.setPosistion(540, 300);

    Button ret("Assets/Gui/return_out.png", "Assets/Gui/return_in.png");

    ret.setPosistion(540, 500);

    Texture info;

    info.loadFromRenderedText("Wybrales  " + file_number, white);

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }

            if (one.handleEvent(&e) == 1)
            {
                file_number = "1";
                info.loadFromRenderedText("Wybrales  " + file_number, white);
            }

            if (two.handleEvent(&e) == 1)
            {
                file_number = "2";
                info.loadFromRenderedText("Wybrales  " + file_number, white);
            }

            if (three.handleEvent(&e) == 1)
            {
                file_number = "3";
                info.loadFromRenderedText("Wybrales  " + file_number, white);
            }

            if (ret.handleEvent(&e) == 1)
            {
                run = false;
                play_menu(t,p);
            }

            if (confirm.handleEvent(&e) == 1)
            {
                if (file_number == "0")
                {
                    info.loadFromRenderedText("Nie wybrales zadnego pliku", white);
                }
                else
                {

                    plik.open("Assets/Saves/" + file_number + ".txt", std::ios::in);

                    int i = 1;

                    if (plik.good() == false)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", "Cannot search save file. Please reinstall game :)", NULL);
                        close(t, p);
                        exit(0);

                    }

                    while (std::getline(plik, dane))
                    {
                        switch (i)
                        {
                        case 1:
                            p->nick = dane;
                            break;
                        case 2:
                            p->id = dane;
                            break;
                        case 3:
                            p->health = stoi(dane);
                            break;
                        case 4:
                            p->strenght = stoi(dane);
                            break;
                        }
                        i++;
                    }

                    p->save = file_number;

                    p->map = 1;
                    run = false;
                }

            }

        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        background.render(0, 0);

        one.render();

        two.render();

        three.render();

        confirm.render();

        ret.render();

        info.render(560, 50);

        SDL_RenderPresent(gRenderer);

    }
}

void about_menu(Tilemap* t, Player* p)
{
    bool run = true;

    SDL_Event e;

    Texture background;

    background.loadFromFile("Assets/Gui/about_bg.png");

    Button ret("Assets/Gui/return_out.png", "Assets/Gui/return_in.png");

    Texture text;

    text.loadFromRenderedText("To fakt nie opinia", white);

    ret.setPosistion(540, 650);

    while (run)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
                close(t, p);
                exit(0);
            }

            if (ret.handleEvent(&e) == 1)
            {
                run = false;
                menu(t,p);
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        SDL_RenderClear(gRenderer);

        background.render(0, 0);

        ret.render();

        text.render(200, 100);

        SDL_RenderPresent(gRenderer);
    }

    background.free();
}


