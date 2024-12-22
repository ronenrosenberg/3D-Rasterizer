
// open a sequence of .xy  files specified on the commmand line
// and draw them.

#include "FPToolkit.c"

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







int draw_object ()
{
  int h,i,j ;
  double xp[100],yp[100] ;
  int np ;

  for (i = 0 ; i < numpolys ; i++) {

    np = psize[i] ;
    for (j = 0 ; j < np ; j++) {
      h = con[i][j] ;
      xp[j] = x[h] ;
      yp[j] = y[h] ;
    }

    G_rgb(red[i], grn[i], blu[i]) ;
    G_fill_polygon(xp,yp,np) ;
  }
}







int main()
{
  FILE *fin ;
  int key,w ;
  char fname[100] ;

  printf("enter name of xy file ") ;
  scanf("%s",fname) ;
  
  fin = fopen(fname,"r") ;
  if (fin == NULL) {
      printf("can't read file, %s\n",fname) ;
      exit(1) ;
  }

  read_object(fin) ;


  
  //  print_object(stdout, 0) ;
  G_init_graphics(800,800) ;

  G_rgb(0,0,0) ;
  G_clear() ;
  draw_object() ;

  key = G_wait_key() ;

}


