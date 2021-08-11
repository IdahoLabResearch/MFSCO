
#include <iostream>
#include <fstream>
#include<list>
#include "county.h"
#include<algorithm>
#include<vector>
#include<cmath>
#include <iterator>
#include <map>
#include<queue>
#include<string>
#include<numeric>
#include <stdlib.h>
#include "sqlite3.h"


using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for (i = 0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

//writing a procedure that import data from sqlite database and process data in the form of arc:
//write a query that select state and required information for arc

map<int, Biomass> LoadBiomassInfo(string theFileName)
{
	ifstream theFile;
	theFile.open(theFileName.c_str());
	int theID_ = 0;
	float Harvest_collect_=0;
	float field_storage_=0;
	float prep_depot_=0;
	float Handling_queing_depot_=0;
	float depot_storage_=0;
	float biorefinery_storage_=0;
	float handling_queing_refinery_ = 0;
	float blending_=0;
	float ch_content_=0;
	float ash_content_=0;
	float moisture_content_=0;
	float dml_=0;
	float transp_pellet_f_;
	float transp_pellet_v_;
	float dml_storage_;
	float dml_preprocessing_;


	map<int, Biomass>map_biomass;
	while (theFile >> theID_ >> Harvest_collect_ >> field_storage_ >> prep_depot_ >> Handling_queing_depot_ >> depot_storage_ >> biorefinery_storage_ >> handling_queing_refinery_ >> blending_ >> ch_content_ >> ash_content_ >>
		moisture_content_ >> dml_ >> transp_pellet_f_ >> transp_pellet_v_ >> dml_storage_ >> dml_preprocessing_)  //county id implies size of the biorefinery and the demand implies total cost
	{
		Biomass a;
		a.Harvest_collect = Harvest_collect_;
		a.field_storage = field_storage_;
		a.prep_depot = prep_depot_;
		a.Handling_queing_depot = Handling_queing_depot_;
		a.depot_storage = depot_storage_;
		a.biorefinery_storage = biorefinery_storage_;
		a.handling_queuing_refinery = handling_queing_refinery_;
		a.blending = blending_;
		a.ch_content = ch_content_;
		a.ash_content = ash_content_;
		a.moisture_content = moisture_content_;
		a.dml = dml_;
		a.transp_pellet_f = transp_pellet_f_;
		a.transp_pellet_v = transp_pellet_v_;
		a.dml_storage = dml_storage_;
		a.dml_preprocessing = dml_preprocessing_;
		map_biomass[theID_] = a;
	}
	theFile.close();
	map<int, Biomass>::iterator it;
	return map_biomass;
}

vector<Arc> LoadDepotSize(string theFileName)
{
	ifstream theFile;
	theFile.open(theFileName.c_str());
	int thecountyID = 0;
	float capacity = 0;
	float  cost = 0;
	vector<Arc> DepotSize;
	while (theFile >> capacity >> cost)  //county id implies size of the biorefinery and the demand implies total cost
	{
		Arc a;
		a.loadProduction(capacity);
		a.loadArcCost(cost);
		DepotSize.push_back(a);
	}
	theFile.close();
	return DepotSize;

}

void LoadArcs(vector<Arc> &allArcs, map<int, Arc> & mapcounty, vector<Arc>&countyArcs, vector<Arc>&depotArcs, vector<Arc>&Depotsize, map<int, map<int, map<int, map<int, int>>>>&county_production, map<int, Biomass>&map_biomass)
{
	
	int i, j, k;
	sqlite3 *db;
	char *zErrMsg = 0;
	int  rc;
	char *sql;

	/* Open database */
	rc = sqlite3_open("C:/Users/RONIMS/Documents/COVID-19/1.1.1.2/billionton16.db", &db);
	if (rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		//return(0);
	}
	else{
		fprintf(stdout, "Opened database successfully\n");
	}

	//Creating  tmp table for data processing 
	sql = "CREATE TABLE tmptable2(fips           int    NOT NULL,feedstock           TEXT    NOT NULL,type         int  NOT NULL,price int NOT NULL, gprice         REAL NOT NULL , cost REAL  NOT NULL,prod  INT  NOT NULL,prod_m  INT NOT NULL,yield  real NOT NULL,lat REAL  NOT NULL, long REAL  NOT NULL, Area REAL NOT NULL ); ";
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else{
		fprintf(stdout, "Table created successfully\n");
	}

	// year =2017 corn stover , year=2019 for swithcgrass 
	//insert data 
	//ag_base.production*.25

	sql = "insert into tmptable2 select * from (select  ag_base.fips, ag_base.Feedstock, case when  ag_base.Feedstock = 'Switchgrass' then 3 when ag_base.Feedstock = 'Miscanthus' then 4 else 1 end as type,"

		" ag_base.'Biomass Price',"

		"case when  ag_base.Feedstock = 'Switchgrass' then ag_base.'Biomass Price' - 11.64 when ag_base.Feedstock = 'Miscanthus' then ag_base.'Biomass Price' - 12.67 else ag_base.'Biomass Price' - 13.69 end as gp,"

		"case when  ag_base.Feedstock = 'Switchgrass' then 42.74 when ag_base.Feedstock = 'Miscanthus' then 47.33 else 49.07 end as other_cost,"

		"ag_base.Production,case when(ag_base.Feedstock = 'Corn stover' and ag_base.Yield>1.99) then ag_base.production Else ag_base.production End as mod_prod," //change f value here 

		"Yield, county_input.Lat, county_input.Long, county_input.Area_mile  from  ag_base inner join county_input on ag_base.fips = county_input.fips1 "

		"where (ag_base.year=2019 and ag_base.Feedstock = 'Switchgrass'  and  ag_base.State = 'Kansas'   and ag_base.Production>.01 and ag_base.'Biomass Price'<90) "

		"or (ag_base.Year = 2019 and  ag_base.Feedstock = 'Switchgrass'   and  ag_base.State = 'Nebraska'   and ag_base.Production>.01 and ag_base.'Biomass Price'<90 )"

		"or (ag_base.Year = 2019 and  ag_base.Feedstock = 'Switchgrass'   and  ag_base.State = 'Colorado'   and ag_base.Production>.01 and ag_base.'Biomass Price'<90)"

		"or (ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'   and  ag_base.State = 'Kansas'   and ag_base.Production>.01 and ag_base.'Biomass Price'<90 )"

		"or(ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'   and  ag_base.State = 'Nebraska'  and  ag_base.Production>.01 and ag_base.'Biomass Price'<80) or"
		"(ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'   and  ag_base.State = 'Colorado'  and ag_base.Production>.01 and ag_base.'Biomass Price'<80) or"

		"(ag_base.year = 2020 and  ag_base.Feedstock = 'Miscanthus'   and  ag_base.State = 'Kansas'  and   ag_base.Production> .01 and ag_base.'Biomass Price'<80) or"

		"(ag_base.year = 2020 and  ag_base.Feedstock = 'Miscanthus'   and  ag_base.State = 'Nebraska' and  ag_base.Production>.01 and ag_base.'Biomass Price'<80)"

		"or(ag_base.year = 2020 and  ag_base.Feedstock = 'Miscanthus'   and  ag_base.State = 'Colorado'  and ag_base.Production> .01 and ag_base.'Biomass Price'<80)"

		"union all select  ag_base.fips, ag_base.Feedstock, "

		"case when  ag_base.Feedstock = 'Corn stover' then 2  Else 0 end as type, ag_base.'Biomass Price', case when  ag_base.Feedstock = 'Corn stover' then ag_base.'Biomass Price' -18.6 else 99999 end as gp,"

		"case when  Feedstock = 'Corn stover' then 54.06  else 9999999 end as other_cost, Production, case when  Feedstock = 'Corn stover' and Yield >1.999 then Production Else 0 End as mod_prod, " //change f value here 

		"ag_base.Yield, county_input.Lat, county_input.Long, county_input.Area_mile  from    ag_base inner join county_input on ag_base.fips = county_input.fips1 "

		"where (ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'and ag_base.State = 'Kansas' and ag_base.Production>.01 and ag_base.'Biomass Price'<80 )"

		"or (ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'   and  ag_base.State = 'Nebraska'  and  ag_base.Production>.01 and ag_base.'Biomass Price'<80)"
		"or (ag_base.year = 2017 and  ag_base.Feedstock = 'Corn stover'   and  ag_base.State = 'Colorado'  and ag_base.Production>.01 and ag_base.'Biomass Price'<80)"
		"order by ag_base.Feedstock, ag_base.yield DESC )";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else{
		fprintf(stdout, "data inserted to tmp table successfully\n");
	}

	///group by county:
	string query;

	sqlite3_stmt *res;

	const char *tall;
	query = "select tmptable2.fips, tmptable2.type, tmptable2.gprice, tmptable2.cost, sum(tmptable2.prod_m)as production, tmptable2.yield,tmptable2.lat,tmptable2.long, tmptable2.Area,tmptable2.price from tmptable2 "
		"group by tmptable2.fips, tmptable2.type, tmptable2.gprice,tmptable2.cost, tmptable2.yield,tmptable2.lat,tmptable2.long, tmptable2.Area,tmptable2.price";
	//rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tall);

	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else{
		fprintf(stdout, "data in table grouped successfully and ready for output \n");
	}

	float dis_ = 0, yld = 0, ocost = 0, en2 = 0;
	int st = 0, en = 0, prd = 0;
	

	while (sqlite3_step(res) == SQLITE_ROW)
	{
		if (sqlite3_column_int(res, 1) != 4)
		{
			Arc *u;
			u = new Arc;
			u->loadStart(sqlite3_column_int(res, 0)); //fips
			u->loadEnd(base_loc); //fips
			u->loadMid1(sqlite3_column_int(res, 9)); //loading farmgate price
			u->loadArcType(20 + sqlite3_column_int(res, 1)); //combination of feedstock and arc type , location from field to depot ( e.g 20+1 =depot to refinery biomass type =1 
			u->loadGroweryPayment((float)sqlite3_column_double(res, 2)); // farmgate-h&c cost
			u->loadSecArcType(2); // location 
			u->loadYield((float)sqlite3_column_double(res, 5));
			u->loadProduction(sqlite3_column_int(res, 4)); //feedstock prd
			u->loadLatitude((float)sqlite3_column_double(res, 6)); //lat
			u->loadLongitude((float)sqlite3_column_double(res, 7)); //long
			u->loadSecArcCost(sqrt((float)sqlite3_column_double(res, 8) / 3.14)); // calcuating draw radius of each county 

			dis_ = sqrt(69.1*69.1*(base_lat - (float)sqlite3_column_double(res, 6))*(base_lat - (float)sqlite3_column_double(res, 6)) +
				53 * 53 * (base_long - (float)sqlite3_column_double(res, 7))*(base_long - (float)sqlite3_column_double(res, 7)));
			if (sqlite3_column_int(res, 0) == base_loc)
			{
				u->loadArcCost(0); //distance from base location
			}
			else
			{
				u->loadArcCost(dis_); //distance from base location
			}
			
			depotArcs.push_back(*u);
			allArcs.push_back(*u);
			
			Arc *u2;
			u2 = new Arc;

			u2->loadLatitude((float)sqlite3_column_double(res, 6)); //lat
			u2->loadLongitude((float)sqlite3_column_double(res, 7)); //long
			mapcounty[sqlite3_column_int(res, 0)] = *u; //declared to track all county and initial depot 
			delete u;
			//creating arc type 0 and 1-4;
			Arc *u1;
			u1 = new Arc;
			u1->loadStart(sqlite3_column_int(res, 0)); //
			u1->loadEnd(sqlite3_column_int(res, 0));
			u1->loadMid1(sqlite3_column_int(res, 9)); //loading farmgate price
			u1->loadArcType(sqlite3_column_int(res, 1)); //feedstock type=1, location from field to depot 
			u1->loadGroweryPayment((float)sqlite3_column_double(res, 2));
			u1->loadSecArcType(0); // field side
			u1->loadYield((float)sqlite3_column_double(res, 5));
			u1->loadProduction(sqlite3_column_int(res, 4)); //feedstock prd
			u1->loadLatitude((float)sqlite3_column_double(res, 6)); //lat
			u1->loadLongitude((float)sqlite3_column_double(res, 7)); //long
			u1->loadSecArcCost(sqrt((float)sqlite3_column_double(res, 8) / 3.14)); // calcuating draw radius of each county 
			u1->loadArcCost(0); //distance from base location
			allArcs.push_back(*u1);
			county_production[sqlite3_column_int(res, 0)][sqlite3_column_int(res, 0)][sqlite3_column_int(res, 1)][sqlite3_column_int(res, 9)] = sqlite3_column_int(res, 4);
			delete u1;
		}
	}


	sql = "drop table tmptable2";
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else{
		fprintf(stdout, "Table dropped successfully\n");
	}
	sqlite3_close(db);

		
	//creating arc from location to depot 

	
	map<int, Arc>::iterator it;
	for (it = mapcounty.begin(); it != mapcounty.end(); it++)
	{
		for (i = 0; i <depotArcs.size(); i++)
		{
			Arc *u;
			u = new Arc;
			u->loadStart(it->first);
			u->loadEnd(depotArcs[i].getStart());
			u->loadMid1(depotArcs[i].getMid1()); //loading farmgate price
			u->loadArcType(depotArcs[i].getArcType() - 10); //feedstock type=1, location from field to depot =1 ( e.g if 23 then 23-10 becomes 13 which is location 1 feedstock type 3) 
			u->loadGroweryPayment(it->second.getGrowerPayment());
			u->loadSecArcType(1); // 1 for field
			u->loadProduction(it->second.getProduction()); //feedstock prd
			u->loadYield(it->second.getYield());
			u->loadLatitude(it->second.getLatitude()); //lat
			u->loadLongitude(it->second.getLongitude()); //long
			u->loadSecArcCost(depotArcs[i].getSecArcCost()); // calcuating draw radius of each county 
			dis_ = sqrt(69.1*69.1*(it->second.getLatitude() - depotArcs[i].getLatitude())*(it->second.getLatitude() - depotArcs[i].getLatitude()) +
				53 * 53 * (it->second.getLongitude() - depotArcs[i].getLongitude())*(it->second.getLongitude() - depotArcs[i].getLongitude()));
			if (it->first == depotArcs[i].getStart())
			{
				u->loadArcCost(depotArcs[i].getSecArcCost());
			}
			else{
				u->loadArcCost(dis_); //distance from county centriod to depot centroid location:
			}
			allArcs.push_back(*u);
			countyArcs.push_back(*u); //loading arcs from county to depot only

			delete u;
		}
	}
	//insert MSW information:
	int msw_fips = 8031, msw_type = 25, msw_dis = 220 /*220*/; //340 from Phelps county nebraska
	float msw_gp = 10, msw_lat = 99999, msw_long = 99999;
	Arc *u3;
	u3 = new Arc;
	u3->loadStart(msw_fips);  //fips
	u3->loadEnd(base_loc);
	u3->loadMid1(9999);
	u3->loadArcType(msw_type); //feedstock type
	u3->loadProduction(msw_prd);
	u3->loadGroweryPayment(msw_gp);
	u3->loadSecArcType(2); //grower payment
	u3->loadLatitude(msw_lat); //does not apply
	u3->loadLongitude(msw_long); // does not apply
	u3->loadArcCost(msw_dis); //distance from base location
	u3->loadYield(0);
	allArcs.push_back(*u3);
	depotArcs.push_back(*u3);
	delete u3;
	cout << " end of preparing input  total   " <<allArcs.size()<<"   arcs" << endl;
} 

