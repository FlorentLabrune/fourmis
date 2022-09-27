/* ========== CAHIER DES CHARGES ============

    La simulation est une simulation de fourmiliere qui évolue dans son environnement.
    Elle comportera :
        - Des fourmis
        - Une fourmiliere de fourmis avec un stock de nourriture
        - Des sources de nourritures fini

    I) Les fourmis
        Chaque fourmis doit

*/


//KeyListenner, voir conio21 de Philippe Latu

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
#include <windows.h>
#include <stdio.h>

#define MAX_fourmi 200
#define MAX_SOURCE 50
#define MAX_CHEMIN 100

#define ET_AVANCER_ALEA 1
#define ET_SUIVRE_TRACE 2
#define ET_RENTRER_HOME 3

const int Y = 50;
const int X = 100;
int idChemin = 0;

int fourmis_base = 20;
int vie_base = 200;
float taux = 0.25;
int nbSources = 10;
int force_base = 40;
int quantite_nour = 500;


using namespace std;

typedef struct{
    int x;
    int y;
}t_coord;

typedef struct{
    t_coord coord;
    int nourriture;
    int vie;
    int direction;
    int idChemin;
    int etat;
}t_fourmi;

typedef struct{
    t_coord coord;
    int force;
}t_phero;

typedef t_phero t_liste_phero[X];
typedef struct{
    t_liste_phero pheros;
    int nbCoord;
    int id;
}chemin_phero;

typedef t_fourmi t_fourmis[MAX_fourmi];
typedef struct{
    t_coord coord;
    int nourriture;
    t_fourmis fourmis;
    int nbfourmis;
}fourmiliere;

typedef struct{
    t_coord coord;
    int reste;
}source_nourriture;

typedef chemin_phero t_chemins[MAX_CHEMIN];
typedef source_nourriture t_sources[MAX_SOURCE];
typedef char t_case[Y][X+1];
typedef struct{
    t_case maMap;
    fourmiliere maison;
    t_sources sources;
    int nbSources;
    t_chemins chemins;
    int nbChemins;
}t_simulation;

string to_string(int a){
    std::ostringstream oss;
    oss << a;
    return oss.str();
}

t_coord getRandomCoord(){
    return {rand()%X-1, rand()%Y};
}

bool coordEquals(t_coord coord1, t_coord coord2){
    return coord1.x == coord2.x && coord1.y == coord2.y;
}

int getIndexChemin(t_simulation&simu, int id){
    for(int i = 0; i < simu.nbChemins; i++)
        if(simu.chemins[i].id == id)
            return i;
    return -1;
}

t_phero getNewPhero(t_coord&coord){
    return {coord, force_base};
}

chemin_phero getNewChemin(){
    return {{}, 0, idChemin++};
}

void addPheroChemin(chemin_phero&chemin, t_coord&coord){
    chemin.pheros[chemin.nbCoord++] = getNewPhero(coord);
}

void spawnfourmi(fourmiliere&fourmil){
    if(fourmil.nourriture > 0 && fourmil.nbfourmis < MAX_fourmi){
        t_fourmi fourmi = {fourmil.coord, 0, rand()%10+(vie_base-5), rand()%4, -1};
        fourmil.fourmis[fourmil.nbfourmis++] = fourmi;
        fourmil.nourriture -= 50;
    }
}

void supprimerSource(t_simulation&simu, int indexSource){
    simu.sources[indexSource] = simu.sources[--simu.nbSources];
}

void supprimerfourmi(fourmiliere&fourmil, int index){
    fourmil.fourmis[index] = fourmil.fourmis[fourmil.nbfourmis-1];
    fourmil.nbfourmis--;
}

void fatiguerfourmis(fourmiliere&fourmil){
    for(int i = 0; i < fourmil.nbfourmis; i++){
        fourmil.fourmis[i].vie -= rand()%3;
        if(fourmil.fourmis[i].vie < 1)
            supprimerfourmi(fourmil, i);
    }
}

