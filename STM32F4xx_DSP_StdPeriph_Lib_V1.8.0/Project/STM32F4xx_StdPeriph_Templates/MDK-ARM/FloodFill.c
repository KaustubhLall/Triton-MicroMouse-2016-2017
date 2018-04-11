#include <stdio.h>
#include <string.h>

/* location to represent the position of the cells in the 2d matrix */
struct location {

  int x;
  int y;
};

/* cell to store the information of a cell in the 2d matrix */
struct cell {

  int wall;
  int distance;
  int visited;
};

/* this is the element for the array to be returned for speed run */
/* straight is the straight direction, and inst is the instruction when empty */
/* walls have been sighted */
struct instruction {

  int straight;
  char inst[50];
};

/* size of the maze */
const int SIZE = 16;

/* even maze */
const int CENTER_MIN = SIZE / 2 - 1;
const int CENTER_MAX = SIZE / 2;
const int NUM_CENTERS = 4;

/* starting cell */
const int START_Y = 0;
const int START_X = 15;

/* walls representations */
const int WALLS = 4;
const int TOP_WALL = 1;
const int RIGHT_WALL = 2;
const int BOTTOM_WALL = 4;
const int LEFT_WALL = 8;
const int ALL_WALLS = 15;
const int walls[WALLS] = {1, 2, 4, 8};

/* walls */
const int DEAD_WALLS = 3;
const int DEAD_WALLS_NEIGHBOR = 2;

/* directions representations */
const int DIRECTIONS = 4;
const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const char *directions[DIRECTIONS] = {"North", "East", "South", "West"};
const char directionSymbols[DIRECTIONS] = {'^', '>', 'v', '<'};

/* maximum number of elements in the stack */
const int MAX_STACK = 257;

/* maximum number of steps to explore after reaching center */
const int MAX_EXPLORE = 60;

/* total number of cells in the maze */
const int TOTAL_CELLS = 257;

/* Variables for initializations */
/* the current direction and position of the mouse */
int currentDirection = NORTH;
struct location currentLocation = {START_X, START_Y};

