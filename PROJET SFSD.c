#include <stdio.h>
#include <stdlib.h>
#define MAX_FB 10
#define DISK_SIZE 100
#include <string.h>
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

struct tenr{
  int id;
  char nom[21];
  int supp; // si supprime =1 , sinon =0
};

struct tbloc{
 struct tenr B[MAX_FB];
 int NE;
 int occup; // si occupe=> 1  si non 0
};

struct tblocChaine {
 struct tenr B [MAX_FB];
 int NE;
 int occup;
 struct tblocChaine *next;
};

struct tMetaD{
  char nomfichier[51];
  int tailleblocs;
  int taillenreg;
  int adrprebloc;
  char modeorgaglobale[51];
  char modeorgainterne[51];
};

struct MS {
    int nb;
    int nblibre;
    struct tbloc m[DISK_SIZE];
};
struct MSc {
    int nb;
    int nblibre;
 struct tblocChaine* tete;
};
// fonction pour cration de blocs en mode contigue
void creatBlocContigue( struct tbloc *BLOC , int numenr){
   BLOC->occup = 1 ;
   BLOC->NE = 0 ;
  for (int i = 0 ; i < numenr ; i++){
    printf("give the id and the name of product number: %d\n" , i+1 );
    scanf(" give the id:");
    scanf("%d\n" , &BLOC->B[i].id);
    scanf("give the name:");
    scanf("%s\n", &BLOC->B[i].nom);
    BLOC->B[i].supp = 1 ; // existe
    BLOC->NE++;
  }
}
// fonction pour creation de blocs en mode chainee
struct tblocChaine *creatBlocChaine (int numenr){
   struct  tblocChaine *newBloc = (struct tblocChaine *)malloc(sizeof(struct tblocChaine));
     newBloc->occup = 1 ;
     newBloc->NE = 0;
     for (int i = 0 ; i < numenr ; i++ ){
        printf("give the id and the name of product number: %d\n" , i+1 );
        scanf(" give the id:");
        scanf("%d\n" , &newBloc->B[i].id);
        scanf("give the name:");
        scanf("%s\n", &newBloc->B[i].nom);
        newBloc->B[i].supp = 0 ; // existe
        newBloc->NE++;
     }
     newBloc->next = NULL;
     return newBloc ;

};

// fonction de triage des produits en mode contigue
void trierContigue(struct tbloc bloc ){
    struct tenr temp;

   for (int i = 0 ; i < bloc.NE - 1 ; i++){
    int indexMin = i ;
    for ( int j = i+1 ; j < bloc.NE ; j++){
        if (bloc.B[j].id < bloc.B[indexMin].id){
            indexMin = j ;
        }
    }
    if (indexMin != i){
        temp = bloc.B[i];
        bloc.B[i] = bloc.B[indexMin];
        bloc.B[indexMin] = temp ;
    }
   }
}

// fontcion de triage des produits en mode chainee
void trierChainee (struct tblocChaine bloc ){
struct tenr temp;

   for (int i = 0 ; i < bloc.NE - 1 ; i++){
    int indexMin = i ;
    for ( int j = i+1 ; j < bloc.NE ; j++){
        if (bloc.B[j].id < bloc.B[indexMin].id){
            indexMin = j ;
        }
    }
    if (indexMin != i){
        temp = bloc.B[i];
        bloc.B[i] = bloc.B[indexMin];
        bloc.B[indexMin] = temp ;
    }
   }

}

// creation de fichier metadonees
void creatMTfile (struct tMetaD *MT){
   char metaFile [56];
   snprintf(metaFile , sizeof(metaFile), "%s.meta" , MT->nomfichier);
   FILE *MTfile = fopen(metaFile, "w");
   fwrite(MT , sizeof(struct tMetaD), 1, metaFile);
   fclose(MTfile);
}

