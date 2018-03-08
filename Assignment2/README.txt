Differences between deliverable 1 and my final project:

I have more than 5 mutex, I have the 5 I said I would have and another 3:
	1. For my dispatcher to wait till things are in the queues
	2. One for my timer so multiple threads don't overwrite each other 
	3. One for my startthreads, instead of using my track mutex to make my 
	   threads all start on the same time, I got a separate mutex for that

I had one extra condition variable for my dispatcher that signals 
for the case where nothing is in the queues.

In deliverable 1 didn't mention how I was planning on track the time, 
but that was implemented the same way as show in the labs

Rest of my P2A is practically the same as my P2B


How my program works:


Main:

	Initialize everything (queues, mutex, pthread, condvars)
	Read input file and track how many trains are going to be added
	Create threads and condvars each in an array
	Initialize the condvars
	Read the input file again and create threads for each train
	Each thread has a node for its void arg, and the node contains:
		Train id
		Loading time
		Crossing time
		Direction
		Condition variable
	
	After creating all the threads, broadcast to let them start
	sleep 5 milliseconds
	Start the clock
	Call the dispatcher
	Destroy the mutex's
	Program finished
	

Thread function (called threadFunction):
	
	Waits on a condition variable to start running 
	Sleeps for its loading time
	Prints ready to go in a direction
	Gets placed in one of the 4 queues, one for each direction (e, E, w, W)
	Signals to the dispatcher that it is in a queue
	Waits on the condition variable stored in the node
	Prints on track
	Sleeps for crossing time
	Prints off track
	Unlocks track after crossing
	
	
Dispatcher function (called Loop):

	Waits initially for something to enter the queue for the first time
	While(More than 1 trains has not crossed):
		if (queues all empty):
			waits for something to enter a queue
		
		Checks Queue E and queue W, if both empty checks queue e and queue waits
		Dequeue the next train to cross and signals the conditional variable in the nodes
		Wait till the thread on the track is finished
		Decrement number of trains
		
		Loop back
		
Time (called getTIme):

	Stores time in Stop, gets time by Start - Stop, and prints the time
	
	


	
	
