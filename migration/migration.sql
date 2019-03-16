pragma encoding = "UTF-8";
pragma foreign_keys = ON;

-- 優先度をインクリメント
update permutations set rank = rank + 1
from permutation inner join files on files.id = ?;

-- 優先度をデクリメント
update permutations set rank = rank - 1
from permutation inner join files on files.id = ?;

-- データの挿入
insert all
    insert into files (sha2, file_size, file_offset, createdat) values (?, ?, ?, ?)
    insert into permutations (fileid)
        select files.id from files.sha2 = ?,
    
    -- タグの数だけ繰り返し
    insert into tags (name)
        select ? from tags where not exists (
            select 1 from tags where name = ?
        ),
    insert into tags2file (fileid, tagid)
        select files.id, tags.id from tags, files 
        where tags.name = ? and files.sha2 = ?;
    -- 繰り返しここまで
select * from DUAL;

-- タグ名からファイルを検索する
select files.file_size, files.file_offset from files, tags2file as t2f
where t2f.fileid = files.id and t2f.tagid = 
    (select tags.name from tags where tags.name in (?, ?, ..., ?))
offset ? limit ? order by files.id ?;

