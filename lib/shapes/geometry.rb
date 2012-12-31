module Shapes
  class GeoObject
    attr_accessor :attributes
    
    def initialize(*args)
      self.attributes = Hash.new
    end  
  end
  
  class Point < GeoObject
    attr_accessor :longitude, :latitude, :altitude
    
    def initialize(lat = nil,long = nil,alt = nil,attributes = {})
      super
      self.latitude   = lat
      self.longitude  = long
      self.altitude = alt
      self.attributes = attributes
    end
  end
  
  class Polygon < GeoObject
    attr_accessor :vertices, :num_vertices
    
    def initialize(vertices = nil,num_vertices = 4,attributes = {})
      super
      self.vertices = vertices if Array === vertices
      self.vertices ||= []
      self.num_vertices = num_vertices
      self.attributes = {}
    end
    
    def add_point(p)
      self.vertices << p
    end
    
    def <<(p)
      add_point p
    end
    
    def xs
      self.vertices.map {|p| p.longitude }
    end
    
    def ys
      self.vertices.map {|p| p.latitude }
    end
    
    def zs
      self.vertices.map {|p| p.altitude }
    end
  end
  
  class Arc < Polygon
  end
  
  class MultiPoint < Polygon
  end
end