#!/usr/bin/env ruby

=begin
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
=end

require 'shellwords'
require 'pathname'
require 'yaml'
require 'uri'

GIT="git"

SubmoduleInfo = Struct.new(:path, :url, :status, :name, :hash)

def call_git(parTokens, parCaptureStdout=true, work_dir: nil)
	git_c_param = ""
	if work_dir then
		raise ScriptError, "Tainted path received in call_git() function" if work_dir.tainted?
		git_c_param = " -C #{work_dir}"
	end

	command_line = ""
	if parTokens.is_a?String then
		raise ScriptError, "Tainted token received in call_git() function" if parTokens.tainted?
		command_line = parTokens
	elsif parTokens.is_a?Array then
		parTokens.each do |tok,idx|
			raise ScriptError, "Tainted token at index #{idx} received in call_git() function" if tok.tainted?
		end
		command_line = parTokens.join(" ")
	else
		raise ScriptError, "Invalid parTokens received in call_git() function"
	end

	#puts "#{GIT}#{git_c_param} #{command_line}"
	if parCaptureStdout then
		return `#{GIT}#{git_c_param} #{command_line}`.chomp
	else
		return Kernel.system("#{GIT}#{git_c_param} #{command_line}")
	end
end

def is_absolute_url?(parUrl)
	return not(/^\.{1,2}\// =~ parUrl)
end

def sanitized(parString)
	retval = Shellwords.escape(parString)
	retval.untaint
	return retval
end

def current_remote_url()
	current_branch = call_git(["rev-parse", "--abbrev-ref", "HEAD"])
	current_remote = call_git(["config", "branch.#{sanitized(current_branch)}.remote"])
	remote_url = call_git(["config", "remote.#{sanitized(current_remote)}.url"])
	return remote_url
end

def make_git_address(parBaseRemoteUrl, parSuffix)
	regex_protocol = /^\w+:\/\//
	m = regex_protocol.match(parBaseRemoteUrl)
	if m then
		URI.join(parBaseRemoteUrl + "/", parSuffix).to_s
	else
		Pathname.new(File.join(parBaseRemoteUrl, parSuffix)).cleanpath.to_s
	end
end

def submodules_info(parBaseRemoteUrl)
	regex_submodule = /^submodule\.(.+?)\.([^.]+)$/
	submodules = Hash.new{ |hash, key| hash[key] = SubmoduleInfo.new }
	call_git(["config", "--file", ".gitmodules", "--name-only", "--list"]).lines.map(&:chomp).each do |line|
		m = regex_submodule.match(line)
		next if m.nil?

		new_value = call_git(["config", "--file", ".gitmodules", sanitized(line)])
		case m[2]
		when "url" then
			submodules[m[1]].url = is_absolute_url?(new_value) ? new_value : make_git_address(parBaseRemoteUrl, new_value)
			submodules[m[1]].name = File.basename(new_value, ".git")
		when "path" then
			submodules[m[1]].path = new_value
		end
	end

	regex_status = /^(.)([0-9a-fA-F]{40})\s+(.+?)(?:\s+\(.+\))?$/
	call_git(["submodule", "status"]).lines.map(&:chomp).each do |line|
		m = regex_status.match(line)
		next if m.nil?

		submodules[m[3]].status = m[1]
		submodules[m[3]].hash = m[2]
	end
	return submodules
end

def is_inside_git_repo(parPath)
	reply = call_git("rev-parse --is-inside-work-tree 2> /dev/null || echo false", work_dir: parPath)
	return "true" == reply
end

class FlatGit
	def initialize(parCloneDir)
		unless File.exists?(parCloneDir) && File.directory?(parCloneDir) then
			raise ArgumentError, "Specified path doesn't exist or is not a valid directory"
		end

		if is_inside_git_repo(sanitized(parCloneDir)) then
			raise ArgumentError, "Specified path is invalid because it appears to be inside a git repository"
		end

		@clone_dir_abs = Pathname.new(parCloneDir).realpath
	end

	def clone_submodules()
		inplace_submodules = if File.file?("flat_git.yml") then
			local_settings = YAML.load_file("flat_git.yml")
			lst = local_settings["inplace_submodules"]
			lst = [lst] if lst.is_a?(String)
			lst.is_a?(Array) && lst || Array.new
		else
			Array.new
		end

		submodules_info(current_remote_url()).each_value do |submod|
			next unless submod.status == "-"

			abs_guessed_clone_dir = File.join(@clone_dir_abs, submod.name)
			if inplace_submodules.include?(submod.name) then
				success = call_git(["submodule", "update", "--init", sanitized(submod.path)], false)
				return false unless success
			else
				if !File.exists?(abs_guessed_clone_dir) || (File.directory?(abs_guessed_clone_dir) && Dir.entries(abs_guessed_clone_dir).empty?) then
					success = call_git(["clone", sanitized(submod.url)], false, work_dir: sanitized(@clone_dir_abs))
					return false unless success

					if File.exists?(abs_guessed_clone_dir) && File.directory?(abs_guessed_clone_dir) then
						call_git(["config", "core.worktree", ".."], work_dir: sanitized(abs_guessed_clone_dir))
						call_git(["reset", "--hard", sanitized(submod.hash)], work_dir: sanitized(abs_guessed_clone_dir))

						Dir.chdir(abs_guessed_clone_dir) do |path|
							if File.file?(".gitmodules") then
								success = clone_submodules()
								return false unless success
							end
						end
					else
						raise RuntimeError, "Unable to guess where git just cloned the repo"
					end
				end
				call_git(["submodule", "init", sanitized(submod.path)])
				File.open(File.join(submod.path, ".git"), 'w') {|file| file.puts("gitdir: #{File.join(abs_guessed_clone_dir, ".git")}")}
			end
		end
		return true
	end
end

def main(parArgv)
	unless ARGV.length == 1 then
		$stderr.puts "Wrong number of arguments"
		exit 2
	end

	flat_git = FlatGit.new(ARGV[0])
	if flat_git.clone_submodules() then
		return 0
	else
		return 1
	end
end

exit(main(ARGV))
