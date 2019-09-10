/*
 * University of Central Florida    
 * COP 4520 - Spring 2019
 * Author: Michael Jimenez
 */
#include <iostream>
#include <thread>
#include <windows.h>
#include <chrono>
#include <mutex>

using namespace std;

//This global variable will be used to store the number of philosophers that is given as a parameter from the command prompt.
int NumPhilosophers = 0;
mutex mutex1;

// I used wikipedia and the article https://www.geeksforgeeks.org/operating-system-dining-philosopher-problem-using-semaphores/ 
// to help me get started with this problem.

// This structure will represent the chopsticks found on the table. 
struct chopstick
{
    int tableLocation;
    int previousUser;
    int threadID;
};
        
//This structure will represent our dining philosophers.
struct philosopher
{
    int tableLocation;
    struct chopstick *leftChopStick;
    struct chopstick *rightChopStick;
    bool isEating;
    bool isThinking;
};

//Function prototypes:
void DinnerTime(philosopher *p);

//this_thread::get_id() cannot be cast into an integer so we must keep track of philosophers by using their location.
void DinnerTime(philosopher *p)
{
    printf("%d is now thinking.\n",p->tableLocation);

    //Loop until the N key is pressed.
    while(true)
    {
        //To exit the program simply press the 'n' key.
        if(GetKeyState('N') & 0x8000)
            break;      

        //If our philosopher is thinking, then he is ready to eat.
        if(p->isThinking == true)
        {
            p->isThinking = false;
            printf("%d is now hungry.\n",p->tableLocation);
        }  

        //If our philosopher is hungry, then check if he has both a left and right chopstick so that he can eat.
        else if(p->leftChopStick->threadID == p->tableLocation && p->rightChopStick->threadID == p->tableLocation)
        {
            p->isEating = true;
            printf("%d is now eating.\n",p->tableLocation);
            // printf("%d just put down a left chopstick\n",p->tableLocation);
            // printf("%d just put down a right chopstick\n",p->tableLocation);

            //After our philosopher has finished eating, he will now continue to think and can give up his chopsticks.
            printf("%d is now thinking.\n",p->tableLocation);

            p->leftChopStick->threadID = -1;
            p->rightChopStick->threadID = -1;
            p->isEating = false;
            p->isThinking = true;
        }

        //If our philosopher does not have both chopsticks, then he must wait for the next available philosopher to finish.
        //Only allow the threads that have not recently used both chopsticks. This prevents deadlock and starvation.
        //http://web.eecs.utk.edu/~mbeck/classes/cs560/560/notes/Dphil/lecture.html provided help with fixing starvation.
        else
        {
            mutex1.lock();
            //If both of the current threads chopsticks are available or the current thread is the LRU, take the chopsticks. 
            if( p->leftChopStick->threadID == -1  && p->rightChopStick->threadID == -1 && p->leftChopStick->previousUser != p->tableLocation && p->rightChopStick->previousUser != p->tableLocation)
            {
                p->leftChopStick->threadID = p->tableLocation;
                //printf("%d just obtained a left chopstick\n",p->tableLocation);
                p->rightChopStick->threadID = p->tableLocation;
                //printf("%d just obtained a right chopstick\n",p->tableLocation);

                //DONT FORGET TO SET THE PREVIOUS USER TO THE CURRENT PHILOSOPHER THAT IS TAKING THE CHOPSTICKS.
                p->leftChopStick->previousUser  = p->tableLocation;
                p->rightChopStick->previousUser = p->tableLocation;
            }
            mutex1.unlock();
        }
    }
}

int main(int argc, char *argv[])
{
    //If the user does not enter the number of philosophers in the command prompt, present the user with this message and exit.
    if(argc < 2)
    {
        cout << "*** Please enter the number of philosophers that you want in this version! ***\n" << endl;
        exit(-1);
    }

    NumPhilosophers = atoi(argv[1]);

    //There are 5 philosophers aka threads and 5 chopsticks aka shared objects.
    //Simulate this problem using the given knowledge.
    //We need to create the shared objects for our chopsticks and philosophers that the threads will use. 
    struct chopstick chopsticks[NumPhilosophers];
    struct philosopher philosophers[NumPhilosophers];

    //Initialize all of our chopstick objects.
    for(int i = 0; i < NumPhilosophers; i++)
    {
        chopsticks[i].tableLocation = i;
        chopsticks[i].threadID = -1;     //This is the default for when no one has possesion of the current chopstick.
        chopsticks[i].previousUser = -1; //This is the default for when no one has seen the chopstick before.
    }

    //Now that all of the shared objects are set up, we need to launch our threads. 
    thread *threads = new thread[NumPhilosophers];

    //Initialize all of our philosopher objects.
    for(int i = 0; i < NumPhilosophers; i++)
    {
        philosophers[i].isEating = false; //By default each philosopher is initially thinking and not eating.
        philosophers[i].isThinking = true;
        philosophers[i].leftChopStick  = &chopsticks[i]; //Set the left and right chopsticks as pointers to the left and right structs.
        philosophers[i].rightChopStick = &chopsticks[(i+1)%NumPhilosophers];
        philosophers[i].tableLocation = i;
        threads[i] = thread(DinnerTime,&philosophers[i]);
    }

    //Wait for all threads to finish before continuing.
    for(int i =0; i < NumPhilosophers; i++)
        threads[i].join();

    return 0;
}