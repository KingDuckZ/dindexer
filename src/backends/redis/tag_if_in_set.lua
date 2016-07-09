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

if group_key == "" then
	return redis.call("SADD", tag_key, file_key)
else
	local found_group_key = redis.call("HGET", file_key, "group_id")
	if found_group_key == group_key then
		return redis.call("SADD", tag_key, file_key)
	else
		return nil
	end
end
