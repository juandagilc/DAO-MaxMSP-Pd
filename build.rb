#!/usr/bin/env ruby

#*******************************************************************************

$mac = (Object::RUBY_PLATFORM =~ /darwin/i) ? true : false
$win = !$mac
puts " "

def build_externals(projects_folder, externals_folder)
    Dir.foreach projects_folder do |filename|

        if $mac && filename.match(/.*xcodeproj/)
            puts "building #{projects_folder}/#{filename}"

            result =
                `cd "#{projects_folder}";
                xcodebuild -scheme Max DSTROOT="../../#{externals_folder}/Max" 2>&1;
                xcodebuild -scheme Pd DSTROOT="../../#{externals_folder}/Pd" 2>&1`

            if result.match(/\*\* BUILD SUCCEEDED \*\*/)
                puts "(success)"
            else
                puts "(fail)"
            end

        elsif $win && filename.match(/.*\.vcxproj/) && !filename.match(/.*\.vcxproj\..*/)
            puts "building #{projects_folder}/#{filename}"

            result =
                `cd "#{projects_folder}" &\
                msbuild /t:Rebuild /p:Platform=x64;Configuration="MaxRelease" &\
                msbuild /t:Rebuild /p:Platform=Win32;Configuration="MaxRelease" &\
                msbuild /t:Rebuild /p:Platform=Win32;Configuration="PdRelease"`

            if result.match(/(0 error|up\-to\-date|Build succeeded\.)/)
                puts "(success)"
            else
                puts "(fail)"
            end
        end

        if File.directory?("#{projects_folder}/#{filename}") &&
            !filename.match(/\./) &&
            !filename.match(/\../) &&
            !filename.match(/.git/) &&
            !filename.match(/_SDK_/) &&
            !filename.match(/DerivedData/) &&
            !filename.match(/Release/) &&
            !filename.match(/Debug/) &&
            !filename.match(/x64/)

            build_externals("#{projects_folder}/#{filename}", externals_folder)
        end
    end
end

def copy_files(projects_folder, externals_folder)
    puts " "
    require 'fileutils'
    require 'pathname'

    def copy(origin, destination)
        origin = Dir[origin]
        FileUtils.mkdir_p destination
        origin.each do |filename|

            base = Pathname.new(filename).basename
            dest = "./" + destination + "/#{base}"

            if (File.exists? filename) && (filename != dest)
                puts "copying " + filename
                FileUtils.copy_file(filename, dest, remove_destination = true)
            end
        end
    end

    def move(origin, destination)
        origin = Dir[origin]
        FileUtils.mkdir_p destination
        origin.each do |filename|

            base = Pathname.new(filename).basename
            dest = "./" + destination + "/#{base}"

            if (File.exists? filename) && (filename != dest)
                puts "moving " + filename
                FileUtils.move(filename, dest)
            end
        end
    end

    # copy max and pd patches
    copy("#{projects_folder}/**/*.maxpat",  "#{externals_folder}/Max")
    copy("#{projects_folder}/**/*.js",      "#{externals_folder}/Max")
    copy("#{projects_folder}/**/*.pd",      "#{externals_folder}/Pd")

    if $win # copy windows externals
      move("#{projects_folder}/**/*.mxe",   "#{externals_folder}/Max")
      move("#{projects_folder}/**/*.mxe64", "#{externals_folder}/Max")
      move("#{projects_folder}/**/*.dll",   "#{externals_folder}/Pd")
    end
end

def cleanup(projects_folder)
    puts " "
    require 'fileutils'

    def remove(folders)
        folders = Dir[folders]
        folders.each do |folder|
            if !folder.match(/.git/) &&
                !folder.match(/_SDK_/)

                puts "removing " + folder
                FileUtils.rm_rf(folder)
            end
        end
    end

    remove("#{projects_folder}/**/DerivedData")
    remove("#{projects_folder}/**/*Release")
    remove("#{projects_folder}/**/*Debug")
    remove("#{projects_folder}/**/x64")
end

#*******************************************************************************

projects_folder = "."
externals_folder = "_externals_"

build_externals(projects_folder, externals_folder)
copy_files(projects_folder, externals_folder)
cleanup(projects_folder)

#*******************************************************************************
