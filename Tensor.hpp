#pragma once
#include <math.h>
#include <stdio.h>
#include <vector>

/*****************************************************
autor: Pablo Cageao Honduvilla
fecha: 22/02/2021
*****************************************************/

/*****************************************************
Este diseño de una clase que incorpora las
operaciones basicas de un tensor utiliza un
diseño recursivo en el que, salvo la capa final,
cada objeto Tensor guarda una lista con los 
objetos representando una dimensión mas.

un ejemplo de un tensor de dimensiones 3x2:

|capas[0]|
|capas[1]|=
|capas[2]|

 |(capas[0].capas[0].valor) (capas[0].capas[1].valor)|
=|(capas[1].capas[0].valor) (capas[1].capas[1].valor)|
 |(capas[2].capas[0].valor) (capas[2].capas[1].valor)|

 Llamamos al Tensor capas[0].capas[1] final ya que no
 contiene mas vectores para aumentar la dimension sino 
 un valor con lo que funciona como un double
******************************************************/

/******************************************************
Errores:
exit 1: Has intentado guardar un double en un Tensor 
		no final
exit 2: Has intentado sumar/restar tensores de tamaños 
		distintos
exit 3: Has intentado acceder al valor de un Tensor 
		no final
exit 4: Has intentado acceder a una posición no valida
exit 5:
exit 6: has introducido un array demasiado largo como 
        argumento
exit 7: 
******************************************************/

struct Tensor{
protected:
	//Vector que apunta a los tensores de la siguiente 
	//dimension si no es capa final
    std::vector<Tensor> capas;
    //Almacena el valor si es capa final
    double valor=0.0;
    //Es 1 si es final y 0 si no
    bool fin=1;
    //Longitud de capas
    unsigned short longitud=1;

public:

	//CONSTRUCTORES Y ACCESO A ELEMENTOS

	//default
    Tensor(){
        valor=0.0;
        fin= 1;
        longitud= 1;
        capas=std::vector<Tensor>();
    }

    //final con valor x
    Tensor(double x){
        fin=1;
        valor=x;
        longitud=1;
        capas=std::vector<Tensor>();
    }

    //Para inicializar con initilizer lists de forma 
    //recursiva. Ejemplo:
    // Tensor({Tensor({Tensor(1),Tensor(2)}) , tensor({Tensor(3),Tensor(4)})})
    Tensor(std::initializer_list<Tensor> v):capas(v){
        fin=0;
        valor=0.0;
        longitud=v.size();
    }

    //Destructor
    ~Tensor(){
        if(!fin){
            capas.clear();
        }
    }


    operator double(){if(fin)return valor; else exit(3);}

    operator int(){if(fin)return (int)valor; else exit(3);}

    //Especifica las dimensiones del Tensor. Ejemplo de definicion 
    //de tensor x de 3x2x4:
    //
    //Tensor x;
    //x<<3<<2<<4;
    Tensor& operator<<(int capa){
        if(fin){
            fin=0;
            longitud=capa;
            for(int i=0;i<capa;i++)capas.push_back(Tensor());
        }
        else for(int i=0;i<this->longitud;i++){capas[i]<<capa;}
        return *this;
    }

    //Cambia el valor de este Tensor final a val
    void operator=(double val){
        if(!fin)exit(1);
        (*this).valor=val;
    }

    //Copia tensor en este Tensor sobreescribiendo lo anterior
    void operator=(Tensor tensor){
        valor=tensor.valor;
        fin=tensor.fin;
        longitud=tensor.longitud;
        capas.clear();
        if(!fin)for(int i=0;i<tensor.longitud;i++)capas.push_back(tensor.capas[i]);
    }

    //Accede al Tensor capas[i] o devuelve una copia del tensor final
    Tensor& operator[](int i){
    	if(i>=longitud || i<0)exit(4);
        if(fin && i==0){
            Tensor *out= new Tensor(valor);
            return *out;
        }
        return capas[i];
    }

    //Devuelve el elemento que estaria en la posicion i si el Tensor 
    //se reordenara como un array de la siguiente manera de forma recursiva:
    //
    //| 0 1 2 |
    //| 3 4 5 | -> | 0 1 2 3 4 5 6 7 8 |
    //| 6 7 8 |
    Tensor& nindex(int i){
        if(fin)return *this;
        int modulo=nelementos()/longitud;
        return capas[(i-i%modulo)/modulo].nindex(i%modulo);
    }

