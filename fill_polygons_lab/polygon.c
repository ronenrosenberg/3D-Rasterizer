#include  "FPToolkit.c"

//written by Ronen Rosenberg (mostly)


//used to make a cool color gradient
double special_counter = 0;

//selection sort for sorting points (thanks Jeff)
void selection_sort (double x[], int n) {
  int i,s,j ;
  double tmp ;

  for (i = 0 ; i < n ; i++) {
    s = i ;
    for (j = i+1 ; j < n ; j++) {
      if (x[j] < x[s]) { s = j ; }
    }
    tmp = x[i] ; x[i] = x[s] ; x[s] = tmp ;
  }
}

//draws a polygon using given point lists
void my_polygon(double x[], double y[], int nxy) {
    for (int i = 1; i < nxy; i++) {
        G_rgb (0.965, 0.765, 0.141);
        G_line(x[i-1], y[i-1], x[i], y[i]);
    }
    G_line(x[0], y[0], x[nxy-1], y[nxy-1]);
}

//fills in drawn polygons using very cool formula
int find_intercept(double x[], double y[], int nxy, int y_level) {
    //defining the list of intercepts for this y level
    double int_x[100];
    
    int j = 0;
    //for each connection, if the connection is at the correct y level find the location it intercepts and add to the intercept list
    for (int i = 1; i < nxy; i++) {
        if ((y[i-1] >= y_level && y_level > y[i]) || (y[i-1] < y_level && y_level <= y[i])) {
            int intercept[2];

            //intercept y is just 
            intercept[1] = y_level;
            //intercept x
            intercept[0] = (((intercept[1]-y[i-1])*(x[i]-x[i-1]))/(y[i]-y[i-1])+x[i-1]);
            
            //add to intercept list
            int_x[j] = intercept[0];
            j += 1;
        }
    }
    //checks the first to last point connection manually, next time remember to try to use fionns magic trick
    if ((y[nxy-1] >= y_level && y_level > y[0]) || (y[nxy-1] < y_level && y_level <= y[0])) {
            int intercept[2];
            //intercept y is just the y level
            intercept[1] = y_level;
            //intercept x
            intercept[0] = (((intercept[1]-y[nxy-1])*(x[0]-x[nxy-1]))/(y[0]-y[nxy-1])+x[nxy-1]);
            
            //add to intercept list
            int_x[j] = intercept[0];
            j += 1;
    }

    selection_sort(int_x, j);
    

    //alternate drawing lines in x intercept list
    for (int i = 1; i < j+1; i++) {
        if (i % 2 == 1) {
            G_rgb (special_counter, special_counter/3, 1-special_counter * 3./5);
            G_line(int_x[i-1], y_level, int_x[i], y_level);
        }
    }

    //used to make a cool color gradient
    special_counter += 0.00125;
}


//assigns points into given lists
int click_and_save(double x[], double y[]) {
    //save inputs and check for end box click
    int i = 0;
    double p[2];

    while (1) {
        //stores click x and y in p[0] and p[1]
        G_wait_click(p);
        if (p[0] < 100 && p[1] < 100) {
            break;
        }
        G_rgb (0.98, 0.502, 0.447);
        G_fill_circle(p[0], p[1], 5);

        x[i] = p[0];
        y[i] = p[1];
        i++;
    }

    return i;
}


int main() {
    double x[100], y[100];
    int nxy;

    int width = 800;
    int height = 800;
    G_init_graphics(width, height);

    //I have no fucking idea what this does
    G_rgb (0.537, 0.812, 0.941); // blue
    G_clear();

    G_rgb (0.537, 0.9, 1); // darker blue
    int box_width = 100;
    int box_height = 100;
    G_fill_rectangle(0, 0, box_width, box_height);

    //nxy is the number of points clicked by the user
    nxy = click_and_save(x, y);
    my_polygon(x, y, nxy);


    //fills the polygon
    for (int i=0; i < 800; i++) {
        find_intercept(x, y, nxy, i);
    }

    //if this isn't here the window will close immediately after drawing everything
    G_wait_key();

    //must close with ctrl + c for image to get save to bmp file
    G_save_to_bmp_file("demo.bmp") ;
}