#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <fstream>
#include <cstdlib>
using namespace std;
using namespace sf;


void configWindow(RenderWindow& W, String title = L"Okno") {
	W.create(VideoMode(1000, 650, 32), title, Style::Close);
	W.setActive(false);
	W.setKeyRepeatEnabled(false);
	W.setVerticalSyncEnabled(true);
	W.setPosition(Vector2i(2, 2));
}

int horner(int wsp[], int st, int x)
{
	if (st == 0)
		return wsp[0];

	return x * horner(wsp, st - 1, x) + wsp[st];
}

int main()
{
	srand(time(0));
	setlocale(LC_CTYPE, "");

	RenderWindow W;
	configWindow(W, L"Wykres funkcji");

	Image icon;
	icon.loadFromFile("resources/icon.png");
	W.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	Music music;
	if (!music.openFromFile("resources/background.wav")) return -1;
	music.play();
	music.setLoop(true);
	music.setVolume(25);
	bool muzykaOn = true;

	bool czyStart = true;

	Clock zegar;
	Time sekunda = seconds(1.f);
	Time cooldown = seconds(2);

	RectangleShape oknoMenu(Vector2f(W.getSize().x, W.getSize().x / 3));
	oknoMenu.setPosition(0, oknoMenu.getSize().y - 200);
	oknoMenu.setFillColor(Color(251, 126, 20, 180));

	RectangleShape oknoNaWzorFunkcji(Vector2f(W.getSize().x, 150));
	oknoNaWzorFunkcji.setPosition(0, 500);
	oknoNaWzorFunkcji.setFillColor(Color(251, 126, 20));

	RectangleShape uklad(Vector2f(W.getSize().x, W.getSize().y - 150));
	uklad.setPosition(0, 0);
	uklad.setFillColor(Color(50, 50, 50));

	CircleShape punktzerozero(1);
	punktzerozero.setOrigin(punktzerozero.getRadius(), punktzerozero.getRadius());
	punktzerozero.setPosition(uklad.getSize().x / 2 + 5, uklad.getSize().y / 2 - 5);
	punktzerozero.setFillColor(Color(255, 255, 255));

	RectangleShape osX(Vector2f(uklad.getSize().x, 2));
	osX.setPosition(0, punktzerozero.getPosition().y);
	osX.setFillColor(Color(255, 255, 255));

	RectangleShape skalaOsX(Vector2f(2, 9));
	skalaOsX.setPosition(punktzerozero.getPosition().x + 10, punktzerozero.getPosition().y - skalaOsX.getSize().y / 2);
	skalaOsX.setFillColor(Color(255, 255, 255));

	CircleShape osXstrzalka(8, 3);
	osXstrzalka.setPosition(W.getSize().x, uklad.getSize().y / 2 - osXstrzalka.getRadius() - 5);
	osXstrzalka.setFillColor(Color(255, 255, 255));
	osXstrzalka.rotate(90);

	RectangleShape osY(Vector2f(2, uklad.getSize().y));
	osY.setPosition(punktzerozero.getPosition().x, 0);
	osY.setFillColor(Color(255, 255, 255));

	RectangleShape skalaOsY(Vector2f(9, 2));
	skalaOsY.setPosition(punktzerozero.getPosition().x - skalaOsY.getSize().x / 2, punktzerozero.getPosition().y - 10);
	skalaOsY.setFillColor(Color(255, 255, 255));

	CircleShape osYstrzalka(8, 3);
	osYstrzalka.setPosition(punktzerozero.getPosition().x - osYstrzalka.getRadius() + 1, 0);
	osYstrzalka.setFillColor(Color(255, 255, 255));

	RectangleShape liniaOddzielajacaFunkcjeOdPorad(Vector2f(W.getSize().x, 2));
	liniaOddzielajacaFunkcjeOdPorad.setPosition(0, (int)W.getSize().y - 40);
	liniaOddzielajacaFunkcjeOdPorad.setFillColor(Color(0, 0, 0));

	int* wspolczynniki;
	int stopien = 0;
	array<int, 121> argument;
	for (size_t i = 0; i < 121; ++i)
	{
		argument[i] = -60 + i;
	}

	wspolczynniki = new int[stopien + 1];

	ostringstream tresc;
	string tresc2 = tresc.str();

	array <int, 121> miejsceNaOsiY;

	array<CircleShape, 121> punkty;
	for (size_t i = 0; i < 121; ++i) {
		punkty[i].setRadius(3);
		punkty[i].setFillColor(Color(145, 145, 145));
	}

	VertexArray liniaWykresu(LineStrip, 121);
	for (int i = 0; i < 121; ++i)
	{
		liniaWykresu[i].color = (Color(251, 126, 20));
	}
	
	Font fontBahnschrift;
	if (!fontBahnschrift.loadFromFile("resources/bahnschrift.ttf")) {
		//jakis blad
	}

	Text jedenX("1", fontBahnschrift, 15);
	jedenX.setFillColor(Color(255, 255, 255));
	jedenX.setOrigin(-514, -258);

	Text jedenY("1", fontBahnschrift, 15);
	jedenY.setFillColor(Color(255, 255, 255));
	jedenY.setOrigin(-486, -228);

	int rozmiarText = 50;
	Text wzorFunkcjiTekst(tresc2, fontBahnschrift, rozmiarText);
	wzorFunkcjiTekst.setFillColor(Color(0, 0, 0));
	wzorFunkcjiTekst.setOrigin(0, -(int)W.getSize().y + 130);

	Text naglowekMenu("MENU", fontBahnschrift, 70);
	naglowekMenu.setFillColor(Color(0, 0, 0));
	naglowekMenu.setOrigin(0, -oknoMenu.getSize().y + 175);
	naglowekMenu.setLetterSpacing(20);

	Text poradyMenu(" [E] - zamknij program\n [R] - restart\n [M] - muzyka\n [G] - siatka\n [P] - punkty\n [F] - wczytaj dane z pilku dane.txt", fontBahnschrift, 25);
	poradyMenu.setFillColor(Color(0, 0, 0));
	poradyMenu.setOrigin(0, -((int)oknoMenu.getSize().y) + 60);
	poradyMenu.setLetterSpacing(2);

	Text muzykaSwitchTekst("MUZYKA", fontBahnschrift, 20);
	muzykaSwitchTekst.setFillColor(Color(0, 0, 0));
	muzykaSwitchTekst.setOrigin(-10, -(int)W.getSize().y + 35);
	muzykaSwitchTekst.setLetterSpacing(2);

	Text siatkaSwitchTekst("SIATKA", fontBahnschrift, 20);
	siatkaSwitchTekst.setFillColor(Color(0, 0, 0));
	siatkaSwitchTekst.setOrigin(-210, -(int)W.getSize().y + 35);
	siatkaSwitchTekst.setLetterSpacing(2);

	Text punktySwitchTekst("PUNKTY", fontBahnschrift, 20);
	punktySwitchTekst.setFillColor(Color(0, 0, 0));
	punktySwitchTekst.setOrigin(-410, -(int)W.getSize().y + 35);
	punktySwitchTekst.setLetterSpacing(2);

	Text ustawieniaHelp("[ESC] - MENU", fontBahnschrift, 20);
	ustawieniaHelp.setFillColor(Color(0, 0, 0));
	ustawieniaHelp.setOrigin(-675, -(int)W.getSize().y + 35);
	ustawieniaHelp.setLetterSpacing(2);


	array <RectangleShape, 100>siatkaA;
	array <RectangleShape, 100>siatkaB;
	for (size_t i = 0; i < 100; ++i)
	{
		siatkaA[i].setSize(Vector2f(1, uklad.getSize().y));
		siatkaA[i].setPosition(i * 10 + 5, 0);
		siatkaA[i].setFillColor(Color(75, 75, 75));
		siatkaB[i].setSize(Vector2f(uklad.getSize().y * 2, 1));
		siatkaB[i].setPosition(0, i * 10 + 5);
		siatkaB[i].setFillColor(Color(75, 75, 75));
	}

	bool siatkaOn = true;
	bool punktyOn = false;
	bool menuOn = false;


	Event e;
	while (W.isOpen()) {
		while (W.pollEvent(e)) {
			if (e.type == Event::Closed || (e.type == Event::KeyPressed && e.key.code == Keyboard::E) && menuOn == true) {
				W.close();
				break;
			}

			if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
				if (menuOn == false) {
					menuOn = true;
				}
				else if (menuOn == true) {
					menuOn = false;
				}
			}

			if (e.type == Event::KeyPressed && e.key.code == Keyboard::F) {
				ifstream input("resources/dane.txt");
				vector<int> dane(10);
				stopien = -1;
				string line;
				int liczbaLiniiTXT = 0;

				for (liczbaLiniiTXT = 0; getline(input, line); ++liczbaLiniiTXT);
				input.close();

				//TWORZE DRUGI RAZ INPUT PONIEWAZ INACZEJ NIE MOGLEM ZALADOWAC WSPOLCZYNNIKOW
				ifstream input2("resources/dane.txt");
				int pom = 0;
				while (getline(input2, line)) {
					dane[pom] = stoi(line);
					++pom;
					++stopien;
				}
				input2.close();

				for (size_t i = 0; i <= stopien; ++i)
				{
					wspolczynniki[i] = dane[i];
				}
				input.close();
				if (stopien == 0) {
					for (size_t i = 0; i < 121; ++i)
					{
						miejsceNaOsiY[i] = wspolczynniki[0];
					}
				}
				else {
					for (size_t i = 0; i < 121; ++i)
					{
						miejsceNaOsiY[i] = horner(wspolczynniki, stopien, argument[i]);
						//cout << "W( " << argument << " ) = " << horner(wspolczynniki, stopien, argument[i]) << endl;
					}
				}

				for (size_t i = 0; i < 121; ++i) {
					punkty[i].setPosition(punktzerozero.getPosition().x + argument[i] * 10 - 3, punktzerozero.getPosition().y - miejsceNaOsiY[i] * 10 - 3);
					liniaWykresu[i].position = (Vector2f(punktzerozero.getPosition().x + argument[i] * 10, punktzerozero.getPosition().y - miejsceNaOsiY[i] * 10));
					liniaWykresu[i].color = (Color(251, 126, 20));
				}
				W.draw(liniaWykresu);

				tresc.str("");
				tresc << " f(x) =";
				for (size_t i = 0; i <= stopien; ++i)
				{
					if (wspolczynniki[i] == 0);
					else if (stopien == 0) tresc << " " << wspolczynniki[i];
					else if ((stopien - i) == 0 && wspolczynniki[i] < 0) tresc << " - " << abs(wspolczynniki[i]);
					else if ((stopien - i) == 0 && wspolczynniki[i] > 0) tresc << " + " << wspolczynniki[i];
					else if (i == 0) tresc << " " << wspolczynniki[i] << "x^" << stopien - i;
					else if (i != 0 && wspolczynniki[i] < 0 && (stopien - i) == 1) tresc << " - " << abs(wspolczynniki[i]) << "x ";
					else if (i != 0 && wspolczynniki[i] >= 0 && (stopien - i) == 1) tresc << " + " << wspolczynniki[i] << "x ";
					else if (i != 0 && wspolczynniki[i] < 0 && (stopien - i) > 1) tresc << " - " << abs(wspolczynniki[i]) << "x^" << stopien - i;
					else if (i != 0 && wspolczynniki[i] >= 0 && (stopien - i) > 1) tresc << " + " << wspolczynniki[i] << "x^" << stopien - i;
				}
				string tresc2 = tresc.str();
				wzorFunkcjiTekst.setString(tresc2);
				rozmiarText = 50;
				if (stopien > 6) rozmiarText = 50 - stopien;
				wzorFunkcjiTekst.setCharacterSize(rozmiarText);
				W.draw(wzorFunkcjiTekst);

				W.display();
				break;
			}

			if (e.type == Event::KeyPressed && e.key.code == Keyboard::G) {
				if (siatkaOn == false) {
					siatkaOn = true;
				}
				else if (siatkaOn == true) {
					siatkaOn = false;
				}
			}

			if (e.type == Event::KeyPressed && e.key.code == Keyboard::P) {
				if (punktyOn == false) {
					punktyOn = true;
				}
				else if (punktyOn == true) {
					punktyOn = false;
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::R) || czyStart == true) {
				tresc.str("");
				tresc2 = " Przejdz do konsoli";
				wzorFunkcjiTekst.setString(tresc2);
				W.draw(oknoNaWzorFunkcji);
				W.draw(wzorFunkcjiTekst);
				W.draw(liniaOddzielajacaFunkcjeOdPorad);
				W.display();
				cout << "Stopień wielomianu: ";
				cin >> stopien;

				if (stopien == 0) {
					cout << "Podaj współczynnik: ";
					cin >> wspolczynniki[0];
				}
				else {
					//wczytanie współczynników
					for (int i = 0; i <= stopien; ++i)
					{
						cout << "Podaj współczynnik stojący przy x^" << stopien - i << ": ";
						cin >> wspolczynniki[i];
					}
				}

				if (stopien == 0) {
					for (size_t i = 0; i < 121; ++i)
					{
						miejsceNaOsiY[i] = wspolczynniki[0];
					}
				}
				else {
					for (size_t i = 0; i < 121; ++i)
					{
						miejsceNaOsiY[i] = horner(wspolczynniki, stopien, argument[i]);
						//cout << "W( " << argument << " ) = " << horner(wspolczynniki, stopien, argument[i]) << endl;
					}
				}

				for (size_t i = 0; i < 121; ++i) {
					punkty[i].setPosition(punktzerozero.getPosition().x + argument[i] * 10 - 3, punktzerozero.getPosition().y - miejsceNaOsiY[i] * 10 - 3);
					liniaWykresu[i].position = (Vector2f(punktzerozero.getPosition().x + argument[i] * 10, punktzerozero.getPosition().y - miejsceNaOsiY[i] * 10));
				}

				W.draw(liniaWykresu);

				tresc.str("");
				tresc << " f(x) =";
				for (size_t i = 0; i <= stopien; ++i)
				{
					if (wspolczynniki[i] == 0);
					else if (stopien == 0) tresc << " " << wspolczynniki[i];
					else if ((stopien - i) == 0 && wspolczynniki[i] < 0) tresc << " - " << abs(wspolczynniki[i]);
					else if ((stopien - i) == 0 && wspolczynniki[i] > 0) tresc << " + " << wspolczynniki[i];
					else if (i == 0) tresc << " " << wspolczynniki[i] << "x^" << stopien - i;
					else if (i != 0 && wspolczynniki[i] < 0 && (stopien - i) == 1) tresc << " - " << abs(wspolczynniki[i]) << "x ";
					else if (i != 0 && wspolczynniki[i] >= 0 && (stopien - i) == 1) tresc << " + " << wspolczynniki[i] << "x ";
					else if (i != 0 && wspolczynniki[i] < 0 && (stopien - i) > 1) tresc << " - " << abs(wspolczynniki[i]) << "x^" << stopien - i;
					else if (i != 0 && wspolczynniki[i] >= 0 && (stopien - i) > 1) tresc << " + " << wspolczynniki[i] << "x^" << stopien - i;
				}
				string tresc2 = tresc.str();
				wzorFunkcjiTekst.setString(tresc2);
				rozmiarText = 50;
				if (stopien != 0) rozmiarText = 120 / stopien;
				if (rozmiarText > 50) rozmiarText = 50;
				wzorFunkcjiTekst.setCharacterSize(rozmiarText);
				W.draw(wzorFunkcjiTekst);

				W.display();
				czyStart = false;
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::M)) {
			if (cooldown > sekunda && muzykaOn == true) {
				music.pause();
				muzykaOn = false;
				cooldown = seconds(0);
				zegar.restart();
			}
			else if (muzykaOn == false && cooldown > sekunda) {
				music.play();
				muzykaOn = true;
				cooldown = seconds(0);
				zegar.restart();
			}
		}

		if (muzykaOn == true) {
			muzykaSwitchTekst.setFillColor(Color(0, 0, 0));
		}
		else if (muzykaOn == false) {
			muzykaSwitchTekst.setFillColor(Color(92, 92, 92));
		}

		cooldown = zegar.getElapsedTime();

		W.draw(uklad);

		if (siatkaOn == true) {
			for (size_t i = 0; i < 100; ++i)
			{
				W.draw(siatkaA[i]);
				W.draw(siatkaB[i]);
			}
			siatkaSwitchTekst.setFillColor(Color(0, 0, 0));
		}
		else if (siatkaOn == false) {
			siatkaSwitchTekst.setFillColor(Color(92, 92, 92));
		}

		W.draw(osX);
		W.draw(osXstrzalka);
		W.draw(skalaOsX);
		W.draw(jedenX);
		W.draw(osY);
		W.draw(skalaOsY);
		W.draw(jedenY);
		W.draw(osYstrzalka);
		W.draw(punktzerozero);
		W.draw(liniaWykresu);

		if (punktyOn == true) {
			for (size_t i = 0; i < 121; ++i) {
				W.draw(punkty[i]);
			}
			punktySwitchTekst.setFillColor(Color(0, 0, 0));
		}
		else if (punktyOn == false) {
			punktySwitchTekst.setFillColor(Color(92, 92, 92));
		}

		W.draw(oknoNaWzorFunkcji);
		W.draw(wzorFunkcjiTekst);
		W.draw(muzykaSwitchTekst);
		W.draw(siatkaSwitchTekst);
		W.draw(punktySwitchTekst);
		W.draw(ustawieniaHelp);
		W.draw(liniaOddzielajacaFunkcjeOdPorad);

		if (menuOn == true) {
			W.draw(oknoMenu);
			W.draw(naglowekMenu);
			W.draw(poradyMenu);
		}

		W.display();

	}

	return 0;
}