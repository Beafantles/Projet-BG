#include "BGHitbox.h"
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

/*  https://openclassrooms.com/forum/sujet/algorithme-2-segments-secant-ou-non
	https://fr.wikipedia.org/wiki/D%C3%A9terminant_(math%C3%A9matiques)#D.C3.A9terminant_de_deux_vecteurs_dans_le_plan_euclidien
	det(AB,AC)*det(AB,AD)<0 et det(CD,CA)*det(CD,CB)<0 si AB et CD sont s�cants*/
bool BGHitbox::areSegmentsSecants(unsigned int _xa, unsigned int _ya, unsigned int _xb, unsigned int _yb, unsigned int _xc, unsigned int _yc, unsigned int _xd, unsigned int _yd) //Notice the xd :eyes:
{
	//On va utiliser ces valeurs � plusieurs reprises, pourquoi ne pas les stocker ? :sunglasses:
	int xAB = _xb - _xa;
	int xAC = _xc - _xa;
	int xAD = _xd - _xa; //:eyes:
	int xCA = _xa - _xc;
	int xCB = _xb - _xc;
	int xCD = _xd - _xc; //:eyes:

	int yAB = _yb - _ya;
	int yAC = _yc - _ya;
	int yAD = _yd - _ya;
	int yCA = _ya - _yc;
	int yCB = _yb - _yc;
	int yCD = _yd - _yc;

	long int detABAC = xAB * yAC - yAB * xAC;
	long int detABAD = xAB * yAD - yAB * xAD;
	long int detCDCA = xCD * yCA - yCD * xCA;
	long int detCDCB = xCD * yCB - yCD * xCB;

	return detABAC * detABAD < 0 && detCDCA * detCDCB < 0;
}

BGHitbox::BGHitbox()
{
	m_finished = false;
}

