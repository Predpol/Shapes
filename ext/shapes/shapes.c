#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include "shapefil.h"
#include "shapes.h"

VALUE shapeClass;
	
void Init_shapeslib() {
	VALUE shapesModule = rb_define_module("ShapesLib");
	shapeClass = rb_define_class_under(shapesModule,"ShapeFile",rb_cObject);
	rb_define_module_function(shapesModule,"open_shape",OpenSHP,2);
}

VALUE OpenSHP(VALUE klass,VALUE filename) {
	VALUE rubyObject;
	struct rbShapeFile *openedFile;
	rubyObject = Data_Make_Struct(shapeClass,struct rbShapeFile,NULL,freeShapeFile,openedFile);
	
	char *cFileName = RSTRING(filename)->ptr;
	openedFile->fileHandle = SHPOpen(cFileName,"rb+");
	
	rb_obj_call_init(rubyObject,NULL,NULL);
	return rubyObject;
}

