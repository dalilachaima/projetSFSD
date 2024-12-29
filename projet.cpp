// les structures
typedef struct ENREGISTREMENT ENREGISTREMENT;
struct ENREGISTREMENT {
	int ID ;
	char champs[60] ;
	bool supprime  ; 
	int Clé;
	
};
typedef struct BLOC BLOC;
struct BLOC{
	ENREGISTREMENT ENREG[FB] ;  //FB=FACTURE DE BLOCAGE
	int nmbE  ; // nombre d' ENREGISTREMENT DANS LE BLOC
	int etat = 0;
	int suppression=0;
};
typedef struct BUFFER BUFFER ;
struct BUFFER {
  BLOC buffer;
};

//fnct1 : Création_fichier
void Création_fichier(FILE *fichier) {
    printf("donne-moi le nom de fichier:\n");
    scanf("%s", nom_fichier);
    printf("donne-moi le nombre d’enregistrements:\n");
    scanf("%d", &nmbEnreg);
    nmbBloc = (nmbEnreg + (FB - 1)) / FB;
    printf("le nombre des bloc est :%d \n", nmbBloc);
    printf("donne-moi le mode d’organisation Globale: (click 0 pour contigue et 1 pour chainee):\n");
    scanf("%d", &modeGlobale);
    printf("donne-moi le mode d’organisation Interne: (click 0 pour trie et 1 pour non trie ):\n");
    scanf("%d", &modeInterne);
    Ouvrir(fichier, nom_fichier, "wb+");
}

//fnct2 : Allouer_Blocs
void Allouer_Blocs(FILE *fichier, int nmbBloc, BLOC *buffer) {
    memset(buffer, 0, sizeof(BLOC));
    for(int i = 0; i < nmbBloc; i++) {
        AllouerBloc(fichier);
        fseek(fichier, 0, SEEK_END);
        fwrite(buffer, sizeof(BLOC), 1, fichier);
    }
    printf("on a alloué les blocs");
} 

//fnct3' : Insertion contigue non triee 
void insertionContigueNonTriee(FILE *fichier, ENREGISTREMENT e, int *nmbBloc) {
BLOC buffer;
bool insere = false; 
for (int i = 0; i < *nmbBloc; i++) {
	fseek(fichier, i * sizeof(BLOC), SEEK_SET);
	fread(&buffer, sizeof(BLOC), 1, fichier);
	if (buffer.nmbE < FB) { buffer.ENREG[buffer.nmbE] = e; 
         buffer.nmbE++; 
	 fseek(fichier, -sizeof(BLOC), SEEK_CUR);
	fwrite(&buffer, sizeof(BLOC), 1, fichier);
	insere = true; break; } }
        if (!insere) {  printf("Erreur : la mémoire est pleine, impossible d'insérer l'enregistrement.\n"); } }

