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
local group_key = ARGV[1]

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

local function dele_tag_from_list(tag_list, dele_tag)
	tag_list = split_string(tag_list or "", ",")

	local z = 1
	for _,str in ipairs(tag_list) do
		if str == dele_tag then
			table.remove(tag_list, z)
			break
		end
		z = z + 1
	end
	return table.concat(tag_list, ",")
end

if group_key ~= "" then
	local found_group_key = redis.call("HGET", file_key, "group_id")
	if found_group_key ~= group_key then
		return nil
	end
end

local dele_tag = split_string(tag_key, ":")
local new_tag_list = dele_tag_from_list(redis.call("HGET", file_key, "tags"), dele_tag[#dele_tag])

if new_tag_list == "" then
	redis.call("HDEL", file_key, "tags")
else
	redis.call("HSET", file_key, "tags", new_tag_list)
end

local retval = redis.call("SREM", tag_key, file_key)
if redis.call("SCARD", tag_key) == 0 then
	redis.call("SREM", tag_key)
end
return retval