    //Devuelve un char con la representaión del Tensor en el siguiente formato:
    //
    //| 0 1 2 |
    //| 3 4 5 | -> { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} }
    //| 6 7 8 |
    char* show(){
        char* out;
        out=(char*)malloc(600);
        if(fin){
            sprintf(out,"%f",(*this).valor);
        }
        else{
            sprintf(out,"{");
            for(int i=0;i<longitud-1;i++){
                sprintf(out,"%s%s,",out,capas[i].show());
            }
            sprintf(out,"%s%s}",out,capas[longitud-1].show());
        }
        return out;
    }

    //OPERACIONES ARITMETICAS Y PRODUCTOS

    //Devuelve un nuevo Tensor que es suma de este y tensor
    Tensor operator+(Tensor tensor){
        Tensor* nuevo=new Tensor();
        *nuevo=*this;
        if(nuevo->fin && tensor.fin){
            nuevo->valor+=tensor.valor;
        }
        else if(nuevo->fin!=tensor.fin || nuevo->longitud!=tensor.longitud)exit(2);
        else{
            for(int i=0;i<nuevo->longitud;i++)nuevo->capas[i]=nuevo->capas[i]+tensor.capas[i];
        }
        return *nuevo;
    }

    //Suma este Tensor y tensor y lo guarda en este
    Tensor operator+=(Tensor tensor){
        *this=*this+tensor;
        return *this;
    }

    //Devuelve un nuevo Tensor que es resta de este y tensor
    Tensor operator-(Tensor tensor){
        Tensor* nuevo=new Tensor();
        *nuevo=*this;
        if(nuevo->fin && tensor.fin){
            nuevo->valor-=tensor.valor;
        }
        else if(nuevo->fin!=tensor.fin || nuevo->longitud!=tensor.longitud)exit(2);
        else{
            for(int i=0;i<nuevo->longitud;i++)nuevo->capas[i]=nuevo->capas[i]-tensor.capas[i];
        }
        return *nuevo;
    }

    //Resta este Tensor y tensor y lo guarda en este
    Tensor operator-=(Tensor tensor){
        *this=*this-tensor;
        return *this;
    }

    //Devuelve el producto externo de este Tensor y tensor
    Tensor externo(Tensor tensor){
        if(fin && tensor.fin){
            return Tensor(valor*tensor.valor);
        }
        else if(fin){
            return tensor.externo(Tensor(valor));
        }
        Tensor* nuevo=new Tensor();
        *nuevo=*this;
        for(int i=0;i<nuevo->longitud;i++)nuevo->capas[i]=nuevo->capas[i].externo(tensor);
        return *nuevo;
    }

    //Devuelve el orden o dimensión del tensor
    int orden(){
        int out=0;
        Tensor* sigue=this;
        while(!sigue->fin){
            out++;
            sigue=&sigue->capas[0];
        }
        return out;
    }

    //Devuelve el numero de elementos de este Tensor
    int nelementos(){
        int i=0;
        int out= 1;
        Tensor *sigue=this;
        while(!sigue->fin){
            out*=sigue->longitud;
            sigue=&sigue->capas[0];
        }
        return out;
    }

    //Devuelve un array acabado en -1 con las dimensiones del Tensor
    int* dimensiones(){
        int i=0;
        int* out= new int[orden()+1];
        Tensor *sigue=this;
        while(!sigue->fin){
            out[i++]=sigue->longitud;
            sigue=&sigue->capas[0];
        }
        out[i]=-1;
        return out;
    }

    //Devuelve el producto kronecker o tensorial de este Tensor y tensor
    Tensor kronecker(Tensor tensor){
        if(fin && tensor.fin){Tensor* nuevo=new Tensor(valor*tensor.valor);return *nuevo;}
        Tensor* nuevo=new Tensor();
        *nuevo<<longitud*tensor.longitud;
        if(fin) for(int j=0;j<tensor.longitud;j++)nuevo->capas[j]=(*this).kronecker(tensor.capas[j]);
        else if(tensor.fin) for(int i=0;i<longitud;i++)nuevo->capas[i]=(*this).capas[i].kronecker(tensor);
        else for(int i=0;i<longitud;i++)for(int j=0;j<tensor.longitud;j++)/*{printf("---");printf("%s---\n",(*this).capas[i].kronecker(tensor[j]).show());*/nuevo->capas[i*tensor.longitud+j]=(*this).capas[i].kronecker(tensor[j]);
        return *nuevo;
    }

