#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCS 1000
#define FB 100
#define nbrMaxFichiers 500

typedef struct ENREGISTREMENT ENREGISTREMENT;
struct ENREGISTREMENT {
 int ID ;
 char champs[60] ;
 int deleted;
};

typedef struct {
    int is_occupied;
    int nbrEnreg;
    int next_bloc; //adresse bloc suivant
    ENREGISTREMENT enreg[FB];
} Bloc;


typedef struct META META;
struct META {
    char nom_fichier[50];
    int taille_blocs;
    int taille_enregistrements;
    int adresse_premier_bloc;
    int organisation_globale;
    int organisation_interne;
};



typedef struct {
    META meta;
    Bloc blocs[MAX_BLOCS];
} File;

Bloc allocation_table[MAX_BLOCS];
Bloc MS[MAX_BLOCS];
File files[MAX_BLOCS];
int total_blocks;
int block_size;
int file_count = 0;




int total_blocks; 
int block_size; 

void initialize_disk() {
    total_blocks = MAX_BLOCS;
    block_size = FB + 3;
    for (int i = 0; i < total_blocks; i++) {
        allocation_table[i].is_occupied = 0;
    }
    allocation_table[1].is_occupied = 1;
    file_count = 0;
    printf("Disc initialis .\n");
}


int check_free_space(int blocks_needed) {
    int free_blocks = 0;
    for (int i = 0; i < total_blocks; i++) {
        if (!allocation_table[i].is_occupied) {
            free_blocks++;
        }
        if (free_blocks >= blocks_needed) {
            return 1;
        }
    }
    return 0;
}









void compact() {
    META meta;
    printf("Compacting disk...\n");
    int next_free_index = 0;
    for (int i = 0; i < file_count; i++) {
        File *file = &files[i];
        int new_blocks[file->meta.taille_blocs];

        for (int j = 0; j < file->taille_blocs; j++) {
            while (allocation_table[next_free_index].is_occupied) {
                next_free_index++;
            }
            new_blocks[j] = next_free_index;
            allocation_table[next_free_index].is_occupied = 1;
            next_free_index++;
        }

        for (int j = 0; j < file->meta.taille_blocs; j++) {
            allocation_table[file->blocs[j]].is_occupied = 0;
        }

        memcpy(file->blocs, new_blocks, sizeof(new_blocks));
    }
    printf("Disk compacted.\n");
}













void delete_file(const char *file_name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].meta.nom_fichier, file_name) == 0) {
            for (int j = 0; j < files[i].meta.taille_blocs; j++) {
                allocation_table[files[i].blocs[j]].is_occupied = 0;
            }
            for (int k = i; k < file_count - 1; k++) {
                files[k] = files[k + 1];
            }
            file_count--;
            printf("File '%s' deleted.\n", file_name);
            return;
        }
    }
    printf("Error: File '%s' not found.\n", file_name);
}


void clear_disk() {
    initialize_disk(MAX_BLOCS, FB+2);
    printf("Disk cleared.\n");
}

void display_allocation_table() {
    printf("Allocation Table:\n");
    for (int i = 0; i < MAX_BLOCS; i++) {
        if(allocation_table[i].is_occupied == 0){
        printf("| Libre ");
}else{

        printf("| %s : %d blocs ", file[i].meta.nom_fichier, file[i].meta.taille_blocs);
    }
  }
    printf(" |\n");
}



void display_files() {
    printf("Files on Disk:\n");
    for (int i = 0; i < file_count; i++) {
        printf("________________________________________________________________________________________________________________________\n");
        if(file[i].meta.organisation_globale == 1 ){ char orgGlo = "contigu ";}else{char orgGlo = "cha n e";}
        if(file[i].meta.organisation_interne == 1){char orgInt ="tri "}else{char orgInt ="non tri "}
        printf("|Nom: %s, adresse premier bloc : %d, taille en blocs : %d, taille en enregistrant : %d, organisation globale : %s, organisation interne : %s | \n", file[i].meta.nom_fichier, file[i].meta.adresse_premier_bloc, file[i].meta.taille_blocs, file[i].meta.taille_enregistrements, orgGlo, orgInt);
        printf("________________________________________________________________________________________________________________________\n");
    }
}




