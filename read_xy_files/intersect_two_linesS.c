
#include "FPToolkit.c"


int window_size ;

double gap = 20 ;

void grid()
{
  int i ;
  for (i = 0 ; i < window_size ; i+= gap) {
    G_line(i,0, i,window_size) ;
    G_line(0,i, window_size,i) ;
  }
}





int intersect_2_lines (double A[2], double B[2],
                       double C[2], double D[2],
                       double intersection[2])
// return 0 if lines do NOT intersect
// return 1 if they do  
{
  // YOU NEED TO IMPLEMENT THIS!!!!!!!
  double determinant;

  //finding other parts of the system of equations
  //A to B
  double a1 = B[1] - A[1];
  double b1 = A[0] - B[0];
  double c1 = a1*(A[0]) + b1*(A[1]);
  //C to D
  double a2 = D[1] - C[1];
  double b2 = C[0] - D[0];
  double c2 = a2*(C[0]) + b2*(C[1]);

  //determinant tells us if there's a solution to our system of equations
  determinant = a1*b2 - a2*b1;

  //no solution case (parallel)
  if (determinant == 0) {
        return 0;
  }
  //find solution using determinant
  else {
      intersection[0] = (b2*c1 - b1*c2)/determinant;
      intersection[1] = (a1*c2 - a2*c1)/determinant;
      return 1;
  }
  
}



int main()
{
   double a[2],b[2],c[2],d[2] ;
   double intersect[2] ;
   double signal, xi,yi ;
   char q ;

   printf("Click two points to determine a line segment,\n");
   printf("then click two more for another line segment.\n");

   window_size = 800 ;
   G_init_graphics(window_size,window_size) ;
   G_rgb(0,0,0) ;
   G_clear() ;
   G_rgb(0.5,0.5,0.5) ;
   grid() ;
   G_rgb(0,1,0) ;
   
   G_wait_click(a) ;
   a[0] = gap*floor((a[0]+0.5*gap)/gap) ;
   a[1] = gap*floor((a[1]+0.5*gap)/gap) ;   
   G_fill_circle(a[0],a[1],3) ;

   
   G_wait_click(b) ;
   b[0] = gap*floor((b[0]+0.5*gap)/gap) ;
   b[1] = gap*floor((b[1]+0.5*gap)/gap) ;
   G_fill_circle(b[0],b[1],3) ;   

   G_line (a[0],a[1], b[0],b[1]) ;

   G_wait_click(c) ;
   c[0] = gap*floor((c[0]+0.5*gap)/gap) ;
   c[1] = gap*floor((c[1]+0.5*gap)/gap) ;
   G_fill_circle(c[0],c[1],3) ;      

   G_wait_click(d) ;
   d[0] = gap*floor((d[0]+0.5*gap)/gap) ;
   d[1] = gap*floor((d[1]+0.5*gap)/gap) ;
   G_fill_circle(d[0],d[1],3) ;         

   G_line (c[0],c[1], d[0],d[1]) ;


   
   signal = intersect_2_lines (a, b,  c, d,
                               intersect) ;
   if (signal == 0) {
     G_rgb(1,0,0) ;
     G_draw_string("The two lines do NOT intersect in a unique point.",20,20) ;
     // this covers the case that the two lines are parallel
     // (also if they are identical)     
   } else {

     xi = intersect[0] ;
     yi = intersect[1] ;

     G_rgb(1, 1, 0) ;
     G_fill_circle(xi,yi,5) ;        
   }

   G_display_image() ;
   q = G_wait_key() ;

}


