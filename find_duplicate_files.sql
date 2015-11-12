--select hash, group_id, count(hash) as hash_count from files group by hash, group_id having count(*) > 1 order by hash;

--select hash, group_id from files group by hash, group_id having count(*) > 1 and count(order by hash;

select files.hash, group_id, t.ct from files inner join (
	select hash, count(*) as ct from files group by hash having count(distinct group_id) > 1
) t on t.hash = files.hash group by files.hash, group_id, t.ct order by files.hash;
