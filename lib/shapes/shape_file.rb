module Shapes
  class ShapeFile
    attr_accessor :handle
    
    def initialize(afile = nil,stype = nil)
      if afile and stype
        self.handle = ShapesLib::create_shape afile, stype.to_s
      elsif afile
        self.handle = ShapesLib::open_shape afile
      else
        return self # get out of the function if no handle was passed
      end
      
      # but if we attempted to create a handle, make sure
      # it actually returned something valid
      return self.handle unless self.handle
    end
    
    def self.create(filename,shape_type,&block)
      shape_handle = ShapesLib::create_shape filename, shape_type.to_s
      return shape_handle unless shape_handle
      
      inst = new
      inst.handle = shape_handle
      
      inst.define(&block)
      
      ShapesLib::close_shape shape_handle
    end
    
    def self.open(filename,&block)
      shape_handle = ShapesLib::open_shape filename
      return shape_handle unless shape_handle
      
      inst = new
      inst.handle = shape_handle
      
      inst.define(&block)
      
      ShapesLib::close_shape shape_handle
    end
    
    def define(&block)
      instance_eval(&block)
    end
    
    def point(p)
      ShapesLib::create_object self.handle, 'point', 1, [p.longitude], [p.latitude], [p.altitude]
    end
    
    def polygon(p)
      ShapesLib::create_object self.handle, 'polygon', p.num_vertices, p.xs, p.ys, p.zs
    end
    
    def arc(a)
      ShapesLib::create_object self.handle, 'arc', a.num_vertices, a.xs, p.ys, p.zs
    end
    
    def multipoint(mp)
      ShapesLib::create_object self.handle, 'multipoint', mp.num_vertices, mp.xs, mp.ys, mp.zs
    end
  end
end