    //Producto escalar entre este Tensor y tensor
    Tensor escalar(Tensor tensor){
        double out=0;
        if(fin && tensor.fin)return Tensor(valor*tensor.valor);
        else if(fin) for(int j=0;j<tensor.longitud;j++)out+=(double)(*this).escalar(tensor.capas[j]);
        else if(tensor.fin) for(int i=0;i<longitud;i++)out+=(double)capas[i].escalar(tensor);
        else for(int i=0;i<longitud;i++)for(int j=0;j<tensor.longitud;j++)out+=(double)capas[i].escalar(tensor.capas[j]);
        return Tensor(out);
    }

    //Producto modoN de este Tensor con tensor 
    Tensor productoModoN(int n, Tensor tensor, int iteracion=1){
        Tensor* nuevo=new Tensor();
        if(iteracion!=n){
            *nuevo<<longitud;
            for(int i=0;i<longitud;i++)nuevo->capas[i]=capas[i].productoModoN(n,tensor,iteracion+1);
        }
        else{
            if(tensor.orden()<2){
                if(!fin){
                    *nuevo=capas[0].kronecker(Tensor(tensor[0]));
                    for(int i=1;i<longitud;i++)*nuevo+=capas[i].kronecker(Tensor(tensor[i]));
                }
                else *nuevo=Tensor(tensor.valor);
            }
            else{
                *nuevo<<tensor.longitud;
                for(int j=0;j<tensor.longitud;j++)nuevo->capas[j]=this->productoModoN(n,tensor[j],iteracion);
            }
        }
        return *nuevo;
    }

    //Producto dot de este Tensor y tensor
    Tensor dot(Tensor tensor){
        if(tensor.orden()<2)return tensor.productoModoN(1,*this);
        return tensor.productoModoN(tensor.orden()-1,*this);
    }

    //multiplica los elementos de este Tensor con los elementos de tensor
    Tensor operator*(Tensor tensor){
        Tensor* nuevo=new Tensor();
        *nuevo=*this;
        if(nuevo->fin && tensor.fin){
            nuevo->valor*=tensor.valor;
        }
        else if(nuevo->fin!=tensor.fin || nuevo->longitud!=tensor.longitud)exit(2);
        else{
            for(int i=0;i<nuevo->longitud;i++)nuevo->capas[i]=nuevo->capas[i]*tensor.capas[i];
        }
        return *nuevo;
    }

    //multiplica los elementos de este Tensor con los elementos de tensor y lo guarda en este Tensor
    Tensor operator*=(Tensor tensor){
        *this=(*this)*tensor;
        return *this;
    }

    //Divide los elementos de este Tensor con los elementos de tensor
    Tensor operator/(Tensor tensor){
        Tensor* nuevo=new Tensor();
        *nuevo=*this;
        if(nuevo->fin && tensor.fin){
            nuevo->valor/=tensor.valor;
        }
        else if(nuevo->fin!=tensor.fin || nuevo->longitud!=tensor.longitud)exit(2);
        else{
            for(int i=0;i<nuevo->longitud;i++)nuevo->capas[i]=nuevo->capas[i]/tensor.capas[i];
        }
        return *nuevo;
    }

    //Divide los elementos de este Tensor con los elementos de tensor y lo guarda en este Tensor
    Tensor operator/=(Tensor tensor){
        *this=(*this)/tensor;
        return *this;
    }

    //Devuelve la normaF de este Tensor
    Tensor normaF(){
        return Tensor(sqrt((double)escalar(*this)));
    }

    //Devuelve le normaL1 de este Tensor
    Tensor normaL1(){
        Tensor out;
        if(fin)return *this;
        else for(int i=0;i<longitud;i++)out+=abs((double)capas[i].normaL1());
        return out;
    }

    //COMPARADORES

    bool operator<(Tensor tensor){
        return normaL1().valor<tensor.normaL1().valor;
    }

    bool operator<=(Tensor tensor){
        return normaL1().valor<=tensor.normaL1().valor;
    }

    bool operator>(Tensor tensor){
        return normaL1().valor>tensor.normaL1().valor;
    }

    bool operator>=(Tensor tensor){
        return normaL1().valor>=tensor.normaL1().valor;
    }

    bool operator==(Tensor tensor){
        if(fin && tensor.fin)return valor==tensor.valor;
        else if(!fin && ! tensor.fin && longitud==tensor.longitud){
            bool out=1;
            for(int i=0;i<longitud;i++)out=out&&capas[i]==tensor[i];
            return out;
        }
        return 0;
    }

    //Devuelve el minimo valor del Tensor
    Tensor reduce_min(){
        if(fin)return Tensor(valor);
        else{
            Tensor maxi=capas[0].reduce_max();
            for(int i=1;i<longitud;i++){
                double val=capas[i].reduce_max().valor;
                if(maxi.valor>val)maxi.valor=val;
            }
            return maxi;
        }
    }

