/* University of Central Florida
 * COP 3502C Fall 2016
 * Author: Michael Jimenez NID: mi535599
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Fibonacci.h"
#include <string.h>

// local functions

int *Reverse_array(unsigned int *,unsigned int); //Function that reverses given array.


// Return a pointer to a new, dynamically allocated HugeInteger struct that
// contains the result of adding the huge integers represented by p and q.
HugeInteger *hugeAdd(HugeInteger *p, HugeInteger *q)
  {
    int larger_length,i;
    unsigned int *SumArray = NULL,*DummyArrayP = NULL,*DummyArrayQ = NULL,*ResultantArray = NULL;

    if(p == NULL || q == NULL)
        return NULL;

    if(p->length > q->length || p->length == q->length) //Finds the larger length of the two arrays contained in p and q.
        larger_length = p->length;
    else
        larger_length = q->length;

    HugeInteger *ptr = malloc(sizeof(HugeInteger));            // Creates space for a structure.
    ptr->digits = malloc(sizeof(int)*larger_length+2); // Creates space for an array in the structure.
    SumArray = calloc(larger_length+2,sizeof(int));   // Creates space for the array that will hold the sum of the other arrays.
    DummyArrayP = calloc(larger_length+2,sizeof(int));// Creates space for an array that will be used to hold p's array.
    DummyArrayQ = calloc(larger_length+2,sizeof(int));// Creates space for an array that will be used to hold q's array.
    ResultantArray = calloc(larger_length+2,sizeof(int));//Creates space for an array that will hold the carried 1 values for integers > 9.

    if(DummyArrayP == NULL || DummyArrayQ == NULL || ptr == NULL || SumArray == NULL || ResultantArray == NULL)
    {
        hugeDestroyer(ptr);
        free(SumArray);
        free(DummyArrayP);
        free(DummyArrayQ);
        free(ResultantArray);
        SumArray = NULL;
        DummyArrayP = NULL;
        DummyArrayQ = NULL;
        ResultantArray = NULL;
        return NULL;
    }

    for(i=0;i<q->length;i++) // Loads all of q into a dummy array.
        DummyArrayQ[i] = q->digits[i];

    for(i=0;i<p->length;i++) // Loads all of p into a dummy array.
        DummyArrayP[i] = p->digits[i];

    for(i=0;i<larger_length+2;i++) // Logic for adding the arrays together.
    {
        if((ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i]) > 9)
        {
            SumArray[i]   = (ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i])%10;
            ResultantArray[i+1] = (ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i])/10; // when the sum of the numbers is greater than 9
            continue;
        }

        SumArray[i] = ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i];

        if(SumArray[i] > 9)
        {
            SumArray[i] = (ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i])%10;
            ResultantArray[i+1] = (ResultantArray[i] + DummyArrayP[i] + DummyArrayQ[i])/10;
        }
    }

    if(p->length == 1 && p->digits[0] == 0 && q->length == 1 && q->digits[0] == 0) // If both numbers are 0 or NULL;
    {
        ptr->length = 1;
        ptr->digits[0] = 0;
        return ptr;
    }

    for(i=(larger_length+1);i>-1;i--) //Determines that length after adding the two arrays together.
    {
        if(SumArray[i] != 0)
        {
            ptr->length = i+1;
            break;
        }
    }

    for(i=0;i<ptr->length;i++) // Puts our final result into our new structure.
        ptr->digits[i] = SumArray[i];


    return ptr;
}

//Frees the pointer passed.
HugeInteger *hugeDestroyer(HugeInteger *p)
{
    if (p == NULL)
        return p;

    free(p->digits);
	free(p);
	p = NULL;
	return p;
}

//Convert a number from string format to HugeInteger format.
HugeInteger *parseString(char *str)
{
    HugeInteger *p = malloc(sizeof *p);
    char *Char_array = NULL;
    int *Int_array = NULL,i;
    int length; //Holds the length of the str passed from main.

    if(p == NULL)
        return NULL;

    if(str == NULL) //Checks if str passed is null.
    {
        p->digits = NULL;
        p->length = NULL;
        return p;
    }

     length = strlen(str);

    p->digits = malloc(sizeof(int)*length+1);

    if(strlen(str) == 0) // If empty string is passed ("") treat it as a 0.
    {
        str = "0";
        length = 1;
        p->length = 1;
    }

    Char_array = malloc(sizeof(char) * length+1);
    Int_array = malloc(sizeof(int) * length+1);

    if(Char_array == NULL || Int_array == NULL)
        return NULL;

    strcpy(Char_array,str);

    for(i=0;i<length;i++) //Loads our int array and makes the char numbers into actual integers by subtracting char '0'.
        Int_array[i] = Char_array[i] -'0';

    Reverse_array(Int_array,length); //Sub function that reverses the integer array.

    p->length = length;

    for(i=0;i<length;i++) //Copies Reverse array into structure.
        p->digits[i] = Int_array[i];
/*
        free(Char_array);
        free(Int_array);
        Char_array = NULL;
        Int_array = NULL;
*/
    return p;
}

