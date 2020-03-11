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

struct leg_s
{
  int legNo;
  float budget;
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
};

const char *getfield(char *line, int num);
int printMenu();
void loadTeams(struct team_s *ptr, int *count);
void loadLegs(struct leg_s *ptr, int *count);
void loadLocations(struct location_s *ptr, int *count);
void addLeg(struct leg_s *ptr, int *legs_count);
void saveLegsToFile(struct leg_s legs[], int legs_count);
void printCurrentLeg(struct leg_s legs[], int legs_count, struct location_s locations[], int locations_count);
void printLeg(struct leg_s leg, struct location_s locations[], int locations_count);
void printLegAtIndex(struct leg_s legs[], struct location_s locations[], int locations_count, int index);
void matchLegLocations(struct leg_s leg, struct location_s locations[], int locations_count, struct location_s *ptr, int *matches_count);

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
      printCurrentLeg(legs, legs_count, locations, locations_count);
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

  do
  {
    valid = 1;

    printf("\nLeg number: ");
    scanf("%d", &legNo);
    printf("Budget: ");
    scanf("%f", &budget);
    printf("Elimination (1:yes/0:no): ");
    scanf("%d", &isElimination);

    if (isElimination < 0 || isElimination > 1)
    {
      valid = 0;
    }

    struct leg_s *start = ptr;
    int current = 0;

    //checking if leg already exists in the file
    while (current < *legs_count)
    {
      if (start->legNo == legNo)
      {
        valid = 0;
        break;
      }

      start++;
      current++;
    }

    if (valid == 0)
    {
      printf("\nInvalid input. Please try again.\n");
    }

  } while (valid == 0);

  int confirmed;
  do
  {
    printf("\nProceed with saving (1:yes/0:no)? ");
    scanf("%d", &confirmed);

    if (confirmed < 0 || confirmed > 1)
    {
      printf("\nInvalid input. Please try again.\n");
    }
  } while (confirmed < 0 || confirmed > 1);

  if (confirmed == 0)
    return;

  struct leg_s *start = ptr;
  ptr += *legs_count;

  ptr->legNo = legNo;
  ptr->budget = budget;
  ptr->isElimination = isElimination;

  *legs_count += 1;

  saveLegsToFile(start, *legs_count);

  printf("\nLeg saved.\n");
}

void printLeg(struct leg_s leg, struct location_s locations[], int locations_count) {
  printf("\n");
  printf("LEG #%d", leg.legNo);
  if (leg.isElimination)
    printf(" (Elimination)\n");
  else
    printf("\n");
  
  for (int i = 0; i < locations_count; i++) {
    struct location_s location = locations[i];
    printf("  Destination %d: %s (%s, %s)\n", location.destinationId, location.name, location.city, location.country);
    printf("    Task: %s\n", location.task);
    printf("    Type: %s\n", location.type);
  }
}

void matchLegLocations(struct leg_s leg, struct location_s locations[], int locations_count, struct location_s *ptr, int *matches_count) {
  for (int locIdx = 0; locIdx < locations_count; locIdx++) {
    struct location_s location = locations[locIdx];

    if (location.legNo == leg.legNo) {
      *ptr = location;
      *matches_count += 1;

      ptr++;
    }
  }
}

void printLegAtIndex(struct leg_s legs[], struct location_s locations[], int locations_count, int index) {
  struct leg_s leg = legs[index];

  struct location_s matched_locations[locations_count];
  int matched_count = 0;

  matchLegLocations(leg, locations, locations_count, matched_locations, &matched_count);

  printLeg(leg, matched_locations, matched_count);
}

void printCurrentLeg(struct leg_s legs[], int legs_count, struct location_s locations[], int locations_count)
{
  printLegAtIndex(legs, locations, locations_count, legs_count - 1);
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

void loadLegs(struct leg_s *ptr, int *count)
{
  FILE *stream = fopen(legsFileName, "r");

  char line[1000];
  while (fgets(line, 1000, stream))
  {
    char *tmp = strdup(line);

    ptr->legNo = atoi(strdup(getfield(strdup(line), 1)));
    ptr->budget = atof(strdup(getfield(strdup(line), 2)));
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
    fprintf(stream, "%d,%.0f,%d", leg.legNo, leg.budget, leg.isElimination);

    if (i < legs_count - 1)
    {
      fprintf(stream, "\n");
    }
  }

  fclose(stream);
}

// Locations

void loadLocations(struct location_s *ptr, int *count) {
  FILE* stream = fopen(locationsFileName, "r");

  char line[1024];
  while(fgets(line, 1024, stream)) {
    char* tmp = strdup(line);

    ptr->legNo = atoi(strdup(getfield(strdup(line), 1)));
    ptr->destinationId = atoi(strdup(getfield(strdup(line), 2)));
    ptr->name = strdup(getfield(strdup(line), 3));
    ptr->city = strdup(getfield(strdup(line), 4));
    ptr->country = strdup(getfield(strdup(line), 5));
    ptr->task = strdup(getfield(strdup(line), 6));
    ptr->type = strdup(getfield(strdup(line), 7));
    ptr->isPitstop = atoi(strdup(getfield(strdup(line), 8)));
    ptr->isEndOfLeg = atoi(strdup(getfield(strdup(line), 9)));

    free(tmp);

    ptr++;
    *count+=1;
  }
}

// CSV management

const char *getfield(char *line, int num)
{
  const char *token;
  for (token = strtok(line, ","); token && *token; token = strtok(NULL, ",\n"))
  {
    if (!--num)
      return token;
  }

  return NULL;
}