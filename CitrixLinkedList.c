/*
Michael Jimenez Citrix interview question!
Persistence and resilience will prevail!
I tried to mimic the question from memory the best that I could!
This program creates a linked list which inserts its data in ascending order. The program also prints our list in ascending order.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int addNodes(int);
void printSortedList();

typedef struct List //Structure that points to the head of our linked list.
{
    struct Node *phead;
}List;

List gTestNode;

typedef struct Node //Structure that is used to store data and pointers to our other nodes.
{
    struct Node *next;
       int data;
}Node;

int addNodes(int integer) //Function that adds our nodes in ascending order upon insertion.
{
    int retVal = 0;

    Node *n = malloc(sizeof(Node)); //Create our new node!

    Node *temp  = NULL; //These are our temp pointers. We will use them to iterate through our list!
    Node *ptemp = NULL;

    if(n == NULL) //If we run out of memory we need to exit.
    {
        printf("Out of memory in addNodes() Function\n");
        exit(1);
    }

    n->data = integer;
    n->next = NULL;

    if(gTestNode.phead == NULL) //If we don't have a head node! Create one!
    {
        gTestNode.phead = n;
        return retVal; //Don't forget to return upon this edge case!
    }

    temp  = gTestNode.phead; //Point our temp pointers at the head of our list.
    ptemp = gTestNode.phead;

    if(n->data <= temp->data) //If our new node is the smallest element in our list. Just insert at the head!
    {
        n->next = temp;
        gTestNode.phead = n;
        return retVal;
    }

    //If our nodes data is larger than the first node,we need to traverse our list! To do this we use temp/ptemp (previous temp).
    while(temp != NULL)
    {
        if(n->data >= temp->data)//If new data is greater than current data,we need to increment temp and look at the next node!
         temp = temp->next;

        else if(n->data <= temp->data) //If the node to be inserted lies in between two nodes currently in our list!
        {
            ptemp->next = n;
            n->next = temp;
            return retVal;
        }

         if(ptemp->next != temp)// This insures that our previous temp pointer is always trailing our temp pointer!
            ptemp = ptemp->next;

        if(temp == NULL) //If we have reached the end of our list, our new node contains the largest element! Tail insert.
        {
         ptemp->next = n;
         return retVal;
        }
    }

    return retVal;
}

void printSortedList()
{
    Node *temp = NULL;

     if(gTestNode.phead == NULL) //If we don't have a head node!
     {
         printf("We don't have any nodes to print!\n");
         return;
     }

     temp = gTestNode.phead; //Temp points to the head of our list.

     printf("Printing our list: HEAD ->");

     while(temp != NULL) //This loop iterates through our list and prints our the data field in order!
     {
         printf(" [%d] ->",temp->data);
         temp = temp->next;
     }

     printf(" NULL\n\n");
}

int main()
{
    int testcase0 = 0; //*** Set the test case to 1 if you would like to see test output ***
    int testcase1 = 0;
    int testcase2 = 0;
    int testcase3 = 0;
    int testcase4 = 0;
    int testcase5 = 0;
    int testcase6 = 0;
    int testcase7 = 0;
    int testcase8 = 1;

    int i = 0;
    int r = 0;

    if(testcase0) //Basic test case, positive and negative numbers randomly inserted!
    {
        addNodes(-1);
        addNodes(10);
        addNodes(100);
        addNodes(200);
        addNodes(1);
        addNodes(30);
        addNodes(150);
        addNodes(301);
        addNodes(301);
        addNodes(5000);
        addNodes(-11234);
        printSortedList();
    }

    if(testcase1) //TestCase that tests what happens if we send NULL to our addNodes function!
    {
        addNodes(NULL);
        printSortedList();
    }

    if(testcase2) //TestCase that tests what happens if we call our printSortedList function without adding anyNodes!
        printSortedList();


    if(testcase3) //Prints a large list and checks that we are tail inserting properly.
    {
        for(i=0; i < 50; i++)
        {
            addNodes(i);
        }
        printSortedList();
    }


    if(testcase4) //Prints a large list and checks that we are head inserting properly.
    {
        for(i=50; i > 0; i--)
        {
            addNodes(i);
        }
        printSortedList();
    }


    if(testcase5) //TestCase that tests large random numbers inserted in a random order! (Positive only).
    {
        srand(time(NULL)); //Seed the random generator!

        for (i = 0; i < 10; i++)
        {
            r = rand() % 20000 + 1;
            addNodes(r);
        }
        printSortedList();
    }


    if(testcase6) //TestCase that tests large random numbers inserted in a random order! (Positive and negative).
    {
        srand(time(NULL)); //Seed the random generator!

        for (i = 0; i < 10; i++)
        {
            r = rand() % 20000 + 1;

            if(r%2 == 0) //Randomly generating negative numbers to insert into the list!
                r = r * -1;
            addNodes(r);
        }
        printSortedList();
    }


    if(testcase7) //TestCase that tests large random numbers inserted in a random order! Prints list at each step (Positive only).
    {
        srand(time(NULL)); //Seed the random generator!

        for (i = 0; i < 10; i++)
        {
            r = rand() % 20000 + 1;
            addNodes(r);
            printSortedList(); //Prints multiple lists to isolate step by step insertion.
        }
    }

    if(testcase8) //TestCase that test large random numbers inserted in a random order! Prints list at each step (Positive and negative).
    {
        srand(time(NULL)); //Seed the random generator!

        for (i = 0; i < 10; i++)
        {
            r = rand() % 20000 + 1;

            if(r%2 == 0) //Randomly generating negative numbers to insert into the list!
                r = r * -1;
            addNodes(r);
            printSortedList(); //Prints multiple lists to isolate step by step insertion.
        }
    }

   return 0;
}
