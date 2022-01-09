#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "heap1024.h"

#define APPKEY 999999      // assumed reserved by OS

#define MAXPOINTER 1000     // maximum number of memory frames, application wants
// to have at any point in time

#define TEST_LENGTH_IN_SECS 10    // duration of test in seconds

void do_error_checking ();        // forward reference

void *pointer_list[MAXPOINTER];   // set of open pointers, consecutively stored
int  pointer_count = 0;           // number of open pointers stored in above list

int mypid;                        // pid of own process; used for contents tests

int number_of_operations = 0;     // for statistics


int main (int argc, char *argv[]) {

    int syn = ( argc == 2 && argv[1][0] == 'S' );  // S = synchronize  // all other = nosysnchronize

    printf ("Synchronization Mode:  %d\n", syn);

    mypid = getpid();  // get own pid for later use

    int rc = heap_init(APPKEY, syn);   // create or access heap

    if (rc != 0) {
        perror ("Heap Access Failed");
    }
    else
        printf ("Heap Access Successfull\n");

    srand (time(NULL));  // for random runs

    long starttime = time(NULL);  // start time of test

    long currtime = starttime;

    while ( currtime - starttime  < TEST_LENGTH_IN_SECS)    // run for defined duration
    {

        // RANDOM MALLOC:

        int x = rand() % MAXPOINTER;  // value within range of max number of application pointers
        // same probability for every value assumed

        // calling malloc_1024 with adaptive probability
        // probability decreases the higher the already allocated number of frames is
        // probabilty goes to zero, if there is no more space for new frames

        if  (x > pointer_count)   // probability decreases the higher pointer_count
        {

            // note: pointer_count < x < MAXPOINTER, hence there is some space

            void *newaddr = malloc_1024();

            if (newaddr == NULL) {
                printf ("Consistency Error: heap should always have enough space  \n");
            }
            else
            {
                *((int *)newaddr) = mypid;  // store my pid in new frame (for later check)

                pointer_list[pointer_count] = newaddr;
                pointer_count++;
            }

            number_of_operations++;

        } // end random malloc


        // RANDOM FREE:

        int y = rand() % MAXPOINTER;  // value within range of max number of application pointers
        // same probability for every value assumed

        // calling free_1024 with adaptive probability
        // probability increases the higher the already allocated number of frames is
        // probabilty goes to zero, if no frames have been allocated so far

        if (y  < pointer_count)  //  probability increases the higher pointer_count

        {

            if (pointer_count > 0)    // something to free
            {

                int pos = y;  // take random pointer to free

                // note:   0 <= pos < pointer_count, and pointer_count > 0,
                //                 hence y points to a valid pointer entry

                void *freeaddr = pointer_list[pos];

                free_1024(freeaddr);  // freeing address

                number_of_operations++;

                // removing pointer from own list (pointer no longer valid):

                for (int i= pos; i < pointer_count-1; i++)
                    pointer_list[i]=pointer_list[i+1];

                pointer_count--;

            }

        } // end random free

        currtime = time(NULL);

    }

    // CHECKING:

    do_error_checking ();

    return(0);
}

//----------------------------------------------------------------------------------------------------
void do_error_checking () {

    int my_used_memory = dbg_get_my_used_mem_count();

    int free_errors = dbg_get_free_errors ();

    printf ("-------------------------------------------------------------------------------------\n");
    printf ("Number of operations:     %d\n", number_of_operations);

    printf ("Number of free errors:    %d", free_errors);
    if (free_errors > 0)
        printf ( "    < ******** ");
    printf ("\n");


    // now check contents:
    int content_errors = 0;
    for (int i = 0; i < pointer_count; i++) {
        if( (* (int *) pointer_list[i]) != mypid )
            content_errors++;

    }
    printf ("Number of content errors: %d", content_errors);
    if (content_errors > 0)
        printf ( "    < ******** ");
    printf ("\n");
    printf ("My active pointers / my frames on heap:     %d / %d", pointer_count, my_used_memory);
    if (pointer_count != my_used_memory)
        printf ( "        < ******** inconsistency!");
    printf ("\n");


    printf ("-------------------------------------------------------------------------------------\n");

}
