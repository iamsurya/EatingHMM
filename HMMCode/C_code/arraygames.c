/* Creating arrays using different methods and accessing them to
** understand how to use them with BSL
** I want to create an array using a[3][3]
** but access using b[7]
*/

#include <stdio.h>

main()
{
    double a_data[] = { 1.0, 0.6, 0.0,
                        0.0, 1.5, 1.0,
                        0.0, 1.0, 1.0 };

    int c[][3] = {
    	{1,2,3},
    	{4,5,6},
    	{7,8,9}};

    	int d[][3][3] = {
    		{{1,2,3},
    		 {4,5,6},
    		 {7,8,9}},
    		{{11,12,13},
    		 {14,15,16},
    		 {17,18,19}}};

    double a_data1[][3] = { 1.0, 0.6, 0.0,
                        0.0, 1.5, 1.0,
                        0.0, 1.0, 1.0 };

/* 2D to 1D rep */
int * b_data = (int *) c[2]; /* Row 2+1 = 3 (You can exclude [0] for element 0*/ 

printf("%d\n", b_data[2]);

/* 3D to 2D rep */

int (*c_data)[3] = (int (*)[3]) &d[1];
//int (*c_data)[3] = (int (*)[3]) &d[1][0][0];

printf("%d\n", c_data[2][1]);


}