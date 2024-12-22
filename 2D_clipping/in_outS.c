#include "FPToolkit.c"

int swidth, sheight ;

//utilities
void selection_sort(double x[], int n) {
  int i,s,j;
  double tmp;

  for (i = 0; i < n; i++) {
    s = i;
    for (j = i+1; j < n; j++) {
      if (x[j] < x[s]) { s = j; }
    }
    tmp = x[i]; x[i] = x[s]; x[s] = tmp;
  }
}
void array_print(double x[], int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf("%lf\n",x[i]);
  }
  printf("\n");
}

int click_and_save (double *x, double *y)
{
  int n ;
  double P[2] ;

  G_rgb(0,1,0.5) ;
  G_fill_rectangle(0,0,swidth,20) ;

  G_rgb(1,0,0) ;
  G_wait_click(P);

  n = 0 ;
  while (P[1] > 20) {
    x[n] = P[0] ;
    y[n] = P[1] ;
    G_circle(x[n],y[n],2) ;
    if (n > 0) { G_line(x[n-1],y[n-1], x[n],y[n]) ;}
    n++ ;
    G_wait_click(P) ;
  }

  return n ;
}



int in_out (double x[], double y[], int n, double P[2])
// return 1 if point P is inside the convex polygon
// else return 0
{
  printf("asdf");
  return 1;
}



int main(int argc, char *argv[])
{
  double xp[1000],yp[1000] ;
  int n,q ;
  double P[2] ;

  //initialization
  swidth = 700 ; sheight = 700 ;
  G_init_graphics(swidth, sheight) ;
  G_rgb(0,0,0) ;
  G_clear() ;

  //click to make polygon
  G_rgb(1,0,0) ;
  n = click_and_save(xp,yp) ;
  G_rgb(0,1,0) ;
  G_fill_polygon(xp,yp,n) ;

  //point
  G_wait_click(P) ;
  G_rgb(0,0,1) ;
  G_fill_circle(P[0],P[1],2) ;


  q = G_wait_key() ;
}