// fonction pour creation des fichiers
void CreeFichier (char nomFichier [51] , int nbrEnreg , int choixGlobale , int choixIntern){ //contigue = 0 , chainee = 1 , trier = 0 , non trier = 1
  //File *file ;
  int nbrBloc ;
  struct tbloc BlocContigue ;
  struct tblocChaine *headBlocChainee = NULL ;
  struct tblocChaine *currentBlocChainee = NULL;
  struct tMetaD MT;

  strncpy(MT.nomfichier, nomFichier, sizeof(MT.nomfichier));

   // create the file
   FILE *file = fopen(nomFichier , "w");
   nbrBloc = (nbrEnreg + MAX_FB -1) / MAX_FB;
   int nbrEdernierBloc = nbrEnreg % MAX_FB;

   MT.taillenreg = nbrEnreg;
   MT.tailleblocs = nbrBloc ;

    if ( choixGlobale == 0 ){
    // le mode globale = contigue
    strncpy(MT.modeorgaglobale , "contigue" , sizeof(MT.modeorgaglobale));
        for (int i = 0 ; i < nbrBloc ; i++ ){
            printf("creation of bloc %d (contigue)\n" , i+1);
            if (i != nbrBloc -1){
                creatBlocContigue(&BlocContigue, MAX_FB);
            }else {
            creatBlocContigue(&BlocContigue, nbrEdernierBloc);
            }
            fwrite(&BlocContigue , sizeof(struct tbloc),1 , file);
            if (i == 0) {
                MT.adrprebloc = (long)&BlocContigue;
            }
            if (choixIntern == 0){
                // choix interne : tier
                strncpy(MT.modeorgainterne , "trier" , sizeof(MT.modeorgainterne));
                trierContigue(BlocContigue);
            } else {
              strncpy(MT.modeorgainterne , "non trier" , sizeof(MT.modeorgainterne));
            }
            fwrite(newBloc , sizeof( struct tbloc), 1 ,file);
        }
    } else if (choixGlobale == 1){
      // le mode globale = chainee
      strncpy(MT.modeorgaglobale , "chainee" , sizeof(MT.modeorgaglobale));
      struct tblocChaine *newBloc = NULL;
      for (int i = 0 ; i < nbrBloc ; i++){
      printf("cration of bloc %d (chainee)\n" , i+1);
      // mode FIFO
      if (i != nbrBloc-1){
        struct tblocChaine *newBloc = creatBlocChaine(MAX_FB);
      } else {
         struct tblocChaine *newBloc = creatBlocChaine(nbrEdernierBloc);
      }
       if (i == 0){
           MT.adrprebloc = (long)newBloc;
       }
        if (choixIntern == 0 ){
            // choix interne = trier
            strncpy(MT.modeorgainterne , "trier" , sizeof(MT.modeorgainterne));
            trierChainee(*newBloc);
        } else {
          strncpy(MT.modeorgainterne , "non trier" , sizeof(MT.modeorgainterne));
        }
       if (headBlocChainee == NULL){
        headBlocChainee = newBloc ;
        currentBlocChainee = newBloc;
       } else {
         currentBlocChainee->next = newBloc;
         currentBlocChainee = newBloc;
       }
       //currentBlocChainee =  newBloc;
       fwrite(newBloc , sizeof( struct tblocChaine), 1 ,file);
    }
    }
    fclose(file);
   creatMTfile(&MT);
}
//initialisation de la MS
void initialiserMS(struct MS *ms) {
    ms->nb = DISK_SIZE;
    ms->nblibre = DISK_SIZE;

    for (int i = 0; i < DISK_SIZE; i++) {
        ms->m[i].occup = 0;
        ms->m[i].NE = 0;
        for (int j = 0; j < MAX_FB; j++) {
            ms->m[i].B[j].supp = 0;
        }
    }
    printf("Memoire secondaire initialisee avec succes.\n");
}
//l'affichage de la MS
void afficherEtatMS(struct MS *ms) {
    printf("\n=== les blocs de la Memoire Secondaire ===\n");
    for (int i = 0; i < ms->nb; i++) {
        if (ms->m[i].occup == 0) {
            printf(GREEN"{Libre}"RESET);
        } else {
            printf(RED"{%d }"RESET, ms->m[i].NE);
        }

        if (i  % 10 == 0) {
            printf("\n");
        }
}

}
// fonction lireMT
 void lireMT (FILE *f , int nc , void* result ){
   rewind(f);
   struct tMetaD MT;
   fread(&MT , sizeof( struct tMetaD) , 1 , f);
   switch(nc){
     case 1 : //fileName
      strcpy((char*)result , MT.nomfichier);
      break ;
     case 2 : // tailleBloc
        *(int*)result = MT.tailleblocs;
        break;
     case 3 : // tailleEnre
        *(int*)result = MT.taillenreg;
        break;
     case 4 : // adressePrBloc
        *(int*)result = MT.adrprebloc;
        break;
     case 5 : // modeGlobal
        strcpy((char*)result , MT.modeorgaglobale);
        break;
     case 6 : // modeIntern
        strcpy((char*)result , MT.modeorgainterne);
        break;
   }
 }
 // mise a jour des matadonnees
 void majmeta(FILE *f, int nc, void *newValue) {
     struct tMetaD meta;
     fread(&meta, sizeof(struct tMetaD),1,f);
    switch(nc) {
        case 1: // file name
            strncpy(meta.nomfichier, (char*)newValue, 50);
            break;
        case 2: // size in blocks
            meta.tailleblocs = *(int*)newValue;
            break;
        case 3: //size in records
            meta.taillenreg = *(int*)newValue;
            break;
        case 4: // first block address
            meta.adrprebloc = *(long*)newValue;
            break;
        case 5: // global organization mode
            strncpy(meta.modeorgaglobale, (char*)newValue, 50);
            break;
        case 6: // internal organization mode
            strncpy(meta.modeorgainterne, (char*)newValue, 50);
            break;
    }

//function to rename a file
void renommerfichier(char *nomfichier, char *nvnom) {
    if (rename(nomfichier, nvnom) == 0) {
        printf("File renomme\n");
    } else {
        printf("Error \n");
    }
}

// recherche d'un enregistrement dans un fichier
void rechercheEnregistrement(FILE *ms, FILE *f, int id, int adrs[2]) {
    char orgGlobale[51], orgInterne[51];
    int numBloc = 1, adr, taille;

    lireMT(f, 4, &adr); // adresse du premier bloc
    lireMT(f, 2, &taille);  // nombre des blocs ds le fichier
    lireMT(f, 5, orgGlobale);
    lireMT(f, 6, orgInterne);

    if (strcmp(orgGlobale, "chainee") == 0) { // on voir s'il s'agit d'un fichier chaine ou non
        struct tblocChaine buffer;
        fseek(ms, adr * sizeof(struct tblocChaine), SEEK_SET);
        while (numBloc <= taille && fread(&buffer, sizeof(struct tblocChaine), 1, ms) == 1) {
            if (strcmp(orgInterne, "trier") == 0 && buffer.occup == 1) {// si le fichier est trie on fait le recherche dichotomique
                int debut = 0, fin = buffer.NE - 1;
                while (debut <= fin) {
                    int milieu = (debut + fin) / 2;
                    if (buffer.B[milieu].id == id && buffer.B[milieu].supp == 0) {
                        adrs[0] = numBloc;
                        adrs[1] = milieu + 1;
                    }
                    if(buffer.B[milieu].id < id) {
                            debut = milieu + 1;
                    }
                    else{
                        fin = milieu - 1;
                }
                }
            } else if (strcmp(orgInterne, "non trier") == 0 && buffer.occup == 1) {
                for (int j = 0; j < buffer.NE; j++) {//sinon on fait la recherche sequentielle
                    if (buffer.B[j].id == id && buffer.B[j].supp == 0) {
                        adrs[0] = numBloc;
                        adrs[1] = j + 1;
                    }
                }
            }
            numBloc++;
            if(buffer.next == NULL){
                break;
                }
            fseek(ms, (long)buffer.next * sizeof(struct tblocChaine), SEEK_SET);
        }
    } else {
        struct tbloc buffer;
        fseek(ms, adr * sizeof(struct tbloc), SEEK_SET);

        while (numBloc <= taille && fread(&buffer, sizeof(struct tbloc), 1, ms) == 1) {
            if (strcmp(orgInterne, "trier") == 0 && buffer.occup == 1) {
                int debut = 0, fin = buffer.NE - 1;

                while (debut <= fin) {
                    int milieu = (debut + fin) / 2;
                    if (buffer.B[milieu].id == id && buffer.B[milieu].supp == 0) {
                        adrs[0] = numBloc;
                        adrs[1] = milieu + 1;
                        return;
                    }
                    if (buffer.B[milieu].id < id) debut = milieu + 1;
                    else fin = milieu - 1;
                }
            } else if (buffer.occup == 1) {
                for (int j = 0; j < buffer.NE; j++) {
                    if (buffer.B[j].id == id && buffer.B[j].supp == 0) {
                        adrs[0] = numBloc;
                        adrs[1] = j + 1;
                        return;
                    }
                }
            }
            numBloc++;
        }
    }

    adrs[0] =-1;// element non trouve
     adrs[1] = -1;
}
//suppression logique
void supprlogique(FILE *ms, FILE *f, int id) {
    int adrs[2];
    int adr;
    char orgGlobale[51];
    lireMT(f, 4, &adr);
    lireMT(f, 5, orgGlobale);
    rechercheEnregistrement(ms, f, id, adrs);
    if (adrs[0] == -1) {
    printf("l'enregistrement n'existe pas");
    }

    if (strcmp(orgGlobale, "contigue") == 0) {// on voir s'il s'agit d'un fichier contigue
        struct tbloc buffer;
        fseek(ms, (adr + adrs[0] - 1) * sizeof(struct tbloc), SEEK_SET);
        if (fread(&buffer, sizeof(struct tbloc), 1, ms) == 1) {
            buffer.B[adrs[1] - 1].supp = 1;// on met la valeur du  champ supp a 1
            buffer.occup = 1;
            fseek(ms, -sizeof(struct tbloc), SEEK_CUR);
            fwrite(&buffer, sizeof(struct tbloc), 1, ms);
        }
    } else {
        struct tblocChaine buffer;
        fseek(ms, adr * sizeof(struct tblocChaine), SEEK_SET);

        for (int i = 1; i < adrs[0] && fread(&buffer, sizeof(struct tblocChaine), 1, ms) == 1; i++) {
            if (buffer.next != NULL) {
                fseek(ms, (long)buffer.next * sizeof(struct tblocChaine), SEEK_SET);// parcourir jusqu'on arrive a l'adresse
            }
        }

        if (fread(&buffer, sizeof(struct tblocChaine), 1, ms) == 1) {
            buffer.B[adrs[1] - 1].supp = 1;
            buffer.occup = 1;
            fseek(ms, -sizeof(struct tblocChaine), SEEK_CUR);
            fwrite(&buffer, sizeof(struct tblocChaine), 1, ms);
        }
    }
     printf("\n=== Suppression logique reussite ===\n", id);
}
// suppression physique
void supprPhysique(FILE *ms, FILE *f, int id) {
    int adrs[2];
    char orgGlobale[51];
    int taille, adr;

    rechercheEnregistrement(ms, f, id, adrs);//recupiration de l'adresse
    if (adrs[0] == -1) {
            printf("l'enregistrement est deja supprime");
   }
    lireMT(f, 5, orgGlobale);
    lireMT(f, 2, &taille);
    lireMT(f, 4, &adr);

    if (strcmp(orgGlobale, "contigue") == 0) {// verifiant de mode d'organisation globale
        struct tbloc buffer;
        fseek(ms, (adr + adrs[0] - 1) * sizeof(struct tbloc), SEEK_SET);//passer directement a l'dresse du bloc
        if (fread(&buffer, sizeof(struct tbloc), 1, ms) == 1) {
            for (int i = adrs[1] - 1; i < buffer.NE - 1; i++) {
                buffer.B[i] = buffer.B[i + 1];//decalage des enregistrements
            }
            buffer.NE--;
            fseek(ms, -sizeof(struct tbloc), SEEK_CUR);//rewrite with new size
            fwrite(&buffer, sizeof(struct tbloc), 1, ms);
             int nve;
            lireMT(f,3, nve);
            majmeta(f, 3, (nve-1));//mise a jour des MT
        }
    } else {
        struct tblocChaine buffer;
        fseek(ms, adr * sizeof(struct tblocChaine), SEEK_SET);
        for (int i = 1; i < adrs[0] && fread(&buffer, sizeof(struct tblocChaine), 1, ms) == 1; i++) {
            if (buffer.next != NULL) {
                fseek(ms, (long)buffer.next * sizeof(struct tblocChaine), SEEK_SET);// passer par les pointeurs
            }
        }

        if (fread(&buffer, sizeof(struct tblocChaine), 1, ms) == 1) {
            for (int i = adrs[1] - 1; i < buffer.NE - 1; i++) {
                buffer.B[i] = buffer.B[i + 1];
            }
            buffer.NE--;
            fseek(ms, -sizeof(struct tblocChaine), SEEK_CUR);
            fwrite(&buffer, sizeof(struct tblocChaine), 1, ms);
             int nve;
            lireMT(f,3, nve);
            majmeta(f, 3, (nve-1));
        }
    }
     printf("\n=== Suppression physique reussite ===\n", id);
}
//insertion d'un enregistrement
void insertion(FILE *ms, FILE *f) {
    rewind(ms);
    struct tenr newenr;
    int a, taille;
    // Retrieve block address and size
    lireCaracteristique(f, 4, a); // Retrieve address of the first block
    lireMT(f, 2, taille); // Retrieve the size of the file in blocks
    printf("Entrez le ID du nouveau enregistrement :\n");
    printf("ID : ");
    scanf("%d", &newenr.id);
    printf("Nom : ");
    scanf(" %[^\n]", newenr.nom);
    char orgaglobale[51];
    char orgainterne[51];
    lireMT(f, 6, orgainterne); // Read internal organization
    lireMT(f, 5, orgaglobale); // Read global organization
    if (strcmp(orgaglobale, "contigue") == 0) {
        struct tbloc buffer;
        if (strcmp(orgainterne, "non trier") == 0) { // Unsorted
            fread(&buffer, sizeof(struct tbloc), 1, ms);
            int adrdernier = (a + taille) - 1;
            fseek(ms, adrdernier * sizeof(struct tbloc), SEEK_SET); // Move to the last block
            if (buffer.NE < MAX_FB) { // Space available in the block
                buffer.B[buffer.NE].id = newenr.id;
                strcpy(buffer.B[buffer.NE].nom, newenr.nom);
                buffer.B[buffer.NE].supp = 0;
                buffer.NE++;
                fseek(ms, -sizeof(struct tbloc), SEEK_CUR); // Move back to write
                fwrite(&buffer, sizeof(struct tbloc), 1, ms);
                int nve;
                lireMT(f, 3, &nve);
                majmeta(f, 3, nve + 1); // Update the record count
            } else { // No space available, allocate a new block
                struct tbloc buffer2;
                creatBlocContigue(&buffer2, 1);
                buffer2.B[0].id = newenr.id;
                strcpy(buffer2.B[0].nom, newenr.nom);
                buffer2.B[0].supp = 0;

                fseek(ms, (adrdernier + 1) * sizeof(struct tbloc), SEEK_SET);
                fwrite(&buffer2, sizeof(struct tbloc), 1, ms);

                int nve;
                lireMT(f, 3, &nve);
                majmeta(f, 3, nve + 1);
                majmeta(f, 2, taille + 1); // Update file size
            }
        } else { // Sorted file
            int j;
            for (int i = 0; i < taille; i++) {
                fseek(ms, (a + i) * sizeof(struct tbloc), SEEK_SET);
                fread(&buffer, sizeof(struct tbloc), 1, ms);

                for (j = 0; j < buffer.NE; j++) { // Find insertion point
                    if (newenr.id < buffer.B[j].id) {
                        break;
                    }
                }

                if (buffer.NE < MAX_FB) { // Space available
                    for (int k = buffer.NE; k > j; k--) {
                        buffer.B[k] = buffer.B[k - 1];
                    }
                    buffer.B[j] = newenr; // Insert
                    buffer.NE++;
                    fseek(ms, (a + i) * sizeof(struct tbloc), SEEK_SET);
                    fwrite(&buffer, sizeof(struct tbloc), 1, ms);

                    int nve;
                    lireMT(f, 3, &nve);
                    majmeta(f, 3, nve + 1);
                    return;
                }
            }

            struct tbloc newBlock; // If no space found, allocate new block
            newBlock.NE = 1;
            newBlock.B[0] = newenr;
            fseek(ms, (a + taille) * sizeof(struct tbloc), SEEK_SET);
            fwrite(&newBlock, sizeof(struct tbloc), 1, ms);

            int nve;
            lireMT(f, 3, &nve);
            majmeta(f, 3, nve + 1);
            majmeta(f, 2, taille + 1); // Update file size
        }
    } else if(strcmp(orgaglobale, "chainee") == 0) {
        struct tblocChaine *buffer, *buffer2;
        fseek(ms, a * sizeof(struct tblocChaine), SEEK_SET); // Start reading from the first block
        fread(&buffer, sizeof(struct tblocChaine), 1, ms);
        if (strcmp(orgainterne, "non trier") == 0) { // Unsorted chained blocks
            // Traverse until the end of the chain to insert at the last position
            while (buffer->next != NULL) {
                buffer = buffer->next; // Move to the next block
            }

            // If space is available in the last block, insert the record
            if (buffer->NE < MAX_FB) {
                buffer->B[buffer->NE].id = newenr.id;
                strcpy(buffer->B[buffer->NE].nom, newenr.nom);
                buffer->B[buffer->NE].supp = 0;
                buffer->NE++;
                fwrite(buffer, sizeof(struct tblocChaine), 1, ms);

                // Update meta data
                int nve;
                lireMT(f, 3, &nve);
                majmeta(f, 3, nve + 1);
            } else {
                struct tblocChaine *newBlock= creatBlocChaine( 1);
                newBlock->NE = 1;
                newBlock->B[0].id = newenr.id;
                strcpy(newBlock->B[0].nom, newenr.nom);
                newBlock->B[0].supp = 0;
                // Update the last block's next pointer to point to the new block
                buffer->next = &newBlock;

                fwrite(&newBlock, sizeof(struct tblocChaine), 1, ms);

                // Update meta data
                int nve;
                lireMT(f, 3, &nve);
                majmeta(f, 3, nve + 1);
                majmeta(f, 2, taille + 1); // Update file size
            }
        } else if (strcmp(orgainterne, "trier") == 0) { // Sorted chained blocks
            struct tblocChaine *prevBuffer = NULL; // Pointer to previous block
            int insertionPointFound = 0;
            // Traverse the chain to find the correct position to insert the new record
            while (buffer != NULL && !insertionPointFound) {
                for (int i = 0; i < buffer->NE; i++) {
                    if (buffer->B[i].id > newenr.id) {
                        insertionPointFound = 1;
                        break; // Found the insertion point
                    }
                }

                // If the insertion point is found, insert the record into the current block
                if (insertionPointFound) {
                    if (buffer->NE < MAX_FB) {
                            int i;
                        for (i = buffer->NE; i > i; i--) {
                            buffer->B[i] = buffer->B[i - 1];
                        }
                        // Insert the new record
                        buffer->B[i] = newenr;
                        buffer->NE++;

                        // Write the updated block back to the file
                        fseek(ms, (a + i) * sizeof(struct tblocChaine), SEEK_SET);
                        fwrite(buffer, sizeof(struct tblocChaine), 1, ms);

                        // Update meta data
                        int nve;
                        lireMT(f, 3, &nve);
                        majmeta(f, 3, nve + 1);
                        return;
                    }
                }
                // Move to the next block in the chain
                prevBuffer = buffer;
                buffer = buffer->next;
            }

            // If the insertion point is not found, create a new block and insert it at the end
            if (prevBuffer != NULL && buffer == NULL) {
                struct tblocChaine *newBlock=creatBlocChaine(1);
                newBlock->NE = 1;
                newBlock->B[0].id = newenr.id;
                strcpy(newBlock->B[0].nom, newenr.nom);
                newBlock->B[0].supp = 0;
                prevBuffer->next = &newBlock;
                // Write the new block to the file
                fwrite(&newBlock, sizeof(struct tblocChaine), 1, ms);
                int nve;
                lireMT(f, 3, &nve);
                majmeta(f, 3, nve + 1);
                majmeta(f, 2, taille + 1); // Update file size
            }
        }
    }
}
// fonction pour lire tous les enregistrements
void lirenregistrements(FILE *ms, FILE *f) {
    char orgaglobale[51];
    int taille;
    int a;

    lireMT(f, 2, &taille); // Read file size in blocks
    lireMT(f, 4, &a);      // Read address of the first block
    lireMT(f, 5, orgaglobale); // Read the global organization mode

    if (strcmp(orgaglobale, "contigue") == 0) {
        struct tbloc buffer;
        fseek(ms, a * sizeof(struct tbloc), SEEK_SET); // Go to the first block

        // Read and display records from contiguous blocks
        for (int i = 0; i < taille; i++) {
            fread(&buffer, sizeof(struct tbloc), 1, ms); // Read the block
            int j = 0;
            printf("BLOC", i+1);
            while (j < buffer.NE) { // Iterate over the records in the block
                    if(buffer.B[j].supp=0){
                printf("ID: %d\nNom: %s\n", buffer.B[j].id, buffer.B[j].nom);
                    }
                    else{
                      printf("ID: %d\nNom: %s\n ( supprime)", buffer.B[j].id, buffer.B[j].nom);
                    }
                j++;
            }

        }
    }
    else if (strcmp(orgaglobale, "chainee") == 0) {
        struct tblocChaine buffer;

        // We assume that 'a' is the address of the first block
        fseek(ms, a * sizeof(struct tblocChaine), SEEK_SET);
        fread(&buffer, sizeof(struct tblocChaine), 1, ms); // Read the first block

        // Traverse the linked list of blocks
        int count = 0; // Counter for total records across all blocks
        while (buffer.next != NULL) {
            int j = 0;
            while (j < buffer.NE) { // Iterate over the records in the block
                printf("ID: %d\nNom: %s\n", buffer.B[j].id, buffer.B[j].nom);
                j++;
            }
            printf("Nombre d'enregistrements dans ce bloc: %d\n", buffer.NE);

            // Move to the next block in the chain
            buffer = *buffer.next; // Dereference the pointer to get the next block

            count++;
            if (buffer.next == NULL) {
                break; // Break if we reached the last block
            }
        }
        printf("Nombre total d'enregistrements: %d\n", count);
    }
}
  //fonction de compactage(proposition authomatique en cas d'espace insuffisant
void compactageMS(FILE *ms) {
    struct MS bufferms;
    fread(&bufferms, sizeof(struct MS), 1, ms);
    int nbrblocs = bufferms.nb;
    for (int i = 0; i < nbrblocs - 1; i++) {
        if (bufferms.m[i].occup == 0) { // Check if the current block is empty
            bufferms.m[i] = bufferms.m[i + 1]; // Move the next block to the current empty block
        }
    }
}
void chargementfichier(FILE *ms, FILE *f, FILE *MT) {
    rewind(ms); // Rewind to the beginning of the file
     initialiserMS(ms);
    char orgaglobale[51];
    int nbrblocs;
    lireMT(f, 2, &nbrblocs);  // Read number of blocks
    lireMT(f, 5, orgaglobale); // Read global organization mode
    if (strcmp(orgaglobale, "contigue") == 0) {
        struct MS bufferms;
        struct tbloc buffer;
        fread(&bufferms, sizeof(struct MS), 1, ms);  // Read the MS structure
        if (bufferms.nblibre >= nbrblocs) {  // If there are enough free blocks
            int adrs = 0;
            for (int j = 0; j < bufferms.nb; j++) {
                if (bufferms.m[j].occup == 1) {
                    adrs++;  // Find the first free block
                }
            }

            int bcons = 0;
            for (int j = adrs; j < (bufferms.nb - adrs); j++) {
                if (bufferms.m[j].occup == 0) {
                    bcons++;  // Count consecutive free blocks
                }
            }

            if (bcons >= nbrblocs) {  // If enough consecutive free blocks are found
                for (int i = 0; i < nbrblocs; i++) {
                    fread(&buffer, sizeof(struct tbloc), 1, ms);
                    bufferms.m[adrs + i] = buffer;  // Load data into the free blocks
                }
            } else {
                printf("Faire le compactage\n");
                compactageMS(ms);  // If there aren't enough consecutive blocks, compact first
                // After compacting, search for free blocks again
                adrs = 0;
                for (int j = 0; j < bufferms.nb; j++) {
                    if (bufferms.m[j].occup == 1) {
                        adrs++;
                    }
                }

                for (int i = 0; i < nbrblocs; i++) {
                    fread(&buffer, sizeof(struct tbloc), 1, ms);
                    bufferms.m[adrs + i] = buffer;  // Load data again into the free blocks
                }
            }
        } else {
            printf("On doit faire le compactage\n");
            compactageMS(ms);  // If not enough free blocks, perform compacting
        }
    } else if (strcmp(orgaglobale, "chainee") == 0) {
        struct MSc bufferms;
        struct tblocChaine buffer;
        fread(&bufferms, sizeof(struct MS), 1, ms);  // Read the MS structure for chained mode
        if (bufferms.nblibre >= nbrblocs) {  // If enough free blocks
            struct tblocChaine *current = &bufferms.tete;  // Start from the head of the chain
            while (current != NULL && current->occup == 1) {
                current = current->next;
            }

            // If a free block is found
            if (current != NULL) {
                fread(&buffer, sizeof(struct tblocChaine), 1, ms);
                current->next = &buffer;  // Link the new block into the chain
                current->occup = 1;  // Mark it as occupied
            }
        } else {
            printf("Espace insuffisant\n");
            // Compacting does not apply here because we are using linked blocks
        }
    }
}

// fonction de suppression du fichier
void suppfichier(FILE *ms, FILE *f, struct MS *MS, const char *nomFichier) {
    char orgGlobale[51], fichierActuel[51];
    int adr, taille;
    lireMT(f, 1, fichierActuel);
    lireMT(f, 4, &adr);
    lireMT(f, 2, &taille);
    lireMT(f, 5, orgGlobale);

    if (strcmp(fichierActuel, nomFichier) != 0) {
        printf("Erreur: Le fichier %s n'existe pas dans la MS\n", nomFichier);
        return;
    }

    int blocsLibres = 0;

    if (strcmp(orgGlobale, "contigue") == 0) {
        // Lib rer les blocs du fichier
        for (int i = adr; i < adr + taille; i++) {
            if (MS->m[i].occup == 1) {
                // Marquer le bloc comme libre
                MS->m[i].occup = 0;
                MS->m[i].NE = 0;
                // Mettre   jour le fichier physique
                fseek(ms, i * sizeof(struct tbloc), SEEK_SET);
                fwrite(&MS->m[i], sizeof(struct tbloc), 1, ms);

                blocsLibres++;
                MS->nblibre++;
            }
        }
    } else if(strcmp(orgGlobale, "chainee") == 0){

        struct tblocChaine buffer;
        long adresseCourante = adr;

        while (adresseCourante != -1 && blocsLibres < taille) {
            fseek(ms, adresseCourante * sizeof(struct tblocChaine), SEEK_SET);
            fread(&buffer, sizeof(struct tblocChaine), 1, ms);

            if (buffer.occup == 1) {
                long prochainBloc = (long)buffer.next; // Sauvegarder l'adresse du prochain bloc
                buffer.occup = 0; // Marquer le bloc comme libre
                buffer.NE = 0;
                buffer.next = NULL;
                fseek(ms, adresseCourante * sizeof(struct tblocChaine), SEEK_SET);
                fwrite(&buffer, sizeof(struct tblocChaine), 1, ms);
                MS->m[adresseCourante].occup = 0;
                MS->m[adresseCourante].NE = 0;

                blocsLibres++;
                MS->nblibre++;

                adresseCourante = prochainBloc;
            }
        }
    }

    // Supprimer le fichier des m tadonn es
    remove(nomFichier);  // Supprimer le fichier physique si existant

}

}

