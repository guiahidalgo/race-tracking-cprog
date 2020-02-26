#include <stdio.h>

int main() {
  int choice;

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
