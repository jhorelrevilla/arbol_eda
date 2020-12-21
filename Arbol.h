#ifndef ARBOL_H
#define ARBOL_H
#include <iostream>
#include <vector>
#include <algorithm>

int id=1;

#include "Bounding_box.h"
using namespace std;
/////////////////////////////////////////////////////////////////// ITEM
struct Item{
	typedef Bounding_box box;
	box bounding;
};
/////////////////////////////////////////////////////////////////// HOJA
struct Hoja:Item{
	vector<Punto> hoja;
	void imprimir(){
		for(int i=0;i<dimensiones;++i){
			hoja[i].imprimir();
		}
	}
};
/////////////////////////////////////////////////////////////////// NODO
struct Nodo:Item{
	vector<Item*> items;
	bool tiene_hojas;
};
/// // // // // // // // // // SORTS // // // // // // // // // // // // // 
////functor para sort(overlap)
template<typename tipo_item>
struct sort_overlap{
	typename tipo_item::box *centro;
	sort_overlap(typename tipo_item::box *c){
		centro=c;
	}
	bool operator ()(tipo_item *a,tipo_item *b){
		if(a->bounding.overlap(*centro) < b->bounding.overlap(*centro)){
			return true;
		}
		return false;
	}
};
////functor_sort
////0 primer
////1 segundo
template<typename tipo_item>
struct sorts_items{
	size_t eje;
	bool n_borde;
	sorts_items(size_t e,bool n){
		eje=e;
		n_borde=n;
	}
	bool operator ()(tipo_item *a,tipo_item *b){
		if(n_borde==true){
			return a->bounding.puntos[eje].first < b->bounding.puntos[eje].first;
		}
		return a->bounding.puntos[eje].second < b->bounding.puntos[eje].second;
	}
};
////functor para sort distancias del centro
template<typename tipo_item>
struct sort_dist_centro{
	typename tipo_item::box * centro;
	sort_dist_centro(typename tipo_item::box * c){
		centro=c;
	}
	bool operator ()(tipo_item *a,tipo_item *b){
		return a->bounding.distancia_centro(*centro)
			< b->bounding.distancia_centro(*centro);
	}
};

template<typename t_item>
void stretch_box(typename t_item::box *a, t_item *b){
	a->stretch(b->bounding);
}
////
template<typename t_item>
struct sort_item_area{
	double a;
	sort_item_area(double t){
		a=t;
	}
	bool operator ()(t_item *b1,t_item *b2){
		return (a - b1->bounding.area() < a - b2->bounding.area());
	}
};
////
bool sort_mbr(pair<float,Hoja> &A,pair<float,Hoja> &B){
	return A.first > B.first;
}
bool sort_punt(pair<float,Punto> &A,pair<float,Punto> &B){
	return A.first > B.first;
}
/// // // // // // // // // // // // // // // // // // // // // // // // // 
///////////////////////R* - TREE////////////////////////////////////////////
class Arbol {
public:
	typedef Item		item_tipo;
	typedef Hoja		hoja_tipo;
	typedef Nodo		nodo_tipo;
	Nodo *root;
	size_t size=0;
	size_t hijos_min=4;
	size_t hijos_max=10;
	///
	
