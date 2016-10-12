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

local hash_key = KEYS[1]
local base_index = ARGV[1]
local file_count = ARGV[2]

for z = base_index, base_index + file_count - 1 do
	redis.call("SREM", hash_key, z)
end
if redis.call("SCARD", hash_key) == 0 then
	redis.call("DEL", hash_key)
end
return nil