void MAJMeta(FILE *f, int nc, void *v) {
META meta;
    rewind(f);
    switch (nc) {

        case 1: fseek(f, offsetof(Files, nom_fichier), SEEK_SET); fwrite(v, sizeof(char[100]), 1, f); break;
		
		case 2: fseek(f, offsetof(Files, adresse_premier_bloc), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;

        case 3: fseek(f, offsetof(Files, taille_enregistrements), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;

        case 4: fseek(f, offsetof(Files, taille_blocs), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;

        case 5: fseek(f, offsetof(Files, organisation_globale), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;

        case 6: fseek(f, offsetof(Files, organisation_interne), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;

        default: printf("Option non valide\n"); break;
    }
}



void creer_fichier(){
    // creation des meta du fichier
    Meta meta;
    printf("Cr ation d'un fichier: \n");
    printf("Nom du fichier: ");
    scanf("%s", meta.nom_fichier);
    printf("Nombre d'enregistrements: ");
    scanf("%d", &meta.taille_enregistrements);
    printf("Mode d'organisation globale (1: contigu, 2: cha n ): ");
    scanf("%d", &meta.mode_globale);
    printf("Mode d'organisation interne (1: tri , 2: non tri ): ");
    scanf("%d", &meta.mode_interne);
    
    meta.taille_blocs = (meta.taille_enregistrements + FB - 1) / FB;
    meta.adresse_premier_bloc = -1;
 
    if (!check_free_space(meta.taille_blocs)) { 
        printf("Erreur: Espace insuffisant pour créer le fichier.\n"); 
        return; 
    }

    FILE *fichier = fopen(meta.nom_fichier, "wb");
    if(fichier == NULL){
        printf("Erreur lors de o'ouverture du fichier\n");
    return;
	}
    
    //ajout du fichier à la liste des fichiers (la première position vide trouvée)
    for(int i = 0; i < nbrMaxFichiers; i++ ){
        if(files[i].meta.nom_fichier[0] == '\0'){ 
            files[i].meta = meta;
            MAJMeta(fichier, 1, &meta.nom_fichier);
			MAJMeta(fichier, 2, &meta.adresse_premier_bloc);
			MAJMeta(fichier, 3, &meta.taille_enregistrements);
			MAJMeta(fichier, 4, &meta.taille_blocs);
			MAJMeta(fichier, 5, &meta.mode_globale); 
			MAJMeta(fichier, 6, &meta.mode_interne);
            printf("Fichier cree avec succes !");
            return;
        }
    }
    
    // Allocation des blocs
    Bloc buffer = {0}; // Initialisation d'un bloc vide
    int adresse_bloc_precedent = -1; // Variable pour cha ner les blocs

    for (int i = 0; i < meta.taille_blocs; i++) {
        // Pour le mode cha n 
        if (meta.mode_globale == 2) {
            buffer.suivant = (i < meta.taille_blocs - 1) ? (meta.adresse_premier_bloc + i + 1) : -1;
        } else {
            buffer.suivant = -1; // Pour le mode contigu
        }

        // Si c'est le premier bloc, on le place dans le fichier et on initialise son adresse
        if (i == 0) {
            meta.adresse_premier_bloc = ftell(fichier) / sizeof(Bloc); // Adresse du premier bloc
        }
        allocation_table[i].is_occupied = 1;
        //  ecriture du bloc dans le fichier
        fwrite(&buffer, sizeof(Bloc), 1, fichier);

        // Mise à jour du bloc précédent pour cha ner les blocs si nécessaire
        if (meta.mode_globale == 2) {
            if (adresse_bloc_precedent != -1) {
                fseek(fichier, adresse_bloc_precedent * sizeof(Bloc), SEEK_SET);
                fread(&buffer, sizeof(Bloc), 1, fichier);
                buffer.suivant = meta.adresse_premier_bloc + i;
                fseek(fichier, adresse_bloc_precedent * sizeof(Bloc), SEEK_SET);
                fwrite(&buffer, sizeof(Bloc), 1, fichier);
            }
            adresse_bloc_precedent = ftell(fichier) / sizeof(Bloc) - 1; // Mise à jour de l'adresse du bloc précédent
        }
    }

    printf("Fichier cré e avec succes !\n");
    fclose(fichier);
}



// Fonction pour renommer un fichier
void renommer_fichier() {
    char ancien_nom[50], nouveau_nom[50];
    int fichier_trouve = 0;

    printf("Entrez le nom du fichier   renommer: ");
    scanf("%s", ancien_nom);

    // Recherche du fichier dans le tableau
    for (int i = 0; i < nbrMaxFichiers; i++) {
        if (strcmp(files[i].meta.nom_fichier, ancien_nom) == 0) {
            // Si le fichier est trouv , demande le nouveau nom
            printf("Entrez le nouveau nom du fichier: ");
            scanf("%s", nouveau_nom);

            // V rifie si le nouveau nom est d j  pris
            int nom_deja_pris = 0;
            for (int j = 0; j < nbrMaxFichiers; j++) {
                if (strcmp(files[j].meta.nom_fichier, nouveau_nom) == 0) {
                    nom_deja_pris = 1;
                    break;
                }
            }

            if (nom_deja_pris) {
                printf("Erreur: Le nom '%s' est d j  utilis  pour un autre fichier.\n", nouveau_nom);
                return;
            }

            // Mise   jour du nom du fichier dans les m tadonn es
            strcpy(files[i].meta.nom_fichier, nouveau_nom);
            MAJMeta(fopen(ancien_nom, "rb+"), 1, &nouveau_nom);
            printf("Le fichier a  t  renomm  avec succ s en '%s'.\n", nouveau_nom);
            fichier_trouve = 1;
            break;
        }
    }

    if (!fichier_trouve) {
        printf("Erreur: Fichier '%s' non trouv .\n", ancien_nom);
    }
}











// Fonction de suppression logique d'un enregistrement
void suppression_logique(int id) {
    int posBloc = -1, posEnreg = -1;

    // Rechercher l'enregistrement avec l'ID donn 
    if (/*rechercher_enregistrement selon les modes d'organisation */ ) { //position et bloc trouv s
        Fichier file = files[0]; // Supposons qu'on travaille sur le premier fichier, ajuster si n cessaire
        BLOC bloc;
        
        // Aller   la position du bloc et lire le bloc
        fseek(fichier, posBloc * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // V rifier si l'enregistrement existe et marquer comme supprim 
        if (bloc.enreg[posEnreg].ID == id) {
            bloc.enreg[posEnreg].supprime = 1; // Marquer l'enregistrement comme supprim 
            fseek(fichier, posBloc * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);
            printf("Enregistrement ID %d supprim  logiquement.\n", id);
            // Mise à jour des métadonnées
			 MAJMeta(fichier, 3, &bloc.enreg[posEnreg]);
        } else {
            printf("Enregistrement ID %d non trouv  dans le bloc.\n", id);
        }
    } else {
        printf("Enregistrement ID %d non trouv .\n", id);
    }
}










void suppression_physique(int id) {
    int posBloc = -1, posEnreg = -1;

    // Rechercher l'enregistrement avec l'ID donn 
    if (!rechercher_enregistrement(id, &posBloc, &posEnreg)) {
        printf("Enregistrement ID %d non trouv .\n", id);
        return;
    }

    BLOC bloc, bloc_suivant;
    Fichier file = files[0]; // Supposons qu'on travaille sur le premier fichier (ajuster si n cessaire)
    
    // Lire le bloc contenant l'enregistrement
    fseek(fichier, posBloc * sizeof(BLOC), SEEK_SET);
    fread(&bloc, sizeof(BLOC), 1, fichier);

    // Supprimer l'enregistrement en d calant les autres enregistrements
    for (int i = posEnreg; i < bloc.nbrEnreg - 1; i++) {
        bloc.enreg[i] = bloc.enreg[i + 1];
    }
    bloc.nbrEnreg--; // R duire le nombre d'enregistrements dans le bloc

    // R  crire le bloc modifi 
    fseek(fichier, posBloc * sizeof(BLOC), SEEK_SET);
    fwrite(&bloc, sizeof(BLOC), 1, fichier);

    // Si le bloc devient vide, supprimer le bloc et r organiser les liens si le fichier est cha n 
    if (bloc.nbrEnreg == 0) {
        int bloc_precedent = -1, bloc_courant = file.meta.adresse_premier_bloc;

        // Trouver le bloc pr c dent pour mettre   jour le lien
        while (bloc_courant != posBloc) {
            bloc_precedent = bloc_courant;

            fseek(fichier, bloc_courant * sizeof(BLOC), SEEK_SET);
            fread(&bloc_suivant, sizeof(BLOC), 1, fichier);

            bloc_courant = bloc_suivant.suivant;
        }

        // R organiser les liens dans le cas de fichiers cha n s
        if (bloc_precedent != -1) {
            fseek(fichier, bloc_precedent * sizeof(BLOC), SEEK_SET);
            fread(&bloc_suivant, sizeof(BLOC), 1, fichier);

            bloc_suivant.suivant = bloc.suivant;
            fseek(fichier, bloc_precedent * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc_suivant, sizeof(BLOC), 1, fichier);
        } else {
            // Si c'est le premier bloc, mettre   jour l'adresse du premier bloc dans les m tadonn es
            file.meta.adresse_premier_bloc = bloc.suivant;
            MAJMeta(fichier, 2, &file.meta.adresse_premier_bloc);
        }

        // R initialiser le bloc supprim 
        bloc.is_occupied = 0;
        bloc.suivant = -1;
        fseek(fichier, posBloc * sizeof(BLOC), SEEK_SET);
        fwrite(&bloc, sizeof(BLOC), 1, fichier);
    }

    printf("Enregistrement ID %d supprim  physiquement.\n", id);
}





// insertion dans le cas chain  bloc non tri 
void insertionChaineeNonTrie(ENREGISTREMENT enreg) {
	printf("Entrer le nom du fichier ou mettre l'enregistrement :\n");
	char nom[50];
	scanf("%49s", nom); 
	FILE *fichier = fopen(nom, "r+");
    META meta;
    BLOC bloc;
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;
    while (pos != -1) {
        fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        if (bloc.nmbE < FB) { // Ins rer dans le bloc
            bloc.ENREG[bloc.nmbE++] = enreg;
            fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);
            printf("Enregistrement ins r  avec succ s.\n");
            // Mise à jour des métadonnées 
			MAJMeta(fichier, 3, &meta.taille_enregistrements);
			fclose(fichier);
			return;
        }
        pos = bloc.nextBloc;
    }

    printf("Erreur : Aucun espace disponible pour ins rer l'enregistrement.\n");
    fclose(fichier);
}









void insertion_chainee_triee(ENREGISTREMENT enreg) {
	
	printf("Entrer le nom du fichier ou mettre l'enregistrement :\n");
	char nom[50];
	scanf("%49s", nom); 
	FILE *fichier = fopen(nom, "r+");
	
    Meta meta;
    BLOC bloc, nouveau_bloc;
    int prevBloc = -1, currBloc;

    // Lire les m tadonn es
    rewind(fichier);
    fread(&meta, sizeof(Meta), 1, fichier);

    currBloc = meta.adresse_premier_bloc;

    // Parcourir les blocs pour trouver la position d'insertion
    while (currBloc != -1) {
        fseek(fichier, currBloc * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Chercher la position dans le bloc o  ins rer
        int i = 0;
        while (i < bloc.nbrEnreg && bloc.enreg[i].Cl  < enreg.Cl ) {
            i++;
        }

        // Si le bloc a de l'espace, ins rer ici
        if (bloc.nbrEnreg < FB) {
            // D caler les enregistrements pour faire de la place
            for (int j = bloc.nbrEnreg; j > i; j--) {
                bloc.enreg[j] = bloc.enreg[j - 1];
            }
            bloc.enreg[i] = enreg; // Ins rer le nouvel enregistrement
            bloc.nbrEnreg++;

            // R  crire le bloc dans le fichier
            fseek(fichier, currBloc * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);
            printf("Enregistrement ins r  avec succ s (cha n  tri ).\n");
            // Mise à jour des métadonnées
			 MAJMeta(fichier, 3, &meta.taille_enregistrements);
			 fclose(fichier);
			return;
        }

        // Passer au bloc suivant
        prevBloc = currBloc;
        currBloc = bloc.suivant;
    }

    // Si aucun espace trouv , cr er un nouveau bloc
    int newBlocIndex = obtenir_bloc_libre(fichier); // Fonction pour trouver un bloc libre
    if (newBlocIndex == -1) {
        printf("Erreur : Aucun espace disponible pour ins rer l'enregistrement.\n");
        return;
    }

    // Initialiser le nouveau bloc
    nouveau_bloc.nbrEnreg = 1;
    nouveau_bloc.enreg[0] = enreg;
    nouveau_bloc.suivant = -1;

    // Lier le nouveau bloc au pr c dent
    if (prevBloc != -1) {
        fseek(fichier, prevBloc * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);
        bloc.suivant = newBlocIndex;
        fseek(fichier, prevBloc * sizeof(BLOC), SEEK_SET);
        fwrite(&bloc, sizeof(BLOC), 1, fichier);
    } else {
        // Si c'est le premier bloc
        meta.adresse_premier_bloc = newBlocIndex;
        rewind(fichier);
        fwrite(&meta, sizeof(Meta), 1, fichier);
    }

    //  crire le nouveau bloc dans le fichier
    fseek(fichier, newBlocIndex * sizeof(BLOC), SEEK_SET);
    fwrite(&nouveau_bloc, sizeof(BLOC), 1, fichier);

    printf("Enregistrement ins r  dans un nouveau bloc (cha n  tri ).\n");
    fclose(fichier);
}









void insertion_contigue_triee(ENREGISTREMENT enreg) {
	
	printf("Entrer le nom du fichier ou mettre l'enregistrement :\n");
	char nom[50];
	scanf("%49s", nom); 
	FILE *fichier = fopen(nom, "r+");
	if (fichier == NULL) { 
	printf("Erreur lors de l'ouverture du fichier.\n");
	 return; 
	 }
	 
    BLOC bloc;
    int blocPosition;
    int enregPosition;

    // Lire les metadonnees
    rewind(fichier);
    fread(&meta, sizeof(Meta), 1, fichier);

    // V rifier si le fichier est plein
    if (meta.taille_enregistrements == meta.taille_blocs * FB) {
        printf("Erreur : Aucun espace disponible pour ins rer l'enregistrement.\n");
		fclose(fichier);
		  return;
    }

    // Parcourir les blocs pour trouver la position d'insertion
    for (blocPosition = meta.adresse_premier_bloc; blocPosition < meta.adresse_premier_bloc + meta.taille_blocs; blocPosition++) {
        fseek(fichier, blocPosition * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Trouver la position d'insertion dans le bloc
        for (enregPosition = 0; enregPosition < bloc.nbrEnreg; enregPosition++) {
            if (bloc.enreg[enregPosition].Cl  > enreg.Cl ) {
                break;
            }
        }

        // Si le bloc a de l'espace, ins rer ici
        if (bloc.nbrEnreg < FB) {
            // D caler les enregistrements pour faire de la place
            for (int j = bloc.nbrEnreg; j > enregPosition; j--) {
                bloc.enreg[j] = bloc.enreg[j - 1];
            }
            bloc.enreg[enregPosition] = enreg; // Ins rer l'enregistrement
            bloc.nbrEnreg++;

            // R  crire le bloc dans le fichier
            fseek(fichier, blocPosition * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);

            // Mettre   jour les m tadonn es
            meta.taille_enregistrements++;
            rewind(fichier);
            fwrite(&meta, sizeof(Meta), 1, fichier);
            // Mise à jour des métadonnées
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
            printf("Enregistrement ins r  avec succ s (contigu tri ).\n");
            fclose(fichier)
            return;
        }
        
    }

    // Si aucun bloc existant n'a de place, allouer un nouveau bloc
    if (meta.taille_blocs < nbrBloc) {
        bloc.nbrEnreg = 1;
        bloc.enreg[0] = enreg;

        fseek(fichier, (meta.adresse_premier_bloc + meta.taille_blocs) * sizeof(BLOC), SEEK_SET);
        fwrite(&bloc, sizeof(BLOC), 1, fichier);

        meta.taille_blocs++;
        meta.taille_enregistrements++;
        rewind(fichier);
        fwrite(&meta, sizeof(Meta), 1, fichier);
        // Mise à jour des métadonnées
        MAJMeta(fichier, 3, &meta.taille_enregistrements);
        printf("Enregistrement ins r  dans un nouveau bloc (contigu tri ).\n");
    } else {
        printf("Erreur : Impossible d'allouer un nouveau bloc.\n");
    }
    fclose(fichier);
}










void insertion_contigue_non_triee(ENREGISTREMENT enreg) {
    
    printf("Entrer le nom du fichier où se trouve l'enregistrement :\n");
    char nom[50];
    scanf("%49s", nom); 
    FILE *fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }
    
    Meta meta;
    BLOC bloc;
    int blocPosition;

    // Lire les métadonnées
    rewind(fichier);
    fread(&meta, sizeof(Meta), 1, fichier);

    // Vérifier si le fichier est plein
    if (meta.taille_enregistrements == meta.taille_blocs * FB) {
        printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
        fclose(fichier);
        return;
    }

    // Parcourir les blocs pour trouver un espace disponible
    for (blocPosition = meta.adresse_premier_bloc; blocPosition < meta.adresse_premier_bloc + meta.taille_blocs; blocPosition++) {
        fseek(fichier, blocPosition * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Vérifier si le bloc a de l'espace
        if (bloc.nbrEnreg < FB) {
            bloc.enreg[bloc.nbrEnreg++] = enreg; // Ajouter l'enregistrement à la fin
            fseek(fichier, blocPosition * sizeof(BLOC), SEEK_SET);
            fwrite(&bloc, sizeof(BLOC), 1, fichier);

            // Mettre à jour les métadonnées
            meta.taille_enregistrements++;
            rewind(fichier);
            fwrite(&meta, sizeof(Meta), 1, fichier);

            // Mise à jour des métadonnées
            MAJMeta(fichier, 3, &meta.taille_enregistrements);

            printf("Enregistrement inséré avec succès (contigu non trié).\n");
            fclose(fichier);
            return;
        }
    }

    // Si tous les blocs existants sont pleins, allouer un nouveau bloc
    if (meta.taille_blocs < nbrBloc) {
        bloc.nbrEnreg = 1;
        bloc.enreg[0] = enreg;

        fseek(fichier, (meta.adresse_premier_bloc + meta.taille_blocs) * sizeof(BLOC), SEEK_SET);
        fwrite(&bloc, sizeof(BLOC), 1, fichier);

        meta.taille_blocs++;
        meta.taille_enregistrements++;
        rewind(fichier);
        fwrite(&meta, sizeof(Meta), 1, fichier);

        // Mise à jour des métadonnées
        MAJMeta(fichier, 3, &meta.taille_enregistrements);

        printf("Enregistrement inséré dans un nouveau bloc (contigu non trié).\n");
    } else {
        printf("Erreur : Impossible d'allouer un nouveau bloc.\n");
    }

    fclose(fichier);
}









int recherche_contigue_non_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc) {
    Meta meta;
    BLOC bloc;
    
    printf("Entrer le nom du fichier où se trouve l'enregistrement :\n");
    char nom[50];
    scanf("%49s", nom); 
    fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 0; // Échec
    }

    // Lire les métadonnées
    rewind(fichier);
    fread(&meta, sizeof(Meta), 1, fichier);

    // Parcourir tous les blocs du fichier
    for (int i = 0; i < meta.taille_blocs; i++) {
        // Charger le bloc actuel
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Parcourir les enregistrements du bloc
        for (int j = 0; j < bloc.nbrEnreg; j++) {
            if (!bloc.enreg[j].supprime && bloc.enreg[j].ID == id) {
                // Enregistrement trouvé
                *blocPosition = i;
                *posDansBloc = j;
                fclose(fichier);
                return 1; // Succès
            }
        }
    }

    // Enregistrement non trouvé
    *blocPosition = -1;
    *posDansBloc = -1;
    fclose(fichier);
    return 0; // Échec
}









int recherche_contigue_triee(int id, int *blocPosition, int *posDansBloc) {
    printf("Entrer le nom du fichier où se trouve l'enregistrement :\n");
    char nom[50];
    scanf("%49s", nom); 
    FILE *fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 0; // Échec
    }
    
    Meta meta;
    BLOC bloc;

    // Lire les métadonnées
    rewind(fichier);
    fread(&meta, sizeof(Meta), 1, fichier);

    // Parcourir tous les blocs du fichier
    for (int i = 0; i < meta.taille_blocs; i++) {
        // Charger le bloc actuel
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Effectuer une recherche binaire dans le bloc
        int gauche = 0, droite = bloc.nbrEnreg - 1;
        while (gauche <= droite) {
            int milieu = (gauche + droite) / 2;
            if (bloc.enreg[milieu].ID == id && !bloc.enreg[milieu].supprime) {
                // Enregistrement trouvé
                *blocPosition = i;
                *posDansBloc = milieu;
                fclose(fichier);
                return 1; // Succès
            } else if (bloc.enreg[milieu].ID < id) {
                gauche = milieu + 1;
            } else {
                droite = milieu - 1;
            }
        }
    }

    // Enregistrement non trouvé
    *blocPosition = -1;
    *posDansBloc = -1;
    fclose(fichier);
    return 0; // Échec
}







int rechercheChaineeNonTrie(FILE *fichier, int ID, int *blocTrouve, int *posTrouve) {
    META meta;
    BLOC bloc;

    printf("Entrer le nom du fichier où se trouve l'enregistrement :\n");
    char nom[50];
    scanf("%49s", nom); 
    fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 0; // Échec
    }

    // Lire les métadonnées
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;

    while (pos != -1) {
        // Lire le bloc courant
        fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Recherche séquentielle dans le bloc
        for (int i = 0; i < bloc.nmbE; i++) {
            if (bloc.ENREG[i].ID == ID && !bloc.ENREG[i].supprime) { // Ignorer les enregistrements supprimés
                if (blocTrouve) *blocTrouve = pos; // Stocker la position du bloc
                if (posTrouve) *posTrouve = i; // Stocker la position dans le bloc
                fclose(fichier);
                return 1; // Enregistrement trouvé
            }
        }

        pos = bloc.suivant; // Passer au bloc suivant
    }

    fclose(fichier);
    return 0; // Enregistrement non trouvé
}






int rechercheChaineeTriee(int ID, int *blocTrouve, int *posTrouve) {
    
    printf("Entrer le nom du fichier où se trouve l'enregistrement :\n");
    char nom[50];
    scanf("%49s", nom); 
    FILE *fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 0; // Échec
    }
    
    META meta;
    BLOC bloc;

    // Lire les métadonnées
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;

    while (pos != -1) {
        // Lire le bloc courant
        fseek(fichier, pos * sizeof(BLOC), SEEK_SET);
        fread(&bloc, sizeof(BLOC), 1, fichier);

        // Recherche séquentielle dans le bloc (arrêt si dépassement)
        for (int i = 0; i < bloc.nmbE; i++) {
            if (bloc.ENREG[i].ID == ID && !bloc.ENREG[i].supprime) { // Ignorer les enregistrements supprimés
                if (blocTrouve) *blocTrouve = pos; // Stocker la position du bloc
                if (posTrouve) *posTrouve = i; // Stocker la position dans le bloc
                fclose(fichier);
                return 1; // Enregistrement trouvé
            } else if (bloc.ENREG[i].ID > ID) { 
                // Puisque le bloc est trié, on peut arrêter la recherche ici
                fclose(fichier);
                return 0; // Enregistrement introuvable
            }
        }

        pos = bloc.suivant; // Passer au bloc suivant
    }

    fclose(fichier);
    return 0; // Enregistrement non trouvé
}








void defragmentation() {
    char name_file[50];
    printf("Entrer le nom du fichier à défragmenter : \n");
    scanf("%s", name_file);
    
    for(int i = 0; i < file_count; i++) {
        if(strcmp(files[i].meta.nom_fichier, name_file) == 0) {
            
            META meta;
            BLOC bloc, blocDest, buffer;
            int posBlocDest = -1; // Position du bloc de destination

            // Lire les métadonnées
            fseek(files[i], 0, SEEK_SET);
            fread(&meta, sizeof(META), 1, files[i]);

            int posBloc = meta.adresse_premier_bloc;

            // Parcourir les blocs du fichier
            while (posBloc != -1) {
                fseek(files[i], posBloc * sizeof(BLOC), SEEK_SET);
                fread(&bloc, sizeof(BLOC), 1, files[i]);

                // Rechercher des enregistrements valides et déplacer les enregistrements valides dans les blocs contigus
                for (int j = 0; j < bloc.nmbE; j++) {
                    if (!bloc.ENREG[j].supprime) {
                        // Si le bloc de destination est trouvé, déplacer l'enregistrement
                        if (posBlocDest == -1) {
                            // Trouver le premier bloc vide pour stocker les enregistrements
                            posBlocDest = posBloc;
                        }

                        // Lire le bloc de destination et ajouter l'enregistrement
                        fseek(files[i], posBlocDest * sizeof(BLOC), SEEK_SET);
                        fread(&blocDest, sizeof(BLOC), 1, files[i]);

                        // Si le bloc de destination a de la place, on déplace l'enregistrement
                        if (blocDest.nmbE < FB) {
                            blocDest.ENREG[blocDest.nmbE++] = bloc.ENREG[j];
                            fseek(files[i], posBlocDest * sizeof(BLOC), SEEK_SET);
                            fwrite(&blocDest, sizeof(BLOC), 1, files[i]);
                        } else {
                            // Si le bloc est plein, passer au bloc suivant
                            posBlocDest = blocDest.suivant;
                            if (posBlocDest == -1) {
                                printf("Erreur: Pas de place disponible pour la défragmentation.\n");
                                return;
                            }
                            j--; // Réessayer d'insérer l'enregistrement dans le bloc suivant
                        }
                    }
                }
                // Passer au bloc suivant
                posBloc = bloc.suivant;
            }

            // Après la réorganisation, mettre à jour les métadonnées pour refléter les changements
            MAJMeta(files[i], 2, &meta.adresse_premier_bloc);
            printf("Défragmentation terminée.\n");
        }
    }
}












int main() {
    int choix;
    do {
        printf(" --- Le Menu Principal ---  \n\n");
        printf("1- Initialiser la Memoire Secondaire \n");
        printf("2- Cr%cer un fichier et le charger en MS \n",130);
        printf("3- Afficher l'%ctat de la M%cmoire Secondaire \n", 130, 130);
        printf("4- Afficher les m%ctadonn%ces des fichiers \n", 130, 130);
        printf("5- Rechercher un enregistrement dans un fichier \n");
        printf("6- Ins%crer un enregistrement dans un fichier \n", 130);
        printf("7- Supprimer un enregistrement d'un fichier \n"); // logique ou physique
        printf("8- D%cfragmenter un fichier \n", 130);
        printf("9- Supprimer un fichier \n");
        printf("10- Renommer un fichier \n");
        printf("11- Compactage de la MS \n");
        printf("12- Vider la MS \n");
        printf("13- Quitter le programme \n");
        printf("\n \nVeuiller saisir votre choix : \n");
        scanf("%d", &choix);
        
        switch (choix) {
            case 1: // initialisation de la MS
                initialize_disk();
                break;
            case 2: // création d'un fichier
                creer_fichier();
                break;
            case 3: // affichage de l'état de la mémoire secondaire
                display_allocation_table();
                break;
            case 4: // affichage des métadonnées des fichiers
                display_files();
                break;
            case 5: // recherche d'un enregistrement
                printf("Entrer l'id de l'enregistrement recherché :\n");
                int id;
                scanf("%d", &id);
                for (int i = 0; i < file_count; i++) {
                    if (strcmp(files[i].meta.nom_fichier, name_file) == 0) {
                        if (files[i].meta.organisation_globale == 1) {
                            if (files[i].meta.organisation_interne == 1) {
                                // contigu trié
                                recherche_contigue_triee(id, NULL, NULL);
                            } else {
                                // contigu non trié
                                recherche_contigue_non_triee(id, NULL, NULL);
                            }
                        } else { 
                            if (files[i].meta.organisation_interne == 1) {
                                // chainée triée
                                rechercheChaineeTriee(id, NULL, NULL);
                            } else {
                                // chainée non triée
                                rechercheChaineeNonTrie(id, NULL, NULL);
                            }
                        }
                    }
                }
                break;
            case 6: // insertion d'un enregistrement
                printf("Entrer les détails de l'enregistrement :\n");
                ENREGISTREMENT enreg;
                // Remplir les détails de l'enregistrement
                // ...
                if (files[i].meta.organisation_globale == 1) {
                    if (files[i].meta.organisation_interne == 1) {
                        // contigu trié
                        insertion_contigue_triee(enreg);
                    } else {
                        // contigu non trié
                        insertion_contigue_non_triee(enreg);
                    }
                } else { 
                    if (files[i].meta.organisation_interne == 1) {
                        // chainée triée
                        insertion_chainee_triee(enreg);
                    } else {
                        // chainée non triée
                        insertionChaineeNonTrie(enreg);
                    }
                }
                break;
            case 7: // suppression d'un enregistrement
                int identifiant;
                printf("Entrer l'id de l'enregistrement à supprimer :\n");
                scanf("%d", &identifiant);
                printf("Choisissez un type de suppression : (1- logique    2- physique)");
                int type;
                scanf("%d", &type);
                if (type == 1) {
                    // suppression logique
                    suppression_logique(identifiant);
                } else if (type == 2) {
                    // suppression physique
                    suppression_physique(identifiant);
                } else {
                    printf("Choix invalide !");
                }
                break;
            case 8: // défragmentation
                defragmentation();
                break;
            case 9: // suppression d'un fichier
                char file_to_delete[50];
                printf("Entrer le nom du fichier à supprimer : \n");
                scanf("%49s", file_to_delete);
                delete_file(file_to_delete);
                break;
            case 10: // renommer un fichier
                renommer_fichier();
                break;
            case 11: // compactage de la MS
                compact();
                break;
            case 12: // vider la MS
                clear_disk();
                break;
            default:
                printf("Choix invalide !");
                break;
        }
    } while (choix != 13);
    
    return 0;
}
