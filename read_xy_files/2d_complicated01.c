
// open a sequence of .xy  files specified on the commmand line
// and draw them.

#include "FPToolkit.c"
#include "M2d_matrix_tools.c"

#define MAXPTS 59000
#define MAXPOLYS 57500


int numpoints ;
int numpolys ;
double x[MAXPTS] ;
double y[MAXPTS] ;
int psize[MAXPOLYS] ;
int con[MAXPOLYS][20] ;
double red[MAXPOLYS],grn[MAXPOLYS],blu[MAXPOLYS] ;



int read_object(FILE *f)
{
  int i,j ;

    // point info
    fscanf(f,"%d",&numpoints) ;

    if (numpoints >= MAXPTS) {
      // need an extra for object centering
      printf("MAXPTS = %d :  exceeded.\n",MAXPTS) ;
      exit(1) ;
    }

    for (i = 0 ; i < numpoints ; i++) {
      fscanf(f,"%lf %lf",&x[i],&y[i]) ;
    }

    // connectivity info
    fscanf(f,"%d",&numpolys) ;
    if (numpolys > MAXPOLYS) {
      printf("MAXPOLYS = %d :  exceeded.\n",MAXPOLYS) ;
      exit(1) ;
    }
    
    
    for (i = 0 ; i < numpolys ; i++) {
      fscanf(f,"%d",&psize[i]) ;
      for (j = 0 ; j < psize[i] ; j++) {
        fscanf(f,"%d",&con[i][j]) ;
      } // end for j
    } // end for i

    
    // color info :
    for (i = 0 ; i < numpolys ; i++) {
      fscanf(f,"%lf %lf %lf",&red[i],&grn[i],&blu[i]) ;
    }    
}




int print_object (FILE *fout)
{
  int i,j ;

  fprintf(fout, "%d\n",numpoints) ;

  for (i = 0 ; i < numpoints ; i++) {
    fprintf(fout, "%12.6lf %12.6lf\n",x[i],y[i]) ;
  }
  

  for (i = 0 ; i < numpolys ; i++) {
    fprintf(fout, "%3d    ",psize[i]) ;

    for (j = 0 ; j < psize[i] ; j++) {
      fprintf(fout, "%2d ", con[i][j]) ;
    }
    fprintf(fout, "\n") ;
  }


  for (i = 0 ; i < numpolys ; i++) {
    fprintf(fout,"%lf %lf %lf\n",red[i],grn[i],blu[i]) ;
  }      
}


//sorting algorithm (thanks jeff)
void selection_sort (double x[], int n) 
{
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

void array_print (double x[], int n)
{
  int i ;
  for (i = 0 ; i < n ; i++) {
    printf("%lf\n",x[i]) ;
  }
  printf("\n") ;
}



int draw_object ()
{
  
  
  
  double x_high = -1000; double y_high = -1000;
  double x_low = 1000; double y_low = 1000;
  
  //find highest and lowest x/y
  for (int i = 0 ; i < numpolys ; i++) {
    int np = psize[i] ;
    double xp[np], yp[np];
      

    for (int j = 0 ; j < np; j++) {
      int h = con[i][j];

      xp[j] = x[h];
      yp[j] = y[h];

      if (xp[j] > x_high) {
        x_high = xp[j];
      }
      if (yp[j] > y_high) {
        y_high = yp[j];
      }
      if (xp[j] < x_low) {
        x_low = xp[j];
      }
      if (yp[j] < y_low) {
        y_low = yp[j];
      }
    }
  }

  //printf("%lf %lf %lf %lf asfasfdasd", x_high, y_high, x_low, y_low);

  //just draws the polygons
  for (int i = 0 ; i < numpolys ; i++) {
    int np = psize[i] ;
    double xp[np], yp[np];
    

    

    for (int j = 0 ; j < np ; j++) {
      int h = con[i][j];

      xp[j] = x[h];
      yp[j] = y[h];
    }

    double width = (x_high - x_low);
    double height = (y_high - y_low);
    double mid_x = (x_high + x_low)/2.0;
    double mid_y = (y_high + y_low)/2.0;
    G_circle(mid_x, mid_y, 3);
    double scale_factor;

    if (width > height) {
      scale_factor = (800/height) * 0.9;
    }
    else {
      scale_factor = (800/width) * 0.9;
    }
    scale_factor = 1.5;

    double translate[3][3];
    double scale[3][3];
    double rotate[3][3];
    double final[3][3];
    M2d_make_identity(translate);
    M2d_make_identity(scale);
    M2d_make_identity(rotate);
    M2d_make_identity(final);
    
    
    M2d_make_translation(translate, -mid_x, -mid_y);
    M2d_make_scaling(scale, scale_factor, scale_factor);
    
    M2d_mat_mult(final, final, scale);

    M2d_mat_mult(final, final, translate);
    M2d_make_translation(translate, 400, 400);
    M2d_mat_mult(final, final, translate);

    for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf("%lf", scale[i][j]);
    }
    printf("\n");
  }
    
    

    for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf("%lf", final[i][j]);
    }
    printf("\n");
  }

    
    

    M2d_mat_mult_points(xp, yp, final, xp, yp, np);
    //array_print(xp_new, np);

    G_rgb(red[i], grn[i], blu[i]) ;
    G_fill_polygon(xp,yp,np) ;
  }
}







int main(int argc, char **argv) {
  FILE *fin ;
  
  //  print_object(stdout, 0) ;
  G_init_graphics(800,800) ;


  int key = 1;
  while (1) {
    fin = fopen(argv[key],"r") ;
    if (fin == NULL) {
      printf("can't read file, %s\n",argv[key]) ;
      exit(1);
    }
    G_rgb(0,0,0) ;
    G_clear() ;
    
    read_object(fin);
    draw_object();


    key = G_wait_key();
    

    if (key >= 48 && key <= 48 + argc - 1) {
      key -= 47;
    }
    else {
      key = 1;
    }
  }
  

  

}


