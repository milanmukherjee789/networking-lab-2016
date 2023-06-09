rm Networking_Assignmenet3.csv
sqlite3 -header -csv Networking_Assignmenet3.db "select * from users;" > Networking_Assignmenet3.csv