// Function to defragment a specific file
void defragmentFile(struct MS *disk, struct tMetaD *meta, const char *fileName) {
int writeIndex = 0; // Position for the next used block
int found = 0;

for (int readIndex = 0; readIndex < disk->nb; readIndex++) {
    if (disk->m[readIndex].occup == 1) { // If the block is used
        //checki,g if the block belongs to the specified file
        for (int j = 0; j < disk->m[readIndex].NE; j++) {
            if (strcmp(disk->m[readIndex].B[j].nom, fileName) == 0) {
                found = 1;
                if (readIndex != writeIndex) {
                    // move the block data to the write position
                    disk->m[writeIndex] = disk->m[readIndex];

                    // supp the old block
                    disk->m[readIndex].occup = 0;
                    disk->m[readIndex].NE = 0;

                    for (int k = 0; k < MAX_FB; k++) {
                        disk->m[readIndex].B[k].id = -1;
                        memset(disk->m[readIndex].B[k].nom, 0, 21);
                        disk->m[readIndex].B[k].supp = 0;
                    }
                }

                //if (strcmp(meta->modeorgaglobale, "Chained") == 0) {
                    //if (writeIndex > 0) {
                    //    disk->m[writeIndex - 1].next = &disk->m[writeIndex];
                    //}
                 //   disk->m[writeIndex].next = NULL; //lst block in the chain
               // }

                writeIndex++;
                break; //,move to the next block
            }
        }
    }
}

if (found) {
    printf("File %s defragmented successfully.\n", fileName);
} else {
    printf("File %s not found.\n", fileName);
}

// Function to initialize the disk
void initializeDisk(struct MS *disk, struct tMetaD *meta) {
    // Initialize metadata
    strcpy(meta->modeorgaglobale, "");  // pas de global organization set yet
    strcpy(meta->modeorgainterne, ""); // pas de internal organization set yet
    meta->tailleblocs = sizeof(struct tblocChaine);
    meta->taillenreg = sizeof(struct tenr);
    meta->adrprebloc = -1;  // No files yet

    // Initialize secondary memory (blocks)
    disk->nb = DISK_SIZE;
    disk->nblibre = DISK_SIZE;

    for (int i = 0; i < DISK_SIZE; i++) {
        disk->m[i].occup = 0;  // Mmrk block as free
        disk->m[i].NE = 0;     // No entries in the block
         // No chaining initially

        // supp all entries in the block
        for (int j = 0; j < MAX_FB; j++) {
            disk->m[i].B[j].id = -1;  // Empty entry
            memset(disk->m[i].B[j].nom, 0, 21); // supp name
            disk->m[i].B[j].supp = 0;  // Not deleted
        }
    }
    printf("Disk initialized with %d blocks, each of size %lu bytes.\n", DISK_SIZE, sizeof(struct tblocChaine));
}

// Function to display metadata in a tabular format (genre un tableau ASCII)
void displayMetadata(struct tMetaD *meta) {
    printf("\n+-----------------------------------------+\n");
    printf("|              Metadata Table             |\n");
    printf("+-----------------------------------------+\n");
    printf("| %-20s | %-20s |\n", "Attribute", "Value");
    printf("+-----------------------------------------+\n");
    printf("| %-20s | %-20d |\n", "Block Size", meta->tailleblocs);
    printf("| %-20s | %-20d |\n", "Record Size", meta->taillenreg);
    printf("| %-20s | %-20d |\n", "First Block Address", meta->adrprebloc);
    printf("| %-20s | %-20s |\n", "Global Organization",
           strlen(meta->modeorgaglobale) ? meta->modeorgaglobale : "Not set");
    printf("| %-20s | %-20s |\n", "Internal Organization",
           strlen(meta->modeorgainterne) ? meta->modeorgainterne : "Not set");
    printf("+-----------------------------------------+\n");
}

// Function to display the current state of the disk
void displayDiskStatus(struct MS *disk) {
    printf("\nDisk Status:\n");
    printf("Total Blocks: %d\n", disk->nb);
    printf("Free Blocks: %d\n", disk->nblibre);
    printf("Used Blocks: %d\n", disk->nb - disk->nblibre);
    printf("Blocks:\n");

    for (int i = 0; i < DISK_SIZE; i++) {
        printf("Block %d: %s\n", i, disk->m[i].occup ? "Used" : "Free");
    }
}

// Function to simulate adding data to a block
void addDataToBlock(struct MS *disk, int blockIndex, const struct tenr *entry) {
    if (blockIndex >= 0 && blockIndex < disk->nb && disk->m[blockIndex].occup == 0 && disk->m[blockIndex].NE < MAX_FB) {
        disk->m[blockIndex].B[disk->m[blockIndex].NE] = *entry; // add entry to the block
        disk->m[blockIndex].NE++;  // increment the number of entries
        disk->m[blockIndex].occup = 1; // mark block as occupied
        disk->nblibre--; // decrease the free block count
    } else {
        printf("Error: Cannot add data to block %d.\n", blockIndex);
    }
}

// Function to clear all data from the disk
void clearDisk(struct MS *disk, struct tMetaD *meta) {
    // Reset all blocks to their initial state
    for (int i = 0; i < disk->nb; i++) {
        disk->m[i].occup = 0;  // Mark block free
        disk->m[i].NE = 0;     // Reset nbr of entries
        //disk->m[i].next = NULL; // Remove chaining

        // Clear all entries in the block
        for (int j = 0; j < MAX_FB; j++) {
            disk->m[i].B[j].id = -1;  // Empty entry
            memset(disk->m[i].B[j].nom, 0, 21); // Clear name
            disk->m[i].B[j].supp = 0;  // Not deleted
        }
    }

    // Reset meta data
    disk->nblibre = disk->nb;
    strcpy(meta->modeorgaglobale, "");  // Re set global organization mode
    strcpy(meta->modeorgainterne, ""); // Re set internal organization mode
    meta->adrprebloc = -1;  // Re set address of the first block

    printf("All data has been cleared. The disk is now empty.\n");
}

// Function to defragment the disk
void defragmentDisk(struct MS *disk, struct tMetaD *meta) {
    int writeIndex = 0; // position for the next used block
    int freeCount = 0;

    for (int readIndex = 0; readIndex < disk->nb; readIndex++) {
        if (disk->m[readIndex].occup == 1) { // If the block is used
            if (readIndex != writeIndex) {
                // Move the block data to the write position
                disk->m[writeIndex] = disk->m[readIndex];

                // Clear the old block
                disk->m[readIndex].occup = 0;
                disk->m[readIndex].NE = 0;
                //disk->m[readIndex].next = NULL;

                for (int j = 0; j < MAX_FB; j++) {
                    disk->m[readIndex].B[j].id = -1;
                    memset(disk->m[readIndex].B[j].nom, 0, 21);
                    disk->m[readIndex].B[j].supp = 0;
                }
            }

            // Update chaining for chained organization
          /*  if (strcmp(meta->modeorgaglobale, "Chained") == 0) {
                if (writeIndex > 0) {
                    disk->m[writeIndex - 1].next = &disk->m[writeIndex];
                }
                disk->m[writeIndex].next = NULL; // last block in the chain
            }*/

            writeIndex++;
        } else {
            freeCount++;
        }
    }

    // update metadata
    disk->nblibre = freeCount;
    printf("Disk defragmentation complete. All used blocks are now contiguous.\n");
}

// Function to update metadata
void updateMetadata(struct MS *disk, struct tMetaD *meta) {
    int freeCount = 0, usedCount = 0, firstBlock = -1;

    for (int i = 0; i < disk->nb; i++) {
        if (disk->m[i].occup == 0) {
            freeCount++;
        } else {
            usedCount++;
            if (firstBlock == -1) {
                firstBlock = i;  // capture the address of the first used block
            }
        }
    }

    // Update metadata fields
        disk->nblibre = freeCount;
    meta->adrprebloc = firstBlock;
    printf("\nMetadata Updated:\n");
    printf("Free Blocks: %d\n", freeCount);
    printf("Used Blocks: %d\n", usedCount);
    printf("First Block Address: %d\n", firstBlock);
}

// Function to check if there are enough free blocks
int checkFreeBlocks(struct MS *disk, int requiredBlocks) {
    if (disk->nblibre >= requiredBlocks) {
        return 1; // enough blocks available
    } else {
        printf("Error: Not enough free blocks. Required: %d, Available: %d.\n", requiredBlocks, disk->nblibre);
        return 0; // insufficient free blocks
    }
}


int main(){
    int choice; int id;
    struct MS ms;
    struct tMetaD meta;
  do{

           printf("Welcome to file management system\n");
    printf("\n--------------------------------------------------------------");
    printf("    1.Initialize the secondary memory    2. Create a file + load it in SM \n");
    printf("    3.Display the secondary memory       4.Dsiplay the metadata           \n");
    printf("    5.Search for a record                6.Insert a record                \n");
    printf("    7.Delete a record (logical/physical)  \n ");
    printf("    8.Defragment a file                  9.Delete a file \n");
    printf("    10.Rename a file                     11.Compact the secondary memory \n");
    printf("    12.Empty the secondary memory        13.Exit the program  \n");
    printf("\n--------------------------------------------------------------");
    printf("Please select the function you want to perform (1-13)");
    scanf("%d", &choice);

    switch(choice){
    case 1:{
        printf ("Initialize the secondary memory\n");
        initialiserMS(&ms);
        afficherEtatMS(&ms);
    } break;
    case 2: {
        int nbrEnreg; int choixGlobale; int choixIntern ; char nomFichier[51];
        printf("Create a file \n");
        printf("Enter the name of the file");
        scanf("%s",&nomFichier);
        printf("\n");
        printf("Enter the number of the records");
        scanf("%d",&nbrEnreg);
        printf("\n");
        printf ("Enter the global mode of organization  (0 for contiguous / 1 for linked )");
        scanf("%d", &choixGlobale);
        printf("\n");
        printf("Enter the intern mode of organization ( 0 for sorted / 1 for unsorted");
        scanf("%d", &choixIntern);
        CreeFichier(nomFichier, nbrEnreg,choixGlobale ,choixIntern);
       chargementfichier(&ms , &f ,&metaf);
        printf("\nEtat de la memoire secondaire apres chargement:\n");
        afficherEtatMS(&ms);
         break;
         }
        case 3:{
           printf("Display the secondary memory\n");
           afficherEtatMS(&ms);
           break;
        }
        case 4:{
            printf("Display the metadat\n");
            displayMetadata(&meta);
            break;
        }case 5:{
            int resultat [2];
            initialiserMS(&ms);
            printf("Search a record\n");
            printf("Give the id");
            scanf ("%d", &id);
            recherchenregistement(&ms, &f , id , resultat );
             printf("\n=== Resultat de la recherche pour l'ID %d ===\n", id);
             if (adrs[0] == -1 && adrs[1] == -1) {
             printf(RED"Enregistrement non trouve.\n"RESET);
                } else {
           printf(GREEN"Enregistrement trouve !\n"RESET);
            printf("Position :\n");
            printf(" - Bloc numero : %d\n", adrs[0]);
            printf(" - Position dans le bloc : %d\n", adrs[1]);
          }
          printf("=====================================\n");
            printf("\n");
            break;
        }case 6:{
           printf("Insert a record\n");
          insertion(&ms,&f);
           printf("\n");
           afficherEtatMS(&ms);
           break;
        } case 7:{
            int choice2;
            printf("Delete a record\n");
            printf("Enter 1 for logical deletion and 2 for the physical deletion\n");
            scanf("%d", &choice2);
            switch(choice2){
             case 1:{
               printf("Logical deletion\n");
               printf("give the id");
               scanf("%d", &id);
              supprlogique(&ms , &f , id);
              afficherEtatMS(&ms);
               break;
             } case 2: {
                printf("Physical deletion");
                printf("give the id");
                scanf("%d", &id);
                supprphysique(&ms , &f , id);
                afficherEtatMS(&ms);
                break;
             }
            }
            break;
          }case 8:{
            printf("Defragmentation of a file\n");
            char fileName [51];
            printf("Give the file name");
            scanf("%s", &fileName);
            defragmentFile(&ms , &meta, fileName);
            break;
          } case 9: {
           printf("File deletion\n");
           char fileName [51];
           char metaName [55] ;
           printf("Give the name of the file");
           scanf("%s", fileName);
           strcpy(metaName, "meta");
           strcat(metaName, fileName);
            FILE *f = fopen(fileName, "rb+");
              FILE *meta = fopen(metaName, "rb+");
        suppfichier(FILE *meta, FILE *f, struct MS *MS, const char *fileName);
           afficherEtatMS(&ms);
           break;
          } case 10:{
              char oldFileName [51]; char newFileName [51];
            printf("Rename a file\n");
            printf("Give the name of the file");
            scanf("%s", &oldFileName);
            printf("Give the new name");
            scanf("%s", &newFileName);
            rename(oldFileName, newFileName);
            break;
          } case 11:{
           printf("Compacting the secondary memory");
          // compactageMS(ms);
           break;
          } case 12:{
            printf("Empty the secondary memory");
            // fonction
            break;
          } case 13:{
            printf("Exit the program\n");
            break;
          }
          default:
          printf("Ivalid choice\n");
    }
  }while(!13);

    return 0;
}

