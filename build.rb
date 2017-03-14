#!/usr/bin/env ruby

#*******************************************************************************

$mac = (Object::RUBY_PLATFORM =~ /darwin/) ? true : false
$windows = (Object::RUBY_PLATFORM =~ /mingw/) ? true : false
$linux = (Object::RUBY_PLATFORM =~ /linux/) ? true : false

puts " "

def build_externals(projects_folder, externals_folder)
    Dir.foreach projects_folder do |filename|

        if $mac && filename.match(/.*xcodeproj/)
            puts "building using xcodebuild: #{projects_folder}/#{filename}"

            result =
                `cd "#{projects_folder}";
                xcodebuild -scheme Max 2>&1;
                xcodebuild -scheme Pd ARCHS=i386 ONLY_ACTIVE_ARCH=NO 2>&1`

            if result.match(/\*\* BUILD SUCCEEDED \*\*/)
                puts "(success)"
            else
                puts "(fail)"
            end

        elsif $windows && filename.match(/.*\.vcxproj/) && !filename.match(/.*\.vcxproj\..*/)
            puts "building using msbuild: #{projects_folder}/#{filename}"

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

        elsif $linux && filename.match(/.*mk/)
            puts "building using make: #{projects_folder}/#{filename}"
            `make -f "#{projects_folder}/#{filename}" 2>&1`
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

    def copy_external(origin, destination)
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

    def move_external(origin, destination)
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
    copy_external("#{projects_folder}/**/*.maxpat",      "#{externals_folder}/Max")
    copy_external("#{projects_folder}/**/*.js",          "#{externals_folder}/Max")
    copy_external("#{projects_folder}/**/*.pd",          "#{externals_folder}/Pd")
    puts " "

    # copy externals
    move_external("#{projects_folder}/**/*.mxo",         "#{externals_folder}/Max")
    move_external("#{projects_folder}/**/*.mxe",         "#{externals_folder}/Max")
    move_external("#{projects_folder}/**/*.mxe64",       "#{externals_folder}/Max")
    move_external("#{projects_folder}/**/*.pd_darwin",   "#{externals_folder}/Pd")
    move_external("#{projects_folder}/**/*.pd_linux",    "#{externals_folder}/Pd")
    move_external("#{projects_folder}/**/*.dll",         "#{externals_folder}/Pd")
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
