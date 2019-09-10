/*
 * University of Central Florida    
 * COP 4520 - Spring 2019
 * Author: Michael Jimenez
 */
#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <math.h>
#include <cmath>
#include <fstream>
#include <time.h>

using namespace std;

unsigned int Max = 100000000;
unsigned int ArrayIndex = 2;
bool *PrimeArray = new bool[100000001]; //A boolean array allocated from the heap that tells us if the indexed number is prime. 
unsigned long long sum = 0;
unsigned int counter = 0;
unsigned int MaxRoot = (unsigned int)sqrt(Max);

//Function Prototypes:
unsigned int getIndex();
void ValidPrime();

//This is how each thread knows what number to check. This needs to be thread safe so we use a mutex lock!
unsigned int getIndex()
{
    lock_guard<mutex> guard(mutex);
    return ArrayIndex++;
}

//This is the Sieve of Eratosthenes written in c++.
void ValidPrime()
{
    unsigned int currentNum = getIndex();

    //We only have to evaluate up to the square root of 10^8 to find all primes.
    while(currentNum <= MaxRoot)
    {
         if (PrimeArray[currentNum] == true) 
        {             
            for (unsigned int i = currentNum*currentNum; i <= Max; i += currentNum) 
            {
                PrimeArray[i] = false; 
            }
        }        
        currentNum = getIndex();
    }
}

int main()
{
    //We need to create an array that will contain our 8 threads.
    thread *threads = new thread[8];

    memset(PrimeArray, true, (Max+1)*sizeof(*PrimeArray)); //Set all of the cells in our boolean prime array to true.

    //0,1 are not prime numbers.
    PrimeArray[0] = false;
    PrimeArray[1] = false;

    //Here we launch our threads and begin computation of our work. Start the timer in milliseconds.
    //auto start = chrono::high_resolution_clock::now();
    clock_t start = clock();

    //Launch our 8 threads.
    for(int i = 0; i < 8; i++)
        threads[i] = thread(ValidPrime);

    //Make sure that all of the threads join before continuing.
    for(int i = 0; i < 8; i++)
        threads[i].join();

    //Stop the clock after we have computed all of the prime numbers from [1...100000000].
    clock_t stop = clock();

    //Calculate the sum of all of our prime numbers and number of unique primes found.
    for (unsigned int p=2; p<=Max; p++) 
    {
        if (PrimeArray[p])
        {
            sum += p;
            counter++;
        } 
    }

    //This array will hold our top 10 largest prime numbers.
    unsigned int TopTenPrimes[10];
    int TopPrimeIndex = 9;

    //Finding the top 10 largest prime numbers in our scope by counting backwards from Max and stopping after seeing the first 10.
    for(int i = Max; i >= 0; i--)
    {
        if(PrimeArray[i] == true)
        {
            TopTenPrimes[TopPrimeIndex] = i;
            --TopPrimeIndex;
        }

        //If we have seen the 10 largest prime numbers break.
        if(TopPrimeIndex < 0)
            break;
    }

    //Deallocate the memory taken up by our boolean array.
    delete PrimeArray;  

    //Print our results to primes.txt.
    ofstream ofp;
    ofp.open("primes.txt");
    ofp << "Execution time: " << (double)(stop-start)/CLOCKS_PER_SEC << " seconds" << endl;
    ofp << "We have found: " << counter << " primes" << " from [1...100000000]" <<endl;
    ofp << "The sum of all primes from [1...100000000] is : " << sum << endl;
      for(int i =0; i < 10; i++)
    {
        if(i == 0)
            ofp << "Largest Primes from [1...100000000]: {";

        (i==9) ? ofp << TopTenPrimes[i] : ofp << TopTenPrimes[i] << ", ";
    }
    ofp << "}" << endl;
    ofp.close();
    return 0;
}