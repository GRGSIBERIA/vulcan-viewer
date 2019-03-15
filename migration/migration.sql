create table files (
    id integer primary key autoincrement not null unique,
    sha2 blob not null unique,
    createdat datetime not null
);

create table tags (
    id integer primary key autoincrement not null unique,
    name text not null unique
);

create table tags2file (
    fileid integer not null,
    tagid integer not null,
    foreign key (fileid) references files(id),
    foreign key (tagid) references tags(id)
);

create table permutations (
    fileid integer not null unique,
    ranking integer not null default 0
);

update permutations set rank = rank + 1
from permutation inner join files on files.id = ?;


insert all
    insert into files (sha2, createdat) values (?, ?)
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