#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_BLOCS 1000
#define FB 100
#define nbrMaxFichiers 500

typedef struct ENREGISTREMENT ENREGISTREMENT;
struct ENREGISTREMENT {
    int ID;
    char champs[60];
    int deleted;
};

typedef struct {
    int is_occupied;
    int nbrEnreg;
    int next_bloc; // adresse bloc suivant
    ENREGISTREMENT enreg[FB];
    int adresse_bloc; 
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

int total_blocks;
int block_size;
int file_count = 0;
Bloc* allocation_table; 
File* files;
void initialize_disk() {
    total_blocks = MAX_BLOCS;
    block_size = FB + 3;
    for (int i = 0; i < total_blocks; i++) {
        allocation_table[i].is_occupied = 0;
    }
    allocation_table[1].is_occupied = 1;
    file_count = 0;
    printf("Disque initialisé.\n");
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
    printf("Compactage du disque...\n");
    int next_free_index = 0;
    for (int i = 0; i < file_count; i++) {
        File *file = &files[i];
        int new_blocks[file->meta.taille_blocs];

        for (int j = 0; j < file->meta.taille_blocs; j++) {
            while (allocation_table[next_free_index].is_occupied) {
                next_free_index++;
            }
            new_blocks[j] = next_free_index;
            allocation_table[next_free_index].is_occupied = 1;
            next_free_index++;
        }

        // Modification ici :
        for (int j = 0; j < file->meta.taille_blocs; j++) {
            allocation_table[file->blocs[j].adresse_bloc].is_occupied = 0;
        }

        memcpy(file->blocs, new_blocks, sizeof(new_blocks));
    }
    printf("Disque compacté.\n");
}

void delete_file(const char *file_name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].meta.nom_fichier, file_name) == 0) {
            for (int j = 0; j < files[i].meta.taille_blocs; j++) {
                allocation_table[files[i].blocs[j].adresse_bloc].is_occupied = 0;
            }
            for (int k = i; k < file_count - 1; k++) {
                files[k] = files[k + 1];
            }
            file_count--;
            printf("Fichier '%s' supprimé.\n", file_name);
            return;
        }
    }
    printf("Erreur : Fichier '%s' introuvable.\n", file_name);
}

void clear_disk() {
    initialize_disk();
    printf("Disque vidé.\n");
}

void display_allocation_table() {
    printf("Table d'allocation :\n");
    for (int i = 0; i < MAX_BLOCS; i++) {
        if (allocation_table[i].is_occupied == 0) {
            printf("| Libre ");
        } else {
            printf("| %s : %d blocs ", files[i].meta.nom_fichier, files[i].meta.taille_blocs);
        }
    }
    printf(" |\n");
}

void display_files() {
    printf("Fichiers sur le disque :\n");
    for (int i = 0; i < file_count; i++) {
        printf("________________________________________________________________________________________________________________________\n");
        char orgGlo[20], orgInt[20];
        if (files[i].meta.organisation_globale == 1) {
            strcpy(orgGlo, "contigu ");
        } else {
            strcpy(orgGlo, "chaîne ");
        }
        if (files[i].meta.organisation_interne == 1) {
            strcpy(orgInt, "trié ");
        } else {
            strcpy(orgInt, "non trié ");
        }
        printf("|Nom: %s, adresse premier bloc : %d, taille en blocs : %d, taille en enregistrements : %d, organisation globale : %s, organisation interne : %s |\n",
               files[i].meta.nom_fichier, files[i].meta.adresse_premier_bloc, files[i].meta.taille_blocs, files[i].meta.taille_enregistrements, orgGlo, orgInt);
        printf("________________________________________________________________________________________________________________________\n");
    }
}

