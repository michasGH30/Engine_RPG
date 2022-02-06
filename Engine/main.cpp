#include <iostream>
#include "Engine.h"
Tilemap tilemap;
Player player(5000, 5000);
int main(int argc, char* argv[])
{
    if (!init())
    {
        printf("Falied to initialize\n");
    }
    else
    {
        int load = loadMedia(&tilemap);

        if (load == 0)
        {
            printf("Failed to load media\n");
        }
        else
        {

            menu(&tilemap, &player);
            if (player.load() == false)
            {
                close(&tilemap, &player);
                exit(0);
            }
            switch (player.map)
            {
            case 1:
                first(&tilemap, &player);
                break;
            default:
                close(&tilemap, &player);
            }
            
        }
    }
    close(&tilemap, &player);
    return 0;
}
