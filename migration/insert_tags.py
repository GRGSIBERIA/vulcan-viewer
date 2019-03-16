import sqlite3
from contextlib import closing
from numba import jit

@jit
def makePlaceHolder(place_holder, tags):
    for tag in tags:
        place_holder.append([tag, tag])

@jit
def makeTags(record):
    return record.split(",")[6].split(" ")

with closing(sqlite3.connect("database.db")) as connection:
    c = connection.cursor()

    records = open("E:/mongo.csv", "r", encoding="utf-8").read().split("\n")
    print("start")

    count = 0
    times = 0
    place_holder = []
    for record in records:
        try:
            tags = makeTags(record)
            makePlaceHolder(place_holder, tags)

            if count == 10000:
                times += 1
                print(times * 10000)
                count = -1
            count += 1
        except:
            pass    # 何もしない

    query = "insert into tags (name) select ? from tags where not exists (select 1 from tags where name=?);"
    c.executemany(query, place_holder)
    connection.commit()