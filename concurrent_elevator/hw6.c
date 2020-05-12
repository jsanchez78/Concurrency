#include "hw6.h"
#include <stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include<time.h>

pthread_mutex_t lock;
/* This is an internal struct used by the enforcement system
   - there is no access to this from hw6.c */
static struct Elevator {
    int floor;//Where elevator is going
    int passenger;

    //Potentially wrong
    //In order to keep track of every elevator
    pthread_barrier_t barrier_e;
    //pthread_mutex_t lock_elevator;
    int current_floor;
    int direction;
    int occupancy;
    //int is_locked;
    enum {ELEVATOR_ARRIVED=1, ELEVATOR_OPEN=2, ELEVATOR_CLOSED=3} state;
} elevators[ELEVATORS];

//int is_locked[ELEVATORS];
/* This is an internal struct used by the enforcement system
   - there is no access to this from hw6.c. */
static struct Passenger {
    int id;
    int from_floor;
    int to_floor;
    int in_elevator; ///Passenger knows if they are in elevator or not
    enum {WAITING, ENTERED, EXITED} state;

    //In order to keep track of every passengers
    pthread_barrier_t barrier_p;
    //pthread_mutex_t lock_passenger;
} passengers[PASSENGERS];

/*

  Create  Queue to keep track of #Passengers

  on each floor


*/
///How many passengers are on per floor
//***** haveing a head pointer for each floor ******
struct node *num_passengers_ON_FLOOR[FLOORS];

struct node {
  int data;
  struct node * next;
};

//make this return an int
//pass an int of which floor
//inserting passenger at current floor
void insert(int curr_floor,int passenger_ID)
{
  /* create a new node after tmp */
	struct node *new = malloc(sizeof(struct node));
	if (!new) {
		fprintf(stderr, "Failed to insert a new element\n");
		return;
	}
  new->data = passenger_ID;
  //EMPTY list
  if(num_passengers_ON_FLOOR[curr_floor] == NULL){
    num_passengers_ON_FLOOR[curr_floor] = new;
    }
  //Insert at the end
  else{
    new->next = num_passengers_ON_FLOOR[curr_floor];
    num_passengers_ON_FLOOR[curr_floor] = new;
  }
}
/* Function to remove the first node
   of the linked list */
int removeFirstNode(int curr_floor)
{
  //struct node *current = num_passengers_ON_FLOOR[curr_floor];
  if (num_passengers_ON_FLOOR[curr_floor] == NULL)
      return -1;
  // Move the head pointer to the next node
  int passenger = num_passengers_ON_FLOOR[curr_floor]->data;

  //SINGLE NODE
  if(num_passengers_ON_FLOOR[curr_floor]->next == NULL){
    num_passengers_ON_FLOOR[curr_floor] = NULL;
    //num_passengers_ON_FLOOR_BACK[curr_floor] = NULL;
  }
  //List w/ Multiple Nodes
  else{
    struct node *tmp = num_passengers_ON_FLOOR[curr_floor];
    num_passengers_ON_FLOOR[curr_floor] = num_passengers_ON_FLOOR[curr_floor]->next;
    free(tmp);
  }
  return passenger;
}
/*

Create two arrays:

array of passengerStructs:


array of elevatorStructs:


Pass by reference
*/
void scheduler_init() {
  //Initialize elevator array
  for(int i = 0; i < ELEVATORS; ++i) {
    elevators[i].current_floor=0;
    elevators[i].direction=-1;
    elevators[i].passenger = -1;
    elevators[i].occupancy=0;
    elevators[i].state=ELEVATOR_CLOSED;
    pthread_barrier_init(&elevators[i].barrier_e, NULL, 2);
  }
  /*
  for(int i = 0; i < ELEVATORS; ++i) {
		is_locked[i] = 0;
	}
  */
  //Initialize passenger array
  for(int i = 0; i < PASSENGERS; ++i) {
    passengers[i].in_elevator = -1;
    passengers[i].id = i;
    pthread_barrier_init(&passengers[i].barrier_p, NULL, 2);
  }
  for(int i = 0; i < FLOORS; ++i) {
    num_passengers_ON_FLOOR[i] = NULL;
  }
  pthread_mutex_init(&lock,0);
}
/*Called for each passenger*/
void passenger_request(int passenger, int from_floor, int to_floor,
        void (*enter)(int, int),
        void(*exit)(int, int))
{
    //Initialize passenger
    //passengers[passenger].from_floor = from_floor;
    passengers[passenger].to_floor = to_floor;
    passengers[passenger].id = passenger;
    pthread_mutex_lock(&lock);
    insert(from_floor,passenger);//add passenger
    pthread_mutex_unlock(&lock);   ///If messes up TIFFANY DID IT
    //Populate current passenger
    pthread_barrier_wait(&passengers[passenger].barrier_p);


    // wait for the elevator to arrive at our origin floor, then get in
        //passengers[passenger].in_elevator ---> generalization for which elevator I'm in
        // Use current time as seed for random generator
        //srand(time(0));
        //passengers[passenger].in_elevator = (rand() % (ELEVATORS-1));

        /*TODO:

        ElevatorID consistency

        */
        if(elevators[passengers[passenger].in_elevator].current_floor == from_floor && elevators[passengers[passenger].in_elevator].state == ELEVATOR_OPEN && elevators[passengers[passenger].in_elevator].occupancy==0) {
            /*
            TODO:
            wait_barrier
             */
            enter(passenger, passengers[passenger].in_elevator);
            elevators[passengers[passenger].in_elevator].occupancy++;
            //DEFINE passenger inside current elevator
            elevators[passengers[passenger].in_elevator].passenger = passenger;
        }
        //call elevator barrier to let it close
      pthread_barrier_wait(&elevators[passengers[passenger].in_elevator].barrier_e);
    // wait for the elevator at our destination floor, then get out
    // wait for the elevator to arrive at our origin floor, then get in

        /*

        Current elevator based on my definition is:

        passengers[passenger].in_elevator;
        */
        pthread_barrier_wait(&passengers[passenger].barrier_p);
        //THIRD CONDITION NOT REALLY NEEDED
        //&& elevators[passengers[passenger].in_elevator].occupancy==1
        if(elevators[passengers[passenger].in_elevator].current_floor == to_floor && elevators[passengers[passenger].in_elevator].state == ELEVATOR_OPEN && elevators[passengers[passenger].in_elevator].occupancy==1) {
            exit(passenger, passengers[passenger].in_elevator);/// 0 ---->passengers[passenger].in_elevator
            elevators[passengers[passenger].in_elevator].occupancy--;
            //Mark passenger as No longer in the elevator
            //passengers[passenger].in_elevator = -1;
            //Mark elevator as Having NO passenger
            //elevators[passengers[passenger].in_elevator].passenger = -1;

        }

        //call elevator barrier to let it close
    pthread_barrier_wait(&elevators[passengers[passenger].in_elevator].barrier_e);
    //pthread_mutex_unlock(&lock);
}