void MAJMeta(FILE *f, int nc, void *v) {
    META meta;
    rewind(f);
    switch (nc) {
        case 1: fseek(f, offsetof(META, nom_fichier), SEEK_SET); fwrite(v, sizeof(char[50]), 1, f); break;
        case 2: fseek(f, offsetof(META, adresse_premier_bloc), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 3: fseek(f, offsetof(META, taille_enregistrements), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 4: fseek(f, offsetof(META, taille_blocs), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 5: fseek(f, offsetof(META, organisation_globale), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 6: fseek(f, offsetof(META, organisation_interne), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        default: printf("Option non valide\n"); break;
    }
}

void creer_fichier() {
    //creation des meta du fichier
    META meta;
    printf("Création d'un fichier :\n");
    printf("Nom du fichier : ");
    scanf("%s", meta.nom_fichier);
    printf("Nombre d'enregistrements : ");
    scanf("%d", &meta.taille_enregistrements);
    printf("Mode d'organisation globale (1: contigu, 2: chaîne) : ");
    scanf("%d", &meta.organisation_globale);
    printf("Mode d'organisation interne (1: trié, 2: non trié) : ");
    scanf("%d", &meta.organisation_interne);

    meta.taille_blocs = (meta.taille_enregistrements + FB - 1) / FB;
    meta.adresse_premier_bloc = -1;

    if (!check_free_space(meta.taille_blocs)) {
        printf("Erreur : Espace insuffisant pour créer le fichier.\n");
        return;
    }

    FILE *fichier = fopen(meta.nom_fichier, "wb");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }
// ajout du fichier à la liste des fichiers
    for (int i = 0; i < nbrMaxFichiers; i++) {
        if (files[i].meta.nom_fichier[0] == '\0') {
            files[i].meta = meta;
            MAJMeta(fichier, 1, &meta.nom_fichier);
            MAJMeta(fichier, 2, &meta.adresse_premier_bloc);
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
            MAJMeta(fichier, 4, &meta.taille_blocs);
            MAJMeta(fichier, 5, &meta.organisation_globale);
            MAJMeta(fichier, 6, &meta.organisation_interne);
            printf("Fichier créé avec succès !");
            return;
        }
    }
// allocation des blocs
    Bloc buffer = {0}; // initialisation d'un bloc vide
    int adresse_bloc_precedent = -1; // variable pour chainer les blocs

    for (int i = 0; i < meta.taille_blocs; i++) {
        // pour le mode chaîné
        if (meta.organisation_globale == 2) {
            buffer.next_bloc = (i < meta.taille_blocs - 1) ? (meta.adresse_premier_bloc + i + 1) : -1;
        } else {
            buffer.next_bloc = -1; // pour le mode contigu
        }
// si c'est le premier bloc, on le place dans le fichier et on initialise son adresse 
        if (i == 0) {
            meta.adresse_premier_bloc = ftell(fichier) / sizeof(Bloc); //adresse du premier bloc 
        }
        allocation_table[i].is_occupied = 1;
        // ecriture du bloc dans le fichier
        fwrite(&buffer, sizeof(Bloc), 1, fichier);
//mise à jour du bloc precedant pour chainer les blocs si necessaire
        if (meta.organisation_globale == 2) {
            if (adresse_bloc_precedent != -1) {
                fseek(fichier, adresse_bloc_precedent * sizeof(Bloc), SEEK_SET);
                fread(&buffer, sizeof(Bloc), 1, fichier);
                buffer.next_bloc = meta.adresse_premier_bloc + i;
                fseek(fichier, adresse_bloc_precedent * sizeof(Bloc), SEEK_SET);
                fwrite(&buffer, sizeof(Bloc), 1, fichier);
            }
            adresse_bloc_precedent = ftell(fichier) / sizeof(Bloc) - 1; ..mise à jour de l'adresse du bloc pecedant 
        }
    }

    printf("Fichier créé avec succès !\n");
    fclose(fichier);
}
// fonction pour renommer un fichier
void renommer_fichier() {
    char ancien_nom[50], nouveau_nom[50];
    int fichier_trouve = 0;

    printf("Entrez le nom du fichier a renommer : ");
    scanf("%s", ancien_nom);
//recherche du fichier dans le tableau
    for (int i = 0; i < nbrMaxFichiers; i++) {
        if (strcmp(files[i].meta.nom_fichier, ancien_nom) == 0) {
            //si le fichier est trouvé, demande le nouveau nom
            printf("Entrez le nouveau nom du fichier : ");
            scanf("%s", nouveau_nom);
// verifie si le nouveau nom est deja pris
            int nom_deja_pris = 0;
            for (int j = 0; j < nbrMaxFichiers; j++) {
                if (strcmp(files[j].meta.nom_fichier, nouveau_nom) == 0) {
                    nom_deja_pris = 1;
                    break;
                }
            }
//mise a jour du mon du fichier dans les meta
            if (nom_deja_pris) {
                printf("Erreur : Le nom '%s' est déjà utilisé pour un autre fichier.\n", nouveau_nom);
                return;
            }

            strcpy(files[i].meta.nom_fichier, nouveau_nom);
            MAJMeta(fopen(ancien_nom, "rb+"), 1, &nouveau_nom);
            printf("Le fichier a été renommé avec succès en '%s'.\n", nouveau_nom);
            fichier_trouve = 1;
            break;
        }
    }

    if (!fichier_trouve) {
        printf("Erreur : Fichier '%s' non trouvé.\n", ancien_nom);
    }
}
//fonction pour la suppression logique d'un enregistrement 
void suppression_logique(int id) {

int recherche_contigue_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc);
int recherche_contigue_non_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc);
int rechercheChaineeTriee(FILE *fichier, int id, int *blocPosition, int *posDansBloc);
int rechercheChaineeNonTrie(FILE *fichier, int id, int *blocPosition, int *posDansBloc);

    
    int posBloc = -1, posEnreg = -1;
    char nom_fichier[50];

    
    printf("Entrer le nom du fichier :\n");
    scanf("%49s", nom_fichier);

    
    FILE *fichier = fopen(nom_fichier, "r+");
    if (fichier == NULL) {
    
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    META meta;
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

   // recherche de l'enregistrement avec l'id donné
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].meta.nom_fichier, nom_fichier) == 0) {
            meta = files[i].meta;

            if (meta.organisation_globale == 1) { 
                if (meta.organisation_interne == 1) { 
                    if (recherche_contigue_triee(fichier, id, &posBloc, &posEnreg)) {
                        break;
                    }
                } else { 
                    if (recherche_contigue_non_triee(fichier, id, &posBloc, &posEnreg)) {
                        break;
                    }
                }
            } else { 
                if (meta.organisation_interne == 1) {
                    if (rechercheChaineeTriee(fichier, id, &posBloc, &posEnreg)) {
                        break;
                    }
                } else { 
                    if (rechercheChaineeNonTrie(fichier, id, &posBloc, &posEnreg)) {
                        break;
                    }
                }
            }
        }
    }

   
    if (posBloc != -1 && posEnreg != -1) {
        Bloc bloc;

    // aller à la position du bloc et lire le bloc 
        fseek(fichier, posBloc * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);
// verifie si l'enregistrement existe et lemarquer comme supprimé
        if (bloc.enreg[posEnreg].ID == id) {
            
            bloc.enreg[posEnreg].deleted = 1; // marquer comme supprimé

            
            fseek(fichier, posBloc * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc, sizeof(Bloc), 1, fichier);

            printf("Enregistrement ID %d supprimé logiquement.\n", id);

           //mise à jour des métadonnées 
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
        } else {
            printf("Enregistrement ID %d non trouvé dans le bloc.\n", id);
        }
    } else {
        printf("Enregistrement ID %d non trouvé.\n", id);
    }

   
    fclose(fichier);
}


