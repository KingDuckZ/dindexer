#     dindexer v0.1.5b released     #
##    Release notes                ##

It's been a while since the last version number change, and as you might already know from reading the README file v0.1.4b had a major flaw and it shouldn't be used at all. I've been exceptionally busy between that release and now, thus the long wait. #dindexer however has not been put aside during this time! Here is what has changed in this release:

*Features and usability*

* Add new "navigate" command
* Fix behaviour of main dindexer when no actions are found
* Add more builtin info that can be viewed with `dindexer -b`
* Identify the type of data on a disk (eg: video dvd)
* Allow searching by hash in `locate`
* Add scripts and functionality to the code to enable bash autocompletion for actions
* Let dindexer give you a hint if you mis-spell an action
* Main dindexer program now understands the `--version` option

*Bug fixes*

* Fix wrong hash for directories
* Fix scan hanging after listing directories
* Fix mimetype retrieval
* Buildfix on ARM 32 and 64 bit

*Code improvements*

* Cleanup for building on gcc 5 and clang
* "install" and "test" targets in cmake
* Add some unit tests
* Scan actions are now done in tasks that can be composed together
* Many improvements in the code
* Improve the build system (cmake scripts)

##    Final notes                  ##

As you might guess from the "b" in the version number this is still an early release, and there is still a lot more I want to do.

One of the things I should take care of as soon as possible is coming up with a nicer name for this project. Many people seem to think of dindexer as a realtime file indexing program (like Baloo for example), and I believe this is due to the *indexer* part in the name. I'd like to address this before the project goes on too much, so any suggestions are welcome! Feel free to give me any suggestions either here or send a private message on IRC freenode to **King_DuckZ**.

It is also very likely that the main repository will be moved to github, but you can count on the bitbucket page to be updated regularly for the foreseeable future.

Happy scanning with the new release, and let me have your comments and ideas! :)

\#dindexer #opensource #linux #cpp
