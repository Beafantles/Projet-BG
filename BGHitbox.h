#ifndef BG_HITBOX_H
#define BG_HITBOX_H

#include <vector>
#include <SFML/Graphics.hpp>


class BGHitbox
{
private:
	std::vector < std::pair < unsigned int, unsigned int > > m_points;
	bool m_finished;

public:
	BGHitbox();
	void addPoint(unsigned int, unsigned int); //Ajouter un nouveau point � partir de ses coordonn�es x et y (si cela est possible)
	void delPoint(short int = -1); //Par d�faut, cela supprimera le dernier point, sinon, utilise l'indice donn� en param�tre (si c'est possible)
	void insertPoint(unsigned char, unsigned int, unsigned int); //Pour ins�rer un point � un indice donn� (si cela est possible)
	void autoFinish(); //Pour terminer la hitbox automatiquement (si cela est possible)
	void movePoint(short int, unsigned int, unsigned int); //Change la position d'un indice � partir de son indice et de sa potientielle nouvelle position
	static bool areSegmentsSecants(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int); //D�termine si deux segments sont s�cants � partir de 4 points (les 2 premiers formant le premier segment et les deux derniers le second)
	sf::ConvexShape showHitboxContent(sf::Color& const = sf::Color(255, 255, 255)); //Renvoie un convex correspondant � la zone de la hitbox
	std::vector<sf::ConvexShape> showLines(unsigned char = 2, sf::Color& const = sf::Color(255, 0, 0)); //Renvoie un vector des lignes (ConvexShape) de la hitbox
	std::vector<sf::CircleShape> showPoints(unsigned char = 3, sf::Color& const = sf::Color(0, 255, 0)); //Renvoie un vector des points (CircleShape) de la hitbox
};

#endif