    //Devuelve el maximo valor del Tensor
    Tensor reduce_max(){
        if(fin)return Tensor(valor);
        else{
            Tensor maxi=capas[0].reduce_max();
            for(int i=1;i<longitud;i++){
                double val=capas[i].reduce_max().valor;
                if(maxi.valor<val)maxi.valor=val;
            }
            return maxi;
        }
    }

    Tensor argmin(int prin=1){
        Tensor* nuevo=new Tensor;
        if(prin==0){
            if(fin) return Tensor(valor);
            Tensor *cosa= new Tensor();
            *cosa=capas[0].reduce_max();
            double maxi=(*cosa).valor;
            *nuevo<<(*cosa).longitud+1;
            for(int i=0;i<longitud;i++){
                delete cosa;
                cosa= new Tensor();
                *cosa=capas[i].reduce_max();
                if((*cosa)[0].valor<maxi){
                    maxi=(*cosa)[0].valor;
                    if(!(*cosa).fin)for(int i=1;i<nuevo->longitud;i++)(*nuevo)[i]=(*cosa)[i-1];
                    nuevo[0].valor=maxi;
                    nuevo[1].valor=i;
                }
            }
        }
        else if(fin)return Tensor(0);
        else{
            Tensor *cosa= new Tensor();
            *cosa=capas[0].reduce_max();
            double maxi=(*cosa).valor;
            *nuevo<<(*cosa).longitud;
            for(int i=0;i<longitud;i++){
                delete cosa;
                cosa= new Tensor();
                *cosa=capas[i].reduce_max();
                if((*cosa)[0].valor<maxi){
                    maxi=(*cosa)[0].valor;
                    if(!(*cosa).fin)for(int i=0;i<nuevo->longitud;i++)(*nuevo)[i]=(*cosa)[i];
                    nuevo[0].valor=i;
                }
            }
        }
        return *nuevo;
    }

        Tensor argmax(int prin=1){
        Tensor* nuevo=new Tensor;
        if(prin==0){
            if(fin) return Tensor(valor);
            Tensor *cosa= new Tensor();
            *cosa=capas[0].reduce_max();
            double maxi=(*cosa).valor;
            *nuevo<<(*cosa).longitud+1;
            for(int i=0;i<longitud;i++){
                delete cosa;
                cosa= new Tensor();
                *cosa=capas[i].reduce_max();
                if((*cosa)[0].valor>maxi){
                    maxi=(*cosa)[0].valor;
                    if(!(*cosa).fin)for(int i=1;i<nuevo->longitud;i++)(*nuevo)[i]=(*cosa)[i-1];
                    nuevo[0].valor=maxi;
                    nuevo[1].valor=i;
                }
            }
        }
        else if(fin)return Tensor(0);
        else{
            Tensor *cosa= new Tensor();
            *cosa=capas[0].reduce_max();
            double maxi=(*cosa).valor;
            *nuevo<<(*cosa).longitud;
            for(int i=0;i<longitud;i++){
                delete cosa;
                cosa= new Tensor();
                *cosa=capas[i].reduce_max();
                if((*cosa)[0].valor>maxi){
                    maxi=(*cosa)[0].valor;
                    if(!(*cosa).fin)for(int i=0;i<nuevo->longitud;i++)(*nuevo)[i]=(*cosa)[i];
                    nuevo[0].valor=i;
                }
            }
        }
        return *nuevo;
    }

    //Reordena el tensor a nuevas dimensiones,
    //Basicamente lo convierte en un array con nindex y 
    //lo vuelve a convertir a tensor con las nuevas dimensiones 
    Tensor reshape(int* nuevas){
        Tensor *nuevo=new Tensor();
        int contador=1;
        int numero_elementos=nelementos();
        while(contador<numero_elementos){
            contador*=(*nuevas);
            *nuevo<<(*nuevas);
            nuevas=nuevas+1;
        }
        if(contador!=numero_elementos){
            delete nuevo;
            exit(5);
        }
        for(int i=0;i<numero_elementos;i++)nuevo->nindex(i)=this->nindex(i);
        return *nuevo;
    }

    Tensor reshape(std::initializer_list<int> dim){
        std::vector<int> nuevas=dim;
        Tensor *nuevo=new Tensor();
        int contador=1;
        int numero_elementos=nelementos();
        int i=0;
        while(contador<numero_elementos && i<nuevas.size()){
            contador*=nuevas[i];
            *nuevo<<nuevas[i++];
        }
        if(contador!=numero_elementos){
            delete nuevo;
            exit(5);
        }
        for(int i=0;i<numero_elementos;i++)nuevo->nindex(i)=this->nindex(i);
        return *nuevo;
    }

