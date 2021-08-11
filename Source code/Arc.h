
//using namespace std;
#include<string>
#include<list>
using namespace std;
class Arc
{
public:
	int ArcID;
	int start;
	int mid1;
	int end;
	float ArcCost;
	float SecArcCost;
	int ArcType;
	int sec_type;
	string arcOwner;
	list<int>path;
	float lat;
	float lng;
	
	string textId;

	float growerpayment;
	float production;

	float yield;

    
	void loadStart(int a);
	void loadMid1(int a);
	void loadEnd(int a);
	void loadArcCost(float a);

	void loadSecArcCost(float a);

	void loadArcID(int ID);
	void loadArcType(int Type);
	void loadSecArcType(int Type);
	void loadarcOwner(string arcO);
	void loadLatitude(float a);
	void loadLongitude(float a);
	void loadTextId(string txt);

	void loadGroweryPayment(float gp );

	void loadProduction(float pr);

	void loadYield(float yl);
		
	int getStart();
	int getMid1();
	int getEnd();
	float getArcCost();

	float getSecArcCost();


	int getArcID();
	int getArcType();
	int getSecArcType();

	string getarcOwner();
	float getLatitude();
	float getLongitude();
	string getTextId();

	float getGrowerPayment();

	float getProduction();

	float getYield();
};


void Arc::loadStart(int st)
{
	start = st;
}

void Arc::loadMid1(int md)
{
	mid1 = md;
}

void Arc::loadEnd(int en)
{
	end = en;
}

void Arc::loadArcCost(float cap)
{
	ArcCost = cap;
}

void Arc::loadSecArcCost(float cap)
{
	SecArcCost = cap;
}

void Arc:: loadArcID( int aID)
{ 
	ArcID = aID;
}
void Arc::loadSecArcType(int Type)
{ 
	sec_type = Type;
}

void Arc::loadArcType(int Type)
{
	ArcType = Type;
}

void Arc:: loadarcOwner(string arcOw)
{ 
	arcOwner = arcOw;
}
void Arc::loadLatitude(float en)
{
	lat = en;
}

void Arc::loadLongitude(float cap)
{
	lng = cap;
}

void Arc::loadTextId(string txt1)
{
	textId = txt1;
}

void Arc::loadGroweryPayment(float gp)
{
	growerpayment = gp;
}

void Arc:: loadProduction(float pr)
{
	production = pr;
}

void Arc::loadYield(float yl)
{
	yield = yl;
}


int Arc::getStart()
{
	return start;
}
int Arc::getMid1()
{
	return mid1;
}


int Arc::getEnd()
{
	return end;
}

float Arc::getArcCost()
{
	return ArcCost;
}


float Arc::getSecArcCost()
{
	return SecArcCost;
}


int Arc::getArcID()
{
	return ArcID;
}
int Arc::getArcType()
{
	return ArcType;
}

int Arc::getSecArcType()
{
	return sec_type;
}

string Arc::getarcOwner()
{
	return arcOwner;
}
float Arc::getLatitude()
{
	return lat;
}

float Arc::getLongitude()
{
	return lng;
}

string Arc::getTextId()
{
	return textId;
}

float Arc::getGrowerPayment()
{
	return growerpayment;
}
float Arc::getProduction()
{
	return production;
}

float Arc::getYield()
{
	return yield;
}



