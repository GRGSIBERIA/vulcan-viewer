import sqlite3
from contextlib import closing

dbname = "database.db"

with closing(sqlite3.connect(dbname)) as connection:
    c = connection.cursor()

    create_table = open("create_table.sql", "r", encoding="utf-8").read().split(";")
    for query in create_table:
        c.execute(query)