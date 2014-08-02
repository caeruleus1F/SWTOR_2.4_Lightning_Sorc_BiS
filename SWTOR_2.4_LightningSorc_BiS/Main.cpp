#include <iostream>
#include <cmath>
#include <time.h>

#include "Player.h"

using namespace std;

int main () {

	cout << "... Initializing stats ...\n\n";

	srand ((unsigned) time(NULL));

	Player AD;
	float StatCompare[12] = {0};

	AD.IN_GROUP = 1;
	AD.COMP_HEALTH_BUFF = 1;
	AD.COMP_CRIT_BUFF = 1;
	AD.COMP_SURGE_BUFF = 1;
	AD.COMP_ACCURACY_BUFF = 1;

	cout << "... Stats Initialized ...\n\n";

	cout << "Hilt/Armorings: \n";
	AD.OffHiltArmorings(StatCompare);

	cout << "\n\nMods: ";
	AD.OffMods(StatCompare);

	cout << "\n\nEnhancements: ";
	AD.OffEnhancements(StatCompare);

	cout << "\n\nEarpiece: ";
	AD.OffEarpieces(StatCompare);

	cout << "\n\nImplants: ";
	AD.OffImplants(StatCompare);

	cout << "\n\nColor Crystals: ";
	AD.OffColorCrystals(StatCompare);

	cout << "\n\nAugments: ";
	AD.OffAugments(StatCompare);

	//2 Relics
	AD.modOffStats("AC", 0.0, "WP", 0.0, "PO", 64.0, "CR", 0.0, "SU", 0.0, "AL", 0.0, 0);
	//show end result stats
	AD.DisplayRotationStats();
	AD.showOffStats();

	cout << "\nPress ENTER to close...";
	cin.get();

	return 0;

}