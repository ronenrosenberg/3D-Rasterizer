#include <stdio.h>
#include "FPToolkit.c"

/**
 * TODO: Implement the Sutherland-Hodgman polygon clipping algorithm
 * Complete the functions below following the pseudocode outlines
 */

/**
 * Function 1: Determine if two points lie on same side of a line
 * 
 * Parameters:
 * xCo, yCo - coefficients of the line equation ax + by + c = 0
 * c - constant term in the equation
 * goodPoint[] - reference point coordinates [x,y]
 * testPoint[] - point to test coordinates [x,y]
 * 
 * Returns: 1 if points on same side, 0 if on opposite sides
 */


/**
 * Function 2: Clip a polygon against another polygon
 * 
 * Parameters:
 * polyX[], polyY[] - coordinates of subject polygon vertices
 * polySize - number of vertices in subject polygon
 * clipX[], clipY[] - coordinates of clip polygon vertices
 * clipSize - number of vertices in clip polygon
 * 
 * Returns: size of resulting clipped polygon
 */
int window(double polyX[], double polyY[], int polySize,
           double clipX[], double clipY[], int clipSize) {
    // TODO Step 1: Validate input
    // Check if clip polygon has at least 3 vertices
    if (clipSize < 3) {
        return -1;
    }
    
        double nextY;
    // TODO Step 2: Declare necessary variables
    // - Arrays for new polygon coordinates
    // - Variables for intersection points
    // - Variables for vertex positions
    // - Counter for new polygon size
    double newPolyX[polySize];
    double newPolyY[polySize];
    //?
    //?
    int new_polygon_size = 0;
    
    // TODO Step 3: For each edge of clip polygon:
    //   3.1 Calculate center point of clip polygon
    //   3.2 For each vertex of subject polygon:
    //       - Get previous vertex (handle wrap-around) X
    //       - Get next clip vertex (handle wrap-around) X
    //       - Calculate line equation coefficients
    //       - Determine vertex positions relative to clip edge
    //       - Handle appropriate case:
    //         * Both vertices inside
    //         * Moving from inside to outside
    //         * Moving from outside to inside
    //         * Both vertices outside
    //   3.3 Update subject polygon for next iteration
    for (int i=0; i < clipSize; i++) {
        break;
    }

    for (int i=1; i < polySize; i++) {
        double prevX;
        double prevY;
        double nextX;
        double nextY;
        if (i == 0) {
            nextX = polyX[i+1];
            nextY = polyY[i+1];
            prevX = polyX[polySize-1];
            prevY = polyY[polySize-1];
        }
        else if (i == polySize-1) {
            nextX = polyX[0];
            nextY = polyY[0];
            prevX = polyX[i-1];
            prevY = polyY[i-1];
        }
        else {
            prevX = polyX[i-1];
            prevY = polyY[i-1];
            nextX = polyX[i+1];
            nextY = polyY[i+1];
        }


        

    }
    
    // TODO Step 4: Return size of clipped polygon
    
    return 0; // Replace with actual return value
}

/**
 * Helper function to find intersection of two lines
 * (Assuming this is already finished)
 */
int intersect_2_lines (double A[2], double B[2], double C[2], double D[2], double cross[2]) {

 /*---------------------------------------------*\
 |  finds the intersection of two infinite lines |
 |  k = (i+1) % count                            |
 |  return 1 if intersect and 0 if don't         |
 |        A[0] = xOne[i] A[1] = yOne[i]          |
 |        B[0] = xOne[k] B[1] = yOne[k]          |
 |        C[0] = xTwo[i] C[1] = yTwo[i]          |
 |        D[0] = xTwo[k] D[1] = yTwo[k]          |
 \*---------------------------------------------*/
    double slopeAB = (B[1]-A[1])/(B[0]-A[0]);
    double slopeCD = (D[1]-C[1])/(D[0]-C[0]);
    double bAB = B[1] - (slopeAB*B[0]);
    double bCD = D[1] - (slopeCD*D[0]);

    if (slopeAB == slopeCD) {
        return 0;
    }

    double x_intercept = (bCD-bAB)/(slopeAB-slopeCD);
    double y_intercept = slopeAB*(x_intercept) + bAB;
    cross[0] = x_intercept;
    cross[1] = y_intercept;
    return 1;
}

int main() {
    double a[2] = {0,0};
    double b[2] = {0, -1};
    double c[2] = {2, 0};
    double d[2] = {0, 2};
    double cross[2];
	printf("%d", inOutEquation(3,-4, 6, a, b));
}
