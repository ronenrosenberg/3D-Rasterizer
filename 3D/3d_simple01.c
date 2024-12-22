#include <time.h>
#include "FPToolkit.c"
#include "M3d_matrix_tools.c"

#define MAXPTS 59000
#define MAXPOLYS 57500

int numpoints;
int numpolys;
double x[MAXPTS];
double y[MAXPTS];
double z[MAXPTS];
int psize[MAXPOLYS];
int con[MAXPOLYS][20];
//double red[MAXPOLYS],grn[MAXPOLYS],blu[MAXPOLYS];

//c - sign change
int sign = 1;
//b - toggle clockwise/counterclockwise winding rendering mode
int backface_toggle = 0;

//xyz selection
int xyz = 3;

//transformation variables
double translate[4][4];
double rotate[4][4];
double final[4][4];
M3d_make_identity(translate);
M3d_make_identity(rotate);
M3d_make_identity(final);

double x_translate_amount = 0;
double y_translate_amount = 0;
double z_translate_amount = 0;

double x_rotate_amount = 0;
double y_rotate_amount = 0;
double z_rotate_amount = 0;

//data structure that holds the information of each polygon
typedef struct {
  int np; 
  double* x;
  double* y;
  double* z;
  double z_avg;
} Polygon;

//polygon comparison for quicksort (needed for painter's)
int poly_compare(const void* a, const void* b) {
  Polygon* poly1 = (Polygon*)a;
  Polygon* poly2 = (Polygon*)b;

  if (poly1->z_avg < poly2->z_avg) return 1;
  if (poly1->z_avg > poly2->z_avg) return -1;
  return 0;
}

int read_object(FILE *f) {
  int i,j;

    // point info
    fscanf(f,"%d",&numpoints);

    if (numpoints >= MAXPTS) {
      // need an extra for object centering
      printf("MAXPTS = %d :  exceeded.\n",MAXPTS);
      exit(1);
    }

    for (i = 0; i < numpoints; i++) {
      fscanf(f,"%lf %lf %lf",&x[i],&y[i],&z[i]);
    }

    // connectivity info
    fscanf(f,"%d",&numpolys);
    if (numpolys > MAXPOLYS) {
      printf("MAXPOLYS = %d :  exceeded.\n",MAXPOLYS);
      exit(1);
    }
    
    for (i = 0; i < numpolys; i++) {
      fscanf(f,"%d",&psize[i]);
      for (j = 0; j < psize[i]; j++) {
        fscanf(f,"%d",&con[i][j]);
      } 
    }
 
    // color info :
    /*
    for (i = 0; i < numpolys; i++) {
      fscanf(f,"%lf %lf %lf",&red[i],&grn[i],&blu[i]);
    }    
    */
}

int backface_elimination(double x[], double y[], double z[], int backface_toggle) {
  double AB[3] = {
    x[1] - x[0],
    y[1] - y[0],
    z[1] - z[0]
  };
  double AC[3] = {
    x[2] - x[0],
    y[2] - y[0],
    z[2] - z[0]
  };
  //for our purposes, no need to normalize the normal vector here
  //found by taking cross product
  double normal[3];
  M3d_x_product(normal, AB, AC);
  //reverse normal direction if file has counterclockwise winding
  if (backface_toggle) {
    normal[0] *= -1; normal[1] *= -1; normal[2] *= -1;
  }

  //the negative of points by defintion constructs a vector which points back to the origin
  double view_vector[3] = {-x[0], -y[0], -z[0]};

  double dot = M3d_dot_product(view_vector, normal); 
  
  //cull if backface
    if (dot < 0) {
      return 1;
    }
    return 0;
}

double luminance_calculator(double x[], double y[], double z[]) {
  //calculating the normal vector of the polygon
  double AB[3] = {
    x[1] - x[0],
    y[1] - y[0],
    z[1] - z[0]
  };
  double AC[3] = {
    x[2] - x[0],
    y[2] - y[0],
    z[2] - z[0]
  };
  //(taking cross product to get normal)
  double normal[3];
  M3d_x_product(normal, AB, AC);
  M3d_normalize(normal);

  //reverse normal direction if file has counterclockwise winding
  if (backface_toggle) {
    normal[0] *= -1; normal[1] *= -1; normal[2] *= -1;
  }

  //points to origin
  double view_vector[3] = {-x[0], -y[0], -z[0]};
  M3d_normalize(view_vector);

  //defining the direction the light comes from
  double light_vector[3] = {-1, -1, -1};
  M3d_normalize(light_vector);
  
  //background radiation of light: this means that it's impossible for any polygon to be rendered as completely black (0.2)
  double background = 0.2; 

  //diffuse lighting: gets stronger as the normal vector of the polygon aligns closer to the vector of the light (0-0.4)
  double diffuse = fmax(0, M3d_dot_product(normal, light_vector)) * 0.4;

  //specular lighting: adds lighting highlights, "shininess" (0-0.4)
  double half_vector[3] = {(view_vector[0]+light_vector[0]), (view_vector[1]+light_vector[1]), (view_vector[2]+light_vector[2])};
  M3d_normalize(half_vector);

  double shininess = 40;
  double specular = fmax(0, pow(M3d_dot_product(normal, half_vector), shininess)) * 0.4;

  //total
  double total = background + diffuse + specular;
  if (total > 1) total = 1; //extra safety bounding
  else if (total < 0) total = 0;
  return total;
}

