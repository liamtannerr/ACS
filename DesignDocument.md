1. How many threads are you going to use? Specify the task that you intend each thread to perform. 

I intend on using a total of 6 threads. 5 of these threads will represent clerks who will continually scan the queues and then serve the customer at the front of the queue. The last thread will handle customer arrivals by placing customers in their correct queue (business or economy) at the time of their arrival.

2. Do the threads work independently? Or, is there an overall “controller” thread? 

There will be an overall controller thread. This thread will run the handleArrivals function. The other 5 threads will be worker threads (clerks). The controller thread and worker threads communicate through the two shared queues.

3. How many mutexes are you going to use? Specify the operation that each mutex will guard. 

There will be 3 mutexes. One to guard each of the queues such that we don’t run into a race condition of multiple clerks dequeuing the same queue at the same time. The last mutex will guard the incrementation of the num_processed variable which keeps track of the total number of customers processed.

4. Will the main thread be idle? If not, what will it be doing?

 After reading from the input file and setting up the threads and mutexes, the main thread will be idle until it joins the threads after all customer have been processed. The main thread will then calculate the average wait times and print them before completion.

5. How are you going to represent customers? what type of data structure will you use?

I will create a customer data structure with an attribute for id, class, arrival time, service time. 

6. How are you going to ensure that data structures in your program will not be modified concurrently? 

With the use of mutexes. Essentially the same answer here as question 3.

7. How many convars are you going to use? For each convar: (a) Describe the condition that the convar will represent. (b) Which mutex is associated with the convar? Why? (c) What operation should be performed once pthread cond wait() has been unblocked and reacquired the mutex? 

With my design, there is no need for any condition variables.

8. Briefly sketch the overall algorithm:

Algorithm Sketch:
    Initialize:
        Read and parse customer data.
        Initialize queues and mutexes.
        Record start time.
    Handle Customer Arrivals:
        For each customer:
            Wait until arrival time.
            Print arrival time.
            If business class:
                Lock business_mutex.
                Add to business queue.
                Unlock business_mutex.
                Print queue status.
            If economy class:
                Lock economy_mutex.
                Add to economy queue.
                Unlock economy_mutex.
                Print queue status.
    Serve Customers (Clerk Threads):
        While customers to be processed:
            Try dequeue from business queue:
                Lock business_mutex.
                If not empty, dequeue customer.
                Unlock business_mutex.
            If no customer dequeued:
                Try dequeue from economy queue:
                    Lock economy_mutex.
                    If not empty, dequeue customer.
                    Unlock economy_mutex.
            If customer dequeued:
                Print start service time.
                Simulate service time.
                Print end service time.
                Lock processed_mutex.
                Increment processed customer count.
                Unlock processed_mutex. 
    Wait for Threads to Complete:
        Main thread joins handleArrivals thread.
        Main thread joins clerk threads.
    Calculate and Print Statistics:
        Calculate average wait times.
        Print average wait times.
    Cleanup:
        Free memory.
        Destroy mutexes.
