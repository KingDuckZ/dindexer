#	dindexer			#
##	Project info and status		##
### Quick project info			###
####	Release build status		####
[![Build Status](https://drone.io/bitbucket.org/King_DuckZ/dindexer/status.png)](https://drone.io/bitbucket.org/King_DuckZ/dindexer/latest)

####	Latest release		###
Latest stable release is __0.1.5b__.

####	Flattr				####
[![Flattr this git repo](http://api.flattr.com/button/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=King_DuckZ&url=https%3A%2F%2Fbitbucket.org%2FKing_DuckZ%2Fdindexer&title=dindexer&language=en_GB&tags=bitbucket&category=software)

Please support the development of this software if you like it!

###	Development news		###
[![dindexer on Diaspora*](http://i.imgur.com/DcBZykA.png)](https://mondiaspora.net/tags/dindexer)
Follow [my posts on Diaspora](https://mondiaspora.net/tags/dindexer) for the latest development news and to post your comments about the project!

##   What is dindexer?   ##
###  Purpose  ###
dindexer (from "Disc Indexer") is a program to help you keep track of where your files are as you back them up on external media. It's like updatedb/locate, but it's thought for never changing removable media, such as DVDs and BluRays.

Running dindexer on your disks will build a list of all your files and directories. You can associate those lists to a descriptive name and a number, so it's easy to find on what disk a file is on the next time you search for that file.

dindexer will also help you keeping track of duplicate data.

###  What dindexer is not?  ###
To further clarify what dindexer's intended usage is, let's make a list of what it *won't* do for you:

* it's not a daemon - it's just a regular program that starts when you invoke it, performs its duties and then quits
* it's not a realtime file indexing utility; I believe there are a few indexing tools out there already, such as Baloo, mlocate and surely others, but dindexer doesn't duplicate any of those, nor wants to replace them
* it's not a backup tool; dindexer won't backup anything at all, won't make any copies of anything nor anything like that, so it's not like rsync; if you lose your DVD you previously scanned with dindexer, sorry but you've lost all of the data on your DVD

###  Usage example  ###
You have a pile of DVDs, BluRays or any other external storage media on which you stored a copy of your files. Maybe you wanted to keep YouTube videos you don't want to have on your hard disk anymore, your pictures from your trip to Portugal a few years ago, your gpg private key backup and your Humble Bundle games. Maybe you're just moving away old stuff from your external hard disk onto DVDs, so you can store new data on your hard disk.

It's been a few years since you burned your first DVD, and your collection of backup has only grown more. You got to the point it's hard to tell what you have on your DVDs (especially those you just marked as "Backup March 2012"). Even worse, you don't know if those pictures from Portugal are already on some DVD - you think you backed them up somewhere, but just to be safe you burn them again on the next "Backup December 2015" DVD.

Five months later, you want to show your pictures of your trip to Portugal to your friend, but you finally deleted them from your hard disk. You probably have at least two copies on your DVDs, but where exactly? As you browse through your "pics backup"-labeled DVD and you realize it's a completely different sort of pictures you have on there, you think you should just give up on your Portugal photos for now.

dindexer aims to save you from that scenario. The intended usage flow is:

1. Prepare the list of files you want to burn in an external program like k3b for example, and make your backup
2. When finished, mount your new backup, for example on /media/cdrom
3. Scan your backup with dindexer using `dindexer scan --setname "Disc 24 - pictures and stuff" /media/cdrom`
4. When done, the list of all the files on your disc will be saved in dindexer's database
5. Make queries to find on which disc you stored your data - this part is still being worked on, but you can imagine something like `dindexer query portugal.jpg` returning "Disc 24 - pictures and stuff"
6. Go to where you dump - er... store your physical discs (the drawer in your desk, a box, the other room), find the disc you marked as "Disc 24", pop it in your cd-reader and voilà, here's where your portugal.jpg was!

**WARNING:** as dindexer is still being heavily developed, any of those steps can change in the future. I have a few ideas on how to make this program more convenient to use, but this will require some work. Contributions are welcome.

###  Configuration  ###
As dindexer needs to write data to a PostgreSQL database, you need to configure access to it. Create a ~/.config/dindexer.yml file with the settings to connect to your database. For example:

    %YAML 1.2
    ---
    db_settings:
        username: bob_db
        password: my_secret_db_password
        dbname: dindexerdb
        port: 5432
        address: 127.0.0.1

###  Usage  ###
Using dindexer is pretty straightforward: mount your DVD and scan it. For example:

    mount /dev/cdrom /media/my_backup
    dindexer scan --setname "My generic backup 2015-11-13" /media/my_backup

The program will go through every file in the path you specify. Hashing everything in that path could take a bit of time, so please be patient.

You can run dindexer --help to see a list of available switches.
To get a list of all commands you can just run `dindexer --help`. To get help on a specific command, run `dindexer <command> --help`.

###		Code architecture		###
I wrote dindexer with the idea of having at least one GUI for it at som point.
Since I'm just writing a CLI interface for now, I've decided to put all of the 'core' functionalities in a shared object (*machinery*). Other useful, lower level bits can be found in *common*.

As the code is being changed all the time, I'll be waiting to get a more stable interface before writing any detailed documentation. In the meantime you can have a look at the various command line programs to find examples of how to use the different parts of machinery, or you can get in touch with me on IRC (see *Contact me* at the bottom).

##   Build instructions   ##
###  Dependencies  ###
The following libraries must be available on your system:

- PostgreSQL (libpq)
- Boost 1.53 or later
- yaml-cpp 0.5.1 or later
- libblkid (from util-linux/misc-utils) *optional, build with -DDINDEXER_WITH_MEDIA_AUTODETECT=Off if you don't have this*

*Note:* Although the CMakeLists.txt will say minimum required version for PostgreSQL is 8.3, this program has only ever been tested with 9.4. 8.3 is simply the threshold under which I'm sure the code will not work. If you want to try dindexer with a PostgreSQL version less than 9.4 you are welcome to do so, but please note I won't be able to make sure everything will work. Patches that address eventual issues are welcome.

###  Linux  ###
If you are a regular user and want to make a release build for yourself:

    mkdir dindexer_build
    cd dindexer_build
    cmake -DCMAKE_BUILD_TYPE=Release -DINDEXER_NATIVE_RELEASE=ON \
	    -DINDEXER_WITH_BUILD_DATE=ON <path_to_source>
    make -j2

Or any variation of the above options.
If you plan to hack on the code and do some development instead:

    mkdir dindexer_build
	cd dindxexer_build
	cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON <path_to_source>
	make -j2
	ctest

Binaries will be in the src directory. Use `make install` to install them to your selected install prefix (`-CMAKE_INSTALL_PREFIX:PATH=<your_path>` option in CMake). For development purposes, I create a symlink to the main program in the root of my build directory with the command `ln -s src/main/dindexer`, so I can run the various subcommands from the top-level build directory directly (eg: `./dindexer navigate`).

###  Other platforms  ###
I never tested dindexer on anything other than Linux. Note that the code assumes paths to be Unix-like. It also makes other assumptions about the current platform being Linux, so porting could be not so straightforward.

##   Useful technical details   ##
###  Program overview  ###
For the sake of clarity, from here on I will refer to the main program (more on it later) simply as "dindexer" and collectively call all the other executables "actions". I will refer to static and shared objects as "libraries".
"dindexer" is also the name of this project as a whole; that meaning will be kept and it will hopefully be clear from the context.

dindexer is made of several actions and libraries. All of them are located in src/, but shared headers are found in include/. The main program, dindexer (the one users are supposed to run), is in src/main/ and it produces the dindexer executable. If you're working on this project, for your convenience you can symlink the dindexer executable into the root of your build directory. dindexer simply takes one action parameter and tries to match it to available actions. If successful it will run the relevant action passing all remaining command line parameters on.

Other than dindexer, in src/ you can find the following actions:

* **delete**
* **locate**
* **query**
* **scan**

and the following libraries:

* **common** a collection of common code useful for the CL fronted
* **machinery** the bulk of dindexer, such as the indexing functionality
* **pq** a c++ wrapper for libpq (PostgreSql)

Additionally inside include/helpers/ you will find header-only implementations of very generic helper tools.

Currently, actions are just the command line frontend to access the functionalities in the libraries. This way writing a new CLI or GUI should be relatively easy.
This structure might change in the future.

###  Queries  ###

#### Find duplicates ####
List all duplicate files belonging to different sets:

    select files.hash, group_id, t.ct from files
    inner join (
        select hash, count(*) as ct from files group by hash having count(distinct group_id) > 1
    ) t
    on t.hash = files.hash
    group by files.hash, group_id, t.ct order by files.hash;

### Set number ###
In the sets table you can find a disk_number column that is currently not used. This is in case you have numbered discs, so you are free to put any number you like in that column.

### Disc type ###
For your convenience, you can store the type of the disc you are going to index. Currently this has no impact on the program's behaviour. Available types are:

* **C** - CD-Rom
* **D** - Directory
* **V** - DVD
* **B** - BluRay
* **F** - Floppy Disk
* **H** - Hard Disk
* **Z** - Iomega Zip
* **O** - Other

##	Future development		##
This section contains simple ideas for features I'd like to add in the future.

###		Project name	###
Although "dindexer" sounds like a very intuitive name for what this program does, I'm not entirely satisfied with the name. I'm open to suggestions for a new name.

###		Autosetup		###
When you first run dindexer you have to make sure the database already exists and the tables have already been created. While this is somewhat fine for developers, and you get an auto-generated sql script in your build directory to make this step as easy as possible, I think it should be completely automated so first time users can just start dindexer from the very first time.

###		Database		###
Currently dindexer only works with PostgreSQL databases. This will sound like a very heavy dependency, but having to access my database from multiple machines I thought this would be the best backend to start from. In my case the database is running on a raspberry pi.

In the future I'd like to add support for other storage backends, such as:

* SQLite
* MySql
* Redis (?)
* ...and possibly others, depending on how much help I get from the community

###		Multiusers		###
As it is, dindexer can already be used by multiple users at the same time, as it relies on PostgreSQL. It would be nice however to have some other features, such as what user scanned a given disc and from which machine.

###		GUI				###
I've been developing this program with GUIs in mind. However, since I don't have much experience with GUI designing and programming, I decided to leave this for later and go for a more straightforward CLI. This is definitely an area I'd like to get help with.

While I was tempted to look into QT5, some people on IRC suggested I write a Dolphin plugin instead of a full-blown GUI program. Neither option exclude the other, since all the core functionality is in shared libraries. In fact if you want to program your own GUI around dindexer you just need to pass the user's input on to the right function.
Or at least that's the idea behind the design of the code :)

##		Contact me		##
The best way to get in touch is to find me on IRC freenode. You can find me as either King_DuckZ, or you can try Ccdc_DuckZ if you get any afk autoreply.
