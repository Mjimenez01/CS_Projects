/* University of Central Florida
 * COP 3502C Fall 2016
 * Author: Michael Jimenez NID: mi535599
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "HashyHash.h"

//This function creates a hash table. Returns a pointer to the new hash table
//(or NULL if any call to a dynamic memory allocation function fails).
HashTable *makeHashTable(int capacity)
{
    int i =0;

    HashTable *HashyHash = malloc(sizeof(HashTable));

    if(HashyHash == NULL)
        return NULL;

    HashyHash->hashFunction = NULL;  // Setting all the fields of our hash table.

    HashyHash->probing = LINEAR;
    HashyHash->stats.collisions = 0;
    HashyHash->stats.opCount = 0;
    HashyHash->size = 0;

    if(capacity <= 0)
        HashyHash->capacity = DEFAULT_CAPACITY;
    else
        HashyHash->capacity = capacity;

        HashyHash->array = malloc(sizeof(int)*HashyHash->capacity);

        if(HashyHash->array == NULL)
        {
            free(HashyHash);
            return NULL;
        }

        for(i=0;i<HashyHash->capacity;i++) // Setting all the indexes of our array to be unused.
            HashyHash->array[i] = UNUSED;

    return HashyHash;
}

//Within the hash table, set the hashFunction field to the hash function that is passed as the second parameter to this function.
//Return HASH_ERR if h is NULL. Otherwise, return HASH_OK (even if hashFunction is NULL).
int setHashFunction(HashTable *h, unsigned int (*hashFunction)(int))
{
    if(h == NULL)
        return HASH_ERR;

    h->hashFunction = hashFunction;
    return HASH_OK;
}

//Free all dynamically allocated memory associated with this hash table. Avoid segmentation faults in the event that h is NULL.
HashTable *destroyHashTable(HashTable *h)
{
    if(h == NULL)
        return NULL;

    if(h->array == NULL)  //Freeing our dynamic memory.
    {
        free(h);
        return NULL;
    }

    free(h->array);
    free(h);

    return NULL;
}

//Within the hash table, set the probing field to either LINEAR or QUADRATIC,
// depending on the value of the probing parameter passed to this function.
// If the function is successful, return HASH_OK.
// If the function fails (either because h is NULL or because the function received
//a probing value other than LINEAR or QUADRATIC), return HASH_ERR.
int setProbingMechanism(HashTable *h, ProbingType probing)
{
    if(((probing != LINEAR) && (probing != QUADRATIC)) || h == NULL)
        return HASH_ERR;

    else if(probing == LINEAR)
        h->probing = LINEAR;
    else
        h->probing = QUADRATIC;

    return HASH_OK;
}

//Using either linear or Quadratic probing, this function will insert a value into our hash table.
// Returns HASH_OK if the operation is successful. Otherwise, returns HASH_ERR.
int insert(HashTable *h, int key)
{
    int i;
    int hval = 0, index = 0;

    if(h == NULL || h->hashFunction == NULL)
        return HASH_ERR;

    if(!isAtLeastHalfEmpty(h)) // If out hash table is not at least half empty (more than half empty) expand the table.
        expandHashTable(h);

   if(h->probing == QUADRATIC)
    {

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;

        i = 0;

        while(h->array[index] != UNUSED) // Where the insertion happens.
        {
            i++;
            index = (hval + i*i) % h->capacity;
            h->stats.collisions = h->stats.collisions + 1;
        }
            h->array[index] = key;
            h->stats.opCount = h->stats.opCount + 1;
            h->size = h->size + 1;
    }

    if(h->probing == LINEAR)
    {

        i =0, hval, index;

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;

        while (h->array[index] != UNUSED) // Where the insertion happens.
        {
            i++;
            index = (hval + i) % h->capacity;
            h->stats.collisions = h->stats.collisions + 1;
        }
        h->array[index] = key;
        h->stats.opCount = h->stats.opCount + 1;
        h->size = h->size + 1;
    }

    return HASH_OK;
}

//Determines whether the hash table is at least half empty.
//Return 1 if the hash table is at least half empty.
//If the hash table is less than half empty (more than half full), or if h is NULL, return 0.
int isAtLeastHalfEmpty(HashTable *h)
{
        if(h->size > (h->capacity)/2 || h == NULL)
            return 0;
        else
            return 1;
}

// Within the hash table, replace the array (whose length is currently capacity) with a longer array.
//If the hash table is set to use linear probing, the length of the new array should be (capacity * 2 + 1).
//If the hash table is set to use quadratic probing, the length of the new array should be the first prime number
//that is greater than or equal to (capacity * 2 + 1). Returns HASH_OK if the expansion of the hash table is successful.
// If it fails for any reason (such as the failure of a memory allocation function), return HASH_ERR.
int expandHashTable(HashTable *h)
{
    int insertedNumbers[h->size],i,k=0;
    int hval, index,count,n;
    int NextPrime;

    if(h == NULL)
        return HASH_ERR;

    if(h->probing == QUADRATIC) // The work for expanding our hash table for Quadratic probing.
    {
        for(i=0;i<h->capacity;i++)
        {
            if(h->array[i] != UNUSED)
            {
                if(k >= h->size)
                    break;

                insertedNumbers[k] = h->array[i]; // Loads pre existing array values into dummy array for transfer.
                k++;
            }
        }

        free(h->array);
        count = h->size;
        h->size = 0;
        NextPrime = nextPrime((h->capacity*2)+1); // Finds next prime and then expands our array to that size.
        h->array = malloc(sizeof(int)*NextPrime);
        h->capacity = NextPrime;

        for(i=0;i<h->capacity;i++)  // sets each index to UNUSED.
                h->array[i] = UNUSED;

        for(n=0;n<count;n++) // Places our existing values into our newly resized table.
        {
            i = 0;
            index = hval = h->hashFunction(insertedNumbers[n]);
            index = index % h->capacity;
            hval = hval % h->capacity;

            while (h->array[index] != UNUSED)
            {
                i++;
                index = (hval + i*i) % h->capacity; // Quadratic
                h->stats.collisions = h->stats.collisions + 1;
            }

                h->array[index] = insertedNumbers[n];
                h->size = h->size + 1;
        }

    }

    if(h->probing == LINEAR) //  The work for expanding our hash table for Linear probing.
    {
        for(i=0;i<h->capacity;i++)
        {
            if(h->array[i] != UNUSED)
            {
                if(k >= h->size)
                    break;

                insertedNumbers[k] = h->array[i]; // Loads pre existing array values into dummy array for transfer.
                k++;
            }
        }

        free(h->array);
        count = h->size;
        h->size = 0;
        h->array = malloc(sizeof(int)*(h->capacity*2)+1);
        h->capacity = (h->capacity*2)+1;

        for(i=0;i<h->capacity;i++)  // sets each index to UNUSED.
                h->array[i] = UNUSED;

        for(n=0;n<count;n++) // Places our existing values into our newly resized table.
        {
            i = 0;
            index = hval = h->hashFunction(insertedNumbers[n]);
            index = index % h->capacity;
            hval = hval % h->capacity;

            while (h->array[index] != UNUSED)
            {
                i++;
                index = (hval + i) % h->capacity; // Linear
                h->stats.collisions = h->stats.collisions + 1;
            }

                h->array[index] = insertedNumbers[n];
                h->size = h->size + 1;
        }
    }

    return HASH_OK;
}

//Search for key in the hash tables array. If the key is found, delete it by setting the cells value to DIRTY.
// If key is deleted successfully, return the index where it resided in the hash table. Otherwise, return -1.
// If h is NULL or the hashFunction member of the hash table is NULL, return -1.
// If there are multiple instances of key in the hash table, simply delete the first one you encounter while probing.
int delete(HashTable *h, int key)
{
    int i, index, hval;

    if(h == NULL || h->hashFunction == NULL)
        return -1;

   if(h->probing == QUADRATIC)
    {
        h->stats.opCount = h->stats.opCount + 1;

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;
        i = 0;

        if(h->array[index] == key)
        {
            h->array[index] = DIRTY;
            h->size = h->size - 1;
        }

        else
        {
            while(1)  // where the deletion happens.
            {
                if(i == h->capacity)
                    return -1;

                if(h->array[index] == key)
                {
                    h->array[index] = DIRTY; // Set our deleted field to Dirty.
                    h->size = h->size - 1;
                    break;
                }

                if(h->array[index] == UNUSED)
                    return -1;

                if(h->array[index] != UNUSED && h->array[index] != key)
                    h->stats.collisions = h->stats.collisions + 1;

                i++;
                index = (hval + i*i) % h->capacity;
            }
        }

    }

    if(h->probing == LINEAR)
    {
        h->stats.opCount = h->stats.opCount + 1;

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;
        i = 0;

        if(h->array[index] == key)
        {
            h->array[index] = DIRTY;
            h->size = h->size - 1;
        }

        else
        {
            while(1)  // Where the deletion happens.
            {
                if(i == h->capacity)
                    return -1;

                if(h->array[index] == key)
                {
                    h->array[index] = DIRTY; // Set to dirty once deleted.
                    h->size = h->size - 1;
                    break;
                }

                if(h->array[index] == UNUSED)
                    return -1;

                if(h->array[index] != UNUSED && h->array[index] != key)
                    h->stats.collisions = h->stats.collisions + 1;

                i++;
                index = (hval + i) % h->capacity;
            }
        }
    }

    return index;
}

// Search's for key in the hash tables array. If the key is found, return the index where it
//resides in the hash table. Otherwise, return -1. If h is NULL or the hashFunction member of the hash table is NULL, return -1.
// If there are multiple instances of key in the hash table, simply return the index of the first one you encounter while probing.
int search(HashTable *h, int key)
{
    int i, index, hval;

    if(h == NULL || h->hashFunction == NULL)
        return -1;

   if(h->probing == QUADRATIC)
    {
        h->stats.opCount = h->stats.opCount + 1;

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;
        i = 0;

        if(h->array[index] == key)
            return index;

        else
        {
            while(1)
            {
                if(i == h->capacity)
                    return -1;

                if(h->array[index] == key)
                    return index;

                if(h->array[index] == UNUSED)
                    return -1;

                if(h->array[index] != UNUSED && h->array[index] != key)
                    h->stats.collisions = h->stats.collisions + 1;

                i++;
                index = (hval + i*i) % h->capacity; // Quadratic
            }
        }
    }

   if(h->probing == LINEAR)
    {
        h->stats.opCount = h->stats.opCount + 1;

        index = hval = h->hashFunction(key);

        index = index% h->capacity;
        hval  = hval% h->capacity;
        i = 0;

        if(h->array[index] == key)
            return index;

        else
        {
            while(1)
            {
                if(i == h->capacity)
                    return -1;

                if(h->array[index] == key)
                    return index;

                if(h->array[index] == UNUSED)
                    return -1;

                if(h->array[index] != UNUSED && h->array[index] != key)
                    h->stats.collisions = h->stats.collisions + 1;

                i++;
                index = (hval + i) % h->capacity; // Linear
            }
        }
    }
}

// Finds the next prime number greater than or equal to n. This function assumes
// that n is strictly greater than 2. If n is exactly equal to 2, this function
// actually returns 3.
int nextPrime(int n)
{
	int i, root, keepGoing = 1;

	if (n % 2 == 0)
		++n;

	while (keepGoing)
	{
		keepGoing = 0;
		root = sqrt(n);

		for (i = 3; i <= root; i++)
		{
			if (n % i == 0)
			{
				// Move on to the next candidate for primality. Since n is odd, we
				// don't want to increment it by 1. That would give us an even
				// integer greater than 2, which would necessarily be non-prime.
				n += 2;
				keepGoing = 1;

				// Leave for-loop. Move on to next iteration of while-loop.
				break;
			}
		}
	}

	return n;
}

// Returns a double indicating how difficult you found this assignment on a scale of
//1.0 (ridiculously easy) through 5.0 (insanely difficult).
double difficultyRating(void)
{
    double How_Hard = 2.0;
    return How_Hard;
}

//Return an estimate (greater than zero) of the number of hours you spent on this assignment.
double hoursSpent(void)
{
    double How_Long = 14.0;
    return How_Long;
}
