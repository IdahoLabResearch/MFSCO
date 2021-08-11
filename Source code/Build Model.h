

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include<map>
#include<cmath>
#include<list>
#include <ilcplex/ilocplex.h>
using namespace std;

typedef IloArray<IloIntVarArray> Arcs;
void createMainModel(IloModel model, IloNumVarArray x, IloNumVarArray z, Arcs w, vector<Arc> &allArcs, map<int, Arc> &mapcounty, vector<Arc>&countyArcs, vector<Arc>&depotArcs, 
	vector<Arc>&Depot_c_cost, vector<Arc>&CountyInfo, map<int, Biomass>&map_biomass, map<int, map<int, map<int, map<int, int>>>>&county_production);

void CPLEXProgram2(vector<Arc> &allArcs, map<int, Arc> & mapcounty, vector<Arc>&countyArcs, vector<Arc>&depotArcs, vector<Arc>&Depot_c_cost, map<int, Biomass>&map_biomass, map<int, map<int, map<int, map<int, int>>>>&county_production)
{
	IloEnv env;
	try {

		int i, j, k;
		ofstream output_2("print_result.txt");
		map<int, Arc>::iterator itw;
		vector<Arc>CountyInfo;

		//keep tracking county id for using in constraints in the model;
		for (itw = mapcounty.begin(); itw != mapcounty.end(); ++itw)
		{
			Arc a;
			a.loadArcID(itw->first);
			CountyInfo.push_back(a);
		}

		//declaring variables
		IloModel model(env);
		IloNumVarArray x(env, allArcs.size(), 0, IloInfinity, ILOFLOAT); //flow variable
		IloNumVarArray z(env, allArcs.size(), 0, 1, ILOINT); //binary variable to select price at a supply curve 
		Arcs w(env, CountyInfo.size()); //depot location variable

		for (i = 0; i < CountyInfo.size(); i++)
		{
			w[i] = IloIntVarArray(env, Depot_c_cost.size(), 0, 1); //depot capacity range ;
		}

		//*************** Writing Economic objective *******************************************
		//need update cost in the arc:
		IloExpr obj(env);
		IloExpr rhs(env);

		IloExpr obj_ash(env);

		//  minimization  problem
		for (j = 0; j < allArcs.size(); j++)
		{
			if (allArcs[j].getArcType() == 1)
			{
				obj += x[j] * (allArcs[j].getGrowerPayment() + map_biomass[1].Harvest_collect + map_biomass[1].field_storage);
			}
			if (allArcs[j].getArcType() == 2)
			{
				obj += x[j] * (allArcs[j].getGrowerPayment() + map_biomass[2].Harvest_collect + map_biomass[2].field_storage);
			}
			if (allArcs[j].getArcType() == 3)
			{
				obj += x[j] * (allArcs[j].getGrowerPayment() + map_biomass[3].Harvest_collect + map_biomass[3].field_storage);
			}
			if (allArcs[j].getArcType() == 4)
			{
				obj += x[j] * (allArcs[j].getGrowerPayment() + map_biomass[4].Harvest_collect + map_biomass[4].field_storage);
			}

			if (allArcs[j].getArcType() == 11)
			{
				obj += x[j] * (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + x[j] * (map_biomass[1].prep_depot + map_biomass[1].depot_storage + map_biomass[1].Handling_queing_depot);
			}
			if (allArcs[j].getArcType() == 12)
			{
				obj += x[j] * (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + x[j] * (map_biomass[2].prep_depot + map_biomass[2].depot_storage + map_biomass[2].Handling_queing_depot);
			}
			if (allArcs[j].getArcType() == 13)
			{
				obj += x[j] * (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + x[j] * (map_biomass[3].prep_depot + map_biomass[3].depot_storage + map_biomass[3].Handling_queing_depot);
			}
			if (allArcs[j].getArcType() == 14)
			{
				obj += x[j] * (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + x[j] * (map_biomass[4].prep_depot + map_biomass[4].depot_storage + map_biomass[4].Handling_queing_depot);
			}

			if (allArcs[j].getArcType() == 21)
			{
				obj += x[j] * (map_biomass[1].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[1].transp_pellet_v + map_biomass[1].biorefinery_storage + map_biomass[1].handling_queuing_refinery + map_biomass[1].blending);
				obj_ash += x[j] * map_biomass[1].ash_content;
			}
			if (allArcs[j].getArcType() == 22)
			{
				obj += x[j] * (map_biomass[2].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[2].transp_pellet_v + map_biomass[2].biorefinery_storage + map_biomass[2].handling_queuing_refinery + map_biomass[2].blending);
				obj_ash += x[j] * map_biomass[2].ash_content;
			}
			if (allArcs[j].getArcType() == 23)
			{
				obj += x[j] * (map_biomass[3].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[3].transp_pellet_v + map_biomass[3].biorefinery_storage + map_biomass[3].handling_queuing_refinery + map_biomass[3].blending);
				obj_ash += x[j] * map_biomass[3].ash_content;
			}
			if (allArcs[j].getArcType() == 24)
			{
				obj += x[j] * (map_biomass[4].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[4].transp_pellet_v + map_biomass[4].biorefinery_storage + map_biomass[4].handling_queuing_refinery + map_biomass[4].blending);
				obj_ash += x[j] * map_biomass[4].ash_content;
			}
			if (allArcs[j].getArcType() == 25)
			{
				obj += x[j] * (allArcs[j].getGrowerPayment() + map_biomass[5].Harvest_collect + map_biomass[5].field_storage + map_biomass[5].prep_depot + map_biomass[5].depot_storage + map_biomass[5].Handling_queing_depot

					+ map_biomass[5].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[5].transp_pellet_v + map_biomass[5].biorefinery_storage + map_biomass[5].handling_queuing_refinery + map_biomass[5].blending);
				obj_ash += x[j] * map_biomass[5].ash_content;
			}

		}

		//determining construction cost of depot 

		for (i = 0; i < mapcounty.size(); i++)
		{
			for (j = 0; j < Depot_c_cost.size(); j++)
			{
				obj += w[i][j] * Depot_c_cost[j].getProduction()* Depot_c_cost[j].getArcCost(); //depot construction cost 
			}
		}


		//adding ash disposal cost in the objective function;

		createMainModel(model, x, z, w, allArcs, mapcounty, countyArcs, depotArcs, Depot_c_cost, CountyInfo, map_biomass, county_production);




		cout << "End of creating main model " << endl;

		//////*****************end of creating basic model****************************************************

		IloObjective objective(env);

		objective = IloAdd(model, IloMinimize(env, obj));

		IloCplex cplex(model);

		//cplex.exportModel("depot_blending_model.lp");

		//cplex.setParam(IloCplex::EpGap, .01);

		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
		{
			output_2 << "modify model and generate result with penalty cost" << endl;
			exit;
		}
		//************** Processing Results*******************************************************
		float depotcost = 0;
		float depot_total_size = 0;
		output_2 << "Main results " << endl;
		output_2 << "Depot location and size " << endl;
		output_2 << "'Location id'"<<"  "<< "'Capacity'" << endl;
		for (i = 0; i < CountyInfo.size(); i++)
		{
			for (j = 0; j<Depot_c_cost.size(); j++)
			if (cplex.getValue(w[i][j])>.01)
			{
				depotcost = depotcost + Depot_c_cost[j].getProduction()*Depot_c_cost[j].getArcCost();
				depot_total_size = depot_total_size + Depot_c_cost[j].getProduction();
				output_2 << CountyInfo[i].getArcID() << "  " << Depot_c_cost[j].getProduction() << "   " << /*cplex.getValue(w[i][j]) <<*/ endl;
			}
		}

		double total_cost = 0, total_biomass = 0, delivered_feedstock_cost = 0;



		f1_other = map_biomass[1].Harvest_collect + map_biomass[1].field_storage + map_biomass[1].prep_depot + map_biomass[1].Handling_queing_depot + map_biomass[1].depot_storage + map_biomass[1].biorefinery_storage
			+ map_biomass[1].handling_queuing_refinery + map_biomass[1].blending;
		f2_other = map_biomass[2].Harvest_collect + map_biomass[2].field_storage + map_biomass[2].prep_depot + map_biomass[2].Handling_queing_depot + map_biomass[2].depot_storage + map_biomass[2].biorefinery_storage
			+ map_biomass[2].handling_queuing_refinery + map_biomass[2].blending;
		f3_other = map_biomass[3].Harvest_collect + map_biomass[3].field_storage + map_biomass[3].prep_depot + map_biomass[3].Handling_queing_depot + map_biomass[3].depot_storage + map_biomass[3].biorefinery_storage
			+ map_biomass[3].handling_queuing_refinery + map_biomass[3].blending;
		f4_other = map_biomass[4].Harvest_collect + map_biomass[4].field_storage + map_biomass[4].prep_depot + map_biomass[4].Handling_queing_depot + map_biomass[4].depot_storage + map_biomass[4].biorefinery_storage
			+ map_biomass[4].handling_queuing_refinery + map_biomass[4].blending;
		f5_other = map_biomass[5].Harvest_collect + map_biomass[5].field_storage + map_biomass[5].prep_depot + map_biomass[5].Handling_queing_depot + map_biomass[5].depot_storage + map_biomass[5].biorefinery_storage
			+ map_biomass[5].handling_queuing_refinery + map_biomass[5].blending;


		//creating blending table 
		double fed1 = 0, fed2 = 0, fed3 = 0, fed4 = 0, fed5 = 0;
		double del_cost1 = 0, del_cost2 = 0, del_cost3 = 0, del_cost4 = 0, del_cost5 = 0;
		double g1 = 0, g2 = 0, g3 = 0, g4 = 0, g5 = 0;


		double fed1t = 0, fed2t = 0, fed3t = 0, fed4t = 0, fed5t = 0;
		double t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0; //transportation cost
		double d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0; //distance * ton



		double fed1_ = 0, fed2_ = 0, fed3_ = 0, fed4_ = 0, fed5_ = 0;
		double t1_ = 0, t2_ = 0, t3_ = 0, t4_ = 0, t5_ = 0; //transportation cost
		double d1_ = 0, d2_ = 0, d3_ = 0, d4_ = 0, d5_ = 0; //distance * ton




		for (int j = 0; j < allArcs.size(); j++)
		if (cplex.getValue(x[j])>.01)
		{
			if (allArcs[j].getArcType() == 1)
			{
				fed1 = fed1 + cplex.getValue(x[j]);
				g1 = g1 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
			}
			if (allArcs[j].getArcType() == 2)
			{
				fed2 = fed2 + cplex.getValue(x[j]);
				g2 = g2 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
			}

			if (allArcs[j].getArcType() == 3)
			{
				fed3 = fed3 + cplex.getValue(x[j]);
				g3 = g3 + cplex.getValue(x[j]) * (allArcs[j].getGrowerPayment());
			}
			if (allArcs[j].getArcType() == 11)
			{
				fed1t = fed1t + cplex.getValue(x[j]);
				t1 = t1 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				d1 = d1 + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 

			}
			if (allArcs[j].getArcType() == 12)
			{
				fed2t = fed2t + cplex.getValue(x[j]);
				t2 = t2 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				d2 = d2 + cplex.getValue(x[j]) * allArcs[j].getArcCost();

			}

			if (allArcs[j].getArcType() == 13)
			{
				fed3t = fed3t + cplex.getValue(x[j]);
				t3 = t3 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				d3 = d3 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
			}

			//summaring type2 arc
			if (allArcs[j].getArcType() == 21)
			{
				fed1_ = fed1_ + cplex.getValue(x[j]);
				t1_ = t1_ + cplex.getValue(x[j]) * ((map_biomass[1].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[1].transp_pellet_v));
				d1_ = d1_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 

			}
			if (allArcs[j].getArcType() == 22)
			{
				fed2_ = fed2_ + cplex.getValue(x[j]);
				t2_ = t2_ + cplex.getValue(x[j]) * ((map_biomass[2].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[2].transp_pellet_v));
				d2_ = d2_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
			}

			if (allArcs[j].getArcType() == 23)
			{
				fed3_ = fed3_ + cplex.getValue(x[j]);
				t3_ = t3_ + cplex.getValue(x[j]) * ((map_biomass[3].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[3].transp_pellet_v));
				d3_ = d3_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
			}

			if (allArcs[j].getArcType() == 25)
			{
				fed5_ = fed5_ + cplex.getValue(x[j]);
				t5_ = t5_ + cplex.getValue(x[j]) * ((map_biomass[5].transp_pellet_f + allArcs[j].getArcCost()* map_biomass[5].transp_pellet_v));
				d5_ = d5_ + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 
			}

		}
		

		output_2 << "Summary of sourced biomass for blending with cost and other information  " << endl;
		output_2 << " 'Biomass type'" << " " << "'Purchased biomass'" << " " << "'Average grower payment'" << "  " << "'Transp-1. cost'" << "'Average Transp.-1 distance'" << "  ";
		output_2 << "'Transp.-1 biomass'" << " " << "'Transp-2. cost'" << "'Average Transp.-2 distance'" << "'Transp.-2 biomass'" << " " << "'Avg. Depot Construciton cost'" << "  ";
		output_2<<"'Other cost'"<<"  "<<"Carb. %'"<<"  "<<"'Moisture %'"<<"  "<<"'Dry matter loss %'"<<endl;

		output_2 <<"3P-CS"<<" "<< fed1 << "  " << g1 / fed1 << "   " << t1 / fed1t << "  " << d1 / fed1t << "  " << fed1t << "   " << t1_ / fed1_ << "  " << d1_ / fed1_ << "   " << fed1_ << "   " << depotcost / depot_total_size << "  ";
		output_2 << f1_other << "  " << c1 << "  " << a1 << "  " << map_biomass[1].moisture_content << "    " << map_biomass[1].dml << endl;

		output_2 << "2P-CS" << "   "<<fed2 << "  " << g2 / fed2 << "   " << t2 / fed2t << "  " << d2 / fed2t << "  " << fed2t << "   " << t2_ / fed2_ << "  " << d2_ / fed2_ << "   " << fed2_ << "   " << depotcost / depot_total_size << "  ";
		output_2 << f2_other << "  " << c2 << "  " << a2 << "  " << map_biomass[2].moisture_content << "    " << map_biomass[2].dml << endl;
		output_2 << "SG" << "   " << fed3 << "  " << g3 / fed3 << "   " << t3 / fed3t << "  " << d3 / fed3t << "  " << fed3t << "   " << t3_ / fed3_ << "  " << d3_ / fed3_ << "   " << fed3_ << "   " << depotcost / depot_total_size << "  ";
		output_2 << f3_other << "  " << c3 << "  " << a3 << "  " << map_biomass[3].moisture_content << "    " << map_biomass[3].dml << endl;
		
		output_2 << "GC" << "   " << fed5_ / (1 - map_biomass[5].dml) << "  " << 10 << "   " << 0 << "  " << 0 << "  " << 0 << "  " << t5_ / fed5_ << "  " << d5_ / fed5_ << "   " << fed5_ << "   " << 0 << "  ";
		output_2 << f5_other << "  " << c5 << "  " << a5 << "  " << map_biomass[5].moisture_content << "    " << map_biomass[5].dml << endl;
		

		//printing county supply and used supply:

		//create table 2: 
		output_2 << "Summary of sourced biomass from different county " << endl;

		output_2 << "'County FIPS'" <<" "<<"'Farmgate Price'"<<"'Available biomass'"<<"  "<<"'Purchased Biomass'"<<"'Yield'"<< endl;
		 for (i = 0; i < allArcs.size(); i++)
		 if (cplex.getValue(x[i])>.01)
		 {
			 if (allArcs[i].getArcType() == 1 )
			 {
				 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"    "<< "3P CS" << "   " << allArcs[i].getProduction()<<"   "<< cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
			 }
			 if (allArcs[i].getArcType() == 2)
			 {
				 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"   "<< "2P CS" << "   " << allArcs[i].getProduction() << "   " << cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
			 }
			 if (allArcs[i].getArcType() == 3 )
			 {
				 output_2 << allArcs[i].getStart() << "   " << allArcs[i].getMid1() <<"   "<< "SG" << "   " << allArcs[i].getProduction() << "   " << cplex.getValue(x[i]) << "   " << allArcs[i].getYield() << endl;
			 }
			 if (allArcs[i].getArcType() == 25 )
			 {
				 output_2 << allArcs[i].getStart() << "   " << 10 <<"  "<< "GC" << "   " << msw_prd << "   " << msw_prd << "   " << allArcs[i].getYield() << endl;
			 }
		 }

		//end of creating table 2
	
	}
	catch(IloException& e) {
	cerr  << " ERROR: " << e << endl;   
	}
	catch(...) {
	cerr  << " ERROR" << endl;   
	}

	env.end();
}

void createMainModel(IloModel model, IloNumVarArray x, IloNumVarArray z, Arcs w, vector<Arc> &allArcs, map<int, Arc> & mapcounty, vector<Arc>&countyArcs, vector<Arc>&depotArcs, vector<Arc>&Depot_c_cost, 
	vector<Arc>&CountyInfo, map<int, Biomass>&map_biomass, map<int, map<int, map<int, map<int, int>>>>&county_production)
{
	int i,j,k=0;
	IloEnv env =model.getEnv();
	char *VarName;
	VarName=new char[200];
	
	for(i=0;i<allArcs.size();i++)
		{
			sprintf( VarName, "x[%d]",i);
			x[i].setName(VarName);
			
		}

	for (i = 0; i< allArcs.size(); i++)
	{
		sprintf(VarName, "z[%d]", i);
		z[i].setName(VarName);
	}
	
	for (i = 0; i < CountyInfo.size(); i++)
	{
		for (j = 0; j<Depot_c_cost.size(); j++)
		{
			sprintf(VarName, "w[%d][%d]", i, j);
			w[i][j].setName(VarName);
		}
	}

	delete [] VarName;

	cout<<" End of declaring printing variable  "<<endl;
	
	//constraint 2, 10,11 (supply constraints, carbo demand, ash req. and biomass demand constraint )
	
	IloExpr totalcarbo(env); //feedstock type 1
	IloExpr totalash(env);
	IloExpr totalfeedstock(env);
	for (int i = 0; i < allArcs.size(); i++)
	{

		if (allArcs[i].getSecArcType() == 0)
		{
			model.add(x[i] <= z[i] * allArcs[i].getProduction()); //constraint 1 
		}

		if (allArcs[i].getArcType() == 21) //three pass
		{
			totalcarbo += x[i] * map_biomass[1].ch_content;
			totalash += x[i] * map_biomass[1].ash_content;
			totalfeedstock += x[i] ;
		}
		if (allArcs[i].getArcType() == 22) //two pass stover
		{
			totalcarbo += x[i] * map_biomass[2].ch_content;
			totalash += x[i] * map_biomass[2].ash_content;
			totalfeedstock += x[i] ;
		}

		if (allArcs[i].getArcType() == 23) //switch grass
		{
			totalcarbo += x[i] * map_biomass[3].ch_content;
			totalash += x[i] * map_biomass[3].ash_content;
			totalfeedstock += x[i] ;
		}
		if (allArcs[i].getArcType() == 24) //miscanthus 
		{
			totalcarbo += x[i] * map_biomass[4].ch_content;
			totalash += x[i] * map_biomass[4].ash_content;
			totalfeedstock += x[i];
		}
		if (allArcs[i].getArcType() == 25) //grass clipping 
		{
			totalcarbo += x[i] *  map_biomass[5].ch_content;
			totalash += x[i] * map_biomass[5].ash_content;
			totalfeedstock += x[i];

			model.add(x[i] <= msw_prd * (1 - map_biomass[5].dml));
		}
	}

	model.add(totalcarbo >= c* total_fed_demand); //constraint total ch demand
	model.add(totalfeedstock == total_fed_demand); //total biomass demand
	totalcarbo.end();
	totalash.end();
	totalfeedstock.end();


	
	//constraint 2 
	//this loop make has multiple set of constraints: 1)  select exactly one farmgate price 2)biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage 

	for (i = 0; i < CountyInfo.size(); i++)
	{
		IloExpr helper01(env); //feedstock type 1
		IloExpr helper02(env);//feedstock type 2
		IloExpr helper03(env); //feedstock type 3
		IloExpr helper04(env); //feedstock type 4
		IloExpr helper05(env); //feedstock type 5


		IloExpr helper01_(env); // track feedstock type 1 from harvestign site to field storage 
		IloExpr helper02_(env);// track feedstock type 2 from harvestign site to field storage 
		IloExpr helper03_(env); // track feedstock type 3 from harvestign site to field storage 
		IloExpr helper04_(env); ///track feedstock type 4 from harvestign site to field storage
		IloExpr helper05_(env); //track feedstock type 5 from harvestign site to field storage

		


		IloExpr helper11(env); //track arc from storage to depot for feedstock type 1
		IloExpr helper12(env); //track arc from storage to depot for feedstock type 2
		IloExpr helper13(env); //track arc from storage to depot for feedstock type 3
		IloExpr helper14(env); //track arc from storage to depot for feedstock type 4
		IloExpr helper15(env); //track arc from storage to depot for feedstock type 5

		for (j = 0; j < allArcs.size(); j++)
		{
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 1)
			{
				helper01 += z[j];
				
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 2)
			{
				helper02 += z[j];
				
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 3)
			{
				helper03 += z[j];
				
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 4)
			{
				helper04 += z[j];
			}


			//outgoing arc from harvest location to storage 
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 1)
			{
				
				helper01_ += (1 - map_biomass[1].dml_storage)*x[j];
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 2)
			{
				
				helper02_ += (1 - map_biomass[2].dml_storage)*x[j];
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 3)
			{
				
				helper03_ += (1 - map_biomass[3].dml_storage)*x[j];
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 4)
			{
				
				helper04_ += (1 - map_biomass[4].dml_storage)*x[j];
			}


			//incoming arc to depot 
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 11)
			{
				helper11 += x[j];
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 12)
			{
				helper12 += x[j];
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 13)
			{
				helper13 += x[j];
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && allArcs[j].getArcType() == 14)
			{
				helper14 += x[j];
			}
		}
		

		model.add(helper01 <= 1); //select exactly one farmgate price for 3P corn stover 
		model.add(helper02 <= 1); //select exactly one farmgate price for 2P corn stover 

		model.add(helper03 <= 1);//elect exactly one farmgate price for switchgrass 

		 
		model.add(helper01_-helper11==0); // biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage for 3P CS
		model.add(helper02_ - helper12 == 0); //biomass coming in field storage = sum of going out to different depot for 2P CS
		model.add(helper03_ - helper13 == 0);//biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage for SG
		model.add(helper04_ - helper14 == 0);//biomass coming in field storage from harvesting site = sum of going out to different depot from filed storage for MC



		helper01.end();
		helper02.end();
		helper03.end();
		helper04.end();
		helper05.end();


		helper01_.end();
		helper02_.end();
		helper03_.end();
		helper04_.end();
		helper05_.end();


		helper11.end();
		helper12.end();
		helper13.end();
		helper14.end();
		helper15.end();
	}

	///adding dml at conservation: 
	bool expfound1, expfound2;
	
	//flow based on each feedstock type 
	//this loop has multiple set of constrtains 1) total biomass flow constraitns at depot 2) individual biomass flow constraitns at depot ,3) depot capacity constaitns ,
	//4) depot utilization cosntraints  5) maximum number of depot can be located at a specific location. 

	for (i = 0; i < CountyInfo.size(); i++)
	{
		IloExpr helper2(env); // expression to incoming
		IloExpr helper21(env); // expression to incoming
		IloExpr helper22(env); // expression to incoming
		IloExpr helper23(env); // expression to incoming
		IloExpr helper24(env); // expression to incoming
		
		IloExpr helper3(env); // expression to outgoing
		IloExpr helper31(env); // expression to outgoing
		IloExpr helper32(env); // expression to outgoing
		IloExpr helper33(env); // expression to outgoing
		IloExpr helper34(env); // expression to outgoing


		IloExpr helper4(env); // expression to sum size of depot 
		IloExpr helper5(env); // expression to sum number  of depot at a location


		expfound1 = false;
		expfound2 = false;
		for (j = 0; j < allArcs.size(); j++)
		{
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 11)) //arc incoming to depot 

			{
				helper2 += x[j] * (1 - map_biomass[1].dml_preprocessing);
				helper21 += x[j] * (1 - map_biomass[1].dml_preprocessing);
				expfound1 = true;
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 12)) //arc incoming to depot 

			{
				helper2 += x[j] * (1 - map_biomass[2].dml_preprocessing);
				helper22 += x[j] * (1 - map_biomass[2].dml_preprocessing);
				expfound1 = true;
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 13)) //arc incoming to depot 

			{
				helper2 += x[j] * (1 - map_biomass[3].dml_preprocessing);
				helper23 += x[j] * (1 - map_biomass[3].dml_preprocessing);
				expfound1 = true;
			}
			if (allArcs[j].getEnd() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 14)) //arc incoming to depot 

			{
				helper2 += x[j] * (1 - map_biomass[4].dml_preprocessing);
				helper24 += x[j] * (1 - map_biomass[4].dml_preprocessing);
				expfound1 = true;
			}

			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 21)) //arc 11= truck arc from biorefinery to blending
			{
				helper3 += x[j];
				helper31 += x[j];
				expfound2 = true;
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 22)) //arc 11= truck arc from biorefinery to blending
			{
				helper3 += x[j];
				helper32 += x[j];
				expfound2 = true;
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 23)) //arc 11= truck arc from biorefinery to blending
			{
				helper3 += x[j];
				helper33 += x[j];
				expfound2 = true;
			}
			if (allArcs[j].getStart() == CountyInfo[i].getArcID() && (allArcs[j].getArcType() == 24)) //arc 11= truck arc from biorefinery to blending
			{
				helper3 += x[j];
				helper34 += x[j];
				expfound2 = true;
			}
		}
		if (expfound1 == true && expfound2 == true)
		{
			model.add(helper2 - helper3 == 0); //flow conservation constraints
			model.add(helper21 - helper31 == 0); //flow conservation constraints
			model.add(helper22 - helper32 == 0); //flow conservation constraints
			model.add(helper23 - helper33 == 0); //flow conservation constraints
			model.add(helper24 - helper34 == 0); //flow conservation constraints
		}



		for (k = 0; k < Depot_c_cost.size(); k++)
		{
			helper4 += Depot_c_cost[k].getProduction()*w[i][k]; 
			helper5 += w[i][k];
		}

		model.add(helper2 - helper4 <= 0); //constraints 7
		model.add(helper2 - capacity_utilization*helper4 >= 0); //constraints 8
		model.add(helper2 - max_capacity_utilization*helper4 <= 0); //constraints 8
		model.add(helper5 <= 1);  ////constraints 9
		 
		helper2.end();
		helper21.end();
		helper22.end();
		helper23.end(); 
		helper24.end();

		helper3.end();
		helper31.end();
		helper32.end();
		helper33.end();
		helper34.end();

		helper4.end();
		helper5.end();

	}

	map<int, map<int, map<int, map<int, int>>>>map_arcs;

	map<int, int>::iterator it1; // Please use it1 this way : it1 = map_arcs[t][l][it3->first][it2->first].begin()
	map<int, map<int, int>>::iterator it2;
	map<int, map<int, map<int, int>>>::iterator it3;

	map<int, map<int, map<int, map<int, int>>>>::iterator it4;

	map<int, map<int, map<int, map<int, map<int, int>>>>>::iterator it5; //please aware that it5=map_arcs.begin()

	for (i = 0; i < allArcs.size(); i++)
	{
		map_arcs[allArcs[i].getStart()][allArcs[i].getEnd()][allArcs[i].getArcType()][allArcs[i].getMid1()] = i; //maparcs[county][county][arc type][farm gate price ]allArcs[i].getMid1()=farmgate price 
	}
	
	map<int, int>::iterator it1_; // Please use it1 this way : it1 = map_arcs[t][l][it3->first][it2->first].begin()
	map<int, map<int, int>>::iterator it2_;
	map<int, map<int, map<int, int>>>::iterator it3_;

	map<int, map<int, map<int, map<int, int>>>>::iterator it4_;

	//contraints sum 2P and 3P limitations 

	for (i = 0; i < CountyInfo.size(); i++)
	{
		for (it1_ = map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].begin(); it1_ != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].end(); it1_++)
		{
			IloExpr helperx(env);
			if (map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].find(it1_->first) != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1].end())
			{
				helperx += x[map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1][it1_->first]];
			}
			if (map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2].find(it1_->first) != map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2].end())
			{
				helperx += x[map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][2][it1_->first]] / Two_pass_resource_factor;
			}
			model.add(helperx <= allArcs[map_arcs[CountyInfo[i].getArcID()][CountyInfo[i].getArcID()][1][it1_->first]].getProduction());
			helperx.end();
		}
	}

}

	