/* maze */
struct cell theMaze[SIZE][SIZE] = {
  {{9,14,0}, {1,13,0}, {1,12,0}, {1,11,0}, {1,10,0}, {1,9,0}, {1,8,0}, {1,7,0}, {1,7,0}, {1,8,0}, {1,9,0}, {1,10,0}, {1,11,0}, {1,12,0}, {1,13,0}, {3,14,0}},
  {{8,13,0}, {0,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {0,12,0}, {2,13,0}},
  {{8,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {2,12,0}},
  {{8,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {2,11,0}},
  {{8,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {2,10,0}},
  {{8,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {2,9,0}},
  {{8,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {2,8,0}},
  {{8,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,0,0}, {0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {2,7,0}},
  {{8,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,0,0}, {0,0,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {2,7,0}},
  {{8,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,1,0}, {0,1,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {2,8,0}},
  {{8,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,2,0}, {0,2,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {2,9,0}},
  {{8,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,3,0}, {0,3,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {2,10,0}},
  {{8,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,4,0}, {0,4,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {2,11,0}},
  {{8,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,5,0}, {0,5,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {2,12,0}},
  {{8,13,0}, {0,12,0}, {0,11,0}, {0,10,0}, {0,9,0}, {0,8,0}, {0,7,0}, {0,6,0}, {0,6,0}, {0,7,0}, {0,8,0}, {0,9,0}, {0,10,0}, {0,11,0}, {0,12,0}, {2,13,0}},
  {{14,14,1}, {12,13,0}, {4,12,0}, {4,11,0}, {4,10,0}, {4,9,0}, {4,8,0}, {4,7,0}, {4,7,0}, {4,8,0}, {4,9,0}, {4,10,0}, {4,11,0}, {4,12,0}, {4,13,0}, {6,14,0}}
};

/* true when we need to fill the cell with all walls */
int deadOn = 0;

/* previous to keep track which cells to fill */
int previousX = 0;
int previousY = 0;

/* for explore */
int exploreSteps = 0;

/* TO BE DELETED */
/* sample maze */
struct cell virtualMaze[SIZE][SIZE] = {
  {{8|1, 0, 0},   {1|4, 0, 0},   {1|4, 0, 0}, {1|4, 0, 0},   {1|4, 0, 0}, {1|4, 0, 0},   {1, 0, 0}, {1, 0, 0},   {1|4, 0, 0}, {1|4, 0, 0},   {1|4, 0, 0}, {1, 0, 0},   {1, 0, 0}, {1, 0, 0},   {1, 0, 0}, {2|1, 0, 0}},
  {{8, 0, 0},     {1|2, 0, 0},   {1|8, 0, 0}, {1, 0, 0},   {1|4, 0, 0}, {1|2|4, 0, 0},   {8|2, 0, 0}, {8, 0, 0},   {1|2, 0, 0}, {1|8, 0, 0},   {1|2, 0, 0}, {8|2|4, 0, 0},   {8|2|4, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8|2, 0, 0}},
  {{8|2, 0, 0},     {8|2, 0, 0},   {8|2, 0, 0}, {8|4, 0, 0},   {1|4, 0, 0}, {1|2, 0, 0},   {8, 0, 0}, {2, 0, 0},   {8, 0, 0}, {2|4, 0, 0},   {8, 0, 0}, {1, 0, 0},   {1|4, 0, 0}, {0, 0, 0},   {2, 0, 0}, {8|2, 0, 0}},
  {{8|2, 0, 0},     {8|2, 0, 0},   {8|4, 0, 0}, {1, 0, 0},   {1|2|4, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8, 0, 0},   {2|4, 0, 0}, {1|8, 0, 0},   {2|4, 0, 0}, {8, 0, 0},   {1, 0, 0}, {2|4, 0, 0},   {8|2, 0, 0}, {8|2, 0, 0}},
  {{8|2, 0, 0},     {8, 0, 0},   {1|2|4, 0, 0}, {8|2, 0, 0},   {1|8|4, 0, 0}, {2, 0, 0},   {8|4, 0, 0}, {4, 0, 0},   {1|4, 0, 0}, {2|4, 0, 0},   {1|8, 0, 0}, {2, 0, 0},   {8|2, 0, 0}, {1|8, 0, 0},   {2|4, 0, 0}, {8|2, 0, 0}},
  {{8|4, 0, 0},     {0, 0, 0},   {1|2|4, 0, 0}, {8|4, 0, 0},   {1|2, 0, 0}, {8|2|4, 0, 0},   {1|8, 0, 0}, {1|4, 0, 0},   {1|4, 0, 0}, {1, 0, 0},   {2, 0, 0}, {8|2, 0, 0},   {8, 0, 0}, {2|4, 0, 0},   {1|8, 0, 0}, {2|4, 0, 0}},
  {{8|1|4, 0, 0},     {0, 0, 0},   {1|4, 0, 0}, {1|4, 0, 0},   {2|4, 0, 0}, {1|8, 0, 0},   {4, 0, 0}, {1|4, 0, 0},   {1|4, 0, 0}, {2, 0, 0},   {8|4, 0, 0}, {0, 0, 0},   {2, 0, 0}, {1|8|4, 0, 0},   {2, 0, 0}, {1|8|2|4, 0, 0}},
  {{8|1|4, 0, 0},     {0, 0, 0},   {1|2|4, 0, 0}, {1|8, 0, 0},   {1|2, 0, 0}, {8, 0, 0},   {1|2, 0, 0}, {1|8, 0, 0},   {1|2, 0, 0}, {8|2|4, 0, 0},   {1|8, 0, 0}, {2, 0, 0},   {8|2|4, 0, 0}, {1|8|4, 0, 0},   {4, 0, 0}, {1|2, 0, 0}},
  {{8|1|4, 0, 0},     {0, 0, 0},   {1|2|4, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8, 0, 0},   {2|4, 0, 0}, {1|8, 0, 0},   {2, 0, 0}, {8|2|4, 0, 0},   {1|8, 0, 0}, {1|2, 0, 0},   {1|8, 0, 0}, {2, 0, 0}},
  {{8|1|2, 0, 0},     {8|4, 0, 0},   {1|2, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8|4, 0, 0},   {0, 0, 0}, {2|4, 0, 0},   {1|8, 0, 0}, {2, 0, 0},   {8|2|4, 0, 0}, {1|8, 0, 0},   {2, 0, 0}, {8|4, 0, 0},   {2|4, 0, 0}, {8|2, 0, 0}},
  {{8|2, 0, 0},     {1|8|4, 0, 0},   {4, 0, 0}, {2, 0, 0},   {8, 0, 0}, {1|2, 0, 0},   {8|4, 0, 0}, {1|4, 0, 0},   {2|4, 0, 0}, {8|2|4, 0, 0},   {1|8, 0, 0}, {2, 0, 0},   {8|2|4, 0, 0}, {1|8|4, 0, 0},   {1|4, 0, 0}, {2, 0, 0}},
  {{8|2, 0, 0},     {1|8, 0, 0},   {1|2|4, 0, 0}, {8|4, 0, 0},   {2, 0, 0}, {8|2, 0, 0},   {1|8|2, 0, 0}, {1|8|2, 0, 0},   {1|8, 0, 0}, {1|2, 0, 0},   {8|2, 0, 0}, {8|2, 0, 0},   {1|8, 0, 0}, {1, 0, 0},   {1|4, 0, 0}, {2, 0, 0}},
  {{8|2, 0, 0},     {8, 0, 0},   {1|2|4, 0, 0}, {1|8, 0, 0},   {2, 0, 0}, {8|4, 0, 0},   {2, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {8|4, 0, 0},   {2|4, 0, 0}, {8, 0, 0},   {2, 0, 0}, {8|2|4, 0, 0},   {1|8|4, 0, 0}, {2, 0, 0}},
  {{8, 0, 0},     {2|4, 0, 0},   {1|8, 0, 0}, {2|4, 0, 0},   {8|2, 0, 0}, {1|8|2, 0, 0},   {8, 0, 0}, {2|4, 0, 0},   {8|2, 0, 0}, {1|8|2, 0, 0},   {1|8, 0, 0}, {2|4, 0, 0},   {8|4, 0, 0}, {1|2, 0, 0},   {1|8|4, 0, 0}, {2, 0, 0}},
  {{8, 0, 0},     {1, 0, 0},   {4, 0, 0}, {1|2|4, 0, 0},   {8|2, 0, 0}, {8|2, 0, 0},   {8|2, 0, 0}, {1|8, 0, 0},   {2|4, 0, 0}, {8|2, 0, 0},   {8|4, 0, 0}, {1|2, 0, 0},   {1|8, 0, 0}, {4, 0, 0},   {1, 0, 0}, {2|4, 0, 0}},
  {{8|2|4, 0, 0}, {8|4, 0, 0}, {1|4, 0, 0}, {1|4, 0, 0},   {4, 0, 0}, {4, 0, 0},   {4, 0, 0}, {4, 0, 0},   {1|4, 0, 0}, {4, 0, 0},   {1|4, 0, 0}, {4, 0, 0},   {4, 0, 0}, {1|4, 0, 0},   {4, 0, 0}, {1|2|4, 0, 0}}
};

/*----------------------------------------------------------------------------*/
/* Helper functions start here */

int noWalls(int wallInfo, int currentDirection) {

  for (int i = 0; i < DIRECTIONS; i++) {

    if (i != currentDirection && i != (currentDirection + 2) % DIRECTIONS) {

      if ((walls[i] & wallInfo) == 0) {

        return 1;
      }
    }
  }

  return 0;  
}

void initializeArray(int outputArray[TOTAL_CELLS]) {

  for (int i = 0; i < TOTAL_CELLS; i++) {

    outputArray[i] = -1;
  }
}

void copyArray(int visitedNeighbors[DIRECTIONS],
int enterableNeighbors[DIRECTIONS]) {

  for (int i = 0; i < DIRECTIONS; i++) {

    visitedNeighbors[i] = enterableNeighbors[i];
  }
}

struct location popStack(struct location myStack[MAX_STACK], int* top) {

  struct location returnElement = myStack[*top];

  myStack[*top].x = 0;
  myStack[*top].y = 0;

  *top = *top - 1;

  return returnElement;
}

void pushStack(struct location myStack[MAX_STACK], int* top,
struct location newElement) {

  *top = *top + 1;

  myStack[*top].x = newElement.x;
  myStack[*top].y = newElement.y;
}

/*
 * Name: isCenter()
 * Parameters: x - this is the x coordinate to be checked.
 * y - this is the y coordinate to be checked.
 * Description: This function will check whether or not the given coordinates
 * combine is one of the centers.
 * Return: A boolean indicating that the given coordinates combine is one of the
 * centers.
 */
int isCenter(int x, int y) {

  /* check if it's one of the centers */
  if (x >= CENTER_MIN && x <= CENTER_MAX) {

    if (y >= CENTER_MIN && y <= CENTER_MAX) {

      return 1;
    }
  }

  /* otherwise, return 0 */
  return 0;
}

/*
 * Name: isOut()
 * Parameters: x - this is the x coordinate to be checked.
 * y - this is the y coordinate to be checked.
 * Description: This function will check whether or not the given coordinates
 * are out of bounds of the 2D array.
 * Return: A boolean indicating that the given coordinates are out of bounds.
 */
int isOut(int x, int y) {

  /* check if x and y is out of bounds of the maze array */
  if (x < 0 || x >= SIZE) {

    return 1;
  }

  else if (y < 0 || y >= SIZE) {

    return 1;
  }

  /* otherwise, return 0 */
  return 0;
}

/*
 * Name: numWalls()
 * Parameters: walls - this is the walls to be counted.
 * Description: This function will be able to count the number of walls from the
 * given walls.
 * Return: An int representing the number of walls around this cell.
 */
int numWalls(int walls) {

  /* the mask to get the first bit */
  int mask = 1;

  /* the counter of 1's */
  int count = 0;

  /* for loop to count the number of walls */
  for (int i = 0; i < DIRECTIONS; i++) {

    if ((walls >> i & mask) == 1) {

      count++;
    }
  }

  return count;
}

/*
 * Name: neighborsVisited()
 * Parameters: currentX - the x position of the current location.
 * currentY - they y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * enterableNeighbors - the array to be populated with distances if the cell
 * is enterable from the current location.
 * Description: This function will be able to determine which neighbors are
 * enterable and populate the given array with the distances. It will keep the
 * element as -1 if there is a wall.
 */
void neighborsVisited(int currentX, int currentY,
struct cell theMaze[SIZE][SIZE], int visitedNeighbors[DIRECTIONS]) {

  /* check all directions whether this neighbor is visited */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* 
     * the i represents the current direction being examined, if there is no
     * wall then add the distance to the array
     */
    switch (i) {

      case NORTH:

        if (theMaze[currentX - 1][currentY].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case EAST:

        if (theMaze[currentX][currentY + 1].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case SOUTH:

        if (theMaze[currentX + 1][currentY].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;

      case WEST:

        if (theMaze[currentX][currentY - 1].visited == 1 &&
        visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 1;
        }

        else if (visitedNeighbors[i] != -1) {

          visitedNeighbors[i] = 0;
        }

        break;
    }
  }
}

/*
 * Name: enterableCells()
 * Parameters: currentX - the x position of the current location.
 * currentY - they y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * enterableNeighbors - the array to be populated with distances if the cell
 * is enterable from the current location.
 * Description: This function will be able to determine which neighbors are
 * enterable and populate the given array with the distances. It will keep the
 * element as -1 if there is a wall.
 */
void enterableCells(int currentX, int currentY, struct cell theMaze[SIZE][SIZE],
int enterableNeighbors[DIRECTIONS]) {

  int currentWall = theMaze[currentX][currentY].wall;

  /* check all directions whether this neighbor is enterable */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* 
     * the i represents the current direction being examined, if there is no
     * wall then add the distance to the array
     */
    switch (i) {

      case NORTH:

        if ((currentWall & TOP_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX - 1][currentY].distance;
        }
        break;

      case EAST:

        if ((currentWall & RIGHT_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX][currentY + 1].distance;
        }
        break;

      case SOUTH:

        if ((currentWall & BOTTOM_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX + 1][currentY].distance;
        }
        break;

      case WEST:

        if ((currentWall & LEFT_WALL) == 0) {

          enterableNeighbors[i] = theMaze[currentX][currentY - 1].distance;
        }
        break;
    }
  }
}

/*
 * Name: findMaxDistance()
 * Parameters: enterableNeighbors() - this is the array that contains the
 * distances of the neighbors.
 * Description: This function will find the max distance to center, except if
 * the distance is -1, unenterable.
 * Return: an integer representing the smallest distance in given array.
 */
int findMaxDistance(int enterableNeighbors[DIRECTIONS]) {

  /* this will keep track of the smallest distance */
  int max = -1;

  /* check all distances */
  for (int i = 0; i < DIRECTIONS; i++) {

    int currentDistance = enterableNeighbors[i];

    if (currentDistance != -1 && currentDistance > max) {

      max = currentDistance;
    }
  }

  return max;
}

/*
 * Name: findMinDistance()
 * Parameters: enterableNeighbors() - this is the array that contains the
 * distances of the neighbors.
 * Description: This function will find the least distance to center, except if
 * the distance is -1, unenterable.
 * Return: an integer representing the smallest distance in given array.
 */
int findMinDistance(int enterableNeighbors[DIRECTIONS]) {

  /* this will keep track of the smallest distance */
  int min = SIZE * SIZE;

  /* check all distances */
  for (int i = 0; i < DIRECTIONS; i++) {

    int currentDistance = enterableNeighbors[i];

    if (currentDistance != -1 && currentDistance < min) {

      min = currentDistance;
    }
  }

  return min;
}

/*
 * Name: stepAtDirection()
 * Parameters: currentLocation - the current location of the mouse.
 * direction - the direction to take a step to.
 * Description: This function will update the current location to a neighbor for
 * a step given the specified direction.
 */
void stepAtDirection(struct location *currentLocation, int direction) {

    switch (direction) {

      case NORTH:
        currentLocation->x -= 1;
        break;
      
      case EAST:
        currentLocation->y +=1;
        break;

      case SOUTH:
        currentLocation->x += 1;
        break;

      case WEST:
        currentLocation->y -= 1;
        break;
    }
}

/*
 * Name: addWall()
 * Parameters: currentX - the x position of the neighbor.
 * currentY - the y position of the neighbor.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * wall - the side of the wall to add.
 * Description: This function will update the wall of the given location with
 * the side of the wall.
 */
void addWall(int currentX, int currentY, struct cell theMaze[SIZE][SIZE],
int wall) {

  /* add the wall at the given location */
  if (!isOut(currentX, currentY)) {

    int *tempWall = &(theMaze[currentX][currentY].wall);
    *tempWall = *tempWall | wall;
  }  
}

/*
 * Name: updateNeighborWall()
 * Parameters: currentX - the x position of the current location.
 * currentY - the y position of the current location.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * direction - the direction of the neighbor to add wall.
 * Description: This function will be able to add the corresponding wall of the
 * the neighbor with the corresponding direction.
 */
void updateNeighborWall(int currentX, int currentY,
struct cell theMaze[SIZE][SIZE], int direction) {

  /* update the wall of the neighbor */
  switch (direction) {

    /* north side neighbor */
    case NORTH:

      addWall(currentX - 1, currentY, theMaze, BOTTOM_WALL);
      break;

    /* east side neighbor */
    case EAST:

      addWall(currentX, currentY + 1, theMaze, LEFT_WALL);
      break;

    /* south side neighbor */
    case SOUTH:

      addWall(currentX + 1, currentY, theMaze, TOP_WALL);
      break;

    /* west side neighbor */
    case WEST:

      addWall(currentX, currentY - 1, theMaze, RIGHT_WALL);
      break;
  }
}

/*
 * Name: populateStack()
 * Parameters: currentX - the x position of the current cell.
 * currentY - the y position of the current cell.
 * direction - the direction of the neighbor to be examined.
 * myStack - the stack to populate.
 * Description: This function will be able to populate the stack with the given
 * neighbor that is not out of bounds.
 */
void populateStack(int currentX, int currentY, int direction,
struct location myStack[MAX_STACK], int *top) {

  switch (direction) {

    case NORTH:
      
      if (!isOut(currentX - 1, currentY)) {

        pushStack(myStack, top, (struct location) {currentX - 1, currentY});
      }
      break;

    case EAST:
      
      if (!isOut(currentX, currentY + 1)) {

        pushStack(myStack, top, (struct location) {currentX, currentY + 1});
      }
      break;

    case SOUTH:
      
      if (!isOut(currentX + 1, currentY)) {

        pushStack(myStack, top, (struct location) {currentX + 1, currentY});
      }
      break;

    case WEST:
      
      if (!isOut(currentX, currentY - 1)) {

        pushStack(myStack, top, (struct location) {currentX, currentY - 1});
      }
      break;
  }
}

/*
 * Name: printMaze()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function has the ability to print the maze using the ASCII
 * characters to represent the walls and the mouse with the given maze
 * representation as defined.
 */
void printMaze(struct cell theMaze[SIZE][SIZE], struct location currentLocation,
int currentDirection) {

  /* get the x and y of the mouse's location */
  int mouseX = currentLocation.x;
  int mouseY = currentLocation.y;

  /* print the walls on the top row */
  for (int i = 0; i < SIZE; i++) {

    printf(" ");
    printf("=");
  }

  /* the extra space at the end of top row */
  printf(" \n");

  /* print the left or bottom walls of the current cell if any */
  for (int i = 0; i < SIZE; i++) {

    /*
     * iterate to print left walls, if any, then print the car if at the current
     * cell
     */
    for (int j = 0; j < SIZE; j++) {

      /* get the current wall */
      int currentWall = theMaze[i][j].wall;

      /* check if there is a left wall, if so, print it */
      if ((currentWall & LEFT_WALL) != 0) {

        printf("|");
      }

      else {

        printf(" ");
      }

      /* check if the mouse is present at this cell, if so, print it */
      if (i == mouseX && j == mouseY) {

        printf("%c", directionSymbols[currentDirection]);
      }

      else {

        printf(" ");
      }
    }

    /* the wall at the very end, then new line */
    printf("|\n");

    /* iterate to print the bottom walls, if any */
    for (int j = 0; j < SIZE; j++) {

      /* get the current wall */
      int currentWall = theMaze[i][j].wall;

      /* check if there is a bottom wall, if so, print it */
      if ((currentWall & BOTTOM_WALL) != 0) {

        printf(" =");
      }

      else {

        printf("  ");
      }
    }

    /* the space at the very end, then new line */
    printf(" \n");
  }
}

/*
 * Name: printArray()
 * Parameters: theMaze - the 2D array to be printed.
 * Description: This function will print the given 2D array, where each element
 * is a cell, which represents walls and distance.
 */
void printArray(struct cell theMaze[SIZE][SIZE]) {

  for (int i = 0; i < SIZE; i++) {

    for (int j = 0; j < SIZE; j++) {

      printf("(%2d,%2d,%d)", theMaze[i][j].wall, theMaze[i][j].distance,
      theMaze[i][j].visited);
    }

    printf("\n");
  }
}

/*
 * Name: checkStatus()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be used to check the current state of the
 * maze and mouse for debugging purposes.
 */
void checkStatus(struct cell theMaze[SIZE][SIZE], struct location currentLocation,
int currentDirection) {

  /* check status of the maze and mouse */
  printMaze(theMaze, currentLocation, currentDirection);
  /* printArray(theMaze); */
}

/*
 * Name: fillWalls()
 * Parameters: currentX - the x position of the location to be isolated.
 * currentY - the y position of the location to be isolated.
 * theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * Description: This function will be able to isolate the cell from other cells,
 * by filling it with walls on all sides because it is a dead end.
 */
void fillWalls(int currentX, int currentY, struct cell theMaze[SIZE][SIZE]) {

  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  for (int i = 0; i < DIRECTIONS; i++) {

    if (enterableNeighbors[i] != -1) {

      updateNeighborWall(currentX, currentY, theMaze, i);
    }
  }

  theMaze[currentX][currentY].wall = ALL_WALLS;
}

/*----------------------------------------------------------------------------*/
/* Core functions start here */

void printEnter(int enter[DIRECTIONS], struct location myElement) {

  printf("(%2d,%2d) Enterable: ", myElement.x, myElement.y);
  for (int i = 0; i < DIRECTIONS; i++) {

    printf("%d, ", enter[i]);
  }

  printf("\n");
}

/*
 * Name: updateDistances()
 * Parameters: currentLocation - the location of the current that is being
 * examined.
 * theMaze - the 2D array representing the maze, where each element is a cell
 * that has a wall and distance member.
 * neighbors - this is an array containing the neighbors that are enterable,
 * represented by a value of 1, 0 otherwise.
 * Description: This function will be able to update the distances of the cells
 * starting from the current cell and the neighbors that has new walls added to
 * them.
 */
void updateDistances(struct location currentLocation,
struct cell theMaze[SIZE][SIZE], int neighbors[DIRECTIONS]) {

  /* get the x and y position of the current location */
  int currentX = currentLocation.x;
  int currentY = currentLocation.y;

  /* stack to store the potentially changing cell's distance */
  struct location myStack[MAX_STACK] = {0};
  int top = -1;

  /* push the current cell */
  pushStack(myStack, &top, currentLocation);

  /* iterate through all directions */
  for (int i = 0; i < DIRECTIONS; i++) {

    // if this neighbor has been set to 1, then put it to stack
    if (neighbors[i] == 1) {

      /* push the neighbor if it's valid, if so, add it to stack */
      populateStack(currentX, currentY, i, myStack, &top);
    }
  }

  /* this will track the element being examined from the stack */
  struct location myElement;

  /* update while there is element in stack */
  while (top != -1) {

    /* get the top element */
    myElement = popStack(myStack, &top);

    /* the x and y positions of the current element */
    int tempX = myElement.x;
    int tempY = myElement.y;

    /* get the distance of the current element */
    int *currentDistance = &(theMaze[tempX][tempY].distance);

    /* array to keep track which neighbor is enterable (open) */
    int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

    /* find out which neighbors can be entered (no wall in between) */
    enterableCells(tempX, tempY, theMaze, enterableNeighbors);
    /* printEnter(enterableNeighbors, myElement); */

    /* find the minimum distance among the neighbors */
    int minDistance = findMinDistance(enterableNeighbors);

    /*
     * the correct distance value of the current cell is minimum distance
     * neighbor + 1
     */
    int correctDistance = minDistance + 1;

    /* 
     * check if the distance of the current element is not correct and it is not
     * the center, if so, update the distance and push the open neighbors to the
     * stack
     */
    if (*currentDistance != correctDistance && *currentDistance != 0) {

      /* update the distance of the current element */
      *currentDistance = correctDistance;

      /* push all reachable neighbors to stack */
      for (int i = 0; i < DIRECTIONS; i++) {

        // if it's not -1, then it's reachable
        if (enterableNeighbors[i] != -1) {

          populateStack(tempX, tempY, i, myStack, &top);
        }
      }
    }
  }
}

/*
 * Name: evaluateCell()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * virtualMaze - the testing maze to see where are the walls.
 * currentLocation - the current location of the mouse.
 * deadOn - the status of a dead end to populate it to other cells.
 * Description: This function will first examine if there are new walls
 * discovered around the cell, it will update the cell's wall status
 * accordingly. Then, it wiill call the helper function to update the distances
 * starting from the current cell and the neighbor cells that have added new
 * walls. It will update the deadOn status when the cell is a dead end.
 */
void evaluateCell(struct cell theMaze[SIZE][SIZE], int actualWalls,
struct location currentLocation, int *deadOn) {

  /* get the x and y position of the current location */
  int currentX = currentLocation.x;
  int currentY = currentLocation.y;

  /* get the status of the wall at the current location */
  int *currentWalls = &(theMaze[currentX][currentY].wall);

  /* 
   * this will keep track which neighbors should be put into the stack for
   * distance update
   */
  int neighbors[DIRECTIONS] = {0};

  /* for loop to check if there is new wall discovered */
  for (int i = 0; i < DIRECTIONS; i++) {

    /* check if there is a wall at this direction */
    if ((actualWalls & walls[i]) != 0) {

      /* 
       * if there is, check if the wall not exist in theMaze, add it if that's
       * the case (new wall discovered)
       */
      if ((*currentWalls & walls[i]) == 0) {

        /* add the wall */
        *currentWalls = *currentWalls | walls[i];

        /* update the neighbor wall */
        updateNeighborWall(currentX, currentY, theMaze, i);

        /* set this neighbor to be put in stack for distance update */
        neighbors[i] = 1;
      }
    }
  }

  /*
   * update the distances starting from the current cell and neighbor cells if
   * applicable
   */
  updateDistances(currentLocation, theMaze, neighbors);

  /* check if it has three walls and not the start, if so, it's dead */
  if (numWalls(theMaze[currentX][currentY].wall) == DEAD_WALLS) {

    if (currentX != START_X || currentY != START_Y) {

      *deadOn = 1;
    }
  }

  else {

    *deadOn = 0;
  }
}

/*
 * Name: move()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be able to decide which way to turn and move.
 * It will first attempt to go straight to the cell that is in the current
 * direction with minimum distance. If not, it will turn the mouse in the order
 * of N, E, S, W that has minimum distance.
 */
int move(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,
int *currentDirection) {

  /* get the x and y position of the current location */
  int currentX = currentLocation->x;
  int currentY = currentLocation->y;

  /* initially, assume neighbors are not enterable */
  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  /* check which neighbors can be entered (no wall in between) */
  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  /* find the minimum distance */
  int minDistance = findMinDistance(enterableNeighbors);

  /* check if the next cell of current direction has minimum distance */
  if (enterableNeighbors[*currentDirection] == minDistance) {

    /* check the current direction then go straight */
    stepAtDirection(currentLocation, *currentDirection);

    /* mark this cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 0;

    /* TODO: Move mouse straight one step */
    return *currentDirection;
  }

  /* otherwise, find the earliest direction then turn that way and move */
  else {

    /* find the direction that has the minimum distance, then go there */
    int index = 0;
    for (; index < DIRECTIONS; index++) {

      if (enterableNeighbors[index] == minDistance) {

        break;
      }
    }

    /* take a step to that direction */
    stepAtDirection(currentLocation, index);

    /* update the direction */
    *currentDirection = index;

    /* mark this cell cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: turn to this direction, then take a step */
    return index;
  }
}

/*
 * Name: explore()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will be able to decide which way to explore like
 * move. It will first attempt to go to the one that is not visited, then the
 * current direction, and finally the maximum distance in the order of N, E, S,
 * W.
 */
int explore(struct cell theMaze[SIZE][SIZE], struct location *currentLocation,
int *currentDirection) {

  /* get the x and y position of the current location */
  int currentX = currentLocation->x;
  int currentY = currentLocation->y;

  /* initially, assume neighbors are not enterable */
  int enterableNeighbors[DIRECTIONS] = {-1, -1, -1, -1};

  /* check which neighbors can be entered (no wall in between) */
  enterableCells(currentX, currentY, theMaze, enterableNeighbors);

  /* find the minimum distance */
  int maxDistance = findMaxDistance(enterableNeighbors);

  /* this will store which neighbor has been visited, initially 0 */
  int visitedNeighbors[DIRECTIONS] = {0};
  copyArray(visitedNeighbors, enterableNeighbors);

  /* find which neighbors are visited and not visited */
  neighborsVisited(currentX, currentY, theMaze, visitedNeighbors);

  /* prioritized unvisited cells */
  for (int i = 0; i < DIRECTIONS; i++) {

    if (visitedNeighbors[i] == 0) {

      /* take a step to that direction */
      stepAtDirection(currentLocation, i);

      /* update the direction */
      *currentDirection = i;

      /* mark this cell cell as visited */
      theMaze[currentLocation->x][currentLocation->y].visited = 1;

      return i;   
    }
  }

  /* check if the next cell of current direction has maximum distance */
  if (enterableNeighbors[*currentDirection] == maxDistance) {

    /* check the current direction then go straight */
    stepAtDirection(currentLocation, *currentDirection);

    /* mark this cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: Move mouse straight one step */
    return *currentDirection;
  }

  /* otherwise, find the earliest direction then turn that way and move */
  else {

    /* find the direction that has the maximum distance, then go there */
    int index = 0;
    for (; index < DIRECTIONS; index++) {

      if (enterableNeighbors[index] == maxDistance) {

        break;
      }
    }

    /* take a step to that direction */
    stepAtDirection(currentLocation, index);

    /* update the direction */
    *currentDirection = index;

    /* mark this cell cell as visited */
    theMaze[currentLocation->x][currentLocation->y].visited = 1;

    /* TODO: turn to this direction, then take a step */
    return index;
  }
}

/*
 * Name: fillCenter()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * currentLocation - the current location of the mouse.
 * currentDirection - the current direction of the mouse.
 * Description: This function will run through the center of the maze, but not
 * actually moving the mouse in order to fill the walls and update the
 * distances.
 */
void fillCenter(struct cell theMaze[SIZE][SIZE],
struct location *currentLocation, int *currentDirection) {

  /* walls */
  int T = TOP_WALL;
  int R = RIGHT_WALL;
  int B = BOTTOM_WALL;
  int L = LEFT_WALL;

  struct location centers[WALLS] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
  int walls[WALLS] = {L|T, T|R, L|B, R|B};

  int currentCenter = 0;
  int tempBool = 0;

  for (int i = 0; i < WALLS; i++) {

    if (centers[i].x != currentLocation->x ||
    centers[i].y != currentLocation->y) {

      /*
       * evaluate the cell to see if there are new walls, then update the
       * distances accordingly
       */
      evaluateCell(theMaze, walls[i], centers[i], &tempBool);

      /* check the status through print outs */
      checkStatus(theMaze, *currentLocation, *currentDirection);

      theMaze[centers[i].x][centers[i].y].visited = 1;
    }
  }

  *currentDirection = *currentDirection + 2 % DIRECTIONS;
}

/*
 * Name: floodMaze()
 * Parameters: theMaze - the 2D array representing the maze, where each element
 * is a cell that has a wall and distance member.
 * Description: This function will be able to flood the distances of each cell
 * of the given maze with initial distances.
 */
void floodMaze(struct cell theMaze[SIZE][SIZE]) {

  /* constants for neighbors searching */
  /*
  const int NUM_NEIGHBORS = 4;
  const int TOP = 0;
  const int RIGHT = 1;
  const int BOTTOM = 2;
  const int LEFT = 3; 

  // define all of the centers
  location center1 = {CENTER_MIN, CENTER_MIN};
  location center2 = {CENTER_MIN, CENTER_MAX};
  location center3 = {CENTER_MAX, CENTER_MIN};
  location center4 = {CENTER_MAX, CENTER_MAX};

  // the queue for storing the neighbors to be updated
  queue<location> myQueue;

  // current distance starts with 0
  int currentDistance = 0;

  // number of cells for this distance 
  int count = NUM_CENTERS;

  int tempCount = 0;

  // push all of the centers to queue
  myQueue.push(center1);
  myQueue.push(center2);
  myQueue.push(center3);
  myQueue.push(center4);

  // while loop to update all of the cell's distaces
  while (!myQueue.empty()) {

    // get the next element from the queue
    location currentPoint = myQueue.front();
    myQueue.pop();

    // update the distance at this location with the current distance
    theMaze[currentPoint.x][currentPoint.y].distance = currentDistance;

    // get the x and y of the current location
    int currentX = currentPoint.x;
    int currentY = currentPoint.y;

    // temporary x and y for the neighbors
    int tempX = 0;
    int tempY = 0;

    // try all of the neighbors
    for (int i = 0; i < NUM_NEIGHBORS; i++) {

      switch (i) {

        // top neighbor
        case TOP:
          tempX = currentX - 1;
          tempY = currentY;
          break;

        // right neighbor
        case RIGHT:
          tempX = currentX;
          tempY = currentY + 1;
          break;

        // bottom neighbor
        case BOTTOM:
          tempX = currentX + 1;
          tempY = currentY;
          break;

        // left neighbor
        case LEFT:
          tempX = currentX;
          tempY = currentY - 1;
          break;
      }

      // check if it's not center or outside
      if (!isCenter(tempX, tempY) && !isOut(tempX, tempY)) {

        // get the distance of this neighbor cell
        int neighborDistance = theMaze[tempX][tempY].distance;

        // check if the distance is 0 and not pending to be changed -1
        if (neighborDistance == 0 && neighborDistance != -1) {

          // add this location to the queue
          myQueue.push({tempX, tempY});
          theMaze[tempX][tempY].distance = -1;
          tempCount++;
        }
      }
    }

    // decrement it to keep track of the remaining cell for currentDistance
    count--;

    // if count becomes 0, then time to reset it
    if (count == 0) {

      // assign tempCount to count, then reset tempCount
      count = tempCount;
      tempCount = 0;

      // increment the currentDistance
      currentDistance++;
    }
  }*/
}

void evaluation(struct cell theMaze[SIZE][SIZE], int *deadOn, int *previousX,
int *previousY, int wallInfo, struct location *currentLocation,
int *currentDirection) {

  if (*deadOn) {

    fillWalls(*previousX, *previousY, theMaze);
  }

  /*
    * evaluate the cell to see if there are new walls, then update the
    * distances accordingly
    */
  evaluateCell(theMaze, wallInfo, *currentLocation, deadOn);
  
  if (*deadOn) {

    *previousX = currentLocation->x;
    *previousY = currentLocation->y;
  }

  /* check the status through print outs */
  checkStatus(theMaze, *currentLocation, *currentDirection);
}

int speedRunPath(struct instruction myInstructions[TOTAL_CELLS]) {

  /* get the directions first */
  int path[TOTAL_CELLS];
  int index = 0;
  initializeArray(path);
  currentLocation.x = START_X;
  currentLocation.y = START_Y;
  currentDirection = NORTH;

  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    path[index++] = move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
    evaluation(theMaze, &deadOn, &previousX, &previousY,
    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
    &currentDirection);
  }

  int count = 0;

  /* status of the location */
  struct location currentLocation = {START_X, START_Y};

  /* directions */
  int currentDirection = path[0];

  /* current index of the instruction array, then its element */
  index = 0;
  struct instruction *structure = &myInstructions[index];

  /* whether or not we are concatenating characters */
  int concatenating = 0;

  for (int i = 0; i < TOTAL_CELLS && path[i] != -1; i++) {

    currentDirection = path[i];

    stepAtDirection(&currentLocation, currentDirection);

    if (concatenating) {

      if (currentDirection == path[i - 1]) {

        concatenating = 0;
        index++;
        structure = &myInstructions[index];

        if (noWalls(theMaze[currentLocation.x][currentLocation.y].wall,
        currentDirection)) {

          strcpy(structure->inst, "");
          structure->straight = currentDirection;

          index++;
          structure = &myInstructions[index];
          
          count++;
        }
      }

      else {

        /* append a direction instruction */
        switch (currentDirection) {
          
          case 0:
            strcat(structure->inst, "N");
            break;

          case 1:
            strcat(structure->inst, "E");
            break;

          case 2:
            strcat(structure->inst, "S");
            break;

          case 3:
            strcat(structure->inst, "W");
            break;

        }

        if (path[i + 1] != -1 && path[i + 1] == currentDirection) {

          concatenating = 0;
          index++;
          structure = &myInstructions[index];
        }
      }
    }

    else if (noWalls(theMaze[currentLocation.x][currentLocation.y].wall,
    currentDirection)) {

      concatenating = 1;
      strcpy(structure->inst, "");
      structure->straight = currentDirection;

      count++;
    }
  }

  return count;
}

void phaseOne() {

  /* keep moving until 0 has been found */
  printf("Phase 1: Find Center\n");
  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
    evaluation(theMaze, &deadOn, &previousX, &previousY,
    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
    &currentDirection);
  }
  
  /* to fill the walls of the other center cells */
  fillCenter(theMaze, &currentLocation, &currentDirection);
}

void phaseTwo() {

  printf("Phase 2: Explore\n");
  checkStatus(theMaze, currentLocation, currentDirection);

  /* preparation */
  deadOn = 0;
  exploreSteps = 0;

  /* perform 60 steps */
  while (exploreSteps < MAX_EXPLORE) {

    explore(theMaze, &currentLocation , &currentDirection);

    /* this will evaluate the cell */
    evaluation(theMaze, &deadOn, &previousX, &previousY,
    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
    &currentDirection);

    exploreSteps++;
  }
}

void phaseThree() {

  printf("Phase 3: Search Again\n");
  /* preparation */
  currentLocation.x = START_X;
  currentLocation.y = START_Y;
  currentDirection = NORTH;

  /* loop til we find the center */
  while (theMaze[currentLocation.x][currentLocation.y].distance != 0) {

    /* move to a cell, smaller value */
    move(theMaze, &currentLocation, &currentDirection);

    /* this will evaluate the cell */
    evaluation(theMaze, &deadOn, &previousX, &previousY,
    virtualMaze[currentLocation.x][currentLocation.y].wall, &currentLocation,
    &currentDirection);
  }
}

void phaseFour() {

  /* TODO: create the array for speed run */

  printf("Phase 4: Speed Run\n");

  /* an array to pass in, then it will return the number of valid elements */
  struct instruction myInstructions[TOTAL_CELLS];
  int count = speedRunPath(myInstructions);
}

int main(int argc, char* argv[]) {

  /* sample maze print out */
  /* TODO: change virtual maze to real wall info */
  printf("Virtual Maze:\n");
  printMaze(virtualMaze, currentLocation, currentDirection);

  printf("Initial Maze: \n");
  checkStatus(theMaze, currentLocation, currentDirection);

  /* Phase 1: Find Center */
  phaseOne();

  /* Phase 2: EXPLORE */
  /* TODO: turn 180 here */
  phaseTwo();

  /* Phase 3: SEARCH AGAIN */
  /* start from the beginning */
  phaseThree();

  /* might need phase 3.5: explore, to get the mouse (same as phase 2) */

  /* PHASE 4: SPEED RUN (store directions in an array) */
  /* start from the beginning */
  phaseFour();
}