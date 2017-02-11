
#pragma once

// traitement d'une liste en fifo


// stucture pour enpiler des positions
// en fonction de leur distance à la cible
typedef struct
{
	Sint32		pos;
	Sint32		dist;
} 
Element;


// traitement d'une pile triée

class CPileTriee
{
private:
	Sint32		m_taille;		// nombre de polongs max
	Sint32		m_max;			// position limite
	Sint32		m_out;			// position pour reprendre
	Element*	m_data;			// données
			
public:
	CPileTriee(Sint32 taille);
	~CPileTriee();
	
	void		put(Sint32 pos, Sint32 dist);
	Sint32		get();
};