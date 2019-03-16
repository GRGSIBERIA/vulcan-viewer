pragma encoding = "UTF-8";
pragma foreign_keys = ON;

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