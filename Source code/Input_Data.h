

   float winding_factor = 1.2; //reference http://www.ncbi.nlm.nih.gov/pmc/articles/PMC3835347/
  
/********************************************** Model input parameter **************************************///
	//Cost information 
	
	
	float DVC_truck = .113; //regression equation from BLM
	float DFC_truck = 3.37; //regression equation from BLM
	
	
	float DVC_truck_pellet = 0.0836; //regression equation from BLM
	float DFC_truck_pellet = 0.65;//regression equation from BLM
	 

	

	
	////* feedstock characteristics : 

	//carbohydrate content in delivered feedstock 
	float c1 = .574; //three pass
	float c2 = .603; //two pass
	float c3 = .666; //switch grass
	float c4 = .817; //miscanthus
	float c5 = .287; //grass clipping
	float c = .59;

	//ash fractions in delivered feedstock 
	float a1 = .122;
	float a2 = .076;
	float a3 = .064;
	float a4 = .025;
	float a5 = .131;
	float a = .05;

	//dry matter loss
	float d1 = .138;
	float d2 = .138;
	float d3 = .098;
	float d4 = .098;
	float d5 = .025;
	
	//other logistics cost
	float f1_other = 48.65;
	float f2_other = 53.59;
	float f3_other = 41.17;
	float f4_other = 47.33; //need to figure out later 
	float f5_other = 21.79;

	double total_fed_demand = 800000;
	
	double fixed_depot_construction_cost = 2.66;

	//ash disposal cost

	float beta = 37.16; //ash disposal cost

	float capacity_utilization = .9;

	float max_capacity_utilization = .95;

	float Two_pass_resource_factor = .487;


	//float base_lat = 39.349445, base_long = -99.880952; //grahama county locaiton
	//int  base_loc = 20165;


	float base_lat = 39.35054, base_long = -100.441; //sheridan county locaiton
	int  base_loc= 20179;

	int base_capacity = 800000;

	float ash_disposal_cost = 37.16;


	int msw_prd =32069;
