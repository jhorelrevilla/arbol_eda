#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H
#include <iostream>
#include <string>
#include <math.h>
const int dimensiones =14;
#define max 999999999


using namespace std;
typedef long double ld;



struct Punto{
	ld data[dimensiones];
	Punto(ld d[dimensiones]={0}){
		for(int i=0;i<dimensiones;++i){
			data[i]=d[i];
		}
	}
	ld* get_datos(){
		return data;
	}
	void imprimir(){
		for(int i=0;i<dimensiones;++i){
			cout<<data[i]<<" ";
		}
		cout<<endl;
	}
	double distancia(Punto t){
		/*ld tempo[dimensiones]=t.get_datos();
		float dist=0;
		for(int i=0;i<dimensiones;++i){
			float cant=(data[i]*data[i]) - (tempo[i]*tempo[i]);
			if(cant<0)
				cant*=-1;
			dist+=cant;
		}
		return sqrt(dist);*/
	}
};

// // // // // // // // // // // // // // // // // // 
class Bounding_box {
public:
	
	pair<float, float> puntos[dimensiones];
	//////
	Bounding_box(){}
	void set_puntos(Bounding_box &T){
		for(size_t i=0;i<dimensiones;++i){
			puntos[i]=T.puntos[i];
		}
	}
	
	///
	void reset(){
		for(size_t i=0;i<dimensiones;++i){
			puntos[i].first=max;
			puntos[i].second=-max;
		}
	}
	///
	void imprimir(){
		for(int i=0;i<dimensiones;++i){
			cout<<"( "<<puntos[i].first<<" - "<<puntos[i].second<<" )"<<endl;
		}
	}
	///
	bool stretch(Bounding_box &bb){
		bool cond = false;
		for (size_t eje = 0; eje < dimensiones; ++eje){
			if (puntos[eje].first > bb.puntos[eje].first){
				puntos[eje].first = bb.puntos[eje].first;
				cond = true;
			}
			if (puntos[eje].second < bb.puntos[eje].second){
				puntos[eje].second = bb.puntos[eje].second;
				cond = true;
			}
		}
		return cond;
	}
	///
	double area(){
		double area=1;
		for (size_t eje = 0; eje < dimensiones; ++eje)
			area *= (double)(puntos[eje].second - puntos[eje].first);
		return area;
	}
	///
	bool is_overlap(Bounding_box &t){
		for(size_t i=0;i<dimensiones;++i){
			if(!(puntos[i].first   < t.puntos[i].second) ||
			   !(t.puntos[i].first < puntos[i].second))
				return false;
		}
		return true;
	}
	///
	int distancias_rect(){
		int distancia=0;
		for(size_t i=0;i<dimensiones;++i){
			distancia+=(puntos[i].second - puntos[i].first);
		}
		return distancia;
	}
	///
	bool overlap(Bounding_box & bb){
		double area = 1.0;
		for (int d = 0; area && d < dimensiones; ++d){
			const int x1 = puntos[d].first;
			const int x2 = puntos[d].second;
			const int y1 = bb.puntos[d].first;
			const int y2 = bb.puntos[d].second;
			// izq fuera izq
			if (x1 < y1){
				// derecho dentro izq
				if (y1 < x2){
					// derecha fuera derecha
					if (y2 < x2)
						area *= (double)( y2 - y1 );
					else
						area *= (double)( x2 - y1 );
					continue;
				}
			}
			// derecho dentro izq
			else if (x1 < y2){
				// derecho fuera derecho
				if (x2 < y2)
					area *= (double)( x2 - x1 );
				else
					area *= (double)( y2 - x1 );
				continue;
			}
			//no hay overlap
			return 0.0;
		}
		return area;
	}
	///
	double distancia_centro(Bounding_box &bb){
		double distancia=0,temporal;
		for(size_t eje=0; eje<dimensiones; ++eje){
			temporal=((double)puntos[eje].first + (double)puntos[eje].second +
				(double)bb.puntos[eje].first + (double)bb.puntos[eje].second)/2.0;   
				distancia+=(temporal*temporal);
		}
		return distancia;
	}
	///	
	bool Punto_dentro(Punto t){
		for(size_t eje=0; eje<dimensiones; ++eje){
			if(puntos[eje].first <= t.data[eje] && puntos[eje].second >= t.data[eje]){
				continue;
			}
			return false;
		}
		return true;
	}
	///
	float MIN_DIST(Punto p){
		float result=0;
		//si esta dentro del MBR la distancia minima es 0
		for(size_t eje=0; eje<dimensiones; ++eje){
			result +=pow(p.data[eje] - puntos[eje].first,2);
		}
		return result;
	}
	///
	
};

#endif

