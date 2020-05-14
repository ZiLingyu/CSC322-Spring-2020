#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define CHARACTER_PC 0
#define CHARACTER_ANIMAL 1
#define CHARACTER_NPC 2

#define STATE_CLEAN 0
#define STATE_HALF_DIRTY 1
#define STATE_DIRTY 2

#define DIRECTION_NORTH 0
#define DIRECTION_SOUTH 1
#define DIRECTION_EAST 2
#define DIRECTION_WEST 3

#define FULL_ROOM 10

struct Creature {
    int type;
    int roomNum;
};

struct Room {
    int state;
    int north;
    int south;
    int east;
    int west;
};

int respectScore = 40;

void playGame(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]);
void look(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]);
void neighbors(struct Room *rooms, struct Creature *creatures, int pc_location);
void clean(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]);
void dirty(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]);
int isFull(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int roomNumber);
int validMoves(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int roomNumber);
int validMove(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int creatureNum, int roomNumber);
void leave(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int creatureNum, int roomNumber);
void otherClean(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[], int creatureNumber);
void otherDirty(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[], int creatureNumber);

int main() {
    srand(time(0));

    int pc_location = -1;

    char *roomState[3] = {
            "clean", "half-dirty", "dirty"
    };

    char *creaturesType[3] = {
            "PC", "animal", "human"
    };

    struct Room rooms[100];
    struct Creature creatures[100];

    printf("Please enter the number of rooms, the number should be within 1 and 100. \n");
    int numOfRooms;
    scanf("%d", &numOfRooms);
    while (numOfRooms <= 1 || numOfRooms >= 100) {
        printf("Please try it again. The number should be within 1 and 100. \n");
        scanf("%d", &numOfRooms);
    }

    for(int i = 0; i < numOfRooms; i++) {
        printf("Please enter five integers in which "
               "\n the first integer represents the state of room %d (0 for clean, 1 for half-dirty, 2 for dirty), "
               "\n the second integer represents the room number to the north,"
               "\n the third integer represents the room number to the south"
               "\n the fourth integer represents the room number to the east,"
               "\n and the fifth integer represents the room number to the west. (-1 signifies no neighbor. Numbering of the rooms starts at 0). \n", i);
        int state, north, south, east, west;
        scanf("%d %d %d %d %d", &state, &north, &south, &east, &west);

        while(state < 0 || state > 2 || north < -1 || north >= numOfRooms || south < -1 || south >= numOfRooms || west < -1 || west >= numOfRooms || east < -1 || east >= numOfRooms) {
            printf("Please try it again. The State is 0 for clean, 1 for half-dirty, 2 for dirty. Numbering of the rooms starts at 0, -1 signifies no neighbor. \n");
            scanf("%d %d %d %d %d", &state, &north, &south, &east, &west);
        }

        rooms[i].state = state;
        rooms[i].north = north;
        rooms[i].south = south;
        rooms[i].east = east;
        rooms[i].west = west;
    }


    printf("Please enter the number of creatures, the number should be within 1 and 100. \n");
    int numOfCreatures;
    scanf("%d", &numOfCreatures);
    while(numOfCreatures <= 1 || numOfCreatures >= 100) {
        printf("Please try it again. The number should be within 1 and 100. \n");
        scanf("%d", &numOfCreatures);
    }

    for(int i = 0; i < numOfCreatures; i++) {
        printf("Please enter two integers in which"
               "\n the first integer represents the creature type (0 for the PC, 1 for an animal, 2 for an NPC),"
               "\n and the second integer represents their locations (Location is the number of the room the creature is in. Numbering starts at 0). \n");
        int creatureType, location;
        scanf("%d %d", &creatureType, &location);
        while(creatureType < 0 || creatureType > 2 || location < 0 || location >= numOfRooms) {
            printf("Please try it again. The creature type is 0 for the PC, 1 for an animal, 2 for an NPC. Location number starts at 0. \n");
            scanf("%d %d", &creatureType, &location);
        }
        creatures[i].type = creatureType;
        creatures[i].roomNum = location;
        if(creatureType == CHARACTER_PC) {
            pc_location = i;
        }
    }
    playGame(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType);
}

