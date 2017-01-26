#!/usr/bin/env ruby

#*******************************************************************************

def build_externals(projects_folder, externals_folder)
    Dir.foreach projects_folder do |folder|

        if folder.match(/.*xcodeproj/)
            puts "building #{projects_folder}/#{folder}"

            result =
                `cd "#{projects_folder}";
                xcodebuild -scheme Max DSTROOT="../../#{externals_folder}/Max" 2>&1;
                xcodebuild -scheme Pd DSTROOT="../../#{externals_folder}/Pd" 2>&1`

            if result.match(/\*\* BUILD SUCCEEDED \*\*/)
                puts "(success)"
            else
                puts "(fail)"
            end
        end

        if File.directory?("#{projects_folder}/#{folder}") &&
            folder != "." &&
            folder != ".." &&
            folder != ".git" &&
            folder != "_SDK_" &&
            folder != "build" &&
            folder != "DerivedData"

            build_externals("#{projects_folder}/#{folder}", externals_folder)
        end
    end
end

def copy_examples(projects_folder, externals_folder)
    require 'fileutils'
    require 'pathname'

    def copy(result, destination)
        FileUtils.mkdir_p destination
        result.each do |filename|
            puts "copying " + filename

            basename = Pathname.new(filename).basename
            FileUtils.copy_file(filename, destination + "/#{basename}", remove_destination = true)
        end
    end

    result = Dir["#{projects_folder}/**/*.maxpat"]
    destination = "#{externals_folder}/Max"
    copy(result, destination)

    result = Dir["#{projects_folder}/**/*.js"]
    destination = "#{externals_folder}/Max"
    copy(result, destination)

    result = Dir["#{projects_folder}/**/*.pd"]
    destination = "#{externals_folder}/Pd"
    copy(result, destination)
end

#*******************************************************************************

projects_folder = "."
externals_folder = "_externals_"

build_externals(projects_folder, externals_folder)
copy_examples(projects_folder, externals_folder)

#*******************************************************************************
