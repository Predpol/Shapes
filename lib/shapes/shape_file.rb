module Shapes
  class ShapeFile
    attr_accessor :handle, :dbf_handle, :fields
    
    def initialize(afile = nil,stype = nil)
      if afile and stype
        self.handle = ShapesLib::create_shape afile, stype.to_s
        self.dbf_handle = ShapesLib::create_dbf afile
      elsif afile
        self.handle = ShapesLib::open_shape afile
        self.dbf_handle = Shapeslib::open_dbf afile
      else
        return self # get out of the function if no handle was passed
      end
      
      self.fields = Hash.new
      
      # but if we attempted to create a handle, make sure
      # it actually returned something valid
      return self.handle unless self.handle
      return self.dbf_handle unless self.dbf_handle
    end
    
    def self.create(filename,shape_type,&block)
      inst = new filename, shape_type
      return inst unless inst
      
      inst.define(&block)
      
      inst.close
      return true
    end
    
    def self.open(filename,&block)
      inst = new filename
      return inst unless inst
      
      inst.define(&block)
      
      inst.close
      return true
    end
    
    def add_field(field_name,field_type,width = 255,decimals = 0)
      rv = ShapesLib::dbf_add_field self.dbf_handle, field_name.to_s, field_type.to_s, width, decimals
      self.fields[field_name] =  {
        name: field_name,
        type: field_type,
        width: width,
        decimals: decimals,
        field_number: rv
      }
      return rv
    end
    
    def close
      ShapesLib::close_shape self.handle
      ShapesLib::close_dbf self.dbf_handle
    end
    
    def define(&block)
      instance_eval(&block)
    end
    
    def save_attribute(key,value,row)
      return false if self.fields[key].nil?
      
      d = self.dbf_handle
      field_info = self.fields[key]
      field_name = key.to_s
      field_type = field_info[:type]
      field_number = field_info[:field_number]
      width = field_info[:width]
      decimals = field_info[:decimals]
      
      saved = case field_type
      when :string then ShapesLib::dbf_write_string d, row, field_number, value.to_s
      when :integer then ShapesLib::dbf_write_integer d, row, field_number, value.to_i
      when :double then ShapesLib::dbf_write_double d, row, field_number, value.to_f
      end
      
      return saved
    end
    
    def point(p)
      i = ShapesLib::create_object self.handle, 'point', 1, [p.longitude], [p.latitude], [p.altitude]
      attrs = p.attributes
      
      attrs.each {|key,val| save_attribute(key,val,i) }
      return i
    end
    
    def polygon(p)
      i = ShapesLib::create_object self.handle, 'polygon', p.num_vertices, p.xs, p.ys, p.zs
      attrs = p.attributes
      
      attrs.each {|key,val| save_attribute(key,val,i) }
      return i
    end
    
    def arc(a)
      i = ShapesLib::create_object self.handle, 'arc', a.num_vertices, a.xs, p.ys, p.zs
      attrs = a.attributes
      
      attrs.each {|key,val| save_attribute(key,val,i)}
      return i
    end
    
    def multipoint(mp)
      i = ShapesLib::create_object self.handle, 'multipoint', mp.num_vertices, mp.xs, mp.ys, mp.zs
      attrs = mp.attributes
      
      attrs.each {|key,val| save_attribute(key,val,i) }
      return i
    end
  end
end