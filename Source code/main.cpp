
#include <iostream>
#include "Arc.h" 
#include "Biomass_info.h"
#include "Input_Data.h"
#include"Process data .h"
#include "Build Model.h"
#include<cmath>
#include<fstream>
#include <ctime>
using namespace std;

int main(int argc, const char * argv[])
{
	float start_s = clock();
	map<int, Biomass>map_biomass;
	map_biomass = LoadBiomassInfo("Biomass_info.txt");
	vector<Arc> Depotsize = LoadDepotSize("Depot_size.txt");
	map<int, Arc> mapcounty;
	vector<Arc> allArcs; 
	vector<Arc>countyArcs;
	vector<Arc>depotArcs;
	map<int, map<int, map<int, map<int, int>>>>county_production;
	LoadArcs(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, county_production, map_biomass);
	CPLEXProgram2(allArcs, mapcounty, countyArcs, depotArcs, Depotsize, map_biomass, county_production); //use for CBS
	int stop_s = clock();
	cout << "time: in seconds " << (stop_s-start_s)/double(CLOCKS_PER_SEC) << endl;
    return 0;
}