	///
	Arbol(){
		root=nullptr;
		size=0;
	}
	///
	nodo_tipo *choosetree(nodo_tipo *n,Bounding_box *b){
		
		if(static_cast<nodo_tipo*>(n->items[0])->tiene_hojas){
			int p=32;
			if(hijos_max > (p*2)/3  && n->items.size() > p ){
				sort(n->items.begin(),n->items.end(),
					 sort_item_area<item_tipo>(b->area()));
				//
				nodo_tipo * temp= static_cast<nodo_tipo*>(* std::min_element(n->items.begin(),n->items.begin()+ p, 
												sort_overlap<item_tipo>(b)));
				return temp;
			}
			nodo_tipo * temp= static_cast<nodo_tipo*>(* min_element(n->items.begin(),n->items.end(),
											sort_overlap<item_tipo>(b)));
			return temp;
		}
		nodo_tipo * temp= static_cast<nodo_tipo*>(* min_element(n->items.begin(),n->items.end(),
																sort_item_area<item_tipo>(b->area())));
		return temp;
	}
	///
	void insertar_data(vector<Punto> l,Bounding_box &b){
		hoja_tipo *hoja_temp=new hoja_tipo();
		hoja_temp->bounding.set_puntos(b);
		hoja_temp->hoja=l;
		if(!root){
			root=new nodo_tipo();
			root->tiene_hojas=true;
			
			root->items.reserve(hijos_min);
			root->items.push_back(hoja_temp);
			root->bounding=b;
			
		}
		else{
		   insertar(hoja_temp,root);
		}
		size++;
	}
	///
	nodo_tipo* insertar(hoja_tipo *h,nodo_tipo *n, bool primer_insert=true){
		
		n->bounding.stretch(h->bounding);
		if(n->tiene_hojas){
			n->items.push_back(h); 
		}
		else{
			nodo_tipo * temp=insertar(h,choosetree(n,&h->bounding),primer_insert);
			if(!temp)
				return nullptr;
			n->items.push_back(temp);
		}
		if(n->items.size() > hijos_max){
			
			return overflowtreatment(n,primer_insert);
		}
		return nullptr;
	}
	///
	nodo_tipo * overflowtreatment(nodo_tipo *l,bool primer_insert){
		
		if(l!=root  && primer_insert){
			
			reinsert(l);
			return nullptr;
		}
		
		nodo_tipo * item_split= split(l);
		
		if(l == root){
			nodo_tipo *temp_2=new nodo_tipo();
			temp_2->tiene_hojas=false;
			
			temp_2->items.reserve(hijos_min);
			temp_2->items.push_back(root);
			temp_2->items.push_back(item_split);
			
			temp_2->bounding.reset();
			for(auto i=temp_2->items.begin(); i!=temp_2->items.end();++i){
				stretch_box<item_tipo>(&temp_2->bounding,*i);
			}
			root=temp_2;
			return nullptr;
		}
		
		return item_split;
	}
	///
	nodo_tipo * split(nodo_tipo * nodo_s){
		
		nodo_tipo * new_nodo=new nodo_tipo();
		new_nodo->tiene_hojas=nodo_s->tiene_hojas;
		///
		size_t cant_items=nodo_s->items.size();
		size_t distribucion=cant_items-2*hijos_min+1;
		size_t split_eje=dimensiones+1, split_borde=0, split_index=0;
		int split_margen=0;
		typename item_tipo::box R1,R2;
		
		///Choose_split_axis
		for(size_t eje=0;eje<dimensiones;++eje){
			
			size_t margen=0;
			double overlap=0,cant_area,cant_overlap=max;
			size_t dista_borde=0,dist_index=0;
			cant_area=max;
			cant_overlap=max;
			//0-> menor 1->mayor
			for(size_t borde=0;borde<2;++borde){
				
				if(borde==0){
					sort(nodo_s->items.begin(),nodo_s->items.end(),
						 sorts_items<item_tipo>(eje,true));
				}
				else
					sort(nodo_s->items.begin(),nodo_s->items.end(),
						 sorts_items<item_tipo>(eje,false));
				
				for(size_t k=0;k<distribucion;++k){
					
					double area=0;
					R1.reset();
					for(auto i=nodo_s->items.begin();i!=nodo_s->items.begin()+(hijos_min+k);++i){
						R1.stretch((*i)->bounding);
						//stretch_box<item_tipo>(&R1,*i);
					}
					R2.reset();
					for(auto i=nodo_s->items.begin()+(hijos_min+k+1);i!=nodo_s->items.end();++i){
						R2.stretch((*i)->bounding);
						
					}
					margen+= R1.distancias_rect() + R2.distancias_rect();
					area+=R1.area()+R2.area();
					overlap=R1.overlap(R2);
					
					///Chooose_split_index
					if(overlap<cant_overlap || (cant_overlap==overlap && area <cant_area)){
						dista_borde=borde;
						dist_index= hijos_min+k;
						cant_overlap=overlap;
						cant_area=area;
					}
					/// /// /// /// /// /// /// ///
				}
			}
			
			if(split_eje == dimensiones+1 || split_margen > margen){
				split_eje=eje;
				split_margen=margen;
				split_borde=dista_borde;
				split_index=dist_index;
			}
		}
		
		/// /// /// /// /// /// /// ///
		//distribuir items en 2 grupos
		//cout<<"split_borde" <<split_borde<<endl;
		if(split_borde==0){
			sort(nodo_s->items.begin(),nodo_s->items.end(),
				 sorts_items<item_tipo>(split_eje,true));
			//sort por el primer borde
		}
		//sort por el segundo
		else if(split_eje!=dimensiones-1){
			sort(nodo_s->items.begin(),nodo_s->items.end(),
				 sorts_items<item_tipo>(split_eje,false));
		}
		
		//distribuir
		new_nodo->items.assign(nodo_s->items.begin()+split_index, nodo_s->items.end());
		
		nodo_s->items.erase(nodo_s->items.begin()+split_index,nodo_s->items.end());
		
		//ajustar al new 
		nodo_s->bounding.reset();
		for(auto i=nodo_s->items.begin();i!=nodo_s->items.end();++i){
			stretch_box<item_tipo>(&nodo_s->bounding,*i);
		}
		
		new_nodo->bounding.reset();
		for(auto i=new_nodo->items.begin();i!=new_nodo->items.end();++i){
			stretch_box<item_tipo>(&new_nodo->bounding,*i);
		}
		return new_nodo;
	}
	///
	void reinsert(nodo_tipo * nodo){
		
		vector<item_tipo*> temp_items;
		size_t cant_items=nodo->items.size();
		size_t p=0.30;
		if(cant_items * p > 0){
			p=cant_items*p;
		}
		else
		   p=1;
		sort(nodo->items.begin(),nodo->items.end(),
			 sort_dist_centro<item_tipo>(&nodo->bounding));
		
		temp_items.assign(nodo->items.end()-p,nodo->items.end());
		nodo->items.erase(nodo->items.end()-p,nodo->items.end());
		
		nodo->bounding.reset();
		for(auto i=nodo->items.begin();i!=nodo->items.end();++i){
			stretch_box<item_tipo>(&nodo->bounding,*i);
		}
		for(auto i=temp_items.begin();i!=temp_items.end();++i){
			insertar(static_cast<hoja_tipo*>(*i),root,false);
		}
	}	
	///
	void knn(Punto temp,int k){
		vector<pair<float,hoja_tipo>> grup_MBR;
		buscar_box(root,temp,grup_MBR);
		sort(grup_MBR.begin(),grup_MBR.end(),sort_mbr);
		
		cout<<grup_MBR[0].second.hoja.size()<<endl;
		
		cout<<grup_MBR[0].first<<endl;
		
		for(int i=0;i<grup_MBR.size() && k>0;++i){
			vector<Punto> p_t= buscar_puntos(temp,grup_MBR[i].second,k);
		}
		
		//static_cast<hoja_tipo>(grup_MBR[0]
		
		
	}
	///
	void buscar_box(nodo_tipo *nodo_actual,Punto c,vector<pair<float,hoja_tipo>> &result){
		//cout<<"cant tamanio "<<nodo_actual->items.size()<<endl;
		//cout<<"hijos? "<<nodo_actual->tiene_hojas<<endl;
		if(nodo_actual->bounding.Punto_dentro(c)  && !nodo_actual->tiene_hojas){
		//if( !nodo_actual->tiene_hojas){
			for(auto i=nodo_actual->items.begin();i!=nodo_actual->items.end();++i){
				buscar_box(static_cast<nodo_tipo*>(*i),c,result);
			}
		}
		else{
			pair<float,hoja_tipo> col;
			for(auto i=nodo_actual->items.begin();i!=nodo_actual->items.end();++i){
				Hoja hoja_t;
				hoja_tipo* w=static_cast<hoja_tipo*>(*i);
				float t=nodo_actual->bounding.MIN_DIST(c);
				
				hoja_t.bounding.set_puntos(w->bounding);
				hoja_t.hoja=w->hoja;
				id++;
				col.first=t;
				col.second=hoja_t;
				result.push_back(col);
//				cout<<"---------------------"<<endl;
//				w->imprimir();
			}
			//cout<<"tam hoja "<<hoja_t.hoja.size()<<endl;
			//cout<<id<<" "<<t<<endl;
		}
	}
	///
	vector<Punto> buscar_puntos(Punto c,hoja_tipo b,int &cant){
		vector<pair<float,Punto>> result;
		
		for(int w=0;w<b.hoja.size() && cant>0;++w){
			cout<<"a "<<endl;
			pair<float,Punto> xd;
			
			xd.first=b.hoja[w].distancia(c);
			
			xd.second=b.hoja[w];
			result.push_back(xd);
			cant--;
		}
		
		sort(result.begin(),result.end(),sort_punt);
		vector<Punto> p_r;
		
		for(int w=0; w<result.size(); ++w){
			
			result[w].second.imprimir();
		}
		/*for(int w=0;w<t->hoja.size();++w){
			pair<float,Punto> t;
			t.first=t->hoja[w].distancia(c);
			
		}
		*/
 	}
private:
};

#endif































