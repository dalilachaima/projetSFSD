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

//fnct1 : Création_fichier
void Création_fichier (FICHIER_DONNEES*fichier)
{
	printf("donne-moi le nom de fichier:\n");
	scanf("%s",FICHIER_DONNEES->nom_fichier);
	printf("donne-moi le nombre d’enregistrements:\n");
	scanf("%d",&FICHIER_DONNEES->nmbEnreg);
	nmbBloc=(nmbEnreg+(FB-1))/FB; // un calcul simple pour avoir le nombre entiere de bloc
	FICHIER_DONNEES.nmbBloc=nmbBloc
	printf("le nombre des bloc est :%d \n",FICHIER_DONNEES->nmbBloc )
	printf("donne-moi le mode d’organisation Globale: (click 0 pour contigue et 1 pour chainee):\n");
	scanf("%s",&FICHIER_DONNEES->modeGlobale);
	printf("donne-moi le mode d’organisation Interne: (click 0 pour contigue et 1 pour chainee):\n");
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
//fnct4 : rechercheProduit contigue triee
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
                printf("Produit trouvé dans le Bloc %d, Position %d\n", milieu + 1, m + 1);
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

    // Si  n'est pas trouvé
    printf("Produit avec ID %d introuvable.\n", ID);
    return -1;
}

//fnct5 : rechercheProduit contigue non triee
void rechercheProduit(FILE *MS, int nmbBloc, int ID, int position[]) {
    BLOC buffer; // Déclaration du buffer pour lire les blocs
    position[0] = -1; // Initialisation : -1 signifie que le produit n'est pas trouvé
    position[1] = -1;
    for (int i = 0; i < nmbBloc; i++) {
        fseek(MS, i * sizeof(BLOC), SEEK_SET);
        fread(&buffer, sizeof(BLOC), 1, MS);
        for (int j = 0; j < buffer.nmbE; j++) {
            if (buffer.ENREG[j].ID == ID) {
                position[0] = i + 1;
                position[1] = j + 1; 
                printf("Produit trouvé dans le Bloc %d, Position %d\n", position[0], position[1]);
                return;
            }
        }
    }
    printf("Produit avec ID %d introuvable.\n", ID);
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

    // écrire les modifications de ce bloc 
    fseek(MS, -1 * sizeof(BLOC), SEEK_CUR);
    fwrite(&buffer, sizeof(BLOC), 1, MS);
    printf("L'enregistrement a été supprimé logiquement.\n");
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
 




