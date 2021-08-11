

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
//#include "county.h"
using namespace std;

typedef IloArray<IloIntVarArray> Arcs;
typedef map<int,  int > RefJob1;//to keep track the number of job created at each state according to biorefinerh size
typedef map<string, RefJob1 > RefJob2;
typedef map<int,RefJob1 > Zmap;

void createMainModel(IloModel model, IloNumVarArray x, IloNumVarArray z, vector<Arc> &allArcs, vector<Arc>&CountyInfo);


//void print_result(IloCplex cplex, IloNumVarArray x, IloNumVarArray y, vector<Arc> &allArcs, vector<county>&CountyInfo, ofstream &output);


void CPLEXProgram2(vector<Arc> &allArcs, map<int,Arc> &mapcounty)
{
	
	IloEnv env;
	try {
		
		int i,j,k; 
		
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
		IloNumVarArray z(env, allArcs.size(), 0, 1, ILOINT);
		

		
	//*************** Writing Economic objective *******************************************
		 //need update cost in the arc:
		IloExpr obj(env);
		IloExpr rhs(env);

		
	
		//  minimization  problem
		for (j = 0; j < allArcs.size(); j++)
		if (allArcs[j].getArcType() == 1 || allArcs[j].getArcType() == 2 || allArcs[j].getArcType() == 3 || allArcs[j].getArcType() == 4 )   //transportation cost corn stover and switchgrass fixed ton =3.62, distance variable= .134;
		{
			obj += x[j] * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + allArcs[j].getLongitude());
		}
		else
		{
			obj += x[j] * (allArcs[j].getSecArcType() + (DFC_truck_pellet + allArcs[j].getArcCost()* DVC_truck_pellet) + allArcs[j].getLongitude());
		}
		

		
		createMainModel(model, x, z, allArcs, CountyInfo);
	
		
		cout<< "End of creating main model "<<endl;

        //////*****************end of creating basic model****************************************************

		
		
		IloObjective objective (env);
		
		objective=IloAdd(model,IloMinimize(env, obj));
		
		IloCplex cplex(model);

		cplex.exportModel("trade model.lp");
		
		//cplex.setParam(IloCplex::EpGap, .01);

		cplex.solve();
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
		{
			output_2 << "modify model and generate result with penalty cost" << endl;
			exit;
		}
		
		//print_result(cplex, x, y, allArcs, CountyInfo, output_2);

		//output_2 << "cplex.getObjValue()   " << cplex.getObjValue() << endl;
		for (j = 0; j < allArcs.size(); j++) 
		 if (cplex.getValue(x[j])>.01)
		 {
			 output_2 << j << "  " << allArcs[j].getArcID()<<"   "<< allArcs[j].getArcType() << "  " << cplex.getValue(x[j]) << "  " << allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + allArcs[j].getLongitude() << endl;
		 }

		 double total_cost = 0, total_biomass = 0, delivered_feedstock_cost = 0;

		 for (j = 0; j < allArcs.size(); j++)
		 if (cplex.getValue(x[j])>.01)
		 {
			 total_biomass = total_biomass + cplex.getValue(x[j]);
			 total_cost = total_cost + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) + allArcs[j].getLongitude());
		 }
		
		 output_2 << "delivered cost" << total_cost/total_biomass ;

		 vector<Arc>CountyInfo1;
		 vector<Arc>CountyInfo2;
		 vector<Arc>CountyInfo3;
		 vector<Arc>CountyInfo4;
		 vector<Arc>CountyInfo5;


		 //creating blending table 
		 double fed1 = 0, fed2 = 0, fed3 = 0, fed4 = 0, fed5 = 0;
		 double del_cost1 = 0, del_cost2 = 0, del_cost3 = 0, del_cost4 = 0, del_cost5 = 0;
		 double g1 = 0, g2 = 0, g3 = 0, g4 = 0, g5 = 0;
		 double t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0; //transportation cost
		 double d1 = 0, d2 = 0, d3 = 0, d4 = 0, d5 = 0; //distance * ton



		 for (int j = 0; j < allArcs.size(); j++)
		 if (cplex.getValue(x[j])>.01)
		 {
			 if (allArcs[j].getArcType() == 1)
			 {
				 fed1 = fed1 + cplex.getValue(x[j]);
				 del_cost1 = del_cost1 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) );
				 g1 = g1 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() );
				 t1 = t1 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				 d1 = d1 + cplex.getValue(x[j]) * allArcs[j].getArcCost(); //weighted distance 

			 }
			 if (allArcs[j].getArcType() == 2)
			 {
				 fed2 = fed2 + cplex.getValue(x[j]);
				 del_cost2 = del_cost2 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) );
				 g2 = g2 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType());
				 t2 = t2 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				 d2 = d2 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
				
			 }

			 if (allArcs[j].getArcType() == 3)
			 {
				 fed3 = fed3 + cplex.getValue(x[j]);
				 del_cost3 = del_cost3 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) );
				 

				 g3 = g3 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType());
				 t3 = t3 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));

				 d3 = d3 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
			 }
			/* if (allArcs[j].getArcType() == 4)
			 {
				 fed4 = fed4 + cplex.getValue(x[j]);
				 del_cost4 = del_cost4 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck) );
				 

				 g4 = g4 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType());
				 t4 = t4 + cplex.getValue(x[j]) * ((DFC_truck + allArcs[j].getArcCost()* DVC_truck));
			 }*/
			 if (allArcs[j].getArcType() == 5)
			 {
				 fed5 = fed5 + cplex.getValue(x[j]);
				 //del_cost5 = del_cost5 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck_pellet + allArcs[j].getArcCost()* DVC_truck_pellet));
				 del_cost5 = del_cost5 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck));

				 g5 = g5 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType());
				 t5 = t5 + cplex.getValue(x[j]) * ((DFC_truck_pellet + allArcs[j].getArcCost()* DVC_truck_pellet));

				 d5 = d5 + cplex.getValue(x[j]) * allArcs[j].getArcCost();
			 }
		 }
		 output_2 << "main resutls " << endl;

		 output_2 << fed1 << "  " << del_cost1 / fed1 + f1_other << "  " << c1 << "  " << a1 << endl;
		 output_2 << fed2 << "  " << del_cost2 / fed2 + f2_other << "  " << c2 << "  " << a2 << endl;
		 output_2 << fed3 << "  " << del_cost3 / fed3 + f3_other << "  " << c3 << "  " << a3 << endl;
		 output_2 << fed4 << "  " << del_cost4 / fed4 + f4_other << "  " << c4 << "  " << a4 << endl;
		 output_2 << fed5 << "  " << del_cost5 / fed5 + f5_other << "  " << c5 << "  " << a5 << endl;


		 
		 
		 
		 output_2 << "blended table " << endl;
		 output_2 << fed1<<"  "<< g1 / fed1 << "   " << t1 / fed1 << "  " << f1_other << "  " << del_cost1 / fed1 + f1_other << "  " << c1 << "  " << a1 << "  " << d1 / fed1 << endl;
		 output_2 << fed2 << "  " << g2 / fed2 << "   " << t2 / fed2 << "  " << f2_other << "  " << del_cost2 / fed2 + f2_other << "  " << c2 << "  " << a2 << "  " << d2 / fed2 << endl;
		 output_2 << fed3 << "  " << g3 / fed3 << "   " << t3 / fed3 << "  " << f3_other << "  " << del_cost3 / fed3 + f3_other << "  " << c3 << "  " << a3 << "  " << d3 / fed3 << endl;
		// output_2 << 4 << "  " << fed4 << "  " << del_cost4 / fed4 + f4_other << "  " << c4 << "  " << a4 << endl;
		 output_2 << 0 << "  " << 0 << "  " <<0 << "  " <<0 << endl;
		 output_2 << fed5 << "  " << g5 / fed5 << "   " << t5 / fed5 << "  " << f5_other << "  " << del_cost5 / fed5 + f5_other << "  " << c5 << "  " << a5 << "  " << d5 / fed5 << endl;
		//output_2 << 5 << "  " << 0 << "  " << 0 << "  " << 0 << "  " << 0 << endl;

		 //printing county supply and used supply:
		
		/* for (i = 0; i < CountyInfo1.size(); i++)
		 {
			 output_2 << "corn stover " << CountyInfo1[i].getArcID() << "  " << CountyInfo1[i].getStart() << "   " << CountyInfo1[i].getEnd() << "  " << CountyInfo1[i].getSecArcType() << "  " << CountyInfo1[i].getArcCost() << endl;
		 }*/

		 for (int j = 0; j < allArcs.size(); j++)
		 if (cplex.getValue(x[j])>.01)
		 {
			 if (allArcs[j].getArcType() == 1)
			 {
				
				 output_2 <<"3PCS" << "  " << allArcs[j].getArcID() << "   " << allArcs[j].getStart() << "  " << cplex.getValue(x[j]) << "  " << allArcs[j].getSecArcType() << "  " << allArcs[j].getArcCost() << endl;

			 }
			 if (allArcs[j].getArcType() == 2)
			 {
				 output_2 <<"2PCS" << "  " << allArcs[j].getArcID() << "   " << allArcs[j].getStart() << "  " << cplex.getValue(x[j]) << "  " << allArcs[j].getSecArcType() << "  " << allArcs[j].getArcCost() << endl;

			 }

			 if (allArcs[j].getArcType() == 3)
			 {
				 output_2 <<"SG" << "  " << allArcs[j].getArcID() << "   " << allArcs[j].getStart() << "  " << cplex.getValue(x[j]) << "  " << allArcs[j].getSecArcType() << "  " << allArcs[j].getArcCost() << endl;

			 }
			 /*
			 if (allArcs[j].getArcType() == 4)
			 {
				 fed4 = fed4 + cplex.getValue(x[j]);
				 del_cost4 = del_cost4 + cplex.getValue(x[j]) * (allArcs[j].getSecArcType() + (DFC_truck + allArcs[j].getArcCost()* DVC_truck));
				 CountyInfo1[i] = allArcs[i];
			 }*/
			 if (allArcs[j].getArcType() == 5)
			 {
				 output_2 << "GC " << "  " << allArcs[j].getArcID() << "   " << allArcs[j].getStart() << "  " << cplex.getValue(x[j]) << "  " << allArcs[j].getSecArcType() << "  " << allArcs[j].getArcCost() << endl;

			 }
		 }


	} 

	catch(IloException& e) {
	cerr  << " ERROR: " << e << endl;   
	}
	catch(...) {
	cerr  << " ERROR" << endl;   
	}

	env.end();
}








