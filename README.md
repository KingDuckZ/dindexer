#		dindexer					 <a target="_blank" href="https://travis-ci.org/KingDuckZ/dindexer">![Travis status][badge.Travis]</a> <a target="_blank" href="https://flattr.com/submit/auto?user_id=King_DuckZ&url=https%3A%2F%2Fgithub.com%2FKingDuckZ%2Fdindexer&title=dindexer&language=en_GB&tags=bitbucket&category=software">![Flattr this project][badge.Flattr]</a>

##		Project info			##
Latest stable release is __0.1.5b__.

[![dindexer on Diaspora*](http://i.imgur.com/DcBZykA.png)](https://mondiaspora.net/tags/dindexer)
Follow [my posts on Diaspora](https://mondiaspora.net/tags/dindexer) for the latest development news and to post your comments about the project.

##		What is dindexer?			##
###		Purpose						###
dindexer (from "Disc Indexer") is a program to help you keep track of where your files are as you back them up on external media. It's like updatedb/locate, but it's designed for never changing removable media, such as DVDs and BluRays.

###		What dindexer is not?		###
To further clarify what dindexer's intended usage is, let's make a list of what it *won't* do for you:

* it's not a daemon - it's just a regular program that starts when you invoke it, performs its duties and then quits
* it's not a realtime file indexing utility; I believe there are a few indexing tools out there already, such as Baloo, mlocate and surely others, but dindexer doesn't duplicate any of those, nor wants to replace them
* it's not a backup tool; dindexer won't backup anything at all, won't make any copies of anything nor anything like that, so it's not like rsync; if you lose your DVD you previously scanned with dindexer, sorry but you've lost all of the data on your DVD

###		Use case					###
You have a pile of DVDs and BluRays holding your files. Maybe you wanted to keep that funny YouTube video, your pictures from your trip to Portugal, your gpg private key backup and your Humble Bundle games.

Your home directory is full again, and it's time to make a new backup DVD. How can you make sure that those files are not on some DVD already? What can you just delete and what needs to be backed up first? This is where dindexer can help you.

###		Usage flow example			###

1. Prepare the list of files you want to burn in an external program like k3b for example, and burn your disc
2. When finished, mount your new backup, for example on /media/cdrom
3. Scan your backup with dindexer using `dindexer scan --setname "Disc 24 - pictures and stuff" /media/cdrom`
4. When done, the list of all the files on your disc will be saved in the database
5. Make queries to find on which disc you stored your data `dindexer locate portugal.jpg`

**WARNING:** as dindexer is still being heavily developed, any of those steps can change in the future. I have a few ideas on how to make this program more convenient to use, but this will require some work. Contributions are welcome.

###		Configuration				###
As dindexer needs to write data to a PostgreSQL database, you need to configure access to it. Create a ~/.config/dindexer.yml file with the settings to connect to your database. For example:

    %YAML 1.2
    ---
    db_settings:
        username: bob_db
        password: my_secret_db_password
        dbname: dindexerdb
        port: 5432
        address: 127.0.0.1

##		Actions						##
Actions are just executable files located in a directory that is searched by dindexer. dindexer will list available actions when invoked with no parameters.

To get help on any specific action you can use the --help switch:

    dindexer <action> --help

For example:

    dindexer scan --help

###		scan						###
The program will go through every file in the path you specify. Hashing everything in that path could take a bit of time, so please be patient.
When dindexer is done analyzing your directory it will save everything to the db in one go, unless the same item was already present in the db.

If you are scanning a defective disc, you can use the `--ignore-errors`. dindexer will then skip unreadable files and mark them as such in the db.

Example

    dindexer scan --setname "My backup 2015-11-13" /media/my_backup

###		delete						###
Delete sets from your db by ID. You can obtain the ID of a set using the locate command.

Example

    #This will delete sets 3 and 28 if they exist
    dindexer delete 3 28

###		locate						###
Find files in the db, similarly to the `locate` command in Linux. Pass the `--set` or `-s` switch to look for sets instead.

Example

    dindexer locate picture.jpg

###		navigate					###
Work in progress. Provides a bash-like CLI so you can explore your sets as if you had put your CDs in the reader (except that you won't be able to read any file data). At the moment only a few commands are recognized, such as `ls`, `pwd` and `cd`.

Example

    dindexer navigate

###		tag							###
Allows you to tag single files or whole sets for easier later retrieval. Also use this command to remove tags (see action help).

Example

    #tag all ogg and flac files in set 31 as music and jpop
    dindexer tag music,jpop --set 31 '*.ogg' '*.flac'

##		Build instructions			##
###		Dependencies				###
The following libraries must be available on your system:

- PostgreSQL (libpq)
- Boost 1.53 or later
- yaml-cpp 0.5.1 or later
- libblkid (from util-linux/misc-utils) *optional, build with -DDINDEXER_WITH_MEDIA_AUTODETECT=Off if you don't have this*

*Note:* Although the CMakeLists.txt will say minimum required version for PostgreSQL is 8.3, this program has only ever been tested with 9.4. 8.3 is simply the threshold under which I'm sure the code will not work. If you want to try dindexer with a PostgreSQL version less than 9.4 you are welcome to do so, but please note I won't be able to make sure everything will work. Patches that address eventual issues are welcome.

###		Build options				###
These are the options understood by cmake. At runtime you might be able to see what *most* of those options were set to using `dindexer --builtin`.

* **DB_OWNER_NAME** Default user name for backends that require one
* **DINDEXER_ACTIONS_PATH** Search path for dindexer actions
* **DINDEXER_CONFIG_FILE** Full path to the yaml configuration file
* **DINDEXER_CXX11_ABI** Set this to off to force newer gcc (5+) to keep using the old ABI - useful on Gentoo if you built your system libraries with gcc 4 and you are trying to build dindexer with gcc 5
* **DINDEXER_DEBUG_CFG_FILE** If set to on, DINDEXER_CONFIG_FILE will be set to a path that is more appropriate for development environments
* **DINDEXER_NATIVE_RELEASE** Set to on to pass `--march=native` to the compiler
* **DINDEXER_WITH_BUILD_DATE** If set to on, the current date will be hardcoded into the final binary - warning: if set to on, some files will need to be rebuilt every day just because the date has changed
* **DINDEXER_WITH_MEDIA_AUTODETECT** Enable code that detects the inserted media type, eg: CD-Rom or DVD - requires libblkid
* **DINDEXER_WITH_NICE_MEDIA_TYPES** If DINDEXER_WITH_MEDIA_AUTODETECT is enabled, this option will print slightly nicer names for media types (for example in the output of `dindexer scan --help`) - requires C++14
* **PBL_WITH_TESTS** When set to on (default when you set BUILD_TESTING to on) it will cause unit tests from the pbl library to be also built - you can safely disable this, as some tests are really slow and the pbl project is not changing anymore

###		Linux						###
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

###		Other platforms				###
I never tested dindexer on anything other than Linux. Note that the code assumes paths to be Unix-like. It also makes other assumptions about the current platform being Linux, so porting could be not so straightforward.

##		Useful technical details	##
###		Code architecture			###
All of the core dindexer functionality is in the *machinery* project. Other useful bits can be found in *common*. Note that *common* depends on *machinery*.
If you want to write your own interface to dindexer, you probably only need to take *machinery* and its dependencies. If you are writing a command line program, stuff in *common* could also be useful to you.

###		Program overview			###
For the sake of clarity, from here on I will refer to the main program (more on it later) simply as "dindexer" and collectively call all the other executables "actions". I will refer to static and shared objects as "libraries".
"dindexer" is also the name of this project as a whole; that meaning will be kept and it will hopefully be clear from the context.

dindexer is made of several actions and libraries. All of them are located in src/, but shared headers are found in include/. The main program, dindexer (the one users are supposed to run), is in src/main/ and it produces the dindexer executable. If you're working on this project, for your convenience you can symlink the dindexer executable into the root of your build directory. dindexer simply takes one action parameter and tries to match it to available actions. If successful it will run the relevant action passing all remaining command line parameters on.

Other than dindexer, in src/ you can find the following actions:

* **delete**
* **locate**
* **query**
* **scan**
* **tag**

and the following libraries:

* **common** a collection of common code useful for the CL fronted
* **machinery** the bulk of dindexer, such as the indexing functionality
* **pq** a c++ wrapper for libpq (PostgreSql)

Additionally inside include/helpers/ you will find header-only implementations of very generic helper tools.

Currently, actions are just the command line frontend to access the functionalities in the libraries. This way writing a new CLI or GUI should be relatively easy.
This structure might change in the future.

###		Queries						###

####	Find duplicates				####
List all duplicate files belonging to different sets:

    select files.hash, group_id, t.ct from files
    inner join (
        select hash, count(*) as ct from files group by hash having count(distinct group_id) > 1
    ) t
    on t.hash = files.hash
    group by files.hash, group_id, t.ct order by files.hash;

###		Set number					###
In the sets table you can find a disk_number column that is currently not used. This is in case you have numbered discs, so you are free to put any number you like in that column.

###		Disc type					###
For your convenience, you can store the type of the disc you are going to index. Currently this has no impact on the program's behaviour. Available types are:

* **C** - CD-Rom
* **D** - Directory
* **V** - DVD
* **B** - BluRay
* **F** - Floppy Disk
* **H** - Hard Disk
* **Z** - Iomega Zip
* **O** - Other

##		Future development			##
This section contains simple ideas for features I'd like to add in the future.

###		Project name				###
Although "dindexer" sounds like a very intuitive name for what this program does, I'm not entirely satisfied with the name. I'm open to suggestions for a new name.

Currently `catafy` (from "catalog" + "file") is the only alternative name that has been proposed.

###		Autosetup					###
When you first run dindexer you have to make sure the database already exists and the tables have already been created. While this is somewhat fine for developers, and you get an auto-generated sql script in your build directory to make this step as easy as possible, I think it should be completely automated so first time users can just start dindexer from the very first time.

###		Database					###
Currently dindexer only works with PostgreSQL databases. This will sound like a very heavy dependency, but having to access my database from multiple machines I thought this would be the best backend to start from. In my case the database is running on a raspberry pi.

In the future I'd like to add support for other storage backends, such as:

* SQLite
* MySql
* Redis (?)
* ...and possibly others, depending on how much help I get from the community

###		Multiusers					###
As it is, dindexer can already be used by multiple users at the same time, as it relies on PostgreSQL. It would be nice however to have some other features, such as what user scanned a given disc and from which machine.

###		GUI							###
I've been developing this program with GUIs in mind. However, since I don't have much experience with GUI designing and programming, I decided to leave this for later and go for a more straightforward CLI. This is definitely an area I'd like to get help with.

While I was tempted to look into QT5, some people on IRC suggested I write a Dolphin plugin instead of a full-blown GUI program. Neither option exclude the other, since all the core functionality is in shared libraries. In fact if you want to program your own GUI around dindexer you just need to pass the user's input on to the right function.
Or at least that's the idea behind the design of the code :)

##		Contact me					##
The best way to get in touch is to find me on IRC freenode. You can find me as either King_DuckZ, or you can try Ccdc_DuckZ if you get any afk autoreply.

<!-- Links -->
[badge.Travis]: https://travis-ci.org/KingDuckZ/dindexer.svg?branch=master
[badge.Flattr]: http://api.flattr.com/button/flattr-badge-large.png
