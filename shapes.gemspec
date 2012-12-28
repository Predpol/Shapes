# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'shapes/version'

Gem::Specification.new do |gem|
  gem.name          = "shapes"
  gem.version       = Shapes::VERSION
  gem.authors       = ["Omar Qazi"]
  gem.email         = ["omar@predpol.com"]
  gem.description   = %q{A simple ruby library for writing shape files}
  gem.summary       = %q{Yeah, read the description}
  gem.homepage      = "http://www.predpol.com/"

  gem.files         = `git ls-files`.split($/)
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.require_paths = ["lib"]
  gem.extensions = ['ext/shapes/extconf.rb']
end
