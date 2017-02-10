
#pragma once

// traitement d'une liste en fifo


// stucture pour enpiler des positions
// en fonction de leur distance à la cible
typedef struct
{
	long		pos;
	long		dist;
} 
Element;


// traitement d'une pile triée

class CPileTriee
{
private:
	long		m_taille;		// nombre de polongs max
	long		m_max;			// position limite
	long		m_out;			// position pour reprendre
	Element*	m_data;			// données
			
public:
	CPileTriee(long taille);
	~CPileTriee();
	
	void		put(long pos, long dist);
	long		get();
};