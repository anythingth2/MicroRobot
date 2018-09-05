#include <stdio.h>
#define SIZE 11
FILE *mapFile;
char map[SIZE][SIZE + 1];
void loadMap()
{
    mapFile = fopen("testmap.txt", "r");
    int i = 0;
    while (fgets(map[i], SIZE + 1, mapFile) != NULL)
    {
        if (map[i][0] != '\n')
            i++;
    }
    fclose(mapFile);
}
void showMap()
{
    printf("\n\n\n Map!\n");
    int i = 0;
    int j = 0;
    for (i = 0; i < SIZE; i++)
    {

        printf("%s\n", map[i]);
        // printf("%s",map[i]);
    }
    printf("\n");
}
int main()
{
    loadMap();
    map[9][9] = 'X';
    showMap();

    return 0;
}