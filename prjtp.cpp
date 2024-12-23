// les structures
typedef struct ENREGISTREMENT ENREGISTREMENT;
struct ENREGISTREMENT {
	int ID ;
	char champs[60] ;
};
typedef struct BLOC BLOC ;
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
typedef struct FICHIER_DONNEES  FICHIER_DONNEES;
struct FICHIER_DONNEES {
	char nom_fichier[100];
	int nmbEnreg, nmbBloc ;
	int modeGlobale, modeInterne ;
};
//fnct1 : Cr�ation_fichier
void Cr�ation_fichier (FICHIER_DONNEES*fichier)
{
	printf("donne-moi le nom de fichier:\n");
	scanf("%s",FICHIER_DONNEES->nom_fichier);
	printf("donne-moi le nombre d�enregistrements:\n");
	scanf("%d",&FICHIER_DONNEES->nmbEnreg);
	nmbBloc=(nmbEnreg+(FB-1))/FB; // un calcul simple pour avoir le nombre entiere de bloc
	FICHIER_DONNEES.nmbBloc=nmbBloc;
	printf("le nombre des bloc est :%d \n",FICHIER_DONNEES->nmbBloc );
	printf("donne-moi le mode d�organisation Globale: (click 0 pour contigue et 1 pour chainee):\n");
	scanf("%s",&FICHIER_DONNEES->modeGlobale);
	printf("donne-moi le mode d�organisation Interne: (click 0 pour contigue et 1 pour chainee):\n");
	scanf("%s",&FICHIER_DONNEES->modeInterne);
}
//fnct2 : Allouer_Blocs
void  Allouer_Blocs(FILE *MS, int nmbBloc) {
    BLOC buffer;
    buffer.ne = 0; // 0 enregistrement dans le bloc
    for (int i = 0; i < nmbBloc; i++) {
        fwrite(&buffer, sizeof(Bloc), 1,MS); // remplire le fichier par des blocs vide
    }
}
//fnct4 : rechercheEnregistrement contigue triee
int rechercheDichotomique(FILE *MS, int nmbBloc, int ID) {
    BLOC buffer;
    int debut = 0, fin = nmbBloc - 1, milieu; // initialisation
    while (debut <= fin) {
        milieu = (debut + fin) / 2;
         // lire le bloc au milieu
        fseek(MS, milieu * sizeof(BLOC), SEEK_SET);
        fread(&buffer, sizeof(BLOC), 1, MS); //we put the content of le bloc au milieu
       // je cherche le ID dans ce bloc
        int debutBloc = 0, finBloc = buffer.nmbE - 1, m;
        while (debutBloc <= finBloc) {
            m = (debutBloc + finBloc) / 2;
            if (buffer.ENREG[m].ID == ID) {
                // le ID et trouve ou milieu de bloc qui est au milieu
                printf("Enregistrement avec ID trouv� dans le Bloc %d, Position %d\n", milieu + 1, m + 1);
                return 0;
            }
            //je cherche dans les enregistrements au-dessus de bloc au milieu
			else if (ID < buffer.ENREG[m].ID) {
                finBloc = m - 1;
            }
			 //je cherche dans le reste
			else {
                debutBloc = m + 1;
            }
        }
        // je cherche dans les blocs au-dessus de bloc de milieu
        if (buffer.ENREG[0].ID > ID) {
            fin = milieu - 1;
        }
         //je cherche dans le reste
		 else if (buffer.ENREG[buffer.nmbE - 1].ID < ID) {
            debut = milieu + 1;
        }
    }
    // Si  n'est pas trouv�
    printf("Enregistrement avec ID %d introuvable.\n", ID);
    return -1;
}
//fnct5 : rechercheEnregistrement contigue non triee
void rechercheProduit(FILE *MS, int nmbBloc, int ID, int position[]) {
    BLOC buffer; // D�claration du buffer pour lire les blocs
    position[0] = -1; // Initialisation : -1 signifie que le produit n'est pas trouv�
    position[1] = -1;
    for (int i = 0; i < nmbBloc; i++) {
        fseek(MS, i * sizeof(BLOC), SEEK_SET);
        fread(&buffer, sizeof(BLOC), 1, MS);
        for (int j = 0; j < buffer.nmbE; j++) {
            if (buffer.ENREG[j].ID == ID) {
                position[0] = i + 1;
                position[1] = j + 1;
                printf("Enregistrement avec ID trouv� dans le Bloc %d, Position %d\n", position[0], position[1]);
                return;
            }
        }
    }
    printf("Enregistrement avec ID %d introuvable.\n", ID);
}
//fnct6 : suppressionLogique contigue
 void suppressionLogique(FILE *MS, int positionBloc, int positionEnregistrement) {
    BLOC buffer;
    // Verification de la positionBloc
    if (positionBloc <= 0) {
        printf("Position du bloc donne est negative !\n");
        return;
    }
    fseek(MS, (positionBloc - 1) * sizeof(BLOC), SEEK_SET); //chonge le pointure vers la Position du bloc choisi
    fread(&buffer, sizeof(BLOC), 1, MS); //read le bloc
 // Verification de la positionEnregistrement
    if (positionEnregistrement <= 0 || positionEnregistrement > buffer.nmbE) {
        printf("Position de l'enregistrement est negative ou n'exicte pas !\n");
        return;
    }
    //la suppressionLogique
    buffer.ENREG[positionEnregistrement - 1].ID = 0;
    buffer.ENREG[positionEnregistrement - 1].champs[0] = null ;
    // �crire les modifications de ce bloc
    fseek(MS, -1 * sizeof(BLOC), SEEK_CUR);
    fwrite(&buffer, sizeof(BLOC), 1, MS);
    printf("L'enregistrement a �t� supprim� logiquement.\n");
}
//fnct7 : Renommer un fichier
void renommerFichier(FILE *MS) {
	char nom_fichier[100],nouveauNom_fichier[100];
	printf("donne moi le nom de fichier qui vous voulez le changer:");
	scanf("%s",nom_fichier);
	printf("donne moi le nouveau nom de ce fichier :");
	scanf("%s",nouveauNom_fichier);
	rename(nom_fichier,nouveauNom_fichier);
	FICHIER_DONNEES.nom_fichier=nouveauNom_fichier;
}
//fnct8 : Supprimer un fichier
void SupprimerFichier(FILE *MS){
 char nom_fichier[100];
  printf("donne moi le nom de fichier qui vous voulez le changer:");
  scanf("%s",nom_fichier);
  remove(nom_fichier);
   printf("le fichier a ete supprime")
  FICHIER_DONNEES.nom_fichier=null;
}
void suppressionPhysiqueEtReorganisation(FILE *MS, int positionBloc, int positionEnregistrement, int *nmbBloc) {
    BLOC buffer, prochainBloc;
    int i, j;
    // �tape 1 : V�rifications initiales
    if (positionBloc <= 0 || positionBloc > *nmbBloc) {
        printf("Position du bloc donn�e est invalide !\n");
        return;
    }
    // Lire le bloc sp�cifi�
    fseek(MS, (positionBloc - 1) * sizeof(BLOC), SEEK_SET);
    fread(&buffer, sizeof(BLOC), 1, MS);
    // V�rification de l'enregistrement
    if (positionEnregistrement <= 0 || positionEnregistrement > buffer.nmbE) {
        printf("Position de l'enregistrement dans le bloc est invalide !\n");
        return;
    }
    // Suppression logique dans le bloc
    for (i = positionEnregistrement - 1; i < buffer.nmbE - 1; i++) {
        buffer.ENREG[i] = buffer.ENREG[i + 1];
    }
    buffer.nmbE--; // R�duire le nombre d'enregistrements dans le bloc
    // R��crire le bloc mis � jour
    fseek(MS, (positionBloc - 1) * sizeof(BLOC), SEEK_SET);
    fwrite(&buffer, sizeof(BLOC), 1, MS);
    // �tape 2 : R�organisation des blocs
    for (i = positionBloc; i < *nmbBloc; i++) {
        fseek(MS, i * sizeof(BLOC), SEEK_SET);
        fread(&prochainBloc, sizeof(BLOC), 1, MS);
        // Transf�rer les enregistrements restants du prochain bloc
        for (j = 0; j < prochainBloc.nmbE; j++) {
            if (buffer.nmbE < FB) { // Ajouter les enregistrements tant qu'il reste de l'espace
                buffer.ENREG[buffer.nmbE++] = prochainBloc.ENREG[j];
            } else {
                break; // Si le bloc courant est plein, arr�ter ici
            }
        }
        // R��crire le bloc courant
        fseek(MS, (i - 1) * sizeof(BLOC), SEEK_SET);
        fwrite(&buffer, sizeof(BLOC), 1, MS);
        // Mettre � jour le prochain bloc (d�placer les enregistrements restants)
        if (j < prochainBloc.nmbE) {
            int restant = prochainBloc.nmbE - j;
            for (int k = 0; k < restant; k++) {
                prochainBloc.ENREG[k] = prochainBloc.ENREG[j + k];
            }
            prochainBloc.nmbE = restant;
        } else {
            prochainBloc.nmbE = 0;
        }
        buffer = prochainBloc; // Passer au prochain bloc
    }
    // �tape 3 : R�duction de la taille physique du fichier
    if (*nmbBloc > 0) {
        (*nmbBloc)--; // R�duire le nombre de blocs
        ftruncate(fileno(MS), (*nmbBloc) * sizeof(BLOC));
    }
    printf("L'enregistrement a �t� supprim� physiquement et le fichier a �t� r�organis�.\n");
}
