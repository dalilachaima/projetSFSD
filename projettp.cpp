
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
  BLOC bloc;	
};
typedef struct FICHIER_DONNEES  FICHIER_DONNEES;
struct FICHIER_DONNEES {
	char nom_fichier[100];
	int nmbEnreg, nmbBloc ;
	int modeGlobale, modeInterne ;
};
void Création_fichier (FICHIER_DONNEES*fichier)
{
	printf("donne-moi le nom de fichier:\n");
	scanf("%s",FICHIER_DONNEES.nom_fichier);
	printf("donne-moi le nombre d’enregistrements:\n");
	scanf("%d",&FICHIER_DONNEES.nmbEnreg);
	nmbBloc=(nmbEnreg+(FB-1)/FB); // un calcul simple pour avoir le nombre entiere de bloc
	FICHIER_DONNEES.nmbBloc=nmbBloc
	printf("le nombre des bloc est :%d \n"FICHIER_DONNEES.nmbBloc )
	printf("donne-moi le mode d’organisation Globale: (click 0 pour contigue et 1 pour chainee):\n");
	scanf("%s",&FICHIER_DONNEES.modeGlobale);
	printf("donne-moi le mode d’organisation Interne: (click 0 pour contigue et 1 pour chainee):\n");
	scanf("%s",&FICHIER_DONNEES.modeInterne);
}

