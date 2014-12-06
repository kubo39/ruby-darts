#!/usr/bin/env rake

task :default => ['compile', 'clean', 'test']

task :compile do
  Dir.chdir File.expand_path("../ext", __FILE__) do
    sh "ruby extconf.rb"
    sh "make"
  end
end

task :clean do
  Dir.chdir File.expand_path("../ext", __FILE__) do
    sh "rm -f *.o Makefile mkmf.log"
  end
end

task :test do
  Dir.chdir File.expand_path("../tests", __FILE__) do
    sh "ruby test_darts.rb"
  end
end
