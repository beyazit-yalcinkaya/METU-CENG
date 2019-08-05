#include <p18f8722.h>

#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

char rBuffer[SIZE_RC_BUF];        // Self-explainatory: Receive buffer
char sendBuffer[SIZE_TM_BUF];     // Self-explainatory: Send buffer
char data_size;                   // Amount of payload to be sent
short direction;                  // It keeps direction in degrees (bottom 180, top 0, left 270, right 90)
char dx, dy;                      // It keeps relative position of robot in a 50*50 square (for example if it has x = 65, then dx = 15)
unsigned char posX, posY;         // It keeps current position starting from 0 (robot starts from 0, 0)
unsigned char maze[4][16];        // Undiscovered and discovered parts of the maze, will be initialized to 0xFF except 0,0 and 0, 15
unsigned char nExplored;          // Number of explored cells, to count if all maze elements are observed
unsigned char env[4][3];          // For MAP command, it simply obtains values from its surroundings
unsigned char nEnv;               // If an observation around robot it changed, it simply counts how many of them is changed.
unsigned char state;              // It shows the states of bot, states are described in common.h
char sentUpdateMap[4][16];        // It shows cells from which map is updated.

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/

 /**********************************************************************
 * ----------------------------- MOVEBOT -----------------------------
 *
 * Simply bot task. It first initializes the robot, then it waits for 
 * $GO: command to be received. Afterwards, it continues its operation.
 * For the first step, it uses follow right hand side algorithm to solve
 * the maze and whenever nExplored == 64, it tries to go second phase 
 * which is reaching to (0, 15) using same algorithm with modification.
 * Then it stops its execution by sending $END: command.
 *
 * States are indicated with preprocessor directives such as _IDLE, 
 * _SOLVING and _EXPLORING. Hence, we can assume that that this task
 * as a state machine.
 *
 * Maze and environment variables are designated as it will seen on LCD,
 * however we gave up doing LCD part but it is ready to be printed without
 * any conversion.
 *
 * dx, dy and direction keeps current position on X and Y and direction,
 * respectively. However, note that dx and dy keeps pixel position (i.e.
 * position at the cell) but posX and posY keeps overall position (i.e.
 * looking from block side).
 *
 **********************************************************************/
 
void initRobot(void)
{
    unsigned int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 16; j++)
        {
            sentUpdateMap[i][j] = 0;
            if (i == 0 && (j == 0 || j == 15))
                maze[i][j] = ' ';
            else if (j == 16)
                maze[i][j] = '\0';
            else
                maze[i][j] = 'X';
        }
    }
    nExplored = 2;
    dx = dy = 25;
    posX = posY = 0;
    direction = 180;
    state = _IDLE;
    nEnv = 0;
}

