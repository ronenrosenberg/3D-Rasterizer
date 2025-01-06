#include "FPToolkit.c"
#include "math.h"

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

// Window dimensions for graphics
int swidth, sheight;

typedef struct{
    double *x;
    double *y;
    double *z;
    int *number;
} Rev;

//utility
void array_print(double x[], int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf("%lf\n",x[i]);
  }
  printf("\n");
}

Rev slicer(double num_slices, double x, double y) {
    double slice_interval = 2*M_PI/num_slices;
    Rev rev;
    rev.x = malloc(num_slices * sizeof(double));
    rev.y = malloc(num_slices * sizeof(double));
    rev.z = malloc(num_slices * sizeof(double));
    rev.number = malloc(num_slices * sizeof(double));

    for (int i=0; i<num_slices; i++) {
        double angle = M_PI + slice_interval*i;

        rev.x[i] = x/100;
        rev.y[i] = y*sin(angle)/100;
        rev.z[i] = y*cos(angle)/100;
    }

    return rev;
}

int click_and_save(double *x, double *y) {
    // YOUR CODE HERE
    //save inputs and check for end box click
    G_rgb(1, 0, 0);
    G_fill_rectangle(0, 0, swidth, 50);
    
    double p[2];

    int i = 1;
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
    return --i;
}

int main() {
    // Arrays to store coordinates of polygon vertices
    double xp[1000], yp[1000]; // first polygon
    int np;                    // number of vertices in each polygon
    double P[2];               // for storing click coordinates

    // 1. Initialize graphics window
    swidth = 700;
    sheight = 700;
    G_init_graphics(swidth, sheight);
    G_rgb(0, 0, 0);  // black background
    G_clear();

    // 2. Get vertices for first polygon
    // YOUR CODE HERE
    np = click_and_save(xp, yp) + 2;

    //add beginning and end
    xp[0] = xp[1];
    yp[0] = 0;
    xp[np-1] = xp[np-2];
    yp[np-1] = 0;
    
    // 3. Fill first polygon in green
    // YOUR CODE HERE
    G_rgb(0,0,1);
    G_fill_polygon(xp, yp, np);
    

    int num_slices = 200;
    
    //generate points of the revolution - each index is one slice
    Rev rev_list[np];
    for (int i=0; i<np; i++) {
        rev_list[i] = slicer(num_slices, xp[i], yp[i]);
    }

    FILE *g;
    g = fopen("revolution.xyz", "w");

    //write vertices
    fprintf(g, "%d\n", np*num_slices);
    for (int i=0; i < np; i++) {
        for (int j=0; j < num_slices; j++) {
            fprintf(g, "     %lf     %lf     %lf\n", rev_list[i].x[j], rev_list[i].y[j], rev_list[i].z[j]);
            rev_list[i].number[j] = i*num_slices + j;
        }
    }

    //write connections
    fprintf(g, "%d\n", np*num_slices-10); //placeholder value - need to find an actual way to calculate this
    for (int i=0; i < np-1; i++) {
        for (int j=0; j<num_slices; j++) {
            fprintf(g, "4 %d %d %d %d\n", rev_list[i].number[j], rev_list[i+1].number[j], rev_list[i+1].number[(j+1)%num_slices], rev_list[i].number[(j+1)%num_slices]);
        }
    }
    
    // 8. Wait for any key before exiting
    int q = G_wait_key();
    return 0;
}
