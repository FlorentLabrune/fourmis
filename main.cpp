/* ========== CAHIER DES CHARGES ============

    La simulation est une simulation de fourmiliere qui évolue dans son environnement.
    Elle comportera :
        - Des fourmies
        - Une fourmiliere de fourmies avec un stock de nourriture
        - Des sources de nourritures fini

    I) Les fourmies
        Chaque fourmies

*/




#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
#include <windows.h>

#define MAX_FOURMIE 50
#define MAX_SOURCE 10
const int X = 195;
const int Y = 50;


using namespace std;

typedef struct{
    int x;
    int y;
}t_coord;

typedef struct{
    t_coord coord;
    int nourriture;
    bool drop;
    int direction;
    bool follow;
    int idChemin;
}t_fourmie;

typedef struct{
    t_coord origine;
    t_coord cible;
}chemin_phero;

typedef t_fourmie t_fourmies[MAX_FOURMIE];
typedef struct{
    t_coord coord;
    int nourriture;
    t_fourmies fourmies;
    int nbFourmies;
}fourmiliere;

typedef struct{
    t_coord coord;
    int reste;
}source_nourriture;

typedef source_nourriture t_sources[MAX_SOURCE];
typedef char t_case[X][Y];
typedef struct{
    t_case maMap;
    fourmiliere maison;
    t_sources sources;
    int nbSources;
}t_simulation;

string to_string(int a){
    std::ostringstream oss;
    oss << a;
    return oss.str();
}

string afficherSimu(t_fourmie fourmie){
    system("cls");
    string toreturn = "";
    for(int i = 0; i < Y; i++){
        for(int j = 0; j < X; j++)
            if(fourmie.coord.x == j && fourmie.coord.y == i)
                if(fourmie.direction <= 1)
                    toreturn += "|";
                else
                    toreturn += "_";
            else
                toreturn += " ";
        toreturn += "\n";
    }
    toreturn += "x :" + to_string(fourmie.coord.x) + "y :" + to_string(fourmie.coord.y);
    return toreturn;
}

void deplacerFourmie(t_fourmie&fourmie){
    if(rand()%5 == 0)
        switch(rand()%4){
        case 0 :    //la fourmie monte
                fourmie.direction = 0;
            break;

        case 1 :    //la fourmie descend
                fourmie.direction = 1;
            break;

        case 2 :    //la fourmie va a droite
                fourmie.direction = 2;
            break;

        case 3 :    //la fourmie va a gauche
                fourmie.direction = 3;
            break;
        }

    switch(fourmie.direction){
        case 0 :    //la fourmie monte
            if(fourmie.coord.y > 0)
                fourmie.coord.y--;
            else
                fourmie.direction = 1;
            break;

        case 1 :    //la fourmie descend
            if(fourmie.coord.y < Y-1)
                fourmie.coord.y++;
            else
                fourmie.direction = 0;
            break;

        case 2 :    //la fourmie va a droite
            if(fourmie.coord.x < X-1)
                fourmie.coord.x++;
            else
                fourmie.direction = 3;
            break;

        case 3 :    //la fourmie va a gauche
            if(fourmie.coord.x > 0)
                fourmie.coord.x--;
            else
                fourmie.direction = 2;
            break;
    }
}

int main()
{
    string cmd = "mode " + to_string(X) + ", " + to_string(Y);
    system(cmd.c_str());
    t_fourmie uneFourmie;
    t_coord coor = {95, 25};
    uneFourmie.coord = coor;
    while(1){
        deplacerFourmie(uneFourmie);
        string res = afficherSimu(uneFourmie);
        cout << res;
        Sleep(50);
    }
    return 0;
}
