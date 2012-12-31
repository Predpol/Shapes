#include <ruby.h>
#include <stdio.h>
#include <stdlib.h>
#include "shapefil.h"
#include "shapes.h"

void Init_shapeslib() {
	VALUE shapesModule = rb_define_module("ShapesLib"); //Create ShapesLib module
	
	//Define some classes to store our C data in
	shapeClass = rb_define_class_under(shapesModule,"ShapeFile",rb_cObject);
	dbfClass = rb_define_class_under(shapesModule,"DBF",rb_cObject);
	
	// Bridge for ShapeFile IO
	rb_define_module_function(shapesModule,"open_shape",OpenSHP,1);
	rb_define_module_function(shapesModule,"close_shape",CloseSHP,1);
	rb_define_module_function(shapesModule,"create_shape",CreateSHP,2);
	
	// ShapeFile manipulation
	rb_define_module_function(shapesModule,"create_object",CreateObject,6);
	
	// DBF IO
	rb_define_module_function(shapesModule,"open_dbf",OpenDBF,1);
	rb_define_module_function(shapesModule,"create_dbf",CreateDBF,1);
	rb_define_module_function(shapesModule,"close_dbf",CloseDBF,1);
	
	// DBF manipulation
	rb_define_module_function(shapesModule,"dbf_field_count",RDBFFieldCount,1);
	rb_define_module_function(shapesModule,"dbf_record_count",RDBFRecordCount,1);
	rb_define_module_function(shapesModule,"dbf_field_index",RDBFFieldIndex,2);
	rb_define_module_function(shapesModule,"dbf_field_type",RDBFGetFieldType,2);
	rb_define_module_function(shapesModule,"dbf_add_field",RDBFAddField,5);
	
	//Attribute read / write
	rb_define_module_function(shapesModule,"dbf_read_int",RDBFReadInt,3);
	rb_define_module_function(shapesModule,"dbf_read_double",RDBFReadDouble,3);
	rb_define_module_function(shapesModule,"dbf_read_string",RDBFReadString,3);
	rb_define_module_function(shapesModule,"dbf_attribute_null?",RDBFIsAttributeNull,3);
	rb_define_module_function(shapesModule,"dbf_write_integer",RDBFWriteInt,4);
	rb_define_module_function(shapesModule,"dbf_write_double",RDBFWriteDouble,4);
	rb_define_module_function(shapesModule,"dbf_write_string",RDBFWriteString,4);
	rb_define_module_function(shapesModule,"dbf_write_null",RDBFWriteNull,3);
	rb_define_module_function(shapesModule,"dbf_record_deleted?",RDBFIsRecordDeleted,2);
	rb_define_module_function(shapesModule,"dbf_delete_record",RDBFDeleteRecord,3);
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

//DBF IO
VALUE OpenDBF(VALUE klass,VALUE filename) {
	VALUE rubyObject;
	struct rbDBF *openedFile;
	rubyObject = Data_Make_Struct(dbfClass,struct rbDBF,NULL,freeDBF,openedFile);
	
	const char *cFileName = StringValueCStr(filename);
	openedFile->fileHandle = DBFOpen(cFileName,"rb+");
	openedFile->open = 1;
	
	rb_obj_call_init(rubyObject,0,0);
	return rubyObject;
}

VALUE CreateDBF(VALUE klass,VALUE filename) {
	VALUE rubyObject;
	struct rbDBF *createdFile;
	rubyObject = Data_Make_Struct(dbfClass,struct rbDBF,NULL,freeDBF,createdFile);
	
	const char *cFileName = StringValueCStr(filename);
	
	createdFile->fileHandle = DBFCreate(cFileName);
	createdFile->open = 1;
	
	rb_obj_call_init(rubyObject,0,0);
	return rubyObject;
}

VALUE CloseDBF(VALUE klass,VALUE handle) {
	struct rbDBF *dbfFile;
	Data_Get_Struct(handle,struct rbDBF,dbfFile);
	DBFHandle fileHandle = dbfFile->fileHandle;
	dbfFile->open = 0;
	DBFClose(fileHandle);
	return Qtrue;
}

//DBF Manipulation

VALUE RDBFFieldCount(VALUE klass,VALUE handle) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int fieldCount = DBFGetFieldCount(d);
	VALUE rv = INT2NUM(fieldCount);
	
	return rv;
}

VALUE RDBFRecordCount(VALUE klass,VALUE handle) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int recordCount = DBFGetRecordCount(d);
	VALUE rv = INT2NUM(recordCount);
	
	return rv;
}

VALUE RDBFFieldIndex(VALUE klass,VALUE handle,VALUE fieldName) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	const char *cFieldName = StringValueCStr(fieldName);
	
	int index = DBFGetFieldIndex(d,cFieldName);
	VALUE rv = INT2NUM(index);
	
	return rv;
}