void playGame(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]) {
    char input[256];
    while (respectScore > 0 && respectScore <= 80) {
        scanf("%s", input);
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }
        if (strcasecmp(input, "look") == 0) {
            look(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType);
        } else if (strcasecmp(input, "clean") == 0) {
            clean(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType);
        } else if (strcasecmp(input, "dirty") == 0) {
            dirty(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType);
        } else if (strcasecmp(input, "north") == 0) {
            if(rooms[creatures[pc_location].roomNum].north != -1) {
                creatures[pc_location].roomNum = rooms[creatures[pc_location].roomNum].north;
                printf("You leave towards the north. You are now in room %d\n", creatures[pc_location].roomNum);
            }
        } else if (strcasecmp(input, "south") == 0) {
            if(rooms[creatures[pc_location].roomNum].south != -1) {
                creatures[pc_location].roomNum = rooms[creatures[pc_location].roomNum].south;
                printf("You leave towards the south. You are now in room %d\n", creatures[pc_location].roomNum);
            }
        } else if (strcasecmp(input, "east") == 0) {
            if(rooms[creatures[pc_location].roomNum].east != -1) {
                creatures[pc_location].roomNum = rooms[creatures[pc_location].roomNum].east;
                printf("You leave towards the east. You are now in room %d\n", creatures[pc_location].roomNum);
            }
        } else if (strcasecmp(input, "west") == 0) {
            if(rooms[creatures[pc_location].roomNum].west != -1) {
                creatures[pc_location].roomNum = rooms[creatures[pc_location].roomNum].west;
                printf("You leave towards the west. You are now in room %d\n", creatures[pc_location].roomNum);
            }
        } else if (strcasecmp(input, "exit") == 0) {
            exit(0);
        } else {
            char *colonLocation = strchr(input, ':');
            if (colonLocation == NULL) {
                printf("invalid user input");
            } else {
                char *ptr;
                int creatureNumber = (int) strtol(input, &ptr, 10);
                char *command = colonLocation + 1;
                if (strcasecmp(command, "clean") == 0) {
                    otherClean(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType, creatureNumber);
                } else if (strcasecmp(command, "dirty") == 0) {
                    otherDirty(rooms, creatures, numOfRooms, numOfCreatures, pc_location, roomState, creaturesType, creatureNumber);
                } else {
                    printf("invalid user input");
                }
            }
        }
    }
}


void look(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]) {
    printf("Room %d, %s, neighbors", creatures[pc_location].roomNum,
           roomState[rooms[creatures[pc_location].roomNum].state]);
    neighbors(rooms, creatures, pc_location);
    printf(", contains:\n");
    for(int i = 0; i < numOfCreatures; i ++) {
        if(creatures[i].roomNum == creatures[pc_location].roomNum) {
            if (creatures[i].type == CHARACTER_PC) {
                printf("%s \n", creaturesType[creatures[i].type]);
            } else {
                printf("%s %d\n", creaturesType[creatures[i].type],i);
            }
        }
    }
}

void neighbors(struct Room *rooms, struct Creature *creatures, int pc_location) {
    int count = 0;
    if (rooms[creatures[pc_location].roomNum].north != -1) {
        printf(" %d to the north", rooms[creatures[pc_location].roomNum].north);
        count++;
    }
    if (rooms[creatures[pc_location].roomNum].south != -1) {
        printf(" %d to the south", rooms[creatures[pc_location].roomNum].south);
        count++;
    }
    if (rooms[creatures[pc_location].roomNum].east != -1) {
        printf(" %d to the east", rooms[creatures[pc_location].roomNum].east);
        count++;
    }
    if (rooms[creatures[pc_location].roomNum].west != -1) {
        printf(" %d to the west", rooms[creatures[pc_location].roomNum].west);
        count++;
    }
    if (count == 0) {
        printf("does not exist for room %d", creatures[pc_location].roomNum);
    }
}

void clean(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]) {
    if (rooms[creatures[pc_location].roomNum].state != STATE_CLEAN) {
        if (rooms[creatures[pc_location].roomNum].state == STATE_HALF_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_CLEAN;
        } else if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_HALF_DIRTY;
        }

        // Animal: for each animal respect score increase one
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_ANIMAL) {
                respectScore = respectScore + 1;
                printf("animal %d licks your face. Respect is now %d\n", i, respectScore);
            }
        }

        // NPC: for each NPC respect score decrease one
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_NPC) {
                respectScore = respectScore - 1;
                printf("human %d grumbles. Respect is now %d\n", i, respectScore);
                if (rooms[creatures[pc_location].roomNum].state == STATE_CLEAN ) {
                    // If the room becomes clean, NPC have to leave the room and move to an adjacent one, each leaving creature picking the adjacent room randomly.
                    // If the adjacent room is not fit for the creature either, the creature will not leave again, but will perform the action that would make the room fit for it, once.
                    // If a creature cannot go to an adjacent room because the respective adjacent room is full, the creature will try to go to another adjacent room.
                    // If all adjacent rooms are full, it will drill a hole in the ceiling and crawl out of the house, which means that this creature will disappear not only from the room but form the whole simulation.
                    // Because the PC has benn as inconsiderate as to chase a creature out of the house, every creature in the room where the PC is will "grumble" or "growl" at the PC once.
                    leave(rooms, creatures, numOfRooms, numOfCreatures, i, creatures[pc_location].roomNum);
                }
            }
        }
    }
}

