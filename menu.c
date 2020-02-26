#include <stdio.h>
#include <stdlib.h>

int checkFile();

int main() {
  int choice, checkFileResult;

  checkFileResult = checkFile();

  if (checkFileResult == 1) {
    printf("The file 'tar_teams.csv' exists. Directing to menu...\n\n\n");
  }
  else {
    printf("The file 'tar_teams.csv' does not exist. Create the file first and add to directory.");
    exit(0);
  }
  
  do {
    printf("* * * * * * * * * *\n");
    printf("*                 *\n");
    printf("*     Tracking    *\n");
    printf("*        the      *\n");
    printf("*   AMAZING RACE  *\n");
    printf("*                 *\n");
    printf("* * * * * * * * * *\n\n\n");
    printf("Menu:\n");
    printf("1. Start a new leg\n");
    printf("2. Display information on current leg\n");
    printf("3. Display information on all legs\n");
    printf("4. Add new destination to current leg\n");
    printf("5. Display podium finishers\n");
    printf("6. Display eliminated teams\n");
    printf("7. Exit the program\n");
    scanf("%d", &choice);

    switch (choice) {
      case 1: printf("Function goes here etc.\n");
        break;
      default: printf("Thanks for your input. Now we wait.\n");
        break;
    }
  }  while (choice !=7);
}

int checkFile() {
  FILE *file;

  if ((file = fopen("tar_teams.csv", "r")) == NULL ) {
    return 0;
  }
  else {
      fclose (file);
      return 1;
  }
} 
