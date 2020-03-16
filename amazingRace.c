#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *teamsFileName = "tar_teams.csv";
const char *legsFileName = "tar_legs.csv";
const char *locationsFileName = "tar_locs.csv";

struct team_s
{
  int teamNo;
  char *name;
  char *member1;
  char *member2;
};

struct team_id_node
{
  int teamNo;
  struct team_id_node *next;
};

struct leg_s
{
  int legNo;
  int budget;
  int isElimination;
};

struct location_s
{
  int legNo;
  int destinationId;
  char *name;
  char *city;
  char *country;
  char *task;
  char *type;
  int isPitstop;
  char isEndOfLeg;
  struct team_id_node *teamIdStart;
};

char *getfield(char *line, int num);
int printMenu();
void loadTeams(struct team_s *ptr, int *count);
void loadLegs(struct leg_s *ptr, int *count);
void loadLocations(struct location_s *ptr, int *count);
void addLeg(struct leg_s *ptr, int *legs_count);
void saveLegsToFile(struct leg_s legs[], int legs_count);
void printCurrentLeg(struct leg_s legs[], int legs_count, struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int showWinnersOnly);
void printLeg(struct leg_s leg, struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int showWinnersOnly);
void printLegAtIndex(struct leg_s legs[], struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int index, int showWinnersOnly);
void matchLegLocations(struct leg_s leg, struct location_s locations[], int locations_count, struct location_s *ptr, int *matches_count);
struct team_s getTeam(int teamNo, struct team_s teams[], int teams_count);
int inputNumber(char *inputlabel);
int inputBoolean(char *inputBooleanlabel);
int inputLegNumber(char *inputlabel, struct leg_s legs[], int legsCount);
char* inputString(char *inputLabel);
void displayLegInfoAll();
void addLocationToCurrentLeg(struct location_s locations[], int numberoflocations);

int main()
{
  printf("* * * * * * * * * *\n");
  printf("*                 *\n");
  printf("*     Tracking    *\n");
  printf("*        the      *\n");
  printf("*   AMAZING RACE  *\n");
  printf("*                 *\n");
  printf("* * * * * * * * * *\n\n\n");

  if (access(teamsFileName, F_OK) == -1)
  {
    printf("Please create tar_teams.csv first");
    return 1;
  }

  struct team_s teams[100];
  int teams_count = 0;
  loadTeams(teams, &teams_count);

  struct leg_s legs[100];
  int legs_count = 0;
  loadLegs(legs, &legs_count);

  struct location_s locations[100];
  int locations_count = 0;
  loadLocations(locations, &locations_count);

  int choice;

  do
  {
    choice = printMenu();

    switch (choice)
    {
    case 1:
      addLeg(legs, &legs_count);
      break;
    case 2:
      printCurrentLeg(legs, legs_count, locations, locations_count, teams, teams_count, 0);
      break;
    case 3:
      displayLegInfoAll();
      break;
    case 4:
      addLocationToCurrentLeg(locations, locations_count);
      break;
    case 5:
      printCurrentLeg(legs, legs_count, locations, locations_count, teams, teams_count, 1);
      break;
    case 6:
      displayEliminatedTeams();
      break;
    case 7:
      break;
    default:
      printf("This feature is not implemented yet.\n\n");
      break;
    }
  } while (choice != 7);

  return 0;
}


void displayEliminatedTeams(){

    struct leg_s theLeg[100];
    int totalNumberOfLegs = 0;
    loadLegs(theLeg, &totalNumberOfLegs);
    
    struct team_s teams[100];
    int teams_count = 0;
    loadTeams(teams, &teams_count);

    struct location_s locations[100];
    int locations_count = 0;
    loadLocations(locations, &locations_count);

    printf("THE AMAZING RACE (Season 20) \n\n");
    printf("Eliminated Teams\n");
    printf("--------------------\n");

    for (int i = 0; i < totalNumberOfLegs; i++)
    {
        struct leg_s leg = theLeg[i];
        for(int ctr = 0; ctr < locations_count; ctr++) {
          struct location_s locat = locations[ctr];
          struct location_s lastLocation = locations[locat.legNo];
          struct team_id_node *ptr = lastLocation.teamIdStart;
      
          if(locat.legNo == leg.legNo && leg.isElimination == 1){

            if (ptr != NULL)
            {
              int theLastTeam = 0;
              while (ptr != NULL)
              {
                theLastTeam = ptr->teamNo;
                ptr = ptr->next;
              }

              struct team_s team = getTeam(theLastTeam, teams, teams_count);
              printf("%s & %s (%s) - Eliminated: Leg #%d\n  ", team.member1, team.member2, team.name, leg.legNo);
            }
          }

        }
     
    }


}


