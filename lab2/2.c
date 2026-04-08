// Name: Awni AlQuraini 
// Date: 04/08/2026
// Title: Lab 2 - Part 2
// Description: Calculates total delays for a couple of cases

#include <stdio.h>
#include <math.h>


int main(){
    int rtt_0 = 3;
    int rtt_1 = 20;
    int rtt_2 = 26;
    int rtt_http = 47;

    int pt_a = rtt_0 + rtt_1 + rtt_2 + (2 * rtt_http); // total dns time with the amount of time for 2 http requests: 1 to request file and 1 to send the data
    printf("Part A - The amount of time required for sending one HTML object: %d ms\n", pt_a);
    
    int pt_b = pt_a + (12 * rtt_http); // same as pt a but with extra overhead of requesting the 12 other objects
    printf("Part B - The amount of time required for sending one HTML object + 6 extra obj: %d ms\n", pt_b);

    int num_p;
    printf("Enter a number for the amount of parallel TCP connections: ");
    scanf("%d", &num_p);
    if(num_p <= 1){
        printf("Not valid for parallel connections");
        return 1;
    }
    
    int obj_batches = (int)ceil((double)6 / num_p); //celing to get the largest integer to the float (if we use a large n it will perform the same as 6 parallel connections.

    int pt_c_no_persist, pt_c_persist;

    pt_c_no_persist = pt_a + (obj_batches * 2 * rtt_http);
    pt_c_persist = pt_a + (obj_batches * rtt_http);

    printf("Part C - time required for sending one HTML object + 6 extra obj using %d parallel connections w/ presistence: %d ms\n", num_p, pt_c_persist);
    printf("Part C - time required for sending one HTML object + 6 extra obj using %d parallel connections w/o presistence: %d ms\n", num_p, pt_c_no_persist);

    return 0;
}