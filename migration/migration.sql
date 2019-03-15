-- pragma encoding = "UTF-8;
-- pragma foreign_keys = ON;

-- ファイル情報
create table files (
    id integer primary key autoincrement not null unique,
    sha2 blob not null unique,
    file_size integer not null,
    file_offset integer not null,
    createdat datetime not null
);

-- タグ
create table tags (
    id integer primary key autoincrement not null unique,
    name text not null unique
);

-- 言語
create table nations (
    id integer primary key autoincrement not null unique,
    name text not null unique
);

-- 国際化されたタグ
create table localized_tags (
    tagid integer primary key autoincrement not null unique,
    nationid integer not null,
    name text not null unique,
    foreign key (tagid) references tags(id),
    foreign key (nationid) references nations(id)
);

-- タグとファイルの中間テーブル
create table tags2file (
    fileid integer not null,
    tagid integer not null,
    foreign key (fileid) references files(id),
    foreign key (tagid) references tags(id)
);

-- 優先度
create table permutations (
    fileid integer not null unique,
    ranking integer not null default 0
);

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