void dirty(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[]) {
    if (rooms[creatures[pc_location].roomNum].state != STATE_DIRTY) {
        if (rooms[creatures[pc_location].roomNum].state == STATE_HALF_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_DIRTY;
        } else if (rooms[creatures[pc_location].roomNum].state == STATE_CLEAN) {
            rooms[creatures[pc_location].roomNum].state = STATE_HALF_DIRTY;
        }

        // Animal: for each animal respect score decrease one
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_ANIMAL) {
                respectScore = respectScore - 1;
                printf("animal %d growl to you. Respect is now %d\n", i, respectScore);
                if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
                    leave(rooms, creatures, numOfRooms, numOfCreatures, i, creatures[pc_location].roomNum);
                }
            }
        }

        // NPC: for each NPC respect score increase one
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_NPC) {
                respectScore = respectScore + 1;
                printf("human %d smiles. Respect is now %d\n", i, respectScore);
            }
        }
    }
}

void otherClean(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[], int creatureNumber) {
    if (rooms[creatures[pc_location].roomNum].state != STATE_CLEAN) {
        if (rooms[creatures[pc_location].roomNum].state == STATE_HALF_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_CLEAN;
        } else if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_HALF_DIRTY;
        }

        // Animal: for each animal respect score increase three
        if (creatures[creatureNumber].roomNum == creatures[pc_location].roomNum && creatures[creatureNumber].type == CHARACTER_ANIMAL) {
            respectScore = respectScore + 3;
            printf("animal %d licks your face a lot. Respect is now %d\n", creatureNumber, respectScore);
        }
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_ANIMAL && i != creatureNumber) {
                respectScore = respectScore + 1;
                printf("animal %d licks your face. Respect is now %d\n", i, respectScore);
                if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
                    leave(rooms, creatures, numOfRooms, numOfCreatures, i, creatures[i].roomNum);
                }
            }
        }


        // NPC: for each NPC respect score decrease three
        if (creatures[creatureNumber].roomNum == creatures[pc_location].roomNum && creatures[creatureNumber].type == CHARACTER_NPC) {
            respectScore = respectScore - 3;
            printf("human %d grumbles a lot. Respect is now %d\n", creatureNumber, respectScore);
            if (rooms[creatures[pc_location].roomNum].state == STATE_CLEAN ) {
                leave(rooms, creatures, numOfRooms, numOfCreatures, creatureNumber, creatures[creatureNumber].roomNum);
            }
        }
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_NPC && i != creatureNumber) {
                respectScore = respectScore - 1;
                printf("human %d grumbles. Respect is now %d\n", i, respectScore);
                if (rooms[creatures[pc_location].roomNum].state == STATE_CLEAN ) {
                    leave(rooms, creatures, numOfRooms, numOfCreatures, i, creatures[i].roomNum);
                }
            }
        }
    }
}

void otherDirty(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int pc_location, char *roomState[], char *creaturesType[], int creatureNumber) {
    if (rooms[creatures[pc_location].roomNum].state != STATE_DIRTY) {
        if (rooms[creatures[pc_location].roomNum].state == STATE_HALF_DIRTY) {
            rooms[creatures[pc_location].roomNum].state = STATE_DIRTY;
        } else if (rooms[creatures[pc_location].roomNum].state == STATE_CLEAN) {
            rooms[creatures[pc_location].roomNum].state = STATE_HALF_DIRTY;
        }

        // Animal: for each animal respect score decrease three
        if (creatures[creatureNumber].roomNum == creatures[pc_location].roomNum &&
            creatures[creatureNumber].type == CHARACTER_ANIMAL) {
            respectScore = respectScore - 3;
            printf("animal %d growls a lot to your face. Respect is now %d\n", creatureNumber, respectScore);
            if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
                leave(rooms, creatures, numOfRooms, numOfCreatures, creatureNumber, creatures[creatureNumber].roomNum);
            }
            for (int i = 0; i < numOfCreatures; i++) {
                if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_ANIMAL &&
                    i != creatureNumber) {
                    respectScore = respectScore - 1;
                    printf("animal %d growls to your face. Respect is now %d\n", i, respectScore);
                    if (rooms[creatures[pc_location].roomNum].state == STATE_DIRTY) {
                        leave(rooms, creatures, numOfRooms, numOfCreatures, i, creatures[i].roomNum);
                    }
                }
            }
        }


        // NPC: for each NPC respect score increase three
        if (creatures[creatureNumber].roomNum == creatures[pc_location].roomNum &&
            creatures[creatureNumber].type == CHARACTER_NPC) {
            respectScore = respectScore + 3;
            printf("human %d smiles a lot. Respect is now %d\n", creatureNumber, respectScore);
        }
        for (int i = 0; i < numOfCreatures; i++) {
            if (creatures[i].roomNum == creatures[pc_location].roomNum && creatures[i].type == CHARACTER_NPC &&
                i != creatureNumber) {
                respectScore = respectScore + 1;
                printf("human %d smiles. Respect is now %d\n", i, respectScore);
            }
        }
    }
}