    //Devuelve el Tensor traspuesto cambiando los indicescomo se indica en param
    //ejemplo:
    //
    //x tiene dimensiones 2x3x4x5
    //x.traspuesta({0,2,1,3}) tiene dimensiones 2x4x3x5
    Tensor traspuesta(int* param=NULL, int* indices=NULL, int len=0){
        int rango=orden();
        if(len==rango){
            Tensor* tensor=this;
            int e=0;
            while(!tensor->fin){
                tensor=&tensor->capas[indices[param[e++]]];
            }
            return *tensor;
        }
        Tensor* nuevo=new Tensor();
        int* numeros=dimensiones();
        if(param==NULL){
            param=new int[rango];
            param[0]=rango-1;
            param[rango-1]=0;
            for(int i=1;i<rango-1;i++)param[i]=i;
        }
        *nuevo<<numeros[param[len]];
        int* indices2=new int[len+1];
        for(int i=0;i<len;i++)indices2[i]=indices[i];
        for(int i=0;i<nuevo->longitud;i++){
            indices2[len]=i;
            nuevo->capas[i]=traspuesta(param,indices2,len+1);
        }
        return *nuevo;
    }

    Tensor traspuesta(std::initializer_list<int> perm, int* indices=NULL, int len=0){
        std::vector<int> v=perm;
        int rango=orden();
        int* param=new int[rango];
        for(int i=0;i<rango;i++)param[i]=v[i];
        if(perm.size()<rango)exit(6);
        if(len==rango){
            Tensor* tensor=this;
            int e=0;
            while(!tensor->fin){
                tensor=&tensor->capas[indices[param[e++]]];
            }
            return *tensor;
        }
        Tensor* nuevo=new Tensor();
        int* numeros=dimensiones();
        if(param==NULL){
            param=new int[rango];
            param[0]=rango-1;
            param[rango-1]=0;
            for(int i=1;i<rango-1;i++)param[i]=i;
        }
        *nuevo<<numeros[param[len]];
        int* indices2=new int[len+1];
        for(int i=0;i<len;i++)indices2[i]=indices[i];
        for(int i=0;i<nuevo->longitud;i++){
            indices2[len]=i;
            nuevo->capas[i]=traspuesta(param,indices2,len+1);
        }
        return *nuevo;
    }

    //junta este Tensor con tensor en la dimensión direccion, ejemplo:
    //
    //x=| 1 2 |
    //  | 3 4 |
    //
    //y=| 5 6 |
    //  | 7 8 |
    //
    //x.junta(y,0)=| 1 2 |
    //			   | 3 4 |
    //			   | 5 6 |
    //			   | 7 8 |
    //
    //x.junta(y,1)=| 1 2 5 6 |
    //			   | 3 4 7 8 |
    //
    //x.junta(y,2)=x
    Tensor junta(Tensor tensor, unsigned short direccion){
        if(direccion==0){
            Tensor* nuevo=new Tensor();
            (*nuevo)<<longitud+tensor.longitud;
            for(int i=0;i<longitud;i++)nuevo->capas[i]=capas[i];
            for(int i=longitud;i<longitud+tensor.longitud;i++)nuevo->capas[i]=tensor.capas[i-longitud];
            return *nuevo;
        }
        else{
            if(longitud!=tensor.longitud)exit(7);
            Tensor* nuevo=new Tensor();
            (*nuevo)<<longitud;
            for(int i=0;i<longitud;i++)nuevo->capas[i]=capas[i].junta(tensor[i],direccion-1);
            return *nuevo;
        }
    }

    //Devuelve el Tensor extendido a una dimension con las posiciones de nindex
    Tensor ravel(){
        Tensor* nuevo=new Tensor();
        int nelem=nelementos();
        (*nuevo)<<nelem;
        for(int i=0;i<nelem;i++)nuevo->capas[i].valor=nindex(i).valor;
        return *nuevo;
    }

    //Devuelve un Tensor al que se le aplica una funcion func a todos los elementos de este Tensor
    Tensor apply(double (*func)(double)){
        if(fin)return Tensor((*func)(valor));
        Tensor* nuevo=new Tensor();
        (*nuevo)<<longitud;
        for(int i=0;i<longitud;i++)nuevo->capas[i]=capas[i].apply(func);
        return *nuevo;
    }

};