void createMainModel(IloModel model, IloNumVarArray x, IloNumVarArray z, vector<Arc> &allArcs, vector<Arc>&CountyInfo)
{
	int i,j,k=0;
	IloEnv env =model.getEnv();
	
	
	char *VarName;
	VarName=new char[200];
	
	for(i=0;i<allArcs.size();i++)
		{
			sprintf( VarName, "x[%d]",i);
			x[i].setName(VarName);
			sprintf(VarName, "z[%d]", i);
			z[i].setName(VarName);
		}
		 
	delete [] VarName;

	
	cout<<" End of declaring printing variable  "<<endl;
	
	//constraint 1 (supply constraints)
	for (int i = 0; i < allArcs.size(); i++)
	{
		model.add(x[i] <= z[i] * allArcs[i].getStart());
	}
	//constraint 2 (supply curve constraints)
	
	for (i = 0; i < CountyInfo.size(); i++)
	{
		IloExpr helper1(env); //feedstock type 1
		IloExpr helper2(env);//feedstock type 2
		IloExpr helper3(env); //feedstock type 3
		IloExpr helper4(env); ///feedstock type 4
		IloExpr helper5(env); //feedstock type 5

		for (j = 0; j < allArcs.size(); j++)
		if (allArcs[j].getArcID() == CountyInfo[i].getArcID())
		{
			if ( allArcs[j].getArcType() == 1) //sum arcs having different price for same feedstock
			{
				helper1 += z[j];
			}
			if (allArcs[j].getArcType() == 2)
			{
				helper2 += z[j];
			}
			if (allArcs[j].getArcType() == 3)
			{
				helper3 += z[j];
			}
			if (allArcs[j].getArcType() == 4)
			{
				helper4 += z[j];
			}
			if (allArcs[j].getArcType() == 5)
			{
				helper5 += z[j];
			}
		}
		
		model.add(helper1 <= 1);
		model.add(helper2 <= 1);
		
		model.add(helper3 <= 1);//switchgrass 
		
		//model.add(helper4 <= 1); //miscanthus
		model.add(helper5 <= 1); //grass clipping 
		
		
		helper1.end();
		helper2.end();
		helper3.end();
		helper4.end();
		helper5.end();
	}
	
	//constraint 3 and 4 : carbohydrate and ash  constraints
	
	IloExpr totalcarbo(env); //feedstock type 1
	IloExpr totalash(env);
	IloExpr totalfeedstock(env);
	for (int i = 0; i < allArcs.size(); i++)
	{
		if (allArcs[i].getArcType() == 1) //three pass
		{
			totalcarbo += x[i] * c1*(1-d1);
			totalash += x[i] * a1*(1 - d1);
			totalfeedstock += x[i] * (1 - d1);
		}
		if (allArcs[i].getArcType() == 2) //two pass stover
		{
			totalcarbo += x[i] * c2*(1-d2);
			totalash += x[i] * a2*(1 - d2);
			totalfeedstock += x[i] * (1 - d2);
		}

		if (allArcs[i].getArcType() == 3) //switch grass
		{
			totalcarbo += x[i] * c3*(1-d3);
			totalash += x[i] * a3*(1 - d3);
			totalfeedstock += x[i] * (1 - d3);
		}
		/*if (allArcs[i].getArcType() == 4) //miscanthus 
		{
			totalcarbo += x[i] * c4*(1-d4);
			totalash += x[i] * a4*(1 - d4);
			totalfeedstock += x[i] * (1 - d4);
		}*/
		if (allArcs[i].getArcType() == 5) //grass clipping 
		{
			totalcarbo += x[i] * c5*(1-d5);
			totalash += x[i] * a5*(1 - d5);
			totalfeedstock += x[i] * (1 - d5);
		}
	}

	model.add(totalcarbo >= c* total_fed_demand);
	//model.add(totalash <= a* total_fed_demand);
	model.add(totalfeedstock == total_fed_demand);
	totalcarbo.end();
	totalash.end();
	totalfeedstock.end();
	//model.add(x[518] >= 24000);
	

	cout << " end of loading  constraitns 3 and 4 " << endl;



}

	