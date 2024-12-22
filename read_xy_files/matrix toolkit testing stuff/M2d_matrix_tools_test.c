
#include "M2d_matrix_tools.c"

int main()
{
  double a[3][3] = { { 1,  2,  3},
                     { 4, -1,  2},
		     {-3,  4,  7}} ;

  double b[3][3] = { {-3,  7,  6},
                     { 1,  4,  5},
		     { 8,  1, -2}} ;  

  double c[3][3], d[3][3] ;


  M2d_mat_mult(c, a,b) ;    
  M2d_print_mat(c) ;
  printf("\n") ;
  
  M2d_make_identity(d) ;
  for (int i = 1 ; i <= 3 ; i++) {
    M2d_mat_mult(d, c,d) ;
  }
  M2d_print_mat(d) ;
  printf("\n") ;

  double x[100] = {100, 130, 140, 170 } ;
  double y[100] = { 70, 230, 515, 431 } ;  

  M2d_mat_mult_points(x,y,  c, x,y,4) ;

  for (int i = 0 ; i < 4 ; i++) {
    printf("%12.4lf %12.4lf\n",x[i],y[i]) ;
  }


  double e[3][3] ;
  double A[2] = {100,200} ;
  M2d_copy_mat(e, a) ;
  M2d_mat_mult_pt(A, e,A) ;
  printf("%lf %lf\n",A[0],A[1]) ;
  
}
