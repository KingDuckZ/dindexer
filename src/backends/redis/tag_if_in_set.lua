-- Copyright 2015, 2016, Michele Santullo
-- This file is part of "dindexer".
--
-- "dindexer" is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- "dindexer" is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with "dindexer".  If not, see <http://www.gnu.org/licenses/>.

local tag_key = KEYS[1]
local file_key = KEYS[2]
local group_id = ARGV[1]

local function split_string(inputstr, sep)
	if sep == nil then
		sep = "%s"
	end
	local t = {}
	local z = 1
	for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
		t[z] = str
		z = z + 1
	end
	return t
end

local function add_tag_to_list(tag_list, new_tag)
	if tag_list == false or tag_list == "" then
		return new_tag
	end
	tag_list = split_string(tag_list, ",")

	local new_tag_found = false
	for _,str in ipairs(tag_list) do
		if str == new_tag then
			new_tag_found = true
			break
		end
	end
	if not new_tag_found then
		tag_list[#tag_list+1] = new_tag
	end
	table.sort(tag_list)
	return table.concat(tag_list, ",")
end

if group_id ~= 0 then
	local found_group_id = redis.call("HGET", file_key, "group_id")
	if found_group_id ~= group_id then
		return nil
	end
end

local new_tag = split_string(tag_key, ":")
local new_tag_list = add_tag_to_list(redis.call("HGET", file_key, "tags"), new_tag[#new_tag])

redis.call("HSET", file_key, "tags", new_tag_list)
return redis.call("SADD", tag_key, file_key)
