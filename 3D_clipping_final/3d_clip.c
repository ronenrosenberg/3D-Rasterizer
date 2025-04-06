#include <time.h>
#include "FPToolkit.c"
#include "M3d_matrix_tools.c"

#define MAXPTS 59000
#define MAXPOLYS 57500
#define EPSILON 1e-7

const int view_x = 1300;
const int view_y = 900;
const double aspect_ratio = (double)view_x/view_y;
const int fov = 90; // in degrees

double fov_scaler;
double half_angle; //in radians

int numpoints;
int numpolys;
double x[MAXPTS];
double y[MAXPTS];
double z[MAXPTS];
int psize[MAXPOLYS];
int con[MAXPOLYS][20];

//to define clipping planes
double a[6],b[6],c[6],d[6];

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

//clipping stuff
int  clip_line_against_plane(double a, double b, double c, double d, 
                             double polyx[], double polyy[], double polyz[], int size,
                             double resx[], double resy[], double resz[]) {
  int num,i,j ;
  double x1,y1,z1,x2,y2,z2,x21,y21,z21,den,t,xintsct,yintsct,zintsct;
  double s1,s2 ;

  num = 0 ;
  for (i = 0 ; i < size ; i++) {
    j = (i + 1) % size ;

    // load up segment to be clipped
    x1 = polyx[i] ; y1 = polyy[i], z1 = polyz[i] ;
    x2 = polyx[j] ; y2 = polyy[j], z2 = polyz[j];

    // clip line segment (x1,y1)-(x2,y2) against line
    s1 = (a*x1 + b*y1 + c*z1 + d) ;
    s2 = (a*x2 + b*y2 + c*z2 + d) ;


    if (fabs(s1) < EPSILON) s1 = 0; //edge cases where almost but not quite 0, set to 0
    if (fabs(s2) < EPSILON) s2 = 0; //edge cases where almost but not quite 0, set to 0

    if ((s1 >= 0) && (s2 >= 0)) {
      // out to out, do nothing
    } else if ((s1 < 0) && (s2 < 0)) {
      // in to in
      resx[num] = x2 ; resy[num] = y2 ; resz[num] = z2; num++ ;
    } else {
      // one is in, the other out, so find the intersection

      x21 = x2 - x1 ; y21 = y2 - y1 ; z21 = z2 - z1;
      den = a*x21 + b*y21 + c*z21;
      if (den == 0) continue ; // do nothing-should never happen (if pretty close to 0)
      if (fabs(den) < EPSILON) {
  // Treat the segment as if it intersects the plane if it's very close
  t = 0; // Treat as directly on the plane (this can be a heuristic)
}
      
      t = -(a*x1 + b*y1 + c*z1 + d)/den ;
      xintsct = x1 + t*x21 ;
      yintsct = y1 + t*y21 ;
      zintsct = z1 + t*z21 ;

      if (fabs(xintsct) < EPSILON) xintsct = 0;
      if (fabs(yintsct) < EPSILON) yintsct = 0;
      if (fabs(zintsct) < EPSILON) zintsct = 0;

      if (s1 < 0) { 
        // in to out
        resx[num] = xintsct ; resy[num] = yintsct ; resz[num] = zintsct ; num++ ;
      } else  {
        // out to in
        resx[num] = xintsct ; resy[num] = yintsct ; resz[num] = zintsct ; num++ ;
        resx[num] = x2      ; resy[num] = y2      ; resz[num] = z2 ; num++ ;
      }
    }
  } // end for i
  return num ;  // return size of the result poly
}
int clip_against_view_frustrum (double px[], double py[], double pz[], int psize) {
    double nx[100],ny[100],nz[100];

    // clip the polygon against plane of the view frustrum
    for (int i=0; i < 6; i++) {
      psize = clip_line_against_plane(a[i],b[i],c[i],d[i],
                                          px,py,pz, psize,
                                          nx,ny,nz) ;
      // copy back in preparation for next pass
      for (int j = 0 ; j < psize ; j++) {
        px[j] = nx[j] ;   
        py[j] = ny[j] ;  
        pz[j] = nz[j] ;
      }
    }
  return psize ;
}
void deform(double x[], double y[], double z[], int np) {
  double variance = 0.1;
  for (int i=0; i < np; i++) {
    x[i] += (rand() / (double)RAND_MAX) * 0.15 - 0.075;
    y[i] += (rand() / (double)RAND_MAX) * 0.15 - 0.075;
    z[i] += (rand() / (double)RAND_MAX) * 0.15 - 0.075;
  }
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
    polygons[i].z_avg = polygons[i].z[0]; //seems to work better when I just grab arbitrary z value
  }

  //sort our list of polygons based on avg z value
  qsort(polygons, numpolys, sizeof(Polygon), poly_compare);

  //if not a backface, then render the polygon
  for (int i = 0; i < numpolys; i++) {
    if (!backface_elimination(polygons[i].x, polygons[i].y, polygons[i].z, backface_toggle)) {
      deform(polygons[i].x, polygons[i].y, polygons[i].z, polygons[i].np); //function that makes everything all shimmery :)

      //luminance value for a given polygon
      double luminance = luminance_calculator(polygons[i].x, polygons[i].y, polygons[i].z);
      
      //clip against the view volume
      polygons[i].np = clip_against_view_frustrum(polygons[i].x, polygons[i].y, polygons[i].z, polygons[i].np);

      //adjust where rendered based on how far/close
      for (int j = 0; j < polygons[i].np; j++) {
        polygons[i].x[j] = (polygons[i].x[j]/polygons[i].z[j] * fov_scaler + (view_x/2));
        polygons[i].y[j]  = (polygons[i].y[j]/polygons[i].z[j] * fov_scaler + (view_y/2));
      }
      
      G_rgb(luminance * 0.4 * polygons[i].x[0]/view_x, luminance * polygons[i].x[0]/view_x * 0.8 * 1.2, luminance * 0.85 * polygons[i].y[0]/view_y * 1.8 * 1.2);
      G_fill_polygon(polygons[i].x, polygons[i].y, polygons[i].np);
      G_rgb(0.05, 0.125, 0.1);
      G_polygon(polygons[i].x, polygons[i].y, polygons[i].np);
    }
  }
}


int main(int argc, char **argv) {
  FILE *fin;

  //define perspective projection stuff
  half_angle = ((fov * (M_PI / 180.0)) / 2.0); // in radians
  fov_scaler = view_y / (2 * tan(half_angle));
  
  //define planes
  a[0] = 0; b[0] = 0; c[0] = -1; d[0] = 2; //near
  a[1] = 0; b[1] = 0; c[1] = 1; d[1] = -10000; //far
  a[2] = 0; b[2] = 1; c[2] = -tan(half_angle); d[2] = 0; //up
  a[3] = 0; b[3] = -1; c[3] = -tan(half_angle); d[3] = 0; //down
  a[4] = 1; b[4] = 0; c[4] = -tan(half_angle)*aspect_ratio; d[4] = 0; //left
  a[5] = -1; b[5] = 0; c[5] = -tan(half_angle)*aspect_ratio; d[5] = 0; //right

  //define matrices
  M3d_make_identity(translate);
  M3d_make_identity(rotate);
  M3d_make_identity(final);

  //initialize graphics
  G_init_graphics(view_x, view_y);

  //currently only opens first command line object
  fin = fopen(argv[1],"r");
  if (fin == NULL) {
    printf("can't read file, %s\n",argv[1]);
    exit(1);
  }
  read_object(fin);

  char key;
  while (1) {
    G_rgb(0.008, 0.012, 0.188);
    G_clear();
    
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