VALUE RDBFGetFieldType(VALUE klass,VALUE handle,VALUE fieldIndex) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cFieldIndex = NUM2INT(fieldIndex);
	
	DBFFieldType fieldType = DBFGetFieldInfo(d,cFieldIndex,NULL,NULL,NULL);
	
	VALUE fieldSymbol;
	
	switch (fieldType) {
	case FTString:
		fieldSymbol = ID2SYM(rb_intern("string"));
		break;
	case FTInteger:
		fieldSymbol = ID2SYM(rb_intern("integer"));
		break;
	case FTDouble:
		fieldSymbol = ID2SYM(rb_intern("double"));
		break;
	case FTLogical:
		fieldSymbol = ID2SYM(rb_intern("logical"));
		break;
	case FTInvalid:
		fieldSymbol = ID2SYM(rb_intern("invalid"));
		break;
	}
	
	return fieldSymbol;
}

VALUE RDBFAddField(VALUE klass,VALUE handle,VALUE fieldName,VALUE fieldType,VALUE width,VALUE decimals) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	const char *cFieldName = StringValueCStr(fieldName);
	const char *cFieldType = StringValueCStr(fieldType);
	
	int cWidth    = NUM2INT(width);
	int cDecimals = NUM2INT(decimals);
	
	DBFFieldType ft = FieldTypeFromString(cFieldType);
	int fieldNumber = DBFAddField(d,cFieldName,ft,cWidth,cDecimals);
	VALUE rv = INT2NUM(fieldNumber);
	
	return rv;
}

VALUE RDBFReadInt(VALUE klass,VALUE handle,VALUE row,VALUE field) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	
	int r = DBFReadIntegerAttribute(d,cRow,cField);
	VALUE rv = INT2NUM(r);
	
	return rv;
}

VALUE RDBFReadDouble(VALUE klass,VALUE handle,VALUE row,VALUE field) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	
	double r = DBFReadDoubleAttribute(d,cRow,cField);
	VALUE rv = DOUBLE2NUM(r);
	
	return rv;
}

VALUE RDBFReadString(VALUE klass,VALUE handle,VALUE row,VALUE field) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	
	const char *r = DBFReadStringAttribute(d,cRow,cField);
	VALUE rv = rb_str_new2(r);
	
	return rv;
}

VALUE RDBFIsAttributeNull(VALUE klass,VALUE handle,VALUE row,VALUE field) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	
	VALUE rv = Qfalse;
	
	if (DBFIsAttributeNull(d,cRow,cField) == TRUE) {
		rv = Qtrue;
	}
	
	return rv;
}

VALUE RDBFWriteInt(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	int cVal = NUM2INT(val);
	
	VALUE rv;
	
	if (DBFWriteIntegerAttribute(d,cRow,cField,cVal) == TRUE) {
		rv = Qtrue;
	} else {
		rv = Qfalse;		
	}
	
	return rv;
}

VALUE RDBFWriteDouble(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	double cVal = NUM2DBL(val);
	
	VALUE rv;
	
	if (DBFWriteDoubleAttribute(d,cRow,cField,cVal) == TRUE) {
		rv = Qtrue;
	} else {
		rv = Qfalse;		
	}
	
	return rv;
}

VALUE RDBFWriteString(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	const char *cVal = StringValueCStr(val);
	
	VALUE rv;
	
	if (DBFWriteStringAttribute(d,cRow,cField,cVal) == TRUE) {
		rv = Qtrue;
	} else {
		rv = Qfalse;		
	}
	
	return rv;
}

VALUE RDBFWriteNull(VALUE klass,VALUE handle,VALUE row,VALUE field) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cField = NUM2INT(field);
	
	VALUE rv;
	
	if (DBFWriteNULLAttribute(d,cRow,cField) == TRUE) {
		rv = Qtrue;
	} else {
		rv = Qfalse;		
	}
	
	return rv;
}

VALUE RDBFIsRecordDeleted(VALUE klass,VALUE handle,VALUE row) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	
	VALUE rv;
	
	if (DBFIsRecordDeleted(d,cRow) == TRUE) {
		rv = Qtrue;
	} else {
		rv = Qfalse;
	}
	
	return rv;
}

VALUE RDBFDeleteRecord(VALUE klass,VALUE handle,VALUE row,VALUE val) {
	struct rbDBF *dbf;
	Data_Get_Struct(handle,struct rbDBF,dbf);
	DBFHandle d = dbf->fileHandle;
	
	int cRow = NUM2INT(row);
	int cVal  = NUM2INT(val);
	
	int deleted = DBFMarkRecordDeleted(d,cRow,cVal);
	
	VALUE rv;
	
	if (deleted == TRUE) {
		rv = Qtrue;	
	} else {
		rv = Qfalse;		
	}
	
	return rv;
}

void freeShapeFile(struct rbShapeFile *f) {
	if (f->open) {
		f->open = 0;
		SHPClose(f->fileHandle);	
	}
}

void freeDBF(struct rbDBF *f) {
	if (f->open) {
		f->open = 0;
		DBFClose(f->fileHandle);
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

DBFFieldType FieldTypeFromString(const char *cFieldType) {
	DBFFieldType fieldType;
	
	if (strcmp(cFieldType,"string") == 0) {
		fieldType = FTString;
	} else if (strcmp(cFieldType,"integer") == 0) {
		fieldType = FTInteger;
	} else if (strcmp(cFieldType,"double") == 0) {
		fieldType = FTDouble;		
	} else if (strcmp(cFieldType,"logical") == 0) {
		fieldType = FTLogical;
	} else {
		fieldType = FTInvalid;
	}
	
	return fieldType;
}