void fatiguerChemins(t_simulation&simu){
    for(int i = 0; i < simu.nbChemins; i++)
        for(int j = 0; j < simu.chemins[i].nbCoord; j++)
            if(simu.chemins[i].pheros[j].force > 0)
                simu.chemins[i].pheros[j].force--;


    for(int i = 0; i < simu.nbChemins; i++){
        bool vide = true;
        for(int j = 0; j < simu.chemins[i].nbCoord; j++)
            if(simu.chemins[i].pheros[j].force > 0){
                vide = false;
                j = simu.chemins[i].nbCoord;
            }
        if(vide)
            simu.chemins[i] = simu.chemins[--simu.nbChemins];
    }
}


/** \brief retourne l'id de la trace de pheromone aux coordonnées coord. Retourne false si il n'y a pas de trace a ces coordonnées
 *
 * \param simu t_simulation
 * \param coord t_coord
 * \return bool
 *
 */
bool isOnPhero(t_simulation simu, t_coord coord, chemin_phero & chemin){
    for(int i = 0; i < simu.nbChemins; i++)
        for(int j = 0; j < simu.chemins[i].nbCoord; j++)
            if(coordEquals(coord, simu.chemins[i].pheros[j].coord))
                if(simu.chemins[i].pheros[j].force > 0){
                    chemin = simu.chemins[i];
                    return true;
                }else
                    return false;
    return false;
}

bool isOnHome(t_simulation simu, t_coord coord){
    return coordEquals(coord, simu.maison.coord);
}

bool isOnNourriture(t_simulation simu, t_coord coord, int&index_nourriture){
    for(int i = 0; i < simu.nbSources; i++)
        if(coordEquals(simu.sources[i].coord, coord)){
            index_nourriture = i;
            return true;
        }
    return false;
}

void deplacerAleafourmi(t_fourmi&fourmi){
    if(rand()%5 == 0)
        switch(rand()%4){
        case 0 :    //la fourmi monte
                fourmi.direction = 0;
            break;

        case 1 :    //la fourmi descend
                fourmi.direction = 1;
            break;

        case 2 :    //la fourmi va a droite
                fourmi.direction = 2;
            break;

        case 3 :    //la fourmi va a gauche
                fourmi.direction = 3;
            break;
        }

    switch(fourmi.direction){
        case 0 :    //la fourmi monte
            if(fourmi.coord.y > 0)
                fourmi.coord.y--;
            else
                fourmi.direction = 1;
            break;

        case 1 :    //la fourmi descend
            if(fourmi.coord.y < Y-1)
                fourmi.coord.y++;
            else
                fourmi.direction = 0;
            break;

        case 2 :    //la fourmi va a droite
            if(fourmi.coord.x < X-1)
                fourmi.coord.x++;
            else
                fourmi.direction = 3;
            break;

        case 3 :    //la fourmi va a gauche
            if(fourmi.coord.x > 0)
                fourmi.coord.x--;
            else
                fourmi.direction = 2;
            break;
    }
}

void deplacerHomefourmi(t_simulation&simu, t_fourmi&fourmi){
    t_coord coord_home = simu.maison.coord;
    t_coord coord_fourm = fourmi.coord;
    float xA, xB, yA, yB;
    yB = coord_fourm.y;
    yA = coord_home.y;
    xB = coord_fourm.x;
    xA = coord_home.x;
    float coef = (yB - yA)/(xB - xA);
    if(xB < xA)
        fourmi.coord.x++;
    else
        fourmi.coord.x--;

    int yOrigine = yA - coef*xA;
    float newY = fourmi.coord.x*coef + yOrigine + 0.5;
    fourmi.coord.y = (int) newY;
}

