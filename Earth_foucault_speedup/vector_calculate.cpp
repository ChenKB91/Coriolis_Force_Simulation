#include <iostream>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>
#include "communicate.h"
using namespace std;
#define pi 3.14159265358979323846L

bool trywrite(char write[])
{
	if(write_to_client(write))
		return true;
	else
	{
		close();
		cout<<"server error "<<GetLastError()<<endl;
		return false;
	}
}

bool tryread(char read[])
{
	if(read_from_client(read))
		return true;
	else
	{
		close();
		cout<<"client error "<<GetLastError()<<endl;
		return false;
	}
}

class vec{
	public:
	long double x;
	long double y;
	long double z;
	
	vec():x(0), y(0), z(0)
	{}
	vec(long double a, long double b, long double c):x(a), y(b), z(c)
	{}
};

inline vec operator+(vec v1, vec v2)
{
	return vec(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline vec operator-(vec v1, vec v2)
{
	return vec(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

inline vec operator*(vec v, const long double& n)
{
	return vec(v.x*n, v.y*n, v.z*n);
}

inline vec operator/(vec v, const long double& n)
{
	return vec(v.x/n, v.y/n, v.z/n);
}

inline vec cross(vec v1, vec v2)
{
	return vec((v1.y*v2.z - v1.z*v2.y), (v1.z*v2.x - v1.x*v2.z), (v1.x*v2.y - v1.y*v2.x));
}

inline vec dot(vec v1, vec v2)
{
	return vec((v1.x*v2.x), (v1.y*v2.y), (v1.z*v2.z));
}

inline long double abs(vec v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline vec norm(vec v)
{
	return v/abs(v);
}

inline vec gravity(vec v)
{
	return norm(v)*-5158707301747.944L/(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline vec springForce(vec axis, long double k, long double Len)
{
	return norm(axis) * -k * (abs(axis) - Len);
}

int main()
{
	create();
	char mess[500] = {0};
	stringstream ss;
	long double dt = 0, m = 0, k = 0, Len = 0, posx = 0, posy = 0, posz = 0;
	vec w, ballpos, ballv, balla, stickpos;
	vec f_ballpos, f_ballv, f_balla, f_stickpos;
	
	tryread(mess);
	ss<<mess;
	ss>>mess;
	if(!strcmp(mess, "init"))
		ss>>m>>k>>Len;
	else
		return 1;
	ss.str("");ss.clear();
	
	tryread(mess);
	ss<<mess;
	ss>>mess;
	if(!strcmp(mess, "start"))
	{
		ss>>posy>>dt;
		w = vec(0, posy, 0);
		ss>>posx>>posy>>posz;
		ballpos = vec(posx, posy, posz);
		ss>>posx>>posy>>posz;
		f_ballpos = vec(posx, posy, posz);
		ballv = (f_ballpos-ballpos)/dt;
	}
	else
		return 1;
	ss.str("");ss.clear();
	
	ss<<scientific<<setprecision(18);
	int count = 0;
	dt = 0.000001L;
	while(true)
	{
		if(!tryread(mess))	break;
		ss<<mess;
		ss>>mess;
		if(strcmp(mess, "c"))
			continue;
		ss>>count;
		ss>>posx>>posy>>posz;
		ballpos = vec(posx, posy, posz);
		ss>>posx>>posy>>posz;
		stickpos = vec(posx, posy, posz);
		ss>>posx>>posy>>posz;
		f_ballpos = vec(posx, posy, posz);
		ss>>posx>>posy>>posz;
		f_stickpos = vec(posx, posy, posz);
		ss.str("");ss.clear();
		
		for(int i = 0 ; i < 1000 ; i++)
		{
			balla = gravity(ballpos) + springForce((ballpos-stickpos), k, Len) / m;
			f_balla = gravity(f_ballpos) + springForce((f_ballpos-f_stickpos), k, Len) / m 
						- cross(w, f_ballv)*2 - cross(w, cross(w, f_ballpos));
			ballv = ballv + balla * dt;
			f_ballv = f_ballv + f_balla * dt;
			ballpos = ballpos + ballv * dt;
			f_ballpos = f_ballpos + f_ballv * dt;
			stickpos = stickpos + cross(w, stickpos)*dt;
		}
		count++;
		ss<<'c'<<'$'<<count<<'$'<<ballpos.x<<'$'<<ballpos.y<<'$'<<ballpos.z<<'$'
			<<f_ballpos.x<<'$'<<f_ballpos.y<<'$'<<f_ballpos.z<<'$';
		ss>>mess;
		while(!trywrite(mess));
		ss.str("");ss.clear();
	}
	close();
}
