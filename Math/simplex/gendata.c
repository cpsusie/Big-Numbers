#include <stdio.h>

main() {
  double x,y;

  for( x = 0; ; x+= 0.1) {
    y = (-2 * x + 30)/3;
    printf("%lg %lg\n",x,y);
    if(y<0) break;
  }
}
