struct rbShapeFile {
	int open;
	SHPHandle fileHandle;
};

VALUE shapeClass;
VALUE OpenSHP(VALUE klass,VALUE filename);
VALUE CreateSHP(VALUE klass,VALUE filename,VALUE shapeType);
VALUE CloseSHP(VALUE klass,VALUE handle);
VALUE CreateObject(VALUE klass,VALUE handle,VALUE shapeType,VALUE numVertices,VALUE x,VALUE y,VALUE z);
void freeShapeFile(struct rbShapeFile *f);
int ShapeFromString(const char *cShapeType);