/*Multi - threading works:

On last passenger attempt to move out of building*/
void elevator_ready(int elevator, int at_floor,
        void(*move_direction)(int, int),
        void(*door_open)(int), void(*door_close)(int)) {

      //pthread_mutex_lock(&lock);
      if(elevators[elevator].state == ELEVATOR_ARRIVED) {
          /*Assign elevator id passenger is entering*/
          //passengers[elevators[elevator]].in_elevator = elevator;
          //pthread_barrier_wait(&passengers[elevators[elevator].passenger].barrier_p);
          door_open(elevator);
          elevators[elevator].state=ELEVATOR_OPEN;
      }

      else if(elevators[elevator].state == ELEVATOR_OPEN) {

          //Unblock passenger barrier to ENTER OR EXIT
          pthread_barrier_wait(&passengers[elevators[elevator].passenger].barrier_p);
          pthread_barrier_wait(&elevators[elevator].barrier_e);///Wait for elevator barrier

          //Waiting for passenger to unblock (ENTER OR EXIT Elevator)

          door_close(elevator);

          elevators[elevator].state=ELEVATOR_CLOSED;
      }

      /*ERROR: Elevator gets stuck here

      Direction is faulty

      Attempting to go out of bounds

      DOWN when at 0

      UP when at TOP_FLOOR

      Check bookeeping

      NOT keeping track of floor

      */

      else {

          //Someone in elevator

          if(elevators[elevator].occupancy == 1){
            //Move elevator
            move_direction(elevator,passengers[elevators[elevator].passenger].to_floor - elevators[elevator].current_floor);
            elevators[elevator].current_floor = passengers[elevators[elevator].passenger].to_floor;
          }
          else{
            //UP
            if(at_floor==0 && elevators[elevator].direction == -1) {
              elevators[elevator].direction = 1;
            }
            //DOWN
            if(at_floor==FLOORS-1 && elevators[elevator].direction == 1) {
              elevators[elevator].direction = -1;
            }
            move_direction(elevator,elevators[elevator].direction);
            elevators[elevator].current_floor=at_floor+elevators[elevator].direction;
          }

          ///elevators[0].state=ELEVATOR_ARRIVED;
          /*
            When do we actually open the elevator?

            1)Elevator is empty AND passenger has submitted request.

            2)Elevator is full AND passenger has reached to_floor
          */
          ///May use Queue have current_passenger
          if(elevators[elevator].occupancy == 0 && num_passengers_ON_FLOOR[elevators[elevator].current_floor] != NULL){
            elevators[elevator].state=ELEVATOR_ARRIVED;
            pthread_mutex_lock(&lock);
            elevators[elevator].passenger = removeFirstNode(elevators[elevator].current_floor);
            pthread_mutex_unlock(&lock);

            //modifiy passenger inside elevator
            passengers[elevators[elevator].passenger].in_elevator = elevator;//ASSIGN ELEVATOR ---> to passenger
          }
          /// data memeber in queue is at_floor  //passen at curr
          else if(elevators[elevator].occupancy == 1 && passengers[elevators[elevator].passenger].to_floor == elevators[elevator].current_floor){
              elevators[elevator].state=ELEVATOR_ARRIVED;
          }
      }
      //pthread_mutex_unlock(&lock);
}
