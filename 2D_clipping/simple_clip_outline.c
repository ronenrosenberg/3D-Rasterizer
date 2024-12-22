#include "FPToolkit.c"
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

// Window dimensions for graphics
int swidth, sheight;

/**
 * Determines if a point lies inside a polygon using ray-casting algorithm
 * Implementation hints:
 * 1. Loop through each edge of polygon (use two adjacent vertices)
 * 2. Check if a horizontal ray from point (x,y) crosses this edge
 * 3. Count number of crossings - odd means inside, even means outside
 * 4. Be careful of edge cases (ray hitting vertex, parallel edges)
 */
int intersect_2_lines (double A[2], double B[2], double C[2], double D[2], double cross[2]) {
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

int inOutEquation(double xCo, double yCo, double c,
                  double AB[2], double CD[2]) {
    // TODO Step 1: Create array to store evaluation results
    // TODO Step 2: Evaluate line equation for goodPoint
    // Hint: multiply each coefficient by corresponding coordinate and sum
    double testAB = (xCo * AB[0]) + (yCo * AB[1]) + c;
    // TODO Step 3: Evaluate line equation for testPoint
    double testCD = (xCo * CD[0]) + (yCo * CD[1]) + c;
    // TODO Step 4: Compare signs of results and return appropriate value
    // Hint: multiply results - negative product means opposite sides
    double tester = testAB * testCD;

    if (tester > 0) {
        return 1;
    }
    return 0;
}

int inside(double x, double y, double *poly_x, double *poly_y, int n) {
    // YOUR CODE HERE
    int num_crossings = 0;

    for (int i=0; i < n; i++) {
        double x2;
        double x1;
        double y2;
        double y1;
        if (i == n-1) {
            x2 = poly_x[0];
            x1 = poly_x[i];
            y2 = poly_y[0];
            y1 = poly_y[i];
        }
        else {
            x2 = poly_x[i+1];
            x1 = poly_x[i];
            y2 = poly_y[i+1];
            y1 = poly_y[i];
        }

        double m = (y2-y1)/(x2-x1);
        double b = y1-m*x1;
        double test_x = (y-b)/m;
        
        

        if (x <= MAX(x1, x2)) {
            if ((y <= MAX(y1, y2)) && (y >= MIN(y1, y2))) {
                if (x <= test_x) {
                    num_crossings += 1;
                }
            }
        }
    }
    return num_crossings % 2;
}

/**
 * Captures mouse clicks to define first polygon vertices
 * Steps to implement:
 * 1. Draw a red click-detection bar at bottom (height 20 pixels)last_inside
 * 2. Enter loop to collect points until user clicks bar
 * 3. For each click above bar:
 *    - Store x,y coordinates in arrays
 *    - Draw a small red circle at click position
 * 4. Return number of points collected
 */
int click_and_save01(double *x, double *y) {
    // YOUR CODE HERE
    //save inputs and check for end box click
    G_rgb(1, 0, 0);
    G_fill_rectangle(0, 0, swidth, 50);
    int i = 0;
    double p[2];

    while (1) {
        //stores click x and y in p[0] and p[1]
        G_wait_click(p);
        if (p[1] < 50) {
            break;
        }
        G_rgb (0.3, 0.1, 0.8);
        G_fill_circle(p[0], p[1], 5);

        x[i] = p[0];
        y[i] = p[1];
        i++;
    }

    return i;
}

/**
 * Captures mouse clicks for second polygon vertices
 * Similar to click_and_save01, but:
 * - Draw points in blue
 * - Check if each new point lies inside first polygon
 * - If point is inside first polygon, draw it in yellow instead
 * - Use inside() function for checking
 */
int click_and_save02(double *x, double *y, double *first_x, double *first_y, int n1) {
    // YOUR CODE HERE
    G_rgb(1, 0, 0);
    G_fill_rectangle(0, 0, swidth, 50);
    int i = 0;
    double p[2];

    while (1) {
        //stores click x and y in p[0] and p[1]
        G_wait_click(p);
        if (p[1] < 50) {
            break;
        }
        G_rgb (0.3, 0.1, 0.8);

        if (inside(p[0], p[1], first_x, first_y, n1)) {
            G_rgb(255, 234, 0);
        }
        G_fill_circle(p[0], p[1], 5);

        x[i] = p[0];
        y[i] = p[1];
        i++;
    }

    return i;
}

/**
 * Displays intersection of two polygons
 * Algorithm:
 * 1. Loop through every vertex in window
 * 2. For each vertex:
 *    - Check if it's inside first polygon (using inside)
 *    - Check if it's inside second polygon
 *    - If inside both, color it yellow
 * Optimization challenge: Can you make this faster than checking every pixel?
 */
// Sutherland-Hodgman polygon clipping algorithm
int clip(double polyX[], double polyY[], int polySize, double clipX[], double clipY[], int clipSize) {
    // Step 1: Validate input
    if (clipSize < 3) {
        return 0;
    }

    // Step 2: Declare necessary variables
    double tempX[1000], tempY[1000];
    double clippedX[1000], clippedY[1000];
    double intersection[2];          
    int newPolySize = polySize;      
    double avg_x = 0;
    double avg_y = 0;

    // Copy original polygon vertices into temporary arrays & add coordinates
    for (int i = 0; i < polySize; i++) {
        tempX[i] = polyX[i];
        tempY[i] = polyY[i];
    }
   
    for (int i = 0; i < clipSize; i++) {
    avg_x += clipX[i];
        avg_y += clipY[i];
    }
   
    // 2.1 Calculate center point of clipping polygon
    double center[2];
    center[0] = avg_x / clipSize;
    center[1] = avg_y / clipSize;

    // Step 3: For each edge of the clipping polygon
    for (int i = 0; i < clipSize; i++) {
        // Define the current clipping edge
        double C[2] = {clipX[i], clipY[i]};
        double D[2] = {clipX[(i + 1) % clipSize], clipY[(i + 1) % clipSize]};

        // Calculate line equation coefficients for clipping edge
        double xCo = D[1] - C[1];
        double yCo = C[0] - D[0];
        double c = -(xCo * C[0] + yCo * C[1]);

        int clippedSize = 0;                  

        // Step 3.2: For each edge of the subject polygon
        for (int j = 0; j < newPolySize; j++) {
           
            double A[2] = {tempX[j], tempY[j]};
            double B[2] = {tempX[(j + 1) % newPolySize], tempY[(j + 1) % newPolySize]};

            // Determine if A and B are inside or outside the clip edge
            if (inOutEquation(xCo, yCo, c, center, A) == 1 && inOutEquation(xCo, yCo, c, center, B) == 1) {
                // G --> G
                clippedX[clippedSize] = B[0];
                clippedY[clippedSize] = B[1];
                clippedSize++;
            } else if (inOutEquation(xCo, yCo, c, center, A) == 1 && inOutEquation(xCo, yCo, c, center, B) == 0) {
                // G --> B
                if (intersect_2_lines(A, B, C, D, intersection) == 1) {
                    clippedX[clippedSize] = intersection[0];
                    clippedY[clippedSize] = intersection[1];
                    clippedSize++;
                }
            } else if (inOutEquation(xCo, yCo, c, center, A) == 0 && inOutEquation(xCo, yCo, c, center, B) == 1) {
                // B --> G
                if (intersect_2_lines(A, B, C, D, intersection) == 1) {
                    clippedX[clippedSize] = intersection[0];
                    clippedY[clippedSize] = intersection[1];
                    clippedSize++;
                }
                clippedX[clippedSize] = B[0];
                clippedY[clippedSize] = B[1];
                clippedSize++;
            }
            // B --> B, do nothin
        }

        // Update the new polygon size and points
        newPolySize = clippedSize;
        for (int k = 0; k < clippedSize; k++) {
            tempX[k] = clippedX[k];
            tempY[k] = clippedY[k];
        }
    }

    // Copy final clipped polygon vertices into original arrays and return the new size
    for (int i = 0; i < newPolySize; i++) {
        polyX[i] = tempX[i];
        polyY[i] = tempY[i];
    }

    G_rgb(1,0,0);
    G_fill_polygon(polyX, polyY, newPolySize);
    return newPolySize;
}

int main() {
    // Arrays to store coordinates of polygon vertices
    double xp[1000], yp[1000]; // first polygon
    double x2[1000], y2[1000]; // second polygon
    int n1, n2;                // number of vertices in each polygon
    double P[2];               // for storing click coordinates

    // 1. Initialize graphics window
    swidth = 700;
    sheight = 700;
    G_init_graphics(swidth, sheight);
    G_rgb(0, 0, 0);  // black background
    G_clear();

    // 2. Get vertices for first polygon
    // YOUR CODE HERE
    n1 = click_and_save01(xp, yp);
    
    // 3. Fill first polygon in green
    // YOUR CODE HERE
    G_rgb(0,0,1);
    G_fill_polygon(xp, yp, n1);
    // 4. Get vertices for second polygon
    // YOUR CODE HERE
    n2 = click_and_save02(x2, y2, xp, yp, n1);
    // 5. Fill second polygon in blue
    // YOUR CODE HERE
    G_rgb(0,1,0);
    G_fill_polygon(x2, y2, n2);
    // 6. Wait for click in bottom bar
    // YOUR CODE HERE
    while (1) {
        G_wait_click(P);
        if (P[1] < 50) {
            break;
        }
    }
    // 7. Show intersection of polygons
    // YOUR CODE HERE
    clip(xp, yp, n1, x2, y2, n2);
    
    // 8. Wait for any key before exiting
    int q = G_wait_key();
    return 0;
}