//Functions
int printMenu()
{
  int choice;
  do
  {
    printf("\n");
    printf("Menu:\n");
    printf(" 1. Start a new leg\n");
    printf(" 2. Display information on current leg\n");
    printf(" 3. Display information on all legs\n");
    printf(" 4. Add new destination to current leg\n");
    printf(" 5. Display podium finishers\n");
    printf(" 6. Display eliminated teams\n");
    printf(" 7. Exit program\n");
    printf("\n");
    printf("What do you wish to do? ");
    scanf("%d", &choice);

    if (choice < 1 || choice > 7)
    {
      printf("\nInvalid input.\n");
    }
  } while (choice < 1 || choice > 7);

  return choice;
}

void addLeg(struct leg_s *ptr, int *legs_count)
{
  int legNo;
  float budget;
  int isElimination;

  int valid = 1;

  legNo = inputLegNumber("Leg number: ", ptr, *legs_count);
  budget = inputNumber("Budget: ");
  isElimination = inputBoolean("Elimination [1]Yes [0]No: ");

  if (inputBoolean("\nProceed with saving? [1]Yes [0]No : ") == 1)
  {
    struct leg_s *start = ptr;
    ptr += *legs_count;
    ptr->legNo = legNo;
    ptr->budget = budget;
    ptr->isElimination = isElimination;

    *legs_count += 1;

    saveLegsToFile(start, *legs_count);
    printf("\nLeg saved.\n");
  }
}

void printLeg(struct leg_s leg, struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int showWinnersOnly) {
  
  struct location_s lastLocation = locations[locations_count - 1];
  struct team_id_node *ptr = lastLocation.teamIdStart;
 
  int winningplace = 1;
  if(showWinnersOnly == 1) {    
      if(lastLocation.isEndOfLeg == 1) 
      {
        printf("THE AMAZING RACE \n \n");
        if (ptr != NULL) {
          while (ptr != NULL) {
              switch(winningplace)
              {
                  case 1:
                  printf("Winning Team: ");
                  break;

                  case 2:
                  printf("2nd Place: ");
                  break;

                  case 3:
                  printf("3rd Place: ");
                  break;
              }

              if (leg.isElimination && ptr->next == NULL)
                  break;
              struct team_s team = getTeam(ptr->teamNo, teams, teams_count);
              printf("%s & %s (%s)\n", team.member1, team.member2, team.name);
              ptr = ptr->next;
              winningplace++;
              if(winningplace > 3)
                  break;
          }
        }
      }
      else
      {
        printf("The race is not yet over..");
      }
      
      
  } 
  else
  {
    printf("\n");
    printf("LEG #%d", leg.legNo);
    if (leg.isElimination)
      printf(" (Elimination)\n");
    else
      printf(" (Non-elimination)\n");

    if (ptr != NULL)
    {
      printf("  Remaining Teams:\n");
      while (ptr != NULL)
      {
        if (leg.isElimination && ptr->next == NULL)
          break;
        struct team_s team = getTeam(ptr->teamNo, teams, teams_count);
        printf("    - %s & %s (%s)\n", team.member1, team.member2, team.name);
        ptr = ptr->next;
      }
    }

    for (int i = 0; i < locations_count; i++)
    {
      struct location_s location = locations[i];
      printf("  Destination %d: %s (%s, %s)\n", location.destinationId, location.name, location.city, location.country);
      printf("    Task: %s\n", location.task);
      printf("    Type: %s\n", location.type);
    }
  }
}

