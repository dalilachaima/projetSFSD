/* Remarque importante : ce fichier contient un peu de tout, ca pourrait vous aidez dans vos fonctions
( je ne suis pas sure de la validité des informations svp vérifiez avant de les utiliser ) */



#include <stdio.h>
#include <stdlib.h>
#define nbrBloc 1000 //le nombre de blocs dns la memoire
#define FB 5 // le nombre des enregistrements possibles dans chaque bloc
#define nbrMaxFichier 100 // le nombre maximum des fichiers dans la memoire
#define First_Bloc 0 // le premier bloc est reservé a la table d'allocation




initialiserMemoire() {
	for(int i = 0; i < nbrBloc; i++){
		TableDAllocation.etatBlocs[i] = 0 // tous libres intialement
		TableDAllocation.etatBlocs[0] = 1 // la table d'allocation 
	}
}

typedef struct BLOC BLOC ;
struct BLOC{
	ENREGISTREMENT ENREG[FB] ;  //FB=FACTEURE DE BLOCAGE
	int nmbE  ; // nombre d' ENREGISTREMENT DANS LE BLOC
	int nextBloc //adresse next bloc dans le cas d'organistion chainée
}

BLOC MS[nbrBloc]; //memoire secondaire sous forme de tableu de blocs




typedef struct Meta Meta;
struct Meta {
    char nom_fichier[50];
    int taille_blocs; // le nombre de blocs dans le fichier
    int taille_enregistrements; // le nombre d'enregistrement dans le fichier
    int adresse_premier_bloc;
    int organisation_globale; //1 pour contigue et 2 pour chainée
    int organisation_interne; // 1 pour trié et 2 pour non trié
};



Meta listeFichiers[nbrBloc]; //liste de tout les fichiers 

void creerFichier(){
	// creation des meta du fichier
	Meta meta;
	printf("Création d'un fichier: \n");
    printf("Nom du fichier: ");
    scanf("%s", meta.nom_fichier);
    printf("Nombre d'enregistrements: ");
    scanf("%d", &meta.taille_enregistrements);
    printf("Mode d'organisation globale (1: contigu, 2: chaîné): ");
    scanf("%d", &meta.mode_globale);
    printf("Mode d'organisation interne (1: trié, 2: non trié): ");
    scanf("%d", &meta.mode_interne);
	
	meta.taille_blocs = (meta.taille_enregistrements + FB - 1) / FB;
    // meta.adresse_premier_bloc = ??
	
	
	//ajout du fichier à la liste des fichiers (la premiére position vide trouvé)
	for(int i=0; i < nbrMaxFichiers; i++ ){
		if(listeFichiers[i].meta.nom_fichier[0] == "\0"){ 
			listeFichiers[i].meta = meta;
			printf("Fichier cree avec succes !");
			return;
		}
	}
	
	//allouer un bloc au fichier 
	BLOC buffer = {0};
    for(int i = 0; i < meta.taille_blocs; i++) {
        if (meta.mode_globale == 2 && i < meta.taille_blocs - 1) {
            buffer.suivant = meta.adresse_premier_bloc + i + 1; // chainé
        } else {
            buffer.suivant = -1; // contigu
        }
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(BLOC), SEEK_SET);
        fwrite(&buffer, sizeof(BLOC), 1, fichier);
    }
    printf("Fichier cree avec succes !");
}

//lire les meta d'un fichier;
void LireMeta(FILE *f, Meta *meta) {
    rewind(f);
    fread(meta, sizeof(Meta), 1, f);
}


// insertion dans le cas chainé bloc non trié
void insertionChainee(FILE *fichier, ENREGISTREMENT enreg) {
    META meta;
    BLOC bloc;
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;
    while (pos != -1) {
        fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        if (bloc.nmbE < FB) { // Insérer dans le bloc
            bloc.ENREG[bloc.nmbE++] = enreg;
            fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);
            printf("Enregistrement inséré avec succès.\n");
            return;
        }
        pos = bloc.nextBloc;
    }

    printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
}





void suppressionPhysique(FILE *fichier, int ID) {
    META meta;
    BLOC bloc, suivant;
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int posBloc = -1, posEnreg = -1;

    for (int i = 0; i < meta.taille_blocs; i++) {
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        for (int j = 0; j < bloc.nmbE; j++) {
            if (bloc.ENREG[j].ID == ID) {
                posBloc = i;
                posEnreg = j;
                break;
            }
        }
        if (posBloc != -1) break;
    }

    if (posBloc == -1) {
        printf("Enregistrement introuvable.\n");
        return;
    }

    // Réorganiser les blocs
    for (int i = posEnreg; i < bloc.nmbE - 1; i++) {
        bloc.ENREG[i] = bloc.ENREG[i + 1];
    }
    bloc.nmbE--;

    fseek(fichier, (meta.adresse_premier_bloc + posBloc) * sizeof(BLOC), SEEK_SET);
    fwrite(&bloc, sizeof(BLOC), 1, fichier);

    printf("Enregistrement supprimé physiquement.\n");
}
















int rechercheChaineeNonTrie(FILE *fichier, int ID) {
    META meta;
    BLOC bloc;
    // Lire les métadonnées
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;

    while (pos != -1) {
        fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Recherche séquentielle dans le bloc
        for (int i = 0; i < bloc.nmbE; i++) {
            if (bloc.ENREG[i].ID == ID) {
               int blocTrouve = pos;
			   int posTrouve = i;
                printf("Enregistrement trouve au bloc : %d, l'emplacement : %d ", blocTrouve, posTrouve);  // Trouvé
                return;
            }
        }

        pos = bloc.suivant; // Passer au bloc suivant
    }
    
    printf("Enregistrement introuvable !");
    return ; 
}







void afficherTableauMeta() {
    FILE *MS = fopen("disque_virtuel.bin", "rb");
    if (!MS) {
        printf("Erreur : impossible d'accéder à la mémoire secondaire.\n");
        return;
    }

    META meta;
    int compteur = 0;

    printf("\n--- Tableau Descriptif des Métadonnées ---\n");
    printf("------------------------------------------------------------\n");
    printf("| N° | Nom Fichier | Blocs | Enregistrements | Organisation Globale | Organisation Interne |\n");
    printf("------------------------------------------------------------\n");

    // Parcourir tous les blocs pour trouver les métadonnées
    while (fread(&meta, sizeof(META), 1, MS) == 1) {
        compteur++;
        printf("| %3d | %-13s | %5d | %15d | %-19s | %-18s |\n",
               compteur,
               meta.nom_fichier,
               meta.taille_blocs,
               meta.taille_enregistrements,
               meta.mode_globale == 1 ? "Contigu" : "Chaîné",
               meta.mode_interne == 1 ? "Trié" : "Non trié");
    }

    printf("------------------------------------------------------------\n");

    fclose(MS);

    if (compteur == 0) {
        printf("Aucune métadonnée trouve !"); 
    }
}