void updateMap(void) {
    sendBuffer[0] = '$';
    sendBuffer[1] = 'S';
    sendBuffer[2] = ':';
    transmitBuffer_push(sendBuffer, 3, 1);
    WaitEvent(TRANSMIT_END);
    ClearEvent(TRANSMIT_END);
    WaitEvent(DATA_RECEIVED);
    ClearEvent(DATA_RECEIVED);
    receiveBuffer_pop(rBuffer);
    if (rBuffer[0] == 'D') {
        if (posX > 0 && maze[posX - 1][posY] == 'X')
            nExplored++;
        if (posX < 3 && maze[posX + 1][posY] == 'X')
            nExplored++;
        if (posY > 0 && maze[posX][posY - 1] == 'X')
            nExplored++;
        if (posY < 15 && maze[posX][posY + 1] == 'X')
            nExplored++;
        if (direction == 0) // looking up
        {
            if (posX > 0) // upper side
                maze[posX - 1][posY] = ((rBuffer[1] == 0) ? (' '): (0xFF));
            if (posX < 3) // bottom side
                maze[posX + 1][posY] = ((rBuffer[3] == 0) ? (' '): (0xFF));
            if (posY > 0) // left side
                maze[posX][posY - 1] = ((rBuffer[4] == 0) ? (' '): (0xFF));
            if (posY < 15) // right side
                maze[posX][posY + 1] = ((rBuffer[2] == 0) ? (' '): (0xFF));
        }
        else if (direction == 90) // looking right
        {
            if (posX > 0) // upper side
                maze[posX - 1][posY] = ((rBuffer[4] == 0) ? (' '): (0xFF));
            if (posX < 3) // bottom side
                maze[posX + 1][posY] = ((rBuffer[2] == 0) ? (' '): (0xFF));
            if (posY > 0) // left side
                maze[posX][posY - 1] = ((rBuffer[3] == 0) ? (' '): (0xFF));
            if (posY < 15) // right side
                maze[posX][posY + 1] = ((rBuffer[1] == 0) ? (' '): (0xFF));
        }
        else if (direction == 180) // looking down
        {
            if (posX > 0) // upper side
                maze[posX - 1][posY] = ((rBuffer[3] == 0) ? (' '): (0xFF));
            if (posX < 3) // bottom side
                maze[posX + 1][posY] = ((rBuffer[1] == 0) ? (' '): (0xFF));
            if (posY > 0) // left side
                maze[posX][posY - 1] = ((rBuffer[2] == 0) ? (' '): (0xFF));
            if (posY < 15) // right side
                maze[posX][posY + 1] = ((rBuffer[4] == 0) ? (' '): (0xFF));
        }
        else if (direction == 270) // looking left
        {
            if (posX > 0) // upper side
                maze[posX - 1][posY] = ((rBuffer[2] == 0) ? (' '): (0xFF));
            if (posX < 3) // bottom side
                maze[posX + 1][posY] = ((rBuffer[4] == 0) ? (' '): (0xFF));
            if (posY > 0) // left side
                maze[posX][posY - 1] = ((rBuffer[1] == 0) ? (' '): (0xFF));
            if (posY < 15) // right side
                maze[posX][posY + 1] = ((rBuffer[3] == 0) ? (' '): (0xFF));
        }
        nEnv = 0;
        if (sentUpdateMap[posX][posY])
            return;
        sentUpdateMap[posX][posY] = 1;
        if (posX > 0) {
            env[nEnv][0] = posY;
            env[nEnv][1] = posX - 1;
            env[nEnv][2] = ((maze[posX - 1][posY] == 0xFF) ? 1 : 0);
            nEnv++;
        }
        if (posX < 3) {
            env[nEnv][0] = posY;
            env[nEnv][1] = posX + 1;
            env[nEnv][2] = ((maze[posX + 1][posY] == 0xFF) ? 1 : 0);
            nEnv++;
        }
        if (posY > 0) {
            env[nEnv][0] = posY - 1;
            env[nEnv][1] = posX;
            env[nEnv][2] = ((maze[posX][posY - 1] == 0xFF) ? 1 : 0);
            nEnv++;
        }
        if (posY < 15) {
            env[nEnv][0] = posY + 1;
            env[nEnv][1] = posX;
            env[nEnv][2] = ((maze[posX][posY + 1] == 0xFF) ? 1 : 0);
            nEnv++;
        }
        data_size = nEnv * 6;
        while (nEnv > 0) {
            nEnv--;
            sendBuffer[nEnv * 6] = '$';
            sendBuffer[nEnv * 6 + 1] = 'M';
            sendBuffer[nEnv * 6 + 2] = env[nEnv][1];
            sendBuffer[nEnv * 6 + 3] = env[nEnv][0];
            sendBuffer[nEnv * 6 + 4] = env[nEnv][2];
            sendBuffer[nEnv * 6 + 5] = ':';
        }
        transmitBuffer_push(sendBuffer, data_size, 0);
        WaitEvent(TRANSMIT_END);
        ClearEvent(TRANSMIT_END);
    }
}

void turnRight(void) {
    do {
        sendBuffer[0] = '$';
        sendBuffer[1] = 'R';
        sendBuffer[2] = ':';
        transmitBuffer_push(sendBuffer, 3, 1);
        WaitEvent(TRANSMIT_END);
        ClearEvent(TRANSMIT_END);
        WaitEvent(DATA_RECEIVED);
        ClearEvent(DATA_RECEIVED);
        receiveBuffer_pop(rBuffer);
        if (rBuffer[0] == 'E') {
            direction += rBuffer[1];
            if (direction >= 360)
                direction -= 360;
        }
    } while (!(direction == 0 || direction == 90 || direction == 180 || direction == 270));
}

void turnLeft(void) {
    do {
        sendBuffer[0] = '$';
        sendBuffer[1] = 'L';
        sendBuffer[2] = ':';
        transmitBuffer_push(sendBuffer, 3, 1);
        WaitEvent(TRANSMIT_END);
        ClearEvent(TRANSMIT_END);
        WaitEvent(DATA_RECEIVED);
        ClearEvent(DATA_RECEIVED);
        receiveBuffer_pop(rBuffer);
        if (rBuffer[0] == 'E') {
            direction -= rBuffer[1];
            if (direction < 0)
                direction += 360;
        }
    } while (!(direction == 0 || direction == 90 || direction == 180 || direction == 270));
}

