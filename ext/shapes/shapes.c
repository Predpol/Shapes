#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include "shapefil.h"
#include "shapes.h"
void Init_shapeslib() {
	VALUE shapesModule = rb_define_module("ShapesLib");
	rb_define_module_function(shapesModule,"open_shp",OpenSHP,2);
}

VALUE OpenSHP(VALUE klass,VALUE filename) {
	
}