void dropPhero(t_simulation&simu, t_fourmi&fourmi){
    if(fourmi.idChemin >= 0){
        bool isSet = false;
        int indexChemin = getIndexChemin(simu, fourmi.idChemin);
        for(int i = 0; i < simu.chemins[indexChemin].nbCoord; i++)
            if(coordEquals(fourmi.coord, simu.chemins[indexChemin].pheros[i].coord)){
                simu.chemins[indexChemin].pheros[i].force = force_base;
                isSet = true;
                i = simu.chemins[indexChemin].nbCoord;
            }
        if(!isSet)
            addPheroChemin(simu.chemins[indexChemin], fourmi.coord);
    }else{
        simu.chemins[simu.nbChemins++] = getNewChemin();
        fourmi.idChemin = simu.chemins[simu.nbChemins-1].id;
        addPheroChemin(simu.chemins[simu.nbChemins-1], fourmi.coord);
    }
}

void evolutionEtat(t_simulation&simu, t_fourmi&fourmi){
    switch(fourmi.etat){

    case ET_AVANCER_ALEA :
        chemin_phero phero;
        int index_nourri;
        if(isOnNourriture(simu, fourmi.coord, index_nourri)){
            if(simu.sources[index_nourri].reste > 50){
                simu.sources[index_nourri].reste -= 50;
                fourmi.nourriture += 50;
            }else{
                fourmi.nourriture += simu.sources[index_nourri].reste;
                supprimerSource(simu, index_nourri);
            }
            fourmi.etat = ET_RENTRER_HOME;
            if(isOnPhero(simu, fourmi.coord, phero))
                fourmi.idChemin = phero.id;
        }else if(isOnPhero(simu, fourmi.coord, phero)){
            fourmi.idChemin = phero.id;
            fourmi.etat = ET_SUIVRE_TRACE;
        }else
            deplacerAleafourmi(fourmi);

        break;


    case ET_RENTRER_HOME :
        if(isOnHome(simu, fourmi.coord)){
            dropPhero(simu, fourmi);
            simu.maison.nourriture += fourmi.nourriture;
            fourmi.nourriture = 0;
            if(fourmi.idChemin < 0)
                fourmi.etat = ET_AVANCER_ALEA;
            else
                fourmi.etat = ET_SUIVRE_TRACE;
        }else{
            dropPhero(simu, fourmi);
            deplacerHomefourmi(simu, fourmi);
        }
        break;

    case ET_SUIVRE_TRACE :
        if(isOnNourriture(simu, fourmi.coord, index_nourri)){
            if(simu.sources[index_nourri].reste > 50){
                simu.sources[index_nourri].reste -= 50;
                fourmi.nourriture += 50;
            }else{
                fourmi.nourriture += simu.sources[index_nourri].reste;
                supprimerSource(simu, index_nourri);
            }

            fourmi.etat = ET_RENTRER_HOME;
        }else{
            int index = getIndexChemin(simu, fourmi.idChemin);
            if(index < 0)
                fourmi.etat = ET_AVANCER_ALEA;
            else{
                bool isNext = false;
                int i = 1;
                for(i = 1; i < simu.chemins[index].nbCoord && !isNext; i++)
                    if(coordEquals(simu.chemins[index].pheros[i].coord, fourmi.coord))
                            isNext = simu.chemins[index].pheros[i-1].force > 0;


                if(!isNext){
                    fourmi.idChemin = -1;
                    fourmi.coord.x++;
                    fourmi.etat = ET_AVANCER_ALEA;
                }else
                    fourmi.coord = simu.chemins[index].pheros[i-2].coord;
            }
        }
        break;

    default :
        fourmi.etat = ET_AVANCER_ALEA;
        break;
    }
}

