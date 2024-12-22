#include <stdio.h>

void array_print (double x[], int n)
{
  int i ;
  for (i = 0 ; i < n ; i++) {
    printf("%lf\n",x[i]) ;
  }
  printf("\n") ;
}

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




int  main()
{
  double a[10]= {24,33,18,12,29} ;
  array_print(a,5) ;
  selection_sort(a,5) ;
  array_print(a,5) ;


  double b[50]= {112,325,389,97,215,298,333} ;
  array_print(b,7) ;
  selection_sort(b,7) ;
  array_print(b,7) ; ;    
}



