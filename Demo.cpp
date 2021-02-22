#include "Tensor.hpp"
#include <stdio.h>

int main(){
    Tensor x,y;
    x<<2<<3<<4;
    y<<2<<3<<4;
    Tensor z={Tensor({Tensor(1),Tensor(2)}),Tensor({Tensor(3),Tensor(4)})};
    y[0][0][0]=2.0;
    y[1][2][3]=3.5;
    x[0][0][0]=2.0;
    printf("%s\n",z.show());
    for(int i=0;i<4;i++)printf("%s==%s\n",z.nindex(i).show(),z[(i-i%2)/2][i%2].show());
    printf("%s\n+\n%s\n=\n%s\n\n%s\n\n",x.show(),y.show(),(x+y).show(),z.kronecker(Tensor({Tensor(2),Tensor(3)})).show());
    int* out=x.dimensiones();
    int i=0;
    while(out[i++]!=-1)printf("%d ",out[i-1]);
    printf("\n%s->%s\n",Tensor({Tensor({Tensor(1),Tensor(2),Tensor(3)}),Tensor({Tensor(4),Tensor(5),Tensor(5)})}).show(),Tensor({Tensor({Tensor(1),Tensor(2),Tensor(3)}),Tensor({Tensor(4),Tensor(5),Tensor(5)})}).reshape({3,2}).show());
    printf("\n%s->%s\n",Tensor({2,3}).productoModoN(1,Tensor({2,3})).show(),Tensor({{1,0},{0,1}}).dot(Tensor({{4,1},{2,2}})).show());
    printf("\n%s->%s\n",Tensor({2,3,4}).show(),Tensor({2,3,4}).apply(exp).show());

    return 0;
}
