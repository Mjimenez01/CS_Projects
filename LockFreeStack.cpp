/*
 * University of Central Florida    
 * COP 4520 - Spring 2019
 * Author: Michael Jimenez
 */
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <atomic>

using namespace std;

//Generic Stack
template <class T>
class Stack
{ public:

    //Each node object will have a generic data field called val and a pointer to the next node object.
    class Node
    { public:
        T val;
        Node *next;
        
        Node(T value)
        {
            val = value;
        }
    };

    //These are the two fields that we want to update at the same time throughout our stack. 
    class descriptor
    { public: 
        atomic<Node*> head;
        atomic<int> size = {0};
        atomic<int> numOps = {0};
    };

    //The head pointer needs to be made atomic so that we prevent multiple threads from changing the pointer around.
    atomic<Node*> head;    
    atomic<descriptor*> descriptorObject;

    //Constructor
    Stack()
    {
        head = nullptr;
        descriptorObject = new descriptor();
    }

    bool push(T data)
    {
        Node *n = new Node(data);
        descriptor* newDescriptor;
        descriptor* oldDescriptor;

        //CAS to ensure that the head was not changed in the time that it took to reach this instruction. 
        do
        {    
            //Create our old descriptor object for the CAS.
            oldDescriptor = descriptorObject.load(memory_order_relaxed);

            //Create our new descriptor object for the CAS.
            newDescriptor = new_descriptor_state("Push", oldDescriptor->size,n,oldDescriptor->head.load(memory_order_relaxed),oldDescriptor->numOps);

        } while (!descriptorObject.compare_exchange_weak(oldDescriptor,newDescriptor,memory_order_relaxed,memory_order_relaxed));

        return true;
    }

    //Returns a pointer to a new descriptor object representing either the push or the pop.
    descriptor* new_descriptor_state(string OPCODE, int size, Node* newNode, Node* head,int numOps)
    {
        //Create our new descriptor object that will represent a push or a pop.
        descriptor* newDescriptor = new descriptor(); 
        
        if(OPCODE == "Push")
        {
            if(size == 0) //If the list is empty, set the newly pushed node as the head of our stack.
            {
                newDescriptor->head = newNode;
                newDescriptor->head.load(memory_order_relaxed)->next = nullptr;
                newDescriptor->size = size + 1;
                newDescriptor->numOps = numOps + 1;
            }
            else //Simple increment the size and set the newly pushed node as the head of the stack.
            {
                Node* temp = newNode;
                temp->next = head;
                head = temp;
                newDescriptor->head = head;
                newDescriptor->size = size + 1;
                newDescriptor->numOps = numOps + 1;
            }
        }
        else //Pop
        {
            if(head == nullptr) //If the stack is empty, return an empty list and keep size the same.
            {
                newDescriptor->head = nullptr;
                newDescriptor->size = size;
                newDescriptor->numOps = numOps + 1;
                return newDescriptor;
            }
            
            //Pop the top element off the stack and decrement the size of the stack.
            Node* temp = head;
            head = temp->next;
            
            // temp->next = nullptr;
            // delete(temp);
            
            newDescriptor->head = head;
            newDescriptor->size = size - 1;
            newDescriptor->numOps = numOps + 1;
        }
        
        return newDescriptor; //Return the newly create descriptor object after completing the push/pop.
    }

    T pop()
    { 
        descriptor* newDescriptor;
        descriptor* oldDescriptor;
        
        //CAS to ensure that head was not changed by another thread in the time that you stored the new head.
        while(true)
        {
            oldDescriptor = descriptorObject.load(memory_order_relaxed);

            if(oldDescriptor->head.load(memory_order_relaxed) == nullptr)
                return NULL;
            
            //Create our new descriptor object for the CAS.
            newDescriptor = new_descriptor_state("Pop", oldDescriptor->size,NULL,oldDescriptor->head.load(memory_order_relaxed),oldDescriptor->numOps);

            //If head has not been changed since it was set, break. Otherwise find head that was overwritten and set it.
            if(descriptorObject.compare_exchange_weak(oldDescriptor,newDescriptor,memory_order_relaxed,memory_order_relaxed))
            {
                break;  
            }
        }

        return oldDescriptor->head.load(memory_order_relaxed)->val;
    }

    //Returns the number of push and pop operations that we have made. 
    int getNumOps()
    {
        long numOps = descriptorObject.load(memory_order_relaxed)->numOps;

        cout << "NumOps: " << numOps << endl;
        return numOps;
    }

    //Test for INTEGERS ONLY!
    void pushTestInteger()
    {
        for(int i = 0; i < 100; i++)
            push(i);
    }

    //Test for STRINGS ONLY!
     void pushTestString()
    {
        string alphabet[26] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z"};

        for(int i = 0; i < 10; i++)
            push(alphabet[i%26]);
    }

    void popTest()
    {
        for(int i = 0; i < 99; i++)
            pop();
    }

     void printStack()
    {
        //Pointer to our atomic head used to print the stack.
        Node* ptr = descriptorObject.load(memory_order_relaxed)->head.load(memory_order_relaxed);
        
        while(ptr != NULL)
        {
            cout << ptr->val << "->";
            ptr = ptr->next;
        }
        cout << "NULL" << endl;
    }

    int size()
    {
        int size = descriptorObject.load(memory_order_relaxed)->size;
        cout << "Stack Size: " << size << endl;
        
        return size;
    }

    void countStack()
    {
        long numNodes = 0;
        //Pointer to the head of our list.
        Node* ptr = descriptorObject.load(memory_order_relaxed)->head.load(memory_order_relaxed);
        
        while(ptr != nullptr)
        {
            ptr = ptr->next;
            numNodes++;
        }

        cout << "The actual size of our stack is: " << numNodes << endl;
    }
};

int main()
{
    //==============================================================================================================================
    //==================================================  INTEGER VERSION  =========================================================
    //==============================================================================================================================

    Stack<int> *s = new Stack<int>();
    
    clock_t start = clock();
    
    thread th (&Stack<int>::pushTestInteger,s);
    thread th9 (&Stack<int>::popTest,s);    
    thread th7 (&Stack<int>::pushTestInteger,s); 
    thread th10(&Stack<int>::popTest,s);   

    th.join();
    th9.join();
    th7.join();
    th10.join();

    clock_t stop = clock();

    cout << "Execution time: " << (double)(stop-start)/CLOCKS_PER_SEC << endl;

    //Once our threads have finished, verify that the expected number of operations have happened.
    thread t1 (&Stack<int>::size,s);
    t1.join();

    thread t5 (&Stack<int>::getNumOps,s);
    t5.join();

   //Print out our stack after all of our operations so that we can see if the stack contains the expected number of elements.
    thread t2 (&Stack<int>::printStack,s);
    t2.join();

    // thread t4 (&Stack<int>::countStack,s);
    // t4.join();

    delete(s);
}