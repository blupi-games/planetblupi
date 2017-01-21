// Class CFifo, gestion d'une liste en fifo


#include <windows.h>
#include "fifo.h"


// gestion d'une pile classée en valeur croissantes
// typiquement reprend les coordonnées les plus proches
// du but en premier lieu

CPileTriee::CPileTriee(long taille)
{
	m_taille = taille;
	m_max = m_out = 0;
	m_data = (Element*) malloc (sizeof(Element)*taille);
}

CPileTriee::~CPileTriee()
{
	free( m_data );
}

long CPileTriee::get()
{
	if (m_out == m_max) return -1;
	long val = m_data [m_out].pos;
	m_out++;
	if (m_out>=m_taille) m_out = 0;
	return val;
}                             

void CPileTriee::put(long pos, long dist)
{
	long	i=m_out;
	long	p,d,m;
	
	while (i!=m_max)
	{
		// le point est-il plus proche que celui-là ?
		if (dist<m_data[i].dist)
		{
			// oui, insert et décale le suivant
			p = m_data[i].pos;
			d = m_data[i].dist;
			m_data[i].pos = pos;
			m_data[i].dist = dist;
			pos = p;
			dist = d;
		}
		i++;
		if (i>=m_taille) i=0;
	}
	
	// ajoute le point éloigné à la suite
	m = m_max+1;
	if (m>=m_taille) m=0;
	if (m!=m_out)
	{
		m_data[m_max].pos = pos;
		m_data[m_max].dist = dist;
		m_max = m;
	}
}