int isFull(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int roomNumber) {
    int count = 0;
    for(int i = 0; i < numOfCreatures; i++) {
        if(creatures[i].roomNum == roomNumber) {
            count++;
        }
    }
    return count == FULL_ROOM;
}

int validMoves(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int roomNumber) {
    int count = 0;
    if(rooms[roomNumber].north != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].north)) {
        count++;
    }
    if(rooms[roomNumber].south != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].south)) {
        count++;
    }
    if(rooms[roomNumber].east != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].east)) {
        count++;
    }
    if(rooms[roomNumber].west != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].west)) {
        count++;
    }
    return count;
}

int validMove(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int creatureNum, int roomNumber) {
    if(rooms[roomNumber].north != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].north)) {
        printf("%d leaves towards the %s.\n", creatureNum, "north" );
        return rooms[roomNumber].north;
    } else if(rooms[roomNumber].south != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].south)) {
        printf("%d leaves towards the %s.\n", creatureNum, "south" );
        return rooms[roomNumber].south;
    } else if(rooms[roomNumber].east != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].east)) {
        printf("%d leaves towards the %s.\n", creatureNum, "east" );
        return rooms[roomNumber].east;
    } else if(rooms[roomNumber].west != -1 && !isFull(rooms, creatures, numOfRooms, numOfCreatures, rooms[roomNumber].west)) {
        printf("%d leaves towards the %s.\n", creatureNum, "west" );
        return rooms[roomNumber].west;
    }
    return -1;
}

void leave(struct Room *rooms, struct Creature *creatures, int numOfRooms, int numOfCreatures, int creatureNum, int roomNumber) {
    int move = validMoves(rooms, creatures, numOfRooms, numOfCreatures, roomNumber);
    if (move == 0) {
        // Creature is exit the game
        creatures[creatureNum].roomNum = -1;
        for(int i = 0; i < numOfCreatures; i++) {
            if(creatures[i].roomNum == roomNumber && i != creatureNum && creatures[roomNumber].type == CHARACTER_ANIMAL) {
                respectScore = respectScore - 1;
                printf("Animal %d growl to you because there is a creature left the world\n", i);
            } if (creatures[i].roomNum == roomNumber && i != creatureNum && creatures[roomNumber].type == CHARACTER_NPC) {
                respectScore = respectScore - 1;
                printf("Human %d grumble because there is a creature left the world\n", i);
            }
        }
        return;
    } else if (move == 1) {
        move = validMove(rooms, creatures, numOfRooms, numOfCreatures, roomNumber, creatureNum);
        creatures[creatureNum].roomNum = move;
    } else {
        while(1) {
            int rnd = rand()%4;
            int newRoomNumber;
            if (rnd == DIRECTION_NORTH) {
                newRoomNumber = rooms[roomNumber].north;
            } else if (rnd == DIRECTION_SOUTH) {
                newRoomNumber = rooms[roomNumber].south;
            } else if (rnd == DIRECTION_EAST) {
                newRoomNumber = rooms[roomNumber].east;
            } else if (rnd == DIRECTION_WEST) {
                newRoomNumber = rooms[roomNumber].west;
            }
            if(!isFull(rooms, creatures, numOfRooms, numOfCreatures, newRoomNumber)) {
                printf("%d leaves towards the ", creatureNum);
                if (rnd == DIRECTION_NORTH) {
                    printf("north.\n");
                } else if (rnd == DIRECTION_SOUTH) {
                    printf("south.\n");
                } else if (rnd == DIRECTION_EAST) {
                    printf("east.\n");
                } else if (rnd == DIRECTION_WEST) {
                    printf("west.\n");
                }
                creatures[creatureNum].roomNum = newRoomNumber;
                break;
            }
        }
    }
    if (rooms[creatures[creatureNum].roomNum].state == STATE_CLEAN && creatures[creatureNum].type == CHARACTER_NPC) {
        rooms[creatures[creatureNum].roomNum].state = STATE_HALF_DIRTY;
    } else if (rooms[creatures[creatureNum].roomNum].state == STATE_DIRTY && creatures[creatureNum].type == CHARACTER_ANIMAL) {
        rooms[creatures[creatureNum].roomNum].state = STATE_HALF_DIRTY;
    }
}