void BGHitbox::addPoint(unsigned int _x, unsigned int _y)
{
	if (!m_finished)
	{
		if (m_points.size() < 3) //Dans le cas o� il y a moins de 3 points, le nouveau segment potientiellement cr�� avec le point ne pourra pas �tre s�cant avec les autres
		{
			bool found = false;
			unsigned char i = 0;
			while (i < m_points.size() && !found) //En revanche, on doit s'assurer qu'il n'est pas confondu avec ceux d�j� cr��s
			{
				if (m_points[i].first == _x && m_points[i].second == _y)
				{
					found = true;
				}
				i++;
			}
			if (!found)
			{
				m_points.push_back(std::make_pair(_x, _y));
				std::cout << "Point ajout� !" << std::endl;
			}
			else
			{
				std::cout << "Ajout du point impossible ! (vous ne pouvez pas ajouter un point qui existe d�j� si la hitbox est constitu�e de moins de 4 points !) :(" << std::endl;
			}
		}
		else
		{
			bool secant = false;
			unsigned char i = 0;
			while (i < m_points.size() - 2 && !secant) //Le nouveau segment potentiellement cr�� ne pourra dans tous les cas pas �tre s�cant avec le segment form� par les deux derniers points, d'o� le m_points.size() - 2
			{
				secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[m_points.size() - 1].first, m_points[m_points.size() - 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				i++;
			}
			if (!secant) //Il ne reste plus qu'� regarder si le point n'existe pas d�j� (sachant que s'il est �gal au premier point, alors cela signifie qu'il "fermera" la hitbox)
			{
				bool found = false;
				unsigned char i = 1;
				while (i < m_points.size() && !found)
				{
					if (m_points[i].first == _x && m_points[i].second == _y)
					{
						found = true;
					}
					i++;
				}
				if (!found)
				{
					if (_x == m_points[0].first && _y == m_points[0].second)
					{
						m_finished = true;
						std::cout << "Hitbox termin�e !" << std::endl;
					}
					else
					{
						m_points.push_back(std::make_pair(_x, _y));
						std::cout << "Point ajout� !" << std::endl;
					}
				}
				else
				{
					std::cout << "Impossible d'ajouter un point d�j� existant !" << std::endl;
				}
			}
			else
			{
				std::cout << "Impossible d'ajouter le point : Segments s�cants !" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Impossible de rajouter un point alors que la hitbox est termin�e. Si vous souhaitez le faire, supprimez d'abord le dernier point !" << std::endl;
	}
}

void BGHitbox::delPoint(short int _index)
{
	if (_index >= -1 && _index <= (unsigned char)(m_points.size() - 1))
	{
		if (m_finished && (_index == -1 || _index == m_points.size() - 1))
		{
			m_finished = false; //Dans le cas o� la hitbox est "finie", supprimer le dernier point revient � consid�rer la hitbox comme non "ferm�e"
			std::cout << "Hitbox plus 'termin�e'" << std::endl;
		}
		else if (m_points.size() < 5) //Dans le cas o� la hitbox a moins de 5 points, la supression de l'un d'eux ne pose aucun probl�me d'intersection avec le nouveau segment
		{
			m_points.erase(m_points.begin() + _index);
			std::cout << "Point supprim� !" << std::endl;
		}
		else
		{
			if (_index == -1)
			{
				_index = m_points.size() - 1;
			}
			//Sch�ma pour comprendre les explications suivantes : http://i.imgur.com/kFT3MGb.png

			//On doit maintenant v�rifier que la suppression de ce point n'entra�ne pas de probl�mes (segments s�cants avec ce le nouveau segment cr�� <-- le rouge)
			bool secant = false;
			unsigned char i = 0;
			unsigned int x1, y1, x2, y2; //Les coordonn�es des points du segment (le rouge) nouveau potientiellement cr�� par la pptientelle supression du point

			if (_index == 0)
			{
				x1 = m_points[m_points.size() - 1].first;
				y1 = m_points[m_points.size() - 1].second;
			}
			else
			{
				x1 = m_points[_index - 1].first;
				y1 = m_points[_index - 1].second;
			}

			if (_index == m_points.size() - 1)
			{
				x2 = m_points[0].first;
				y2 = m_points[0].second;
			}
			else
			{
				x2 = m_points[_index + 1].first;
				y2 = m_points[_index + 1].second;
			}

			while (i < _index - 2 && !secant) //Dans tous les cas, cela ne sert � rien de tester si les segments adjacents � celui qui va �tre potentiellement cr�� sont s�cants avec lui (les segments bleus ne pourront jamais �tre s�cants avec le segment rouge)
			{
				secant = BGHitbox::areSegmentsSecants(x1, y1, x2, y2, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				i++;
			}
			if (!secant)
			{
				i = _index + 2;
				while (i < m_points.size() - 2 && !secant)
				{
					secant = BGHitbox::areSegmentsSecants(x1, y1, x2, y2, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
i++;
				}
				if (!secant)
				{
					m_points.erase(m_points.begin() + _index);
					std::cout << "Point supprim� !" << std::endl;
				}
				else
				{
					std::cout << "Impossible de supprimer ce point : segments s�cants !" << std::endl;
				}
			}
			else
			{
				std::cout << "Impossible de supprimer ce point : segments s�cants !" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Indice incorrect..." << std::endl;
	}
}

void BGHitbox::autoFinish()
{
	if (!m_finished)
	{
		unsigned char i = 1;
		bool secant = false;
		while (i < m_points.size() - 2 && !secant) //Les deux segments adjacents au potentiel segment reliant le "d�but et la fin" ne peuvent pas �tre s�cants � lui
		{
			secant = BGHitbox::areSegmentsSecants(m_points[m_points.size() - 1].first, m_points[m_points.size() - 1].second, m_points[0].first, m_points[0].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
			i++;
		}
		if (!secant)
		{
			m_finished = true;
			std::cout << "Hitbox 'termin�e' !" << std::endl;
		}
		else
		{
			std::cout << "Impossible : segments s�cants !" << std::endl;
		}
	}
	else
	{
		std::cout << "La hitbox est d�j� termin�e !" << std::endl;
	}
}

void BGHitbox::insertPoint(unsigned char _index, unsigned int _x, unsigned int _y)
{
	if (_index >= -1 && _index <= (unsigned char)(m_points.size() - 1))
	{
		//Voir sch�ma
		unsigned char i = 0;
		bool secant = false;
		while (i < _index - 1 && !secant)
		{
			secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index].first, m_points[_index].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
			if (!secant && i != _index - 2)
			{
				secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index - 1].first, m_points[_index - 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
			}
			i++;
		}
		if (!secant)
		{
			i = _index;
			while (i < m_points.size() - 1 && !secant)
			{
				secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index - 1].first, m_points[_index - 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				if (!secant && i != _index)
				{
					secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index].first, m_points[_index].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				}
				i++;
			}
			if (!secant)
			{
				m_points.insert(m_points.begin() + _index, std::make_pair(_x, _y));
				std::cout << "Point ins�r� !" << std::endl;
			}
			else
			{
				std::cout << "Impossible : segments s�cants !" << std::endl;
			}
		}
		else
		{
			std::cout << "Impossible : segments s�cants !" << std::endl;
		}
	}
	else
	{
		std::cout << "Indice incorrect..." << std::endl;
	}
}


void BGHitbox::movePoint(short int _index, unsigned int _x, unsigned int _y)
{
	if (_index >= -1 && _index <= (unsigned char)(m_points.size() - 1))
	{
		unsigned char i = 0;
		bool secant = false;

		while (i < _index - 1 && !secant) //Le potientiel nouveau segment ne pourra pas �tre s�cant aux segments adjacents � lui
		{
			secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index + 1].first, m_points[_index + 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
			if (!secant && i != _index - 2)
			{
				secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index - 1].first, m_points[_index - 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
			}
			i++;
		}
		if (!secant)
		{
			i = _index + 1;
			while (i < m_points.size() - 1 && !secant)
			{
				secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index - 1].first, m_points[_index - 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				if (!secant && i != _index + 1)
				{
					secant = BGHitbox::areSegmentsSecants(_x, _y, m_points[_index + 1].first, m_points[_index + 1].second, m_points[i].first, m_points[i].second, m_points[i + 1].first, m_points[i + 1].second);
				}
				i++;
			}
			if (!secant)
			{
				m_points[_index].first = _x;
				m_points[_index].second = _y;
				std::cout << "Point d�plac� !" << std::endl;
			}
			else
			{
				std::cout << "Impossible : segments s�cants !" << std::endl;
			}
		}
		else
		{
			std::cout << "Impossible : segments s�cants !" << std::endl;
		}
	}
	else
	{
		std::cout << "Indice incorrect..." << std::endl;
	}
}


sf::ConvexShape BGHitbox::showHitboxContent(sf::Color& const _color)
{
	sf::ConvexShape convex;

	convex.setPointCount(m_points.size());

	for (unsigned char i = 0; i < m_points.size(); i++)
	{
		convex.setPoint(i, sf::Vector2f(m_points[i].first, m_points[i].second));
	}

	convex.setFillColor(_color);

	return convex;
}

std::vector<sf::ConvexShape> BGHitbox::showLines(unsigned char _thickness, sf::Color& const _color)
{
	std::vector<sf::ConvexShape> lines;
	for (unsigned char i = 0; i < m_points.size() - 1; i++)
	{
		sf::ConvexShape convex(4);
		convex.setOutlineThickness(_thickness);
		convex.setOutlineColor(_color);
		convex.setPoint(0, sf::Vector2f(m_points[i].first, m_points[i].second));
		convex.setPoint(1, sf::Vector2f(m_points[i + 1].first, m_points[i + 1].second));
		convex.setPoint(2, sf::Vector2f(m_points[i + 1].first, m_points[i + 1].second));
		convex.setPoint(3, sf::Vector2f(m_points[i].first, m_points[i].second));
		lines.push_back(convex);
	}

	if (m_finished)
	{
		sf::ConvexShape convex(4);
		convex.setOutlineThickness(_thickness);
		convex.setOutlineColor(_color);
		convex.setPoint(0, sf::Vector2f(m_points[m_points.size() - 1].first, m_points[m_points.size() - 1].second));
		convex.setPoint(1, sf::Vector2f(m_points[0].first, m_points[0].second));
		convex.setPoint(2, sf::Vector2f(m_points[0].first, m_points[0].second));
		convex.setPoint(3, sf::Vector2f(m_points[m_points.size() - 1].first, m_points[m_points.size() - 1].second));
		lines.push_back(convex);
	}

	return lines;
}


std::vector<sf::CircleShape> BGHitbox::showPoints(unsigned char _size, sf::Color& const _color)
{
	std::vector<sf::CircleShape> points;

	for (unsigned char i = 0; i < m_points.size(); i++)
	{
		sf::CircleShape point(_size);
		point.setFillColor(_color);
		point.setPosition(m_points[i].first - _size, m_points[i].second - _size); //Car _size correspond au rayon, autrement, cela aurait �t� _size / 2
		points.push_back(point);
	}

	return points;
}