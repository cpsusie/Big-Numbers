// mercator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*

. The radius at the equator is 6378 km; the radius at the poles is 6357 km

  Mercator projection - se http://www.ualberta.ca/~norris/navigation/Mercator.html

L = latitude in radians (positive north)
Lo = longitude in radians (positive east)
E = easting (meters)
N = northing (meters)

For the sphere 

E = r Lo
N = r ln [ tan (pi/4 + L/2) ]

where 

r = radius of the sphere (meters)
ln() is the natural logarithm

For the ellipsoid 


E = a Lo
N = a * ln ( tan (pi/4 + L/2) * ( (1 - e * sin (L)) / (1 + e * sin (L))) ** (e/2)  )

a = the length of the semi-major axis of the ellipsoid (meters)
e = the first eccentricity of the ellipsoid

Note: the equation for northing does not have a closed form inverse.
The inverse can be calculated by iteration; it converges rapidly.


*/


int main(int argc, char* argv[])
{
	return 0;
}
