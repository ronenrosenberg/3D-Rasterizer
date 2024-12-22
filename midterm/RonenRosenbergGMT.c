#include "FPToolkit.c"
#include "M2d_matrix_tools.c"
#include "math.h"

// Run the program, jetmat.exec, by typing  ./jetmat.exec

// By continually holding down on a key, you will see a movie.
// Your task is to duplicate this program.

// Notice that the jet keeps moving until the user types the 'q' key,
// and then the program terminates.  Also notice that the program
// prints the number of each frame that is displayed.

// With each key-press, the jet moves 2 units (each pixel is 1 unit by 1 unit).

// You must move the jet by only using matrix operations-NOT procedurally.

// For full credit, compute any 3x3 matrices you need BEFORE the loop
// that displays the movie.  An efficient solution does not need
// to perpetually rebuild them with each new scene.


//jet
double jx[5] = {0, 40, 35, 10,  0} ;
double jy[5] = {0,  0,  8,  8, 15} ;

//written by Ronen Rosenberg
int main()
{
  //add M_PI to get rocket to rotation origin
  double angle = M_PI + atan2(500, -500);
  double current_x = 0;
  double current_y = 500;

  //make all matrices, set rocket to starting position
  double rotation_matrix[3][3];
  double translation_matrix[3][3];
  double mirror_matrix[3][3];
  double final[3][3];

  M2d_make_identity(rotation_matrix);
  M2d_make_identity(translation_matrix);
  M2d_make_identity(mirror_matrix); mirror_matrix[0][0] = -1;
  M2d_make_identity(final);

  M2d_make_rotation(rotation_matrix, angle);
  M2d_make_translation(translation_matrix, 0, 500);

  M2d_mat_mult(final, rotation_matrix, final);
  M2d_mat_mult(final, translation_matrix, final);

  M2d_mat_mult_points(jx, jy, final, jx, jy, 8);

  //draws first frame
  G_init_graphics(600,600) ;
  G_rgb(0,0,0) ;  G_clear() ;
  G_rgb(0,0,1) ;  G_fill_polygon(jx,jy,5) ;
  G_rgb(1,1,0) ;  G_draw_string("any key to continue", 250,10) ;
  G_rgb(1,0,1) ;  G_line(0,500,500,0) ;
  G_wait_key() ;

  //translation matrix should be this during the loop so it moves 1 at a time
  M2d_make_translation(translation_matrix, 1, -1);

  //frame counter
  int frame = 1;
  int user_input;
  //main loop
  while (1) {
    M2d_make_identity(final);

    //reset rocket to origin, flip on x_axis, then place back with new correct rotation
    if (frame % 50 == 0) {
      //reset to origin, mirror
      M2d_make_translation(translation_matrix, -current_x, -current_y);
      M2d_make_rotation(rotation_matrix, -angle);

      M2d_mat_mult(final, translation_matrix, final);
      M2d_mat_mult(final, rotation_matrix, final);
      M2d_mat_mult(final, mirror_matrix, final);
      
      //put back, with new rotation
      M2d_make_rotation(rotation_matrix, angle - M_PI); //b/c flip on x, we subtract PI when we make new rotation
      M2d_make_translation(translation_matrix, current_x, current_y);

      M2d_mat_mult(final, rotation_matrix, final);
      M2d_mat_mult(final, translation_matrix, final);
      
      //reset to normal translation matrix
      M2d_make_translation(translation_matrix, 1, -1);
    }
    
    //translate by translation matrix
    M2d_mat_mult(final, translation_matrix, final);
    //apply final matrix
    M2d_mat_mult_points(jx, jy, final, jx, jy, 8);

    //draws frame
    G_rgb(0,0,0) ;  G_clear() ;
    G_rgb(0,0,1) ;  G_fill_polygon(jx,jy,5) ;
    G_rgb(1,1,0) ;  
    G_rgb(1,0,1) ;  G_line(0,500,500,0) ;
    user_input = G_wait_key();
    
    //update current x/y as well as frame
    current_x += 1;
    current_y += -1;
    frame += 1;
    printf("%d \n", frame);
    printf("%d", user_input);

    //quits if user presses q
    if (user_input == 113) {
      break;
    }
  }
}
