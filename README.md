#   What is dindexer?   #
dindexer (from "Disc Indexer") is a program to help you keep track of where your files are as you back them up on external media. It's like updatedb/locate, but it's thought for never changing removable media, such as DVDs and BluRays.

Running dindexer on your disks will build a list of all your files and directories. You can associate those lists to a descriptive name and a number, so it's easy to find on what disk a file is on the next time you search for that file.

dindexer will also help you keeping track of duplicate data.

##  Configuration  ##
As dindexer needs to write data to a PostgreSQL database, you need to configure access to it. Create a ~/.config/dindexer.yml file with the settings to connect to your database. For example:

    %YAML 1.2
    ---
    db_settings:
        username: bob_db
        password: my_secret_db_password
        dbname: dindexerdb
        port: 5432
        address: 127.0.0.1

##  Usage  ##
Using dindexer is pretty straightforward: mount your DVD and scan it. For example:

    mount /dev/cdrom /media/my_backup
    dindexer --setname "My generic backup 2015-11-13" /media/my_backup

You can run dindexer --help to see a list of available switches.

#   Build instructions   #
##  Linux  ##

    mkdir dindexer_build
    cd dindexer_build
    cmake -DCMAKE_BUILD_TYPE=Release <path to dindexer source>
    make -j2

##  Other platforms  ##
I never tested dindexer on anything other than Linux. Note that the code assumes paths to be Unix-like. Other than that, there shouldn't be any major problem to porting the code.

##  Project status  ##
This project is still being developed. Currently, only the indexer has been implemented. You can search the database manually if you know how to write the SQL queries you will need.