int draw_object () {
  //list of polygons from xyz file
  Polygon polygons[numpolys];

  //generate the list of polygons with associated properties
  for (int i = 0; i < numpolys; i++) {
    int np = psize[i];
    polygons[i].np = np;
    polygons[i].x = malloc(np * sizeof(double));
    polygons[i].y = malloc(np * sizeof(double));
    polygons[i].z = malloc(np * sizeof(double));

	  for (int j = 0; j < np; j++) {
	    int h = con[i][j];
	    polygons[i].x[j] = x[h];
	    polygons[i].y[j] = y[h];
      polygons[i].z[j] = z[h];
	  }
  }
  
  //apply transformations
	for (int i = 0; i < numpolys; i++) {
    M3d_make_identity(final);
    //rotation
    M3d_make_x_rotation_cs(rotate, cos(x_rotate_amount), sin(x_rotate_amount));
    M3d_mat_mult(final, rotate, final);

    M3d_make_y_rotation_cs(rotate, cos(y_rotate_amount), sin(y_rotate_amount));
    M3d_mat_mult(final, rotate, final);

    M3d_make_z_rotation_cs(rotate, cos(z_rotate_amount), sin(z_rotate_amount));
    M3d_mat_mult(final, rotate, final);

    //translation
    M3d_make_translation(translate, x_translate_amount, y_translate_amount, z_translate_amount);
    M3d_mat_mult(final, translate, final);

    //apply transformations
    M3d_mat_mult_points(polygons[i].x, polygons[i].y, polygons[i].z, final, polygons[i].x, polygons[i].y, polygons[i].z, polygons[i].np);
    
    //calculate our avg z value after transformations (for painter's)
    for (int j = 0; j < polygons[i].np; j++) {
	    int h = con[i][j];
	    polygons[i].z_avg += polygons[i].z[j];
	  }
    //polygons[i].z_avg = polygons[i].np;
    polygons[i].z_avg = polygons[i].z[0]; //this seems to work better?
  }

  //sort our list of polygons based on avg z value
  qsort(polygons, numpolys, sizeof(Polygon), poly_compare);
  
  for (int i = 0; i < numpolys; i++) {
    printf("%lf ", polygons[i].z_avg);
  }
  printf("\n");

  //if not a backface, then render the polygon
  for (int i = 0; i < numpolys; i++) {
    if (!backface_elimination(polygons[i].x, polygons[i].y, polygons[i].z, backface_toggle)) {
      //luminance value for a given polygon
      double luminance = luminance_calculator(polygons[i].x, polygons[i].y, polygons[i].z);

      //adjust where rendered based on how far/close
      double fov = 450; //default 400
      for (int j = 0; j < polygons[i].np; j++) {
        polygons[i].x[j] = (polygons[i].x[j]/polygons[i].z[j] * fov + 400);
        polygons[i].y[j]  = (polygons[i].y[j]/polygons[i].z[j] * fov + 400);
      }
      
      G_rgb(luminance * 0.4, luminance, luminance * 0.85);
      G_fill_polygon(polygons[i].x, polygons[i].y, polygons[i].np);
      G_rgb(0.05, 0.125, 0.1);
      G_polygon(polygons[i].x, polygons[i].y, polygons[i].np);
    }
  }
}


int main(int argc, char **argv) {
  FILE *fin;

  G_init_graphics(800, 800);

  char key;
  while (1) {
    //currently only opens first command line object
    fin = fopen(argv[1],"r");
    if (fin == NULL) {
      printf("can't read file, %s\n",argv[key]);
      exit(1);
    }
    
    G_rgb(0.008, 0.012, 0.188);
    G_clear();
    
    read_object(fin);
    draw_object();

    //get user input
    key = (char)G_wait_key();

    //quit
    if (key == 'q'){
      break;
    }
    //reverse sign of movement
    else if (key == 'c') {
      if (sign == 1) sign = -1;
      else sign = 1;
    }
    //toggle between clockwise and counterclockwise mode for backface elimination
    else if (key == 'b') {
      if (backface_toggle == 1) backface_toggle = 0;
      else backface_toggle = 1;
    }
    //xyz selection
    else if (key == 'x') {
      xyz = 1;
    }
    else if (key == 'y') {
      xyz = 2;
    }
    else if (key == 'z') {
      xyz = 3;
    }
    //translate or rotate
    else if (key == 't') {
      if (xyz == 1) {
        x_translate_amount += 0.4 * sign;
      }
      else if (xyz == 2) {
        y_translate_amount += 0.4 * sign;
      }
      else if (xyz == 3) {
        z_translate_amount += 0.4 * sign;
      }
    }
    else if (key == 'r') {
      if (xyz == 1) {
        x_rotate_amount += 0.05 * sign;
      }
      else if (xyz == 2) {
        y_rotate_amount += 0.05 * sign;
      }
      else if (xyz == 3) {
        z_rotate_amount += 0.05 * sign;
      }
    }
  }
}