void majMap(t_simulation&simu){

    for(int i = 0; i < Y; i++){
        for(int j = 0; j < X; j++)
            simu.maMap[i][j] = ' ';
        simu.maMap[i][X] = '\0';
    }

    for(int i = 0; i < simu.maison.nbfourmis; i ++){
        t_fourmi fourmi = simu.maison.fourmis[i];
        char fourm;
        if(fourmi.direction < 2)
            fourm = '|';
        else
            fourm = '_';
        simu.maMap[fourmi.coord.y][fourmi.coord.x] = fourm;
    }

    for(int i = 0; i < simu.nbChemins; i++)
        for(int j = 0; j < simu.chemins[i].nbCoord; j++)
            if(simu.chemins[i].pheros[j].force > 0)
                if(simu.maMap[simu.chemins[i].pheros[j].coord.y][simu.chemins[i].pheros[j].coord.x] == '|' || simu.maMap[simu.chemins[i].pheros[j].coord.y][simu.chemins[i].pheros[j].coord.x] == '_')
                    simu.maMap[simu.chemins[i].pheros[j].coord.y][simu.chemins[i].pheros[j].coord.x] = 'i';
                else
                    simu.maMap[simu.chemins[i].pheros[j].coord.y][simu.chemins[i].pheros[j].coord.x] = '.';

    for(int i = 0; i < simu.nbSources; i++)
        simu.maMap[simu.sources[i].coord.y][simu.sources[i].coord.x] = 'N';

    simu.maMap[simu.maison.coord.y-1][simu.maison.coord.x-2] = 30;
    simu.maMap[simu.maison.coord.y][simu.maison.coord.x-2] = 219;
    simu.maMap[simu.maison.coord.y][simu.maison.coord.x-1] = 219;
    simu.maMap[simu.maison.coord.y-1][simu.maison.coord.x] = 30;
    simu.maMap[simu.maison.coord.y][simu.maison.coord.x] = 219;
    simu.maMap[simu.maison.coord.y-1][simu.maison.coord.x] = 30;
    simu.maMap[simu.maison.coord.y][simu.maison.coord.x] = 219;

}


int menuPrincipal(){
    int choix = 0;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  endl
                <<  "MENU" << endl
                <<  "1) Mode automatique" << endl
                <<  "2) Mode nanuel (parametrez votre simulation comme vous le souhaitez)" << endl
                <<  endl
                <<  "Mode de simulation : ";
        char c;
        fflush(stdin);
        cin >> c;
        choix = c - '0';
    }while(choix < 1 || choix > 2);
    return choix;
}

int menuParametre(){
    int choix = 0;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  endl
                <<  "MENU DE PARAMETRAGE" << endl
                <<  "1) Modifier le nombre de fourmis en début de partie" << endl
                <<  "2) Modifier la vie de base d'une fourmie" << endl
                <<  "3) Modifier le taux de natalité de la fourmiliere" << endl
                <<  "4) Modifier le nombre de source de nourriture sur la map" << endl
                <<  "5) Modifier la quantite de nourriture dans les sources" << endl
                <<  "6) Modifier la force d'une trace de pheromone" << endl
                <<  "7) Terminer le parametrage et lancer la simulation" << endl
                <<  endl
                <<  "Mode de simulation : ";
        char c;
        fflush(stdin);
        cin >> c;
        choix = c - '0';
    }while(choix < 1 || choix > 7);
    return choix;
}

void parametreNbFourmis(){
    int choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage du nombre de fourmis en début de partie" << endl
                <<  endl
                <<  "Parametre actuel : " << fourmis_base << endl
                <<  "Nouvelle valeur (comprise entre 1 et 200) : " << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 1 || choix > 200);
    fourmis_base = choix;
}

void parametreVieFourmis(){
    int choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage du nombre de points de vie d'une fourmi" << endl
                <<  endl
                <<  "Parametre actuel : " << vie_base << endl
                <<  "Nouvelle valeur (comprise entre 1 et 5000) : "  << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 1 || choix > 5000);
    vie_base = choix;
}

void parametreTauxNaissance(){
    float choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage du taux de naissance" << endl
                <<  endl
                <<  "0 correspond à aucune naissance et 1 à une naissance par tour"
                <<  "Parametre actuel : " << taux << endl
                <<  "Nouvelle valeur (comprise entre 0 et 1) : " << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 0 || choix > 1);
    taux = choix+0.000000000001;
}