struct team_s getTeam(int teamNo, struct team_s teams[], int teams_count)
{
  struct team_s matched;

  for (int i = 0; i < teams_count; i++)
  {
    struct team_s team = teams[i];
    if (team.teamNo == teamNo)
    {
      matched = team;
      break;
    }
  }

  return matched;
}

void matchLegLocations(struct leg_s leg, struct location_s locations[], int locations_count, struct location_s *ptr, int *matches_count)
{
  for (int locIdx = 0; locIdx < locations_count; locIdx++)
  {
    struct location_s location = locations[locIdx];

    if (location.legNo == leg.legNo)
    {
      *ptr = location;
      *matches_count += 1;

      ptr++;
    }
  }
}

void printLegAtIndex(struct leg_s legs[], struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int index, int showWinnersOnly)
{
  struct leg_s leg = legs[index];

  struct location_s matched_locations[locations_count];
  int matched_locs_count = 0;

  matchLegLocations(leg, locations, locations_count, matched_locations, &matched_locs_count);

  printLeg(leg, matched_locations, matched_locs_count, teams, teams_count, showWinnersOnly);
}

void printCurrentLeg(struct leg_s legs[], int legs_count, struct location_s locations[], int locations_count, struct team_s teams[], int teams_count, int showWinnersOnly)
{
  printLegAtIndex(legs, locations, locations_count, teams, teams_count, legs_count - 1, showWinnersOnly);
}

// Teams

void loadTeams(struct team_s *ptr, int *count)
{
  FILE *stream = fopen(teamsFileName, "r");

  char line[100];
  while (fgets(line, 100, stream))
  {
    char *tmp = strdup(line);

    ptr->teamNo = atoi(strdup(getfield(strdup(line), 1)));
    ptr->name = strdup(getfield(strdup(line), 2));
    ptr->member1 = strdup(getfield(strdup(line), 3));
    ptr->member2 = strdup(getfield(strdup(line), 4));

    free(tmp);

    ptr++;
    *count += 1;
  }
}

// Legs

void displayLegInfoAll() {
    struct leg_s theLeg[100];
    int totalNumberOfLegs = 0;
    loadLegs(theLeg, &totalNumberOfLegs);

    for (int i = 0; i < totalNumberOfLegs; i++)
    {
        struct team_s teams[100];
        int teams_count = 0;
        loadTeams(teams, &teams_count);

        struct leg_s legs[100];
        int legs_count = 0;
        loadLegs(legs, &legs_count);

        struct location_s locations[100];
        int locations_count = 0;
        loadLocations(locations, &locations_count);

        printLegAtIndex(legs, locations, locations_count, teams, teams_count, i, 0);
    }
       
}

void loadLegs(struct leg_s *ptr, int *count)
{
  FILE *stream = fopen(legsFileName, "r");

  char line[1000];
  while (fgets(line, 1000, stream))
  {
    char *tmp = strdup(line);

    ptr->legNo = atoi(strdup(getfield(strdup(line), 1)));
    ptr->budget = atoi(strdup(getfield(strdup(line), 2)));
    ptr->isElimination = atoi(strdup(getfield(strdup(line), 3)));

    free(tmp);

    ptr++;
    *count += 1;
  }

  fclose(stream);
}

void saveLegsToFile(struct leg_s legs[], int legs_count)
{
  FILE *stream = fopen(legsFileName, "w");

  for (int i = 0; i < legs_count; i++)
  {
    struct leg_s leg = legs[i];
    fprintf(stream, "%d,%d,%d", leg.legNo, leg.budget, leg.isElimination);

    if (i < legs_count - 1)
    {
      fprintf(stream, "\n");
    }
  }

  fclose(stream);
}

// Locations

