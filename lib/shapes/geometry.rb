module Shapes
  class Point
    attr_accessor :longitude, :latitude, :altitude, :projection
    
    def initialize(lat = nil,long = nil,alt = nil,proj = :wgs84)
      self.latitude   = lat
      self.longitude  = long
      self.altitude = alt
      self.projection = proj
    end
  end
  
  class Polygon
    attr_accessor :vertices, :num_vertices
    
    def initialize(vertices = nil,num_vertices = 4)
      self.vertices = vertices if Array === vertices
      self.vertices ||= []
      self.num_vertices = num_vertices
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