void parametreNbSources(){
    int choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage du nombre de sources de nourriture en début de partie" << endl
                <<  endl
                <<  "Parametre actuel : " << nbSources << endl
                <<  "Nouvelle valeur (comprise entre 1 et 50) : " << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 1 || choix > 50);
    nbSources = choix;
}

void parametreQuantiteNour(){
    int choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage de la quantite de nourriture dans les sources" << endl
                <<  endl
                <<  "Parametre actuel : " << quantite_nour << endl
                <<  "Nouvelle valeur (comprise entre 1 et 5000) : " << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 1 || choix > 5000);
    quantite_nour = choix;
}

void parametreForcePhero(){
    int choix = 1;
    do{
        system("cls");
        cout    <<  "\t\t\t    Bienvenue sur le simulateur de fourmis 3000 !" << endl
                <<  "=====================================================================================================" << endl
                <<  endl
                <<  "Parametrage de la force des pheromones" << endl
                <<  endl
                <<  "Parametre actuel : " << force_base << endl
                <<  "Nouvelle valeur (comprise entre 1 et 500) : " << endl;
        fflush(stdin);
        cin >> choix;
    }while(choix < 1 || choix > 500);
    force_base = choix;
}

int main()
{
    srand(time(0));
    string cmd = "mode " + to_string(X+1) + ", " + to_string(Y+1);
    system(cmd.c_str());

    int choix = menuPrincipal();
    if(choix == 2)
        do{
            choix = menuParametre();
            switch(choix){
                case 1 :    parametreNbFourmis();
                            break;

                case 2 :    parametreVieFourmis();
                            break;

                case 3 :    parametreTauxNaissance();
                            break;

                case 4 :    parametreNbSources();
                            break;

                case 5 :    parametreQuantiteNour();
                            break;

                case 6 :    parametreForcePhero();
                            break;
            }
        }while(choix != 7);

    fourmiliere fourmil;
    fourmil.nbfourmis = fourmis_base;
    fourmil.nourriture = 0;
    fourmil.coord = {50, 30};



    for(int i = 0; i < fourmil.nbfourmis; i++)
        fourmil.fourmis[i] = {fourmil.coord, 0, vie_base, i%4, -1, ET_AVANCER_ALEA};

    t_simulation simu;
    simu.maison = fourmil;
    simu.nbChemins = 0;
    simu.nbSources = 0;

    for(int i = 0; i < nbSources; i++){
        t_coord coord;
        do{
            coord = getRandomCoord();
        }while(coordEquals(coord, fourmil.coord));
        simu.sources[simu.nbSources++] = {coord, quantite_nour};
    }

    simu.chemins[0] = {{}, -1, -1};

    int nbTour = 0;
    while(simu.maison.nbfourmis > 0 && (simu.maison.nourriture > 0 || taux != 0.000000000001)){
        nbTour++;
        for(int i = 0; i < simu.maison.nbfourmis; i++)
            evolutionEtat(simu, simu.maison.fourmis[i]);

        if(rand()%((int)(1./taux)) == 1 && simu.maison.nourriture > 0)
            spawnfourmi(simu.maison);

        fatiguerfourmis(simu.maison);
        fatiguerChemins(simu);

        majMap(simu);
        string affiche = "";
        for(int i = 0; i < Y; i++)
            affiche += string(simu.maMap[i]) + "\n";
        system("cls");
        cout << affiche;
        cout << "nbfourmis : " << simu.maison.nbfourmis << "      nbTour : " << nbTour << "      nbNour : " << simu.maison.nourriture;
        Sleep(100);
    }

    system("cls");
    cout << "Votre fourmilière est morte au bout de " << nbTour << " tours.\n A bientot sur le similuateur de fourmis 3000 !";
    return 0;
}
