#Concurrent Elevator Controller


Jacob Sanchez
Course Name  : Computer Systems
Homework  6  : Elevator Simulation
-------------------------------------------------------------------------------

##Description

This project implements a concurrent elevator controller that ensures that all passengers receive service, minimizing the amount of time it takes to serve all of the passengers, and how much CPU time is used to run the controller. The simulator imitates an M story highrise which has N elevators in a single “elevator bank”, each able to serve every floor of the building.

##Indtroduction
-------------
This is the  advanced development of the elevator. In this code elevator picks
up multiple people. Number of floors, elevators, and passengers can be 
manipulated in hw6.h
1. First, person thread gets initialised it starts adding people in a global
   linked list at certain "arrival time"
2. Each elevator thread then works in following way::
	a. Each elevator is kept on first, and last floor alternately for
	   better functioning
	b. Elevator then checks whether there are people in its own list
		i) If so and if there are no people in the global list, it
		   starts its work without applying locks and without
		   accessing the global list
		ii)If there is no person it threads own list then it waits
		   for global list to have atleast one person in it
	c. The destination and direction of the linked list is set, and any
	   other person who wants to go in the same direction and leave the
	   elevator before or on the same floor as that of the elevators
	   destination.
	d. Then the own list of the elevator is checked to print the person id
	   who will enter in the elevator, and set the flag for each person
	   who has entered the elevator
	e. The the function to drop the people to certain floor is called then
           Depending on the direction of each person and the flag telling that
	   ther person has entered the elevator the person node is removed
	   from the elevator.
3.When the simulation time gets over both elevator and person thread stops
working, and memory will be freed.	

##Testing

The run_tests script tests some useful configurations, and the last test has the configuration used for testing on Gradescope. Your program should be able to pass all the tests. This is to give some type of intermediate testing. The first test is entirely sequential, and there are a couple of tests that are sequential on one type of thread (eg 1 passenger - 4 elevators, 50 passengers - 1 elevator) Further custom testing is also possible. You can play with all these values in hw6.h.


Python run_test.py

##Logging

To add more logging output, try using the log() function instead of printf. log(loglevel, format_string, parameters) works just like fprintf(stderr,...) except it only prints if the configured log level is higher than the loglevel argument provided. To set the log level to, for example, 8 use the following flag to gcc -DLOGLEVEL=8


##References

http://www.ibm.com/developerworks/linux/library/l-posix1.html?S_TACT=105AGX03&S_CMP=EDU
http://www.ibm.com/developerworks/linux/library/l-posix2/?S_TACT=105AGX03&S_CMP=EDU
http://www.ibm.com/developerworks/linux/library/l-posix3/index.html?S_TACT=105AGX03&S_CMP=EDU
https://computing.llnl.gov/tutorials/pthreads/#ConVarSignal
http://pages.cs.wisc.edu/~travitch/pthreads_primer.html
