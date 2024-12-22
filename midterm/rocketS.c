#include "FPToolkit.c"
#include "M2d_matrix_tools.c"
#include "math.h"


int main()
{
  // rocket
  double rx[8] = {0, 16,  7,  7,  0, -7, -7, -16 } ;
  double ry[8] = {0,  0, 15, 35, 50, 35, 15,   0 } ;

  double x_big = 16;
  double y_big = 50;
  double x_small = -16;
  double y_small = 0;

  double width = x_big - x_small;
  double height = y_big - y_small;

  G_init_graphics(700,700) ;  

  G_rgb(0,0,0) ;
  G_clear() ;
  G_rgb(0,1,0) ;

  double a[2];
  double b[2];

  G_wait_click(a);
  G_circle(a[0], a[1], 3);
  G_wait_click(b);
  G_circle(b[0], b[1], 3);

  double x_dist = b[0] - a[0];
  double y_dist = b[1] - a[1];
  double slope = y_dist/x_dist;

  printf("%lf %lf", y_dist, height);
  double scale = sqrt(x_dist * x_dist + y_dist * y_dist)/height;
  double scale_matrix[3][3];
  double rotation_matrix[3][3];
  double translation_matrix[3][3];
  double final[3][3];
  M2d_make_identity(scale_matrix);
  M2d_make_identity(rotation_matrix);
  M2d_make_identity(translation_matrix);
  M2d_make_identity(final);

  double angle = (-M_PI/2) + atan2(y_dist, x_dist) ;
  

  M2d_make_rotation(rotation_matrix, angle);
  M2d_make_scaling(scale_matrix, scale, scale);
  M2d_make_translation(translation_matrix, a[0], a[1]);

  M2d_mat_mult(final, rotation_matrix, final);
  M2d_mat_mult(final, scale_matrix, final);
  M2d_mat_mult(final, translation_matrix, final);


  M2d_mat_mult_points(rx, ry, final, rx, ry, 8);

  
  G_fill_polygon(rx,ry,8) ;
  G_wait_key() ;


}
