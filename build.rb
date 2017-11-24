#!/usr/bin/env ruby

#*******************************************************************************

$mac = (Object::RUBY_PLATFORM =~ /darwin/) ? true : false
$windows = (Object::RUBY_PLATFORM =~ /mingw/) ? true : false
$linux = (Object::RUBY_PLATFORM =~ /linux/) ? true : false
$noargs = (ARGV.length == 0)

puts " "

def build_externals(projects_folder)
    Dir.foreach projects_folder do |filename|

        if $mac && filename.match(/.*mk/) && ARGV[0] == "BBB"
            puts "building using make for BBB+Bela: #{projects_folder}/#{filename}"
            `make -f "#{projects_folder}/#{filename}" BBB=1 2>&1`
        end

        if $mac && filename.match(/.*xcodeproj/)
            puts "building using xcodebuild: #{projects_folder}/#{filename}"

            result =
                `cd "#{projects_folder}";
                xcodebuild -scheme Max -configuration Release 2>&1;
                xcodebuild -scheme Pd ARCHS=i386 ONLY_ACTIVE_ARCH=NO -configuration Release 2>&1`

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
            puts "building using make for linux desktop: #{projects_folder}/#{filename}"
            `make -f "#{projects_folder}/#{filename}" 2>&1`
        end

        if File.directory?("#{projects_folder}/#{filename}") &&
            !filename.match(/\./) &&
            !filename.match(/\../) &&
            !filename.match(/.git/) &&
            !filename.match(/_COMMON_/) &&
            !filename.match(/_SDK_/) &&
            !filename.match(/DerivedData/) &&
            !filename.match(/Release/) &&
            !filename.match(/Debug/) &&
            !filename.match(/x64/)

            build_externals("#{projects_folder}/#{filename}")
        end
    end
end

def copy_files(projects_folder, externals_folder)
    puts " "
    require 'fileutils'
    require 'pathname'

    def copy_external(origin, destination)
        origin = Dir[origin]
        origin.each do |filename|

            file = Pathname.new(filename).basename
            project = Pathname.new(File.expand_path("../..", filename)).basename
            dest = "./" + destination + "/" + "#{project}"

            if (File.exists? filename) &&
                (filename != dest) &&
                !filename.match(/.git/) &&
                !filename.match(/_COMMON_/) &&
                !filename.match(/_SDK_/) &&
                (Pathname.new(File.expand_path("..", filename)).basename.to_s == "Products")

                FileUtils.mkdir_p dest
                puts "copying " + filename + " to " + dest + "/#{file}"
                FileUtils.copy_file(filename, dest + "/#{file}", remove_destination = true)
            end
        end
    end

    def move_external(origin, destination)
        origin = Dir[origin]
        origin.each do |filename|

            file = Pathname.new(filename).basename
            project = Pathname.new(File.expand_path("../..", filename)).basename
            dest = "./" + destination + "/" + "#{project}"

            if (File.exists? filename) &&
                (filename != dest) &&
                !filename.match(/.git/) &&
                !filename.match(/_COMMON_/) &&
                !filename.match(/_SDK_/) &&
                (Pathname.new(File.expand_path("..", filename)).basename.to_s == "Products")

                FileUtils.mkdir_p dest
                puts "moving " + filename
                FileUtils.move(filename, dest + "/#{file}")
            end
        end
    end

    # copy patches, audio, text, and javascript files
    copy_external("#{projects_folder}/**/*.maxpat",      "#{externals_folder}/Max")
    copy_external("#{projects_folder}/**/*.wav",         "#{externals_folder}/Max")
    copy_external("#{projects_folder}/**/*.txt",         "#{externals_folder}/Max")
    copy_external("#{projects_folder}/**/*.js",          "#{externals_folder}/Max")

    copy_external("#{projects_folder}/**/*.pd",          "#{externals_folder}/Pd")
    copy_external("#{projects_folder}/**/*.wav",         "#{externals_folder}/Pd")
    copy_external("#{projects_folder}/**/*.txt",         "#{externals_folder}/Pd")
    puts " "

    # move externals
    move_external("#{projects_folder}/**/*.mxo",         "#{externals_folder}/Max")
    move_external("#{projects_folder}/**/*.mxe",         "#{externals_folder}/Max")
    move_external("#{projects_folder}/**/*.mxe64",       "#{externals_folder}/Max")

    move_external("#{projects_folder}/**/*.pd_darwin",   "#{externals_folder}/Pd")
    move_external("#{projects_folder}/**/*.pd_linux",    "#{externals_folder}/Pd")
    move_external("#{projects_folder}/**/*.dll",         "#{externals_folder}/Pd")

    # remove pd patch for bela if not built for it
    if $noargs
        FileUtils.rm Dir.glob("#{externals_folder}/Pd/**/_main.pd")
    end
end

def cleanup(projects_folder)
    puts " "
    require 'fileutils'

    def remove(folders)
        folders = Dir[folders]
        folders.each do |folder|
            if !folder.match(/.git/) &&
                !folder.match(/_COMMON_/) &&
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
externals_folder = "_EXTERNALS_"

build_externals(projects_folder)
copy_files(projects_folder, externals_folder)
cleanup(projects_folder)

#*******************************************************************************