void addLocationToCurrentLeg(struct location_s locations[], int numberoflocations)
{
  
  struct location_s lastLocation = locations[numberoflocations -1];
  if(lastLocation.isEndOfLeg == 1) {
    printf("Unable to Add New Destination. The race is already Over..");
  }
  else {
    struct leg_s currentlegs[100];
    int currentlegs_count = 0;
    loadLegs(currentlegs, &currentlegs_count);
    struct leg_s curr_leg = currentlegs[currentlegs_count - 1];

    // TODO: Check if the leg has already ended

    int legNo = curr_leg.legNo;

    int destinationId = inputNumber("Destination ID: ");
    char* name = inputString("Destination Name: ");
    char* city = inputString("City: ");
    char* country = inputString("Country: ");
    char* task = inputString("Task: ");
    char* type = inputString("Type: ");
    int isPitstop = inputBoolean("Pitstop? [1]Yes [0]No : ");
    int isEndOfLeg = inputBoolean("End of Leg? [1]Yes [0]No : ");

    FILE *stream = fopen(locationsFileName, "a");
    fprintf(stream, "%d,%d,%s,%s,%s,%s,%s,%d,%d", legNo, destinationId, name, city, country, task, type, isPitstop, isEndOfLeg);
    fprintf(stream, "\n");
    fclose(stream);
  }


}

void loadLocations(struct location_s *ptr, int *count)
{
  FILE *stream = fopen(locationsFileName, "r");

  char line[1024];
  while (fgets(line, 1024, stream))
  {
    char *tmp = strdup(line);

    ptr->legNo = atoi(strdup(getfield(strdup(line), 1)));
    ptr->destinationId = atoi(strdup(getfield(strdup(line), 2)));
    ptr->name = strdup(getfield(strdup(line), 3));
    ptr->city = strdup(getfield(strdup(line), 4));
    ptr->country = strdup(getfield(strdup(line), 5));
    ptr->task = strdup(getfield(strdup(line), 6));
    ptr->type = strdup(getfield(strdup(line), 7));
    ptr->isPitstop = atoi(strdup(getfield(strdup(line), 8)));
    ptr->isEndOfLeg = atoi(strdup(getfield(strdup(line), 9)));
    ptr->teamIdStart = NULL;

    int idIndex = 10;
    char *rawId = getfield(strdup(line), idIndex);

    struct team_id_node *curr = ptr->teamIdStart;

    while (rawId != NULL)
    {
      struct team_id_node *node = (struct team_id_node *)malloc(sizeof(struct team_id_node));
      node->teamNo = atoi(strdup(rawId));

      if (curr == NULL)
      {
        ptr->teamIdStart = node;
        curr = node;
      }
      else
      {
        curr->next = node;
        curr = node;
      }

      idIndex++;
      rawId = getfield(strdup(line), idIndex);
    }

    free(tmp);

    ptr++;
    *count += 1;
  }
}

// CSV management

char *getfield(char *line, int num)
{
  char *token;
  for (token = strtok(line, ","); token && *token; token = strtok(NULL, ",\n"))
  {
    if (!--num)
      return token;
  }

  return NULL;
}

//validators.

int inputBoolean(char *inputBooleanlabel)
{
  int flag;
  do
  {
    flag = inputNumber(inputBooleanlabel);
    if (flag > 2)
      printf("\nInput only 1 or 0\n");
  } while (flag != 1 && flag != 0);
  return flag;
}

int inputNumber(char *inputlabel)
{
  int inputval;
  char ch;

  int valid = 1;
  do
  {
    printf("\n%s", inputlabel);
    valid = scanf("%d", &inputval);
 
    ch = getchar();
    while (ch != '\n')
      ch = getchar();

    if (valid == 0)
      printf("\nInvalid input. Enter a number.\n");

  } while (valid != 1);

  return inputval;
}

int inputLegNumber(char *inputlabel, struct leg_s legs[], int legsCount)
{
  int inputVal;
  int valid = 1;
  do
  {
    valid = 1;
    inputVal = inputNumber(inputlabel);

    for (int i = 0; i < legsCount; i++)
    {
      struct leg_s leg = legs[i];
      if (leg.legNo == inputVal)
      {
        valid = 0;
        break;
      }
    }

    if (valid == 0)
    {
      printf("\nInvalid input. The leg number already exists.\n");
    }
  } while (valid == 0);

  return inputVal;
}

char* inputString(char *inputLabel) {
  char inputVal[100];

  printf("\n%s", inputLabel);
  scanf("%[^\n]%*c", inputVal);

  return strdup(inputVal);
}
