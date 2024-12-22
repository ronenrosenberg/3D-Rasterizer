#include  "FPToolkit.c"

int main(){
    double a[2],b[2],c[2], I[2] ;
    int slope, z , miny,maxy;
    
    G_init_graphics(800, 800) ; // makes base
    G_rgb(0.9,1,1) ;
    G_clear () ;

    //Initial two clicks
    G_rgb(0.9,0.5,0.5) ;
    G_wait_click(a) ;
    G_fill_circle(a[0],a[1], 2) ; 
    G_wait_click(b) ;
    G_fill_circle(b[0], b[1], 2) ; 
    G_line(a[0],a[1], b[0], b[1]) ;

    //Final Click
    G_wait_click(c) ;
    G_fill_circle(c[0], c[1], 2) ; 
    G_line(0,c[1],800, c[1]) ; // draws horizontal line from point clicked

    if (a[1] < b[1]) {miny = a[1];maxy = b[1];}
    else {miny = b[1]; maxy = a[1];}

    if (miny < c[1] && c[1] < maxy) {
        I[1] = c[1];
        I[0] = (((I[1]-a[1])*(b[0]-a[0]))/(b[1]-a[1])+a[0]);
        G_rgb(1,0.6,0.6) ;  //sets color of circle
        G_fill_circle(I[0],I[1], 2) ; 
    }
    else{ printf("does NOT intersect segments");}

    int key ;   
    key =  G_wait_key() ; // pause so user can see results

}
