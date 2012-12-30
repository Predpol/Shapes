#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include "shapefil.h"
#include "shapes.h"

void Init_shapeslib() {
	VALUE shapesModule = rb_define_module("ShapesLib");
	shapeClass = rb_define_class_under(shapesModule,"ShapeFile",rb_cObject);
	rb_define_module_function(shapesModule,"open_shape",OpenSHP,1);
	rb_define_module_function(shapesModule,"close_shape",CloseSHP,1);
	rb_define_module_function(shapesModule,"create_shape",CreateSHP,2);
	rb_define_module_function(shapesModule,"create_object",CreateObject,6);
}

VALUE OpenSHP(VALUE klass,VALUE filename) {
	VALUE rubyObject;
	struct rbShapeFile *openedFile;
	rubyObject = Data_Make_Struct(shapeClass,struct rbShapeFile,NULL,freeShapeFile,openedFile);
	
	char *cFileName = StringValuePtr(filename);
	openedFile->fileHandle = SHPOpen(cFileName,"rb+");
	openedFile->open = 1;
	
	rb_obj_call_init(rubyObject,0,0);
	return rubyObject;
}

VALUE CreateSHP(VALUE klass,VALUE filename,VALUE shapeType) {
	VALUE rubyObject;
	struct rbShapeFile *createdFile;
	rubyObject = Data_Make_Struct(shapeClass,struct rbShapeFile,NULL,freeShapeFile,createdFile);
	
	const char *cFileName = StringValueCStr(filename);
	
	VALUE shapeTypeString = StringValue(shapeType);
	const char *cShapeType = StringValueCStr(shapeTypeString);
	
	int shape = ShapeFromString(cShapeType);
	
	if (shape == -1) {
		return Qfalse;
	}
	
	createdFile->fileHandle = SHPCreate(cFileName,shape);
	createdFile->open = 1;
	
	rb_obj_call_init(rubyObject,0,0);
	return rubyObject;
}

VALUE CreateObject(VALUE klass,VALUE handle,VALUE shapeType,VALUE numVertices,VALUE x,VALUE y,VALUE z) {
	struct rbShapeFile *shapeFile;
	Data_Get_Struct(handle,struct rbShapeFile,shapeFile);
	SHPHandle fileHandle = shapeFile->fileHandle;
	
	if (!shapeFile->open) {
		return Qfalse;
	}
	
	int xlen = RARRAY_LEN(x);
	int ylen = RARRAY_LEN(y);
	int zlen = RARRAY_LEN(z);
	
	
	int xylen = (xlen < ylen) ? xlen : ylen; //Get the lesser of the two
	int len = (zlen > 0 && zlen < xylen) ? zlen : xylen;
	
	const char *cShapeType = StringValueCStr(shapeType);
	int shape = ShapeFromString(cShapeType);
	
	if (shape == -1) {
		return Qfalse;
	}
	
	double *xs, *ys, *zs;
	xs = (double *)malloc(sizeof(double) * len);
	ys = (double *)malloc(sizeof(double) * len);
	zs = (double *)malloc(sizeof(double) * len);
	
	int i;
	for (i = 0;i < len;i++) {
		VALUE ax = rb_ary_pop(x);
		VALUE ay = rb_ary_pop(y);
		VALUE az = rb_ary_pop(z);
		
		xs[i] = NUM2DBL(ax);
		ys[i] = NUM2DBL(ay);
		
		if (az != Qnil) {
			zs[i] = az;
		}
	}
	
	if (zlen == 0) {
		zs = NULL;
	}
	
	SHPObject *newObject = SHPCreateSimpleObject(shape,len,xs,ys,zs);
	int objNum = SHPWriteObject(fileHandle,-1,newObject);
	SHPDestroyObject(newObject);
	return INT2NUM(objNum);
}

VALUE CloseSHP(VALUE klass,VALUE handle) {
	struct rbShapeFile *shapeFile;
	Data_Get_Struct(handle,struct rbShapeFile,shapeFile);
	SHPHandle fileHandle = shapeFile->fileHandle;
	shapeFile->open = 0;
	SHPClose(fileHandle);
	return Qtrue;
}

void freeShapeFile(struct rbShapeFile *f) {
	if (f->open) {
		SHPClose(f->fileHandle);	
	}
}

int ShapeFromString(const char *cShapeType) {
	int shape;
	
	if (strcmp(cShapeType,"point") == 0) {
		shape = SHPT_POINT;
	} else if (strcmp(cShapeType,"arc") == 0) {
		shape = SHPT_ARC;	
	} else if (strcmp(cShapeType,"polygon") == 0) {
		shape = SHPT_POLYGON;
	} else if (strcmp(cShapeType,"multipoint") == 0) {
		shape = SHPT_MULTIPOINT;
	} else {
		shape = -1;		
	}
	
	return shape;
}