//fnct3 :Insertion d’un nouvel enregistrement contigue triee
void insertioncontiguetriee(FILE *fichier, ENREGISTREMENT e) {
    bool Trouv, continu;
    int i = 0, j = 0, k;
    ENREGISTREMENT x;
    Ouvrir(fichier, nom_fichier, "a+");
    rechercheDichotomique(fichier, nmbBloc, e.ID, Trouv, &milieu, m);
    if (!Trouv) {
        continu = true;
        while (continu && i <= LireEntete(fichier, (nmbBloc - 1))) {
            LireBloc(fichier, milieu, buffer);
            x = buffer.ENREG[buffer.NE];
            k = buffer.NE;
            while (k > j) {
                buffer.ENREG[k] = buffer.ENREG[k - 1];
                k = k - 1;
            }
            buffer.ENREG[j] = e;
            if (buffer.NE < FB) {
                buffer.NE = buffer.NE + 1;
                buffer.ENREG[buffer.NE] = x;
                EcrireBloc(fichier, milieu, buffer);
                continu = false;
            } else {
                EcrireBloc(fichier, milieu, buffer);
                i = i + 1;
                j = 1;
                e = x;
            }
        }
        if (milieu > LireEntete(fichier, (nmbBloc - 1))) {
            buffer.ENREG[0] = e;
            buffer.NE = 1;
            EcrireBloc(fichier, milieu, buffer);
        }
    }
    Fermer(fichier);
}
//fnct3 :Insertion d’un nouvel enregistrement Chainee Triee:
void insertionChaineeTriee(FILE *fichier, ENREGISTREMENT e, int *nmbBloc) { 
	BLOC buffer; 
	bool inserted = false;
	for (int i = 0; i < *nmbBloc; i++) { 
	fseek(fichier, i * sizeof(BLOC), SEEK_SET); 
	fread(&buffer, sizeof(BLOC), 1, fichier);
	for (int j = 0; j < buffer.nmbE; j++) {
	if (buffer.ENREG[j].ID > e.ID) {
		for (int k = buffer.nmbE; k > j; k--) {
		buffer.ENREG[k] = buffer.ENREG[k - 1]; }
		buffer.ENREG[j] = e; buffer.nmbE++; 
		fseek(fichier, -sizeof(BLOC), SEEK_CUR); 
		fwrite(&buffer, sizeof(BLOC), 1, fichier); 
		inserted = true; break; } } if (inserted) break; }
	if (!inserted) { 
		if (buffer.nmbE < FB) {
			buffer.ENREG[buffer.nmbE] = e; buffer.nmbE++; } 
		else { fseek(fichier, *nmbBloc * sizeof(BLOC), SEEK_SET);
		      buffer.nmbE = 1; buffer.ENREG[0] = e;
		      fwrite(&buffer, sizeof(BLOC), 1, fichier);
		      (*nmbBloc)++; }
	}
}
//fnct3 :Insertion d’un nouvel enregistrement Chainee non Triee:
void insertionChaineeNonTriee(FILE *fichier, ENREGISTREMENT e, int *nmbBloc) {
  BLOC buffer;
  bool insere = false;
  for (int i = 0; i < *nmbBloc; i++) { 
 fseek(fichier, i * sizeof(BLOC), SEEK_SET);
 fread(&buffer, sizeof(BLOC), 1, fichier); 
	  if (buffer.nmbE < FB) { 
	  buffer.ENREG[buffer.nmbE] = e;
	  buffer.nmbE++; fseek(fichier, -sizeof(BLOC), SEEK_CUR);
	  fwrite(&buffer, sizeof(BLOC), 1, fichier); insere = true; break; } }
	if (!insere) { buffer.nmbE = 1;
		      buffer.ENREG[0] = e;
		      fseek(fichier, *nmbBloc * sizeof(BLOC), SEEK_SET); 
		      fwrite(&buffer, sizeof(BLOC), 1, fichier); (*nmbBloc)++; } 
}

//fnct4 : rechercheEnregistrement contigue triee
int rechercheDichotomique(FILE *fichier, int nmbBloc, int ID, bool Trouv, int *milieu, int m) {
    bool stop = false;
    Trouv = false;
    BLOC buffer;
    int debut = 0, fin = LireEntete(fichier, nmbBloc - 1);

    while (debut <= fin && !Trouv && !stop) {
        *milieu = (debut + fin) / 2;
        LireBloc(fichier, *milieu, &buffer);

        if (ID >= buffer.ENREG[0].ID && ID <= buffer.ENREG[buffer.nmbE - 1].ID) {
            int debutBloc = 0, finBloc = buffer.nmbE - 1, m;
            while (debutBloc <= finBloc && !Trouv) {
                m = (debutBloc + finBloc) / 2;
                if (buffer.ENREG[m].ID == ID) {
                    Trouv = true;
                    printf("Enregistrement avec ID trouvé dans le Bloc %d, Position %d\n", *milieu + 1, m + 1);
                    return 1;
                } else if (ID < buffer.ENREG[m].ID) {
                    finBloc = m - 1;
                } else {
                    debutBloc = m + 1;
                }
            }
        }

        if (buffer.ENREG[0].ID > ID) {
            fin = *milieu - 1;
        } else if (buffer.ENREG[buffer.nmbE - 1].ID < ID) {
            debut = *milieu + 1;
        }
    }

    printf("Enregistrement avec ID %d introuvable.\n", ID);
    return -1;
}
//Suppression d’un enregistrement : 
////fnct6 : suppressionLogique contigue
void suppressionLogique(FILE *fichier, int positionBloc, int positionEnregistrement) {
    BLOC buffer;
    if (positionBloc <= 0) {
        printf("Position du bloc donnée est invalide !\n");
        return;
    }

    fseek(fichier, (positionBloc - 1) * sizeof(BLOC), SEEK_SET);
    fread(&buffer, sizeof(BLOC), 1, fichier);

    if (positionEnregistrement <= 0 || positionEnregistrement > buffer.nmbE) {
        printf("Position de l'enregistrement est invalide !\n");
        return;
    }
    buffer.ENREG[positionEnregistrement - 1].Effacé = true; 
    buffer.ENREG[positionEnregistrement - 1].ID = 0;
    buffer.ENREG[positionEnregistrement - 1].champs[0] = '\0';
    fseek(fichier, -1 * sizeof(BLOC), SEEK_CUR);
    fwrite(&buffer, sizeof(BLOC), 1, fichier);

    printf("L'enregistrement a été supprimé logiquement.\n");
}