int recherche_contigue_non_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc);

void suppression_physique(int id) {
    int posBloc = -1, posEnreg = -1;
    char nom_fichier[50];

    printf("Entrer le nom du fichier :\n");
    scanf("%49s", nom_fichier);

    FILE *fichier = fopen(nom_fichier, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    if (!recherche_contigue_non_triee(fichier, id, &posBloc, &posEnreg)) {
        printf("Enregistrement ID %d non trouvé.\n", id);
        fclose(fichier);
        return;
    }
//rechercher l'enregistrement avec l'id donné
    Bloc bloc, bloc_suivant;
// lire le bloc contenant l'enregistrement 
    fseek(fichier, posBloc * sizeof(Bloc), SEEK_SET);
    fread(&bloc, sizeof(Bloc), 1, fichier);
// supprimer l'enregistrement en decalant les autres enregistrements 
    for (int i = posEnreg; i < bloc.nbrEnreg - 1; i++) {
        bloc.enreg[i] = bloc.enreg[i + 1];
    }
    bloc.nbrEnreg--; // reduire le nombre d'enregistrement dans le bloc 
//reecrire le bloc modifié
    fseek(fichier, posBloc * sizeof(Bloc), SEEK_SET);
    fwrite(&bloc, sizeof(Bloc), 1, fichier);
// si le bloc devient vide, supprimer le bloc et réorganisation des liens si le fichier est chaînée 
    if (bloc.nbrEnreg == 0) {
        int bloc_precedent = -1, bloc_courant = -1;
        // trouver le bloc precedant pour mettre à jour le lien
        for (int i = 0; i < file_count; i++) {
            if (strcmp(files[i].meta.nom_fichier, nom_fichier) == 0) {
                bloc_courant = files[i].meta.adresse_premier_bloc;
                break;
            }
        }

        while (bloc_courant != posBloc) {
            bloc_precedent = bloc_courant;
            fseek(fichier, bloc_courant * sizeof(Bloc), SEEK_SET);
            fread(&bloc_suivant, sizeof(Bloc), 1, fichier);
            bloc_courant = bloc_suivant.next_bloc;
        }
//reorganiser les liens dans le cas des fichiers chainees
        if (bloc_precedent != -1) {
            fseek(fichier, bloc_precedent * sizeof(Bloc), SEEK_SET);
            fread(&bloc_suivant, sizeof(Bloc), 1, fichier);
            bloc_suivant.next_bloc = bloc.next_bloc;
            fseek(fichier, bloc_precedent * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc_suivant, sizeof(Bloc), 1, fichier);
        } else {
            // si c'est le premier bloc, mettre à jour l'adresse du premier bloc dans les metadonnees 
            for (int i = 0; i < file_count; i++) {
                if (strcmp(files[i].meta.nom_fichier, nom_fichier) == 0) {
                    files[i].meta.adresse_premier_bloc = bloc.next_bloc;
                    MAJMeta(fichier, 2, &files[i].meta.adresse_premier_bloc);
                    break;
                }
            }
        }

        //reinitialiser le bloc supprimé
        allocation_table[posBloc].is_occupied = 0;
    }

    printf("Enregistrement ID %d supprimé physiquement.\n", id);
    fclose(fichier);
}

// insertion chainee non trié 
void insertionChaineeNonTrie(ENREGISTREMENT enreg) {
    char nom[50];
    printf("Entrer le nom du fichier où mettre l'enregistrement :\n");
    scanf("%49s", nom);
    FILE *fichier = fopen(nom, "r+");
    META meta;
    Bloc bloc;
    fseek(fichier, 0, SEEK_SET);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;
    while (pos != -1) {
        fseek(fichier, pos * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        if (bloc.nbrEnreg < FB) { // inserer dns le bloc 
            bloc.enreg[bloc.nbrEnreg++] = enreg;
            fseek(fichier, pos * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc, sizeof(Bloc), 1, fichier);
            printf("Enregistrement inséré avec succès.\n");
            //mise a jour des métadonnées 
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
            fclose(fichier);
            return;
        }
        pos = bloc.next_bloc;
    }

    printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
    fclose(fichier);
}

void insertion_chainee_triee(ENREGISTREMENT enreg) {
    char nom[50];
    printf("Entrer le nom du fichier où mettre l'enregistrement :\n");
    scanf("%49s", nom);
    FILE *fichier = fopen(nom, "r+");

    META meta;
    Bloc bloc, nouveau_bloc;
    int prevBloc = -1, currBloc;

    // lire les métadonnées 
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

        currBloc = meta.adresse_premier_bloc;
    // Parcourir les blocs pour trouver la position d'insertion
    while (currBloc != -1) {
        fseek(fichier, currBloc * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        // Chercher la position dans le bloc où insérer
        int i = 0;
        while (i < bloc.nbrEnreg && bloc.enreg[i].ID < enreg.ID) {
            i++;
        }

        // Si le bloc a de l'espace, insérer ici
        if (bloc.nbrEnreg < FB) {
            //decaler les enregistrements pour faire de la place 
            for (int j = bloc.nbrEnreg; j > i; j--) {
                bloc.enreg[j] = bloc.enreg[j - 1];
            }
            bloc.enreg[i] = enreg; // inserer le nouvel enregistrement 
            bloc.nbrEnreg++;

            // reecrire le bloc dans le fichier 
            fseek(fichier, currBloc * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc, sizeof(Bloc), 1, fichier);
            printf("Enregistrement inséré avec succès (chaîné trié).\n");
            //mise a jour des métadonnées 
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
            fclose(fichier);
            return;
        }

        //passer au bloc suivant 
        prevBloc = currBloc;
        currBloc = bloc.next_bloc;
    }

    // si aucun espace libre trouvé, créer un nouveau bloc 
    int newBlocIndex = -1;
    for (int i = 0; i < total_blocks; i++) {
        if (!allocation_table[i].is_occupied) {
            newBlocIndex = i;
            allocation_table[i].is_occupied = 1;
            break;
        }
    }

    // si aucun bloc trouvé, creer un nouveau bloc 
    if (newBlocIndex == -1) {
        printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
        fclose(fichier);
        return;
    }

    //initialiser un nouveau bloc 
    nouveau_bloc.nbrEnreg = 1;
    nouveau_bloc.enreg[0] = enreg;
    nouveau_bloc.next_bloc = -1;

    // lier le nouveau bloc au precedant 
    if (prevBloc != -1) {
        fseek(fichier, prevBloc * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);
        bloc.next_bloc = newBlocIndex;
        fseek(fichier, prevBloc * sizeof(Bloc), SEEK_SET);
        fwrite(&bloc, sizeof(Bloc), 1, fichier);
    } else {
        //si c'est le premier bloc 
        meta.adresse_premier_bloc = newBlocIndex;
        rewind(fichier);
        fwrite(&meta, sizeof(META), 1, fichier);
    }

    //creer le nouveau bloc dns le fichier 
    fseek(fichier, newBlocIndex * sizeof(Bloc), SEEK_SET);
    fwrite(&nouveau_bloc, sizeof(Bloc), 1, fichier);

    printf("Enregistrement inséré dans un nouveau bloc (chaîné trié).\n");
    fclose(fichier);
}

void insertion_contigue_triee(ENREGISTREMENT enreg) {
    char nom[50];
    printf("Entrer le nom du fichier où mettre l'enregistrement :\n");
    scanf("%49s", nom);
    FILE *fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    META meta;
    Bloc bloc;
    int blocPosition;
    int enregPosition;

    // lire les métadonnées 
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    // verifier si le fichier est plein 
    if (meta.taille_enregistrements == meta.taille_blocs * FB) {
        printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
        fclose(fichier);
        return;
    }

    //parcourir les blocs pour trouver un espace disponible
    for (blocPosition = meta.adresse_premier_bloc; blocPosition < meta.adresse_premier_bloc + meta.taille_blocs; blocPosition++) {
        fseek(fichier, blocPosition * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        for (enregPosition = 0; enregPosition < bloc.nbrEnreg; enregPosition++) {
            if (bloc.enreg[enregPosition].ID > enreg.ID) {
                break;
            }
        }

        //verifier si le bloc a de l'espace 
        if (bloc.nbrEnreg < FB) {
            for (int j = bloc.nbrEnreg; j > enregPosition; j--) {
                bloc.enreg[j] = bloc.enreg[j - 1];
            }
            bloc.enreg[enregPosition] = enreg;
            bloc.nbrEnreg++;

            fseek(fichier, blocPosition * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc, sizeof(Bloc), 1, fichier);

            //mettre a jour les métadonnées 
            meta.taille_enregistrements++;
            rewind(fichier);
            fwrite(&meta, sizeof(META), 1, fichier);
            //mise a jour des métadonnées 
            MAJMeta(fichier, 3, &meta.taille_enregistrements);
            printf("Enregistrement inséré avec succès (contigu trié).\n");
            fclose(fichier);
            return;
        }
    }
// si tout les blocs existant sont pleins, allouer un nouveau bloc 
    if (meta.taille_blocs < MAX_BLOCS) {
        bloc.nbrEnreg = 1;
        bloc.enreg[0] = enreg;

        fseek(fichier, (meta.adresse_premier_bloc + meta.taille_blocs) * sizeof(Bloc), SEEK_SET);
        fwrite(&bloc, sizeof(Bloc), 1, fichier);

        meta.taille_blocs++;
        meta.taille_enregistrements++;
        rewind(fichier);
        fwrite(&meta, sizeof(META), 1, fichier);
        // mise a jour des métadonnées 
        MAJMeta(fichier, 3, &meta.taille_enregistrements);
        printf("Enregistrement inséré dans un nouveau bloc (contigu trié).\n");
    } else {
        printf("Erreur : Impossible d'allouer un nouveau bloc.\n");
    }
    fclose(fichier);
}

void insertion_contigue_non_triee(ENREGISTREMENT enreg) {
    char nom[50];
    printf("Entrer le nom du fichier où mettre l'enregistrement :\n");
    scanf("%49s", nom);
    FILE *fichier = fopen(nom, "r+");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    META meta;
    Bloc bloc;
    int blocPosition;

    //lire les métadonnée 
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    if (meta.taille_enregistrements == meta.taille_blocs * FB) {
        printf("Erreur : Aucun espace disponible pour insérer l'enregistrement.\n");
        fclose(fichier);
        return;
    }

    for (blocPosition = meta.adresse_premier_bloc; blocPosition < meta.adresse_premier_bloc + meta.taille_blocs; blocPosition++) {
        fseek(fichier, blocPosition * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        if (bloc.nbrEnreg < FB) {
            bloc.enreg[bloc.nbrEnreg++] = enreg;
            fseek(fichier, blocPosition * sizeof(Bloc), SEEK_SET);
            fwrite(&bloc, sizeof(Bloc), 1, fichier);

            meta.taille_enregistrements++;
            rewind(fichier);
            fwrite(&meta, sizeof(META), 1, fichier);
            MAJMeta(fichier, 3, &meta.taille_enregistrements);

            printf("Enregistrement inséré avec succès (contigu non trié).\n");
            fclose(fichier);
            return;
        }
    }

    if (meta.taille_blocs < MAX_BLOCS) {
        bloc.nbrEnreg = 1;
        bloc.enreg[0] = enreg;

        fseek(fichier, (meta.adresse_premier_bloc + meta.taille_blocs) * sizeof(Bloc), SEEK_SET);
        fwrite(&bloc, sizeof(Bloc), 1, fichier);

        meta.taille_blocs++;
        meta.taille_enregistrements++;
        rewind(fichier);
        fwrite(&meta, sizeof(META), 1, fichier);

        MAJMeta(fichier, 3, &meta.taille_enregistrements);
        printf("Enregistrement inséré dans un nouveau bloc (contigu non trié).\n");
    } else {
        printf("Erreur : Impossible d'allouer un nouveau bloc.\n");
    }
    fclose(fichier);
}

int recherche_contigue_non_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc) {
    META meta;
    Bloc bloc;

    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    // parcourir tous les blocs du fichier
    for (int i = 0; i < meta.taille_blocs; i++) {
        // charger le bloc actuel
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        //parcourir les enregistrements du bloc 
        for (int j = 0; j < bloc.nbrEnreg; j++) {
            if (!bloc.enreg[j].deleted && bloc.enreg[j].ID == id) {
                // enregistrement trouvé
                *blocPosition = i;
                *posDansBloc = j;
                return 1; // succes 
            }
        }
    }

    //enregistrement non trouvé
    *blocPosition = -1;
    *posDansBloc = -1;
    return 0; // échec 
}

int recherche_contigue_triee(FILE *fichier, int id, int *blocPosition, int *posDansBloc) {
    META meta;
    Bloc bloc;

    //lire les métadonnées
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    //parcourir tous ls blocs du fichier 
    for (int i = 0; i < meta.taille_blocs; i++) {
        //carger le bloc actuel
        fseek(fichier, (meta.adresse_premier_bloc + i) * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        //effectuer une recherche binaire dans le bloc 
        int gauche = 0, droite = bloc.nbrEnreg - 1;
        while (gauche <= droite) {
            int milieu = (gauche + droite) / 2;
            if (bloc.enreg[milieu].ID == id && !bloc.enreg[milieu].deleted) {
                // enregistrement trouvé
                *blocPosition = i;
                *posDansBloc = milieu;
                return 1; //success
            } else if (bloc.enreg[milieu].ID < id) {
                gauche = milieu + 1;
            } else {
                droite = milieu - 1;
            }
        }
    }

    // enregistreent non trouvé
    *blocPosition = -1;
    *posDansBloc = -1;
    return 0; // échec 
}

int rechercheChaineeNonTrie(FILE *fichier, int ID, int *blocTrouve, int *posTrouve) {
    META meta;
    Bloc bloc;

    //lire les métadonnées 
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;

    while (pos != -1) {
        //lire le bloc courant 
        fseek(fichier, pos * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        //recherche sequentielle dans le bloc 
        for (int i = 0; i < bloc.nbrEnreg; i++) {
            if (bloc.enreg[i].ID == ID && !bloc.enreg[i].deleted) { // ignorer les enregistrement supprimés 
                if (blocTrouve) *blocTrouve = pos; // stocker la position du bloc 
                if (posTrouve) *posTrouve = i; // stocker la position dans le bloc 
                return 1; // enregistrement trouvé
            }
        }

        pos = bloc.next_bloc; // passer au bloc suivant 
    }

    return 0; // enregistrement non trouvé
}

int rechercheChaineeTriee(FILE *fichier, int id, int *blocPosition, int *posDansBloc) {
    char nom[50];
    META meta;
    Bloc bloc;

    // lire les métadonnées
    rewind(fichier);
    fread(&meta, sizeof(META), 1, fichier);

    int pos = meta.adresse_premier_bloc;

    while (pos != -1) {
        //lire le bloc courant 
        fseek(fichier, pos * sizeof(Bloc), SEEK_SET);
        fread(&bloc, sizeof(Bloc), 1, fichier);

        // recherche sequentielle dans le bloc (arrêt si dépassement )
        for (int i = 0; i < bloc.nbrEnreg; i++) {
            if (bloc.enreg[i].ID == id && !bloc.enreg[i].deleted) { // ignorer les enregistrements supprimés 
                if (blocPosition) *blocPosition = pos; // stocker la position du bloc 
                if (posDansBloc) *posDansBloc = i; // stocker la position dans le bloc 
                return 1; // enregistreent trouvé
            } else if (bloc.enreg[i].ID > id) {
                // puisque le bloc est trié, on peut arreter la rechrche ici 
                return 0; // enregistrement introuvable 
            }
        }
        pos = bloc.next_bloc; // passer au bloc suivant 
    }
    return 0; // enregistrement non trouvé
}

void defragmentation() {
    char name_file[50];
    printf("Entrer le nom du fichier à défragmenter :\n");
    scanf("%s", name_file);

    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].meta.nom_fichier, name_file) == 0) {
            FILE *fichier = fopen(name_file, "r+");
            if (fichier == NULL) {
                printf("Erreur lors de l'ouverture du fichier.\n");
                return;
            }

            META meta = files[i].meta;
            Bloc bloc, blocDest;

            int posBloc = meta.adresse_premier_bloc;
            int posBlocDest = meta.adresse_premier_bloc; // position de bloc de destination 

            // parcourir les blocs du fichier 
            while (posBloc != -1) {
                fseek(fichier, posBloc * sizeof(Bloc), SEEK_SET);
                fread(&bloc, sizeof(Bloc), 1, fichier);

                // rechercher des enregistrements valides, et deplacer les enregistrements valides dans les blocs contigus
                for (int j = 0; j < bloc.nbrEnreg; j++) {
                    if (!bloc.enreg[j].deleted) {
                        //si le bloc de destination est trouvé, deplacer l'enregistrement 
                        if (posBlocDest == -1) {
                            // trouver le premier bloc vide pour stocker les enregistrements 
                            posBlocDest = posBloc;
                        }
                        // lire le bloc de destination et ajouter l'enregistrement 
                        fseek(fichier, posBlocDest * sizeof(Bloc), SEEK_SET);
                        fread(&blocDest, sizeof(Bloc), 1, fichier);

                        // si le bloc de destination a de la place, on deplace l'enregistrement 
                        if (blocDest.nbrEnreg < FB) {
                            blocDest.enreg[blocDest.nbrEnreg++] = bloc.enreg[j];
                            fseek(fichier, posBlocDest * sizeof(Bloc), SEEK_SET);
                            fwrite(&blocDest, sizeof(Bloc), 1, fichier);
                        } else {
                            // si le bloc est plein, passer au bloc suivant 
                            posBlocDest = blocDest.next_bloc;
                            if (posBlocDest == -1) {
                                printf("Erreur : Pas de place disponible pour la défragmentation.\n");
                                fclose(fichier);
                                return;
                            }
                            j--; // réessayer d'inserer l'enregistrement dans le bloc suivant 
                        }
                    }
                }
                // passer au bloc suivant 
                posBloc = bloc.next_bloc;
            }
            // mise a jour des metadonnes pour refléter les changements apres la réorganisation 
            MAJMeta(fichier, 2, &meta.adresse_premier_bloc);
            fclose(fichier);
            printf("Défragmentation terminée.\n");
        }
    }
}


int main() {
    int choix;
    int blocPosition;
    int posDansBloc;

    do {
        printf(" --- Le Menu Principal ---  \n\n");
        printf("1- Initialiser la Memoire Secondaire \n");
        printf("2- Creer un fichier et le charger en MS \n");
        printf("3- Afficher l'etat de la Memoire Secondaire \n");
        printf("4- Afficher les metadonnees des fichiers \n");
        printf("5- Rechercher un enregistrement dans un fichier \n");
        printf("6- Inserer un enregistrement dans un fichier \n");
        printf("7- Supprimer un enregistrement d'un fichier \n"); // logique ou physique
        printf("8- Defragmenter un fichier \n");
        printf("9- Supprimer un fichier \n");
        printf("10- Renommer un fichier \n");
        printf("11- Compactage de la MS \n");
        printf("12- Vider la MS \n");
        printf("13- Quitter le programme \n");
        printf("\n \nVeuillez saisir votre choix : \n");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                // initialiser la MS
                initialize_disk();
                break;
            case 2:
              // Creation d'un fichier 
                creer_fichier();
                break;
            case 3:
           // afficher l'etat de la MS
                display_allocation_table();
                break;
            case 4:
                // afficher les metadonnes des fichiers sous forme d'un tableau 
                display_files();
                break;
            case 5: {
               // recherche d'un enregistrement 
                printf("Entrer l'id de l'enregistrement recherché :\n");
                int id;
                scanf("%d", &id);
                char fichier[50];
                printf("Entrer le nom du fichier à rechercher :\n");
                scanf("%49s", fichier);

                FILE *file = fopen(fichier, "r+");
                if (file == NULL) {
                    printf("Erreur lors de l'ouverture du fichier %s.\n", fichier);
                    break;
                }

                for (int i = 0; i < file_count; i++) {
                    if (strcmp(files[i].meta.nom_fichier, fichier) == 0) {
                        if (files[i].meta.organisation_globale == 1) {
                            if (files[i].meta.organisation_interne == 1) {
                                //contigu trié
                                recherche_contigue_triee(file, id, &blocPosition, &posDansBloc);
                            } else {
                                //contigu non trie
                                recherche_contigue_non_triee(file, id, &blocPosition, &posDansBloc);
                            }
                        } else {
                            if (files[i].meta.organisation_interne == 1) {
                                // chainee trié
                                rechercheChaineeTriee(file, id, &blocPosition, &posDansBloc);
                            } else {
                                // chainée non trié 
                                rechercheChaineeNonTrie(file, id, &blocPosition, &posDansBloc);
                            }
                        }
                    }
                }
                fclose(file);
                break;
            }
            case 6: {
                // insertion d'un enregistrement 
                printf("Entrer les détails de l'enregistrement :\n");
                ENREGISTREMENT enreg;
                printf("ID : ");
                scanf("%d", &enreg.ID);
                printf("Champs : ");
                scanf("%s", enreg.champs);
                enreg.deleted = 0;
                char fichier[50];
                printf("Entrer le nom du fichier pour l'insertion :\n");
                scanf("%49s", fichier);

                for (int i = 0; i < file_count; i++) {
                    if (strcmp(files[i].meta.nom_fichier, fichier) == 0) {
                        if (files[i].meta.organisation_globale == 1) {
                            if (files[i].meta.organisation_interne == 1) {
                                // insertion contigu trié
                                insertion_contigue_triee(enreg);
                            } else {
                                // insertion contigu non trie
                                insertion_contigue_non_triee(enreg);
                            }
                        } else {
                            if (files[i].meta.organisation_interne == 1) {
                                // insertion chaînée triée
                                insertion_chainee_triee(enreg);
                            } else {
                                // insertion chaînée non triée 
                                insertionChaineeNonTrie(enreg);
                            }
                        }
                    }
                }
                break;
            }
            case 7: {
                // suppression d'un enregistrement 
                int identifiant;
                printf("Entrer l'id de l'enregistrement à supprimer :\n");
                scanf("%d", &identifiant);
                printf("Choisissez un type de suppression : (1- logique  2- physique)\n");
                int type;
                scanf("%d", &type);
                if (type == 1) {
                    // suppression logique 
                    suppression_logique(identifiant);
                } else if (type == 2) {
                    // suppression physique 
                    suppression_physique(identifiant);
                } else {
                    printf("Choix invalide !\n");
                }
                break;
            }
            case 8:
              // defragmentation 
                defragmentation();
                break;
            case 9: {
                // suppression d'un fichier 
                char file_to_delete[50];
                printf("Entrer le nom du fichier a supprimer :\n");
                scanf("%49s", file_to_delete);
                delete_file(file_to_delete);
                break;
            }
            case 10:
                //renommer un fichier 
                renommer_fichier();
                break;
            case 11:
               // compacter un fichier
                compact();
                break;
            case 12:
               // vider la MS
                clear_disk();
                break;
            default:
                printf("Choix invalide !\n");
                break;
        }
    } while (choix != 13);

    return 0;
}
