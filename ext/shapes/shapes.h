#define TRUE		1
#define	FALSE		0

struct rbShapeFile {
	int open;
	SHPHandle fileHandle;
};

struct rbDBF {
	int open;
	DBFHandle fileHandle;
};

VALUE shapeClass;
VALUE dbfClass;

// ShapeFile IO
VALUE OpenSHP(VALUE klass,VALUE filename);
VALUE CreateSHP(VALUE klass,VALUE filename,VALUE shapeType);
VALUE CloseSHP(VALUE klass,VALUE handle);

//ShapeFile manipulation
VALUE CreateObject(VALUE klass,VALUE handle,VALUE shapeType,VALUE numVertices,VALUE x,VALUE y,VALUE z);

//DBF IO
VALUE OpenDBF(VALUE klass,VALUE filename);
VALUE CreateDBF(VALUE klass,VALUE filename);
VALUE CloseDBF(VALUE klass,VALUE handle);

//DBF Manipulation

VALUE RDBFFieldCount(VALUE klass,VALUE handle);
VALUE RDBFRecordCount(VALUE klass,VALUE handle);
VALUE RDBFFieldIndex(VALUE klass,VALUE handle,VALUE fieldName);
VALUE RDBFGetFieldType(VALUE klass,VALUE handle,VALUE fieldIndex);
VALUE RDBFAddField(VALUE klass,VALUE handle,VALUE fieldName,VALUE fieldType,VALUE width,VALUE decimals);

// DBF read / write
VALUE RDBFReadInt(VALUE klass,VALUE handle,VALUE row,VALUE field);
VALUE RDBFReadDouble(VALUE klass,VALUE handle,VALUE row,VALUE field);
VALUE RDBFReadString(VALUE klass,VALUE handle,VALUE row,VALUE field);
VALUE RDBFIsAttributeNull(VALUE klass,VALUE handle,VALUE row,VALUE field);
VALUE RDBFWriteInt(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val);
VALUE RDBFWriteDouble(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val);
VALUE RDBFWriteString(VALUE klass,VALUE handle,VALUE row,VALUE field,VALUE val);
VALUE RDBFWriteNull(VALUE klass,VALUE handle,VALUE row,VALUE field);
VALUE RDBFIsRecordDeleted(VALUE klass,VALUE handle,VALUE row);
VALUE RDBFDeleteRecord(VALUE klass,VALUE handle,VALUE row,VALUE val);
	
void freeShapeFile(struct rbShapeFile *f);
void freeDBF(struct rbDBF *f);
int ShapeFromString(const char *cShapeType);
DBFFieldType FieldTypeFromString(const char *cFieldType);