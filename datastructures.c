#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h> // Required for srand(time(NULL))

#define MAX_LIFTS 10
#define MAX_REQUESTS 50

//-------------------------- Part 1: Lift Data ------------------------
// Structure & Initialization
// Badal's Part
typedef struct {
    int id;
    int currentFloor;
    int direction; // -1 = down, 0 = idle, 1 = up
    int status;    // 0 = idle, 1 = moving
} Lift;

void initializeLifts(Lift lifts[], int n) {
    for (int i = 0; i < n; i++) {
        lifts[i].id = i + 1;
        // Floors 1-10
        lifts[i].currentFloor = rand() % 10 + 1;
        lifts[i].direction = 0; // idle
        lifts[i].status = 0;    // idle
    }
}

void printLifts(Lift lifts[], int n) {
    printf("\nLift Status:\n");
    printf("ID\tFloor\tDir\tStatus\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\n", lifts[i].id,
                                   lifts[i].currentFloor,
                                   lifts[i].direction,
                                   lifts[i].status);
    }
}

//-------------------------- Part 2: User Request Handling ------------------------
// Devojeet's Part
typedef struct {
    int floor;
    int direction; // 1 = up, -1 = down
} Request;

typedef struct {
    Request queue[MAX_REQUESTS];
    int front;
    int rear;
} RequestQueue;

void initQueue(RequestQueue *q) {
    q->front = -1;
    q->rear = -1;
}

int isEmpty(RequestQueue *q) {
    return q->front == -1;
}

void enqueue(RequestQueue *q, int floor, int dir) {
    // Basic validation for floor and direction
    if (floor < 1 || floor > 10 || (dir != 1 && dir != -1)) {
        printf("Invalid floor or direction. Request ignored.\n");
        return;
    }

    if (q->rear == MAX_REQUESTS - 1) {
        printf("Request queue full!!\n");
        return;
    }
    if (isEmpty(q)) q->front = 0;
    q->rear++;
    q->queue[q->rear].floor = floor;
    q->queue[q->rear].direction = dir;
}

Request dequeue(RequestQueue *q) {
    // Initializing with a known "invalid" state: floor -1, direction 0
    Request r = {-1, 0};

    if (isEmpty(q)) {
        printf("Queue empty!\n");
        return r;
    }

    // Get the request at the front
    r = q->queue[q->front];

    if (q->front == q->rear) {
        // Queue becomes empty
        q->front = q->rear = -1;
    } else {
        // Move front pointer
        q->front++;
    }
    return r;
}

//-------------------------- Part 3: Optimization & Lift Assignment ------------------------
// Sahil's Part
int calculateCost(Lift lift, int userFloor, int userDir)
{
    // Distance cost: absolute number of floors away
    int distance = abs(lift.currentFloor - userFloor);
    int penalty = 0;

    // Penalty cost: penalize if the lift is moving AND its direction is opposite to the request
    if (lift.direction != 0 && lift.direction != userDir) {
        penalty = 5;
    }

    return distance + penalty;
}

int findBestLift(Lift lifts[], int n, int userFloor, int userDir)
{
    int best = -1;
    int minCost = 9999;
    for (int i = 0; i < n; i++) {
        if (lifts[i].status == 0) { // idle lifts only are considered
            int cost = calculateCost(lifts[i], userFloor, userDir);

            if (cost < minCost) {
                minCost = cost;
                best = i;
            }
        }
    }
    return best; // Returns index of the best lift, or -1 if none are idle
}

//-------------------------- Part 4: Simulation & Logging ------------------------
// Anurag's Part
void updateLift(Lift *lift, int targetFloor) {
    // Set direction based on movement
    if (lift->currentFloor < targetFloor) lift->direction = 1;
    else if (lift->currentFloor > targetFloor)
        lift->direction = -1;
    else lift->direction = 0;

    lift->currentFloor = targetFloor;
    lift->status = 0; // after reaching floor, set idle
}

void simulateSystem(Lift lifts[], int n, RequestQueue *rq) {
    if (isEmpty(rq)) {
        printf("\nSimulation skipped: Request queue is empty.\n");
        return;
    }

    printf("\n--- Starting Simulation ---\n");
    while (!isEmpty(rq)) {
        Request r = dequeue(rq);

        // Skip processing the default error request from dequeue
        if (r.floor == -1) continue;

        int liftIndex = findBestLift(lifts, n, r.floor, r.direction);

        if (liftIndex == -1) {
            // Note: In a real system, this request would be re-enqueued or logged,
            // but for this simple model, it is discarded.
            printf("No IDLE lift available for request at floor %d (Dir: %d). Request dropped.\n", r.floor, r.direction);
            continue;
        }

        printf("\nRequest at floor %d (dir %d) assigned to Lift %d\n",
               r.floor, r.direction, lifts[liftIndex].id);

        lifts[liftIndex].status = 1; // Set status to moving (temporarily)
        updateLift(&lifts[liftIndex], r.floor);

        // Log state after assignment and "movement"
        printLifts(lifts, n);
    }
    printf("\n--- Simulation Complete ---\n");
}

//-------------------------- Main Function ------------------------
int main() {
    // Initialize random seed for different initial lift positions
    srand((unsigned int)time(NULL));

    Lift lifts[MAX_LIFTS];
    RequestQueue rq;
    initQueue(&rq);

    initializeLifts(lifts, MAX_LIFTS);
    printLifts(lifts, MAX_LIFTS);

    int choice;
    do {
        printf("\n1. Add Request\n2. Start Simulation\n3. Exit\n");
        printf("Enter choice: ");

        // Robust input handling
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Clear the input buffer to prevent infinite loop
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
            continue;
        }

        if (choice == 1) {
            int floor, dir;
            printf("Enter floor (1-10): ");
            // Check for valid floor input
            if (scanf("%d", &floor) != 1) {
                printf("Invalid floor input.\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            printf("Enter direction (1=up, -1=down): ");
            // Check for valid direction input
            if (scanf("%d", &dir) != 1) {
                printf("Invalid direction input.\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            enqueue(&rq, floor, dir);
        } else if (choice == 2) {
            simulateSystem(lifts, MAX_LIFTS, &rq);
        }
    } while (choice != 3);

    printf("Exiting simulator.\n");
    return 0;
}