////fnct7 : Renommer un fichier
void renommerFichier(FILE *fichier){
	printf("donne moi le nom de fichier qui vous voulez le changer:");
	scanf("%s",nom_fichier);
	printf("donne moi le nouveau nom de ce fichier :");
	scanf("%s",nouveauNom_fichier);
	strcpy(nom_fichier, nouveauNom_fichier);
	printf(" le nom de fichier est changer .");
}


//fnct8 : Supprimer un fichier
void SupprimerFichier(FILE *fichier){
  printf("donne moi le nom de fichier qui vous voulez le changer:");
  scanf("%s",nom_fichier);
  remove(nom_fichier);
  printf("le fichier a ete supprime");
}


//fnct9 : suppressionphysique
void suppressionPhysiqueEtReorganisation(FILE *fichier, int positionBloc, int positionEnregistrement, int *nmbBloc) {
    BLOC buffer, prochainBloc;
    int i, j;

    if (positionBloc <= 0 || positionBloc > *nmbBloc) {
        printf("Position du bloc donnée est invalide !\n");
        return;
    }

    fseek(MS, (positionBloc - 1) * sizeof(BLOC), SEEK_SET);
    fread(&buffer, sizeof(BLOC), 1, fichier);

    if (positionEnregistrement <= 0 || positionEnregistrement > buffer.nmbE) {
        printf("Position de l'enregistrement dans le bloc est invalide !\n");
        return;
    }

    for (i = positionEnregistrement - 1; i < buffer.nmbE - 1; i++) {
        buffer.ENREG[i] = buffer.ENREG[i + 1];
    }
    buffer.nmbE--;

    fseek(fichier, (positionBloc - 1) * sizeof(BLOC), SEEK_SET);
    fwrite(&buffer, sizeof(BLOC), 1, fichier);

    for (i = positionBloc; i < *nmbBloc; i++) {
        fseek(fichier, i * sizeof(BLOC), SEEK_SET);
        fread(&prochainBloc, sizeof(BLOC), 1, fichier);

        for (j = 0; j < prochainBloc.nmbE; j++) {
            if (buffer.nmbE < FB) {
                buffer.ENREG[buffer.nmbE++] = prochainBloc.ENREG[j];
            } else {
                break;
            }
        }

        fseek(fichier, (i - 1) * sizeof(BLOC), SEEK_SET);
        fwrite(&buffer, sizeof(BLOC), 1, fichier);

        if (j < prochainBloc.nmbE) {
            int restant = prochainBloc.nmbE - j;
            for (int k = 0; k < restant; k++) {
                prochainBloc.ENREG[k] = prochainBloc.ENREG[j + k];
            }
            prochainBloc.nmbE = restant;
        } else {
            prochainBloc.nmbE = 0;
        }

        buffer = prochainBloc;
    }

    if (*nmbBloc > 0) {
        (*nmbBloc)--;
        ftruncate(fileno(fichier), (*nmbBloc) * sizeof(BLOC));
    }

    printf("L'enregistrement a été supprimé physiquement et le fichier a été réorganisé.\n");
}


int main(){
	#define FB 100;
	FILE *fichier;
	char nom_fichier[100];
	nouveauNom_fichier[100];
	int nmbBloc;
	int modeGlobale,modeInterne;
}