//Function that reverses given array and returns that array.
int *Reverse_array(unsigned int *array, unsigned int len)
{
    int i,*array2 = NULL,len_Index = len;

        array2 = malloc(sizeof(int) * len+1);

        if(array2 == NULL)
            return NULL;

    for(i=0;i<len;i++) //Loads dummy array in reverse.
        array2[i] = array[--len_Index];

    for(i=0;i<len;i++) //Copies Reverse array into array.
        array[i] = array2[i];
/*
    free(array2);
    array2 = NULL;
*/
    return array; //Returns our reversed array.
}

// Convert the unsigned integer n to HugeInteger format.
HugeInteger *parseInt(unsigned int n)
{
    HugeInteger *p = malloc(sizeof(HugeInteger));
    char *Char_array2 = NULL;
    int *Number_array2 = NULL;
    int i=0;
    unsigned int Counter = n;
    unsigned int NumLen = 0;

    if(n == 0)
        NumLen = 1;

        if(p == NULL)
            return NULL;

    while(Counter > 0) //Finds how large the int passed is.
    {
        NumLen++;
        Counter = Counter/10;
    }

    p->digits = malloc(sizeof(int)*NumLen+1);

    Char_array2 = malloc(sizeof(char) * (NumLen+1));

    sprintf(Char_array2,"%u",n); // Converts unsigned integer to string.

    Number_array2 = malloc(sizeof(unsigned int) *(NumLen+1));

    if(p->digits == NULL || Char_array2 == NULL || Number_array2 == NULL)
        return NULL;

    for(i=0; i<NumLen;i++)
        Number_array2[i] = Char_array2[i]-'0';

    Reverse_array(Number_array2,NumLen);

    p->length = NumLen;

        for(i=0;i<NumLen;i++) //Copies Reverse array into structure.
            p->digits[i] = Number_array2[i];

            Number_array2 = NULL;
            Char_array2 = NULL;
            free(Number_array2);
            free(Char_array2);

    return p;
}

// Converts the integer represented by p to a dynamically allocated unsigned int, and return a pointer to that value.
unsigned int *toUnsignedInt(HugeInteger *p)
{
    unsigned int *NumberPTR = NULL;
    char         *Char_Array = NULL;
    unsigned int *ToUnsignedNum = NULL;
    unsigned long  value = 0;
    unsigned int  i,j;

    ToUnsignedNum = malloc(sizeof(int)*p->length+1);

    if(ToUnsignedNum == NULL)
        return NULL;

    for(i=0,j=p->length-1;i<p->length;i++) // Takes the reversed array and returns it to normal.
        ToUnsignedNum[i] = p->digits[j--];

    if(p->length == 1 && p->digits[0] == 0) // If the length is 1 and the number is 0 return.
        return ToUnsignedNum;

    Char_Array = malloc(sizeof(char)*p->length+1);

    if(Char_Array == NULL)
        return NULL;

       for(i=0,j=p->length-1;i<p->length;i++) // Converts int array into char array.
        Char_Array[i] = ToUnsignedNum[i] + '0';

    NumberPTR = malloc(sizeof(unsigned int));

    if(NumberPTR == NULL)
        return NULL;

    value = strtoul(Char_Array,&Char_Array,10); //Converts the char array into a unsigned int.

    NumberPTR[0] = value;

    if(value > UINT_MAX || p->length > 10 || p->length == 0)
            return NULL;

    Char_Array = NULL;
    ToUnsignedNum = NULL;
    free(Char_Array);
    free(ToUnsignedNum);

        return NumberPTR;
}

//Calculates the Fibonacci number F(n).
HugeInteger *fib(int n)
{
    int counter;
    HugeInteger *z = malloc(sizeof(HugeInteger));
    HugeInteger *q = malloc(sizeof(HugeInteger));
    HugeInteger *p = malloc(sizeof(HugeInteger));
    p->digits = calloc(n,sizeof(int));
    q->digits = calloc(n,sizeof(int));
    z->digits = calloc(n,sizeof(int));

    if(p->digits == NULL || q->digits == NULL || z->digits == NULL || z == NULL || p == NULL || q == NULL)
            return NULL;

    if(n < 2)
    {
        p = parseInt(n);
        return p;
    }

    z = parseInt(0);
    q = parseInt(1);
    counter = 2;

    while(counter <= n) // Where the magic happens for calculating F(n).
    {
       p = q;
       q = hugeAdd(q,z);
       z = p;
       counter ++;
    }
/*
    p->digits = NULL;
    p = NULL;
    z->digits = NULL;
    z = NULL;
    hugeDestroyer(z);
    hugeDestroyer(p);
*/
    return q;
}

// A double indicating how difficult you found this assignment
//on a scale of 1.0 (ridiculously easy) through 5.0 (insanely difficult).
double difficultyRating(void)
{
    double HowHard = 3.0;

    return HowHard;
}

//An estimate (greater than zero) of the number of hours you spent on this assignment.
double hoursSpent(void)
{
    double HowLong = 27.0;

    return HowLong;
}