void moveForward(void) {
    do {
        sendBuffer[0] = '$';
        sendBuffer[1] = 'F';
        sendBuffer[2] = ':';
        transmitBuffer_push(sendBuffer, 3, 1);
        WaitEvent(TRANSMIT_END);
        ClearEvent(TRANSMIT_END);
        WaitEvent(DATA_RECEIVED);
        ClearEvent(DATA_RECEIVED);
        receiveBuffer_pop(rBuffer);
        if (rBuffer[0] == 'E') {
            if (direction == 0) // looking up
            {
                dx -= rBuffer[1];
                if (dx < 0)
                {
                    posX--;
                    dx += 50;
                }
            }
            else if (direction == 90) // looking right
            {
                dy += rBuffer[1];
                if (dy > 50)
                {
                    posY++;
                    dy -= 50;
                }
            }
            else if (direction == 180) // looking right
            {
                dx += rBuffer[1];
                if (dx > 50)
                {
                    posX++;
                    dx -= 50;
                }
            }
            else if (direction == 270) // looking right
            {
                dy -= rBuffer[1];
                if (dy < 0)
                {
                    posY--;
                    dy += 50;
                }
            }
        }
    } while (!(dx == 25 && dy == 25));
}

void moveOneCell() {
    if (dx == 25 && dy == 25 && (direction == 0 || direction == 90 || direction == 180 || direction == 270)) {
        if (direction == 0) // looking up
        {
            if (posY < 15 && maze[posX][posY + 1] == ' ') {
                turnRight();
                moveForward();
            } else if (posX > 0 && maze[posX - 1][posY] == ' ') {
                moveForward();
            } else {
                turnLeft();
            }
        }
        else if (direction == 90) // looking right
        {
            if (posX < 3 && maze[posX + 1][posY] == ' ') {
                turnRight();
                moveForward();
            } else if (posY < 15 && maze[posX][posY + 1] == ' ') {
                moveForward();
            } else {
                turnLeft();
            }
        }
        else if (direction == 180) // looking down
        {
            if (posY > 0 && maze[posX][posY - 1] == ' ') {
                turnRight();
                moveForward();
            } else if (posX < 3 && maze[posX + 1][posY] == ' ') {
                moveForward();
            } else {
                turnLeft();
            }
        }
        else if (direction == 270) // looking left
        {
            if (posX > 0 && maze[posX - 1][posY] == ' ') {
                turnRight();
                moveForward();
            } else if (posY > 0 && maze[posX][posY - 1] == ' ') {
                moveForward();
            } else {
                turnLeft();
            }
        } 
    }
}

/**********************************************************************
 * ------------------------------ MOVEBOT -------------------------------
 * Simply bot task. It first initializes the robot, then it waits for 
 * $GO: command to be received. Afterwards, it continues its operation.
 * For the first step, it uses follow right hand side algorithm to solve
 * the maze and whenever nExplored == 64, it tries to go second phase 
 * which is reaching to (0, 15) using same algorithm with modification.
 * Then it stops its execution by sending $END: command.
 **********************************************************************/

TASK(MOVEBOT) 
{
    initRobot();
    PIE1bits.RCIE = 1;
    while(1) {
        if (state == _IDLE) {
            WaitEvent(DATA_RECEIVED);
            ClearEvent(DATA_RECEIVED);
            data_size = receiveBuffer_pop(rBuffer);
            if (data_size != 0 && rBuffer[0] == 'G' && rBuffer[1] == 'O') {
                state = _EXPLORING;
                SetEvent(TRANSMIT_ID, START_TRANSMIT);
            }
        } else if (state == _EXPLORING) {
            updateMap();
            moveOneCell();
            if (nExplored >= 64) {
                state = _SOLVING;
                if (posX == 0 && posY == 15)
                    break;
            }
        } else if (state == _SOLVING) {
            moveOneCell();
            if (posX == 0 && posY == 15)
                break;
        }
    }
    sendBuffer[0] = '$';
    sendBuffer[1] = 'E';
    sendBuffer[2] = 'N';
    sendBuffer[3] = 'D';
    sendBuffer[4] = ':';
    transmitBuffer_push(sendBuffer, 5, 1);
    WaitEvent(TRANSMIT_END);
    ClearEvent(TRANSMIT_END);
	TerminateTask();
}

/* End of File : movebot.c */