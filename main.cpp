#include<iostream>
#include <fstream>
#include "Arbol.h"
#include "Bounding_box.h"
#include <string>
#define cant_datos 169908


using namespace std;
typedef long double ld;
typedef Arbol arbol_r;
//typedef Arbol<int,dimensiones> arbol_r;

arbol_r r;
int main (int argc, char *argv[]) {
	ld t[dimensiones];
	
	
	
	ifstream datos("bd.csv");
	if(datos.is_open()){
		int top=0;
		string::size_type sz;
		string linea;
		vector<ld> c_min(dimensiones,(ld)max);
		vector<ld> c_max(dimensiones,-(ld)max);
		c_min.resize(dimensiones);
		c_max.resize(dimensiones);
		ld t[dimensiones];
		vector<Punto> vec_puntos;
		int c=1;
		while(getline(datos,linea)){
			if(top<dimensiones){
				for(int w=0;w<dimensiones;w++){
					int pos=linea.find(",");
					string t_1=linea.substr(0,pos);
					linea=linea.substr(pos+1,linea.size());
					ld cant=stod(t_1,&sz);
					t[w]=cant;
				}
				Punto p_t(t);
				vec_puntos.push_back(p_t);
				top++;
				
				continue;
			}
			for(int w=0;w<dimensiones;w++){
				for(int x=0;x<dimensiones;x++){
					if(vec_puntos[x].data[w] > c_max[w]){
						c_max[w]=vec_puntos[x].data[w];
					}
					else if(vec_puntos[x].data[w] < c_min[w]){
						c_min[w]=vec_puntos[x].data[w];
					}
				}
			}
			Bounding_box b;
			for(int w=0;w<dimensiones;w++){
				b.puntos[w].first=c_min[w];
				b.puntos[w].second=c_max[w];
				
			}
//			
//			for(int w=0;w<dimensiones;w++){
//				cout<<c_min[w]<<endl;
//				cout<<c_max[w]<<endl;
//				cout<<"----------------------"<<endl;
//				
//			}
			//cout<<"se inserto "<<c<<endl;
			//c++;
			/*cout<<"se insertara "<<endl;
			for(int w=0;w<dimensiones;w++){
				for(int x=0;x<dimensiones;x++){
					cout<<vec_puntos[w].data[x]<<" ";
				}
				cout<<endl<<endl;
			}
				*/
			r.insertar_data(vec_puntos,b);
			vec_puntos.clear();
			top=0;
		}
		//cout<<"------------------------"<<endl;
		//cout<<r.root->items.size()<<endl;
		//cout<<r.size<<endl;
	}

	ld at[14];
	at[0]=0.9;
	at[1]=0.5;
	at[2]=46307;
	at[3]=0.057;
	at[4]=0;
	at[5]=0.04;
	at[6]=7;
	at[7]=0.23;
	at[8]=-21;
	at[9]=1;
	at[10]=5;
	at[11]=0.03;
	at[12]=108;
	at[13]=0.5;
	Punto xdd(at);
	
	
	
	r.knn(xdd,6);
	
	
	
	
		//cout<<xd[t].first<<endl;
		//xd[t].second.imprimir();
		
		//cout<<"------------------\n";
	
	cout<<endl;
	
	return 0;
}

