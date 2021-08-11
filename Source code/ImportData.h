
#include <iostream>
#include <fstream>
#include<list>
#include<algorithm>
#include<vector>
#include<cmath>
#include <iterator>
#include <map>
#include<queue>
#include<string>
#include<numeric>

using namespace std;

bool MyDataSortPredicate(const Arc& k1, const Arc& k2)
{
  return  k1.ArcCost < k2.ArcCost;
}




void LoadArcs(vector<Arc> &allArcs, map<int,Arc> &county)
{
   
    int i, j,k,l;
    float thedistance = 0; 
	ifstream theFile;
	int st = 0, en = 0,  prd = 0;
	float dis = 0, yld = 0, ocost = 0, en2 = 0 ;

	theFile.open("input_data.txt");
	if (theFile.is_open())
	{
		while (theFile >> st >> en >> en2 >> prd >> yld >> dis >> ocost)
		{
			{
				Arc u;
				u.loadArcID(st);  //fips
				u.loadArcType(en); //feedstock type
				u.loadSecArcType(en2); //price at farmgate
				u.loadStart(prd); //feedstock prd
				u.loadLatitude(yld); //yield
			    u.loadArcCost(dis); //cost
				u.loadLongitude(ocost); //other logistics
				allArcs.push_back(u);
				county[st] = u;
			}
		}
	}
	theFile.close();

} 

