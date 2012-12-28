#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include "shapelib/shapefil.h"

void Init_shapeslib() {
	printf("nice\n");
	
	SHPHandle sf = SHPCreate("nig",SHPT_POLYGON);
	
	double y[5] = {47.24737275423729,47.24737275423729,47.246029627118645,47.246029627118645,47.24737275423729};
	double x[5] = {-122.5539122142857,-122.55194424107142,-122.55194424107142,-122.5539122142857,-122.5539122142857};
	double z[5] = {30,30,30,30,30};
	
	SHPObject *obj = SHPCreateSimpleObject(SHPT_POLYGON,5,x,y,z);
	SHPWriteObject(sf,-1,obj);
	
	SHPClose(sf);
	
	printf("on!\n");
}