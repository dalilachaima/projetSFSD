
typedef struct Meta Meta;
struct Meta {
    char nom_fichier[50];
    int taille_blocs;
    int taille_enregistrements;
    int adresse_premier_bloc;
    char organisation_globale[10];
    char organisation_interne[10];
};






void CreerFichierMeta(FILE *f, char *nom, int tailleBlocs, int tailleEnregs, int adresse, char *orgGlobale, char *orgInterne) {
    FICHIER_DONNEES meta;
    strcpy(meta.nom_fichier, nom);
    meta.nmbBloc = tailleBlocs;
    meta.nmbEnreg = tailleEnregs;
    meta.modeGlobale = adresse;
    strcpy(meta.modeGlobale, orgGlobale);
    strcpy(meta.modeInterne, orgInterne);
    fwrite(&meta, sizeof(FICHIER_DONNEES), 1, f);
}


void MAJMeta(FILE *f, int nc, void *v) {
    rewind(f);
    switch (nc) {
        case 1: fseek(f, offsetof(FICHIER_DONNEES, nom_fichier), SEEK_SET); fwrite(v, sizeof(char[100]), 1, f); break;
        case 2: fseek(f, offsetof(FICHIER_DONNEES, nmbEnreg), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 3: fseek(f, offsetof(FICHIER_DONNEES, nmbBloc), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 4: fseek(f, offsetof(FICHIER_DONNEES, modeGlobale), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        case 5: fseek(f, offsetof(FICHIER_DONNEES, modeInterne), SEEK_SET); fwrite(v, sizeof(int), 1, f); break;
        default: printf("Option non valide\n"); break;
    }
}



void AfficherMeta(FICHIER_DONNEES meta) {
    printf("Nom du fichier : %s\n", meta.nom_fichier);
    printf("Taille en blocs : %d\n", meta.nmbBloc);
    printf("Taille en enregistrements : %d\n", meta.nmbEnreg);
    printf("Mode d'organisation globale : %d\n", meta.modeGlobale);
    printf("Mode d'organisation interne : %d\n", meta.modeInterne);
}

void LireMeta(FILE *f, FICHIER_DONNEES *meta) {
    rewind(f);
    fread(meta, sizeof(FICHIER_DONNEES), 1, f);
}


