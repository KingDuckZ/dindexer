### What's cooking? ###

It's been roughly a month since I released version 0.1.5b and most of the activity has been on the [dev branch](https://bitbucket.org/King_DuckZ/dindexer/branch/dev) since then. It's still a bit too early to make a new release, but if you want to get a peek at the new features this is the right moment as commit e2275ce should give you a working version of #dindexer.

Most notably, you will have a `tag` command, that will allow you to tag files. You should also be able to list files by tag, using the `dindexer locate --tags <my_tags>` command.

The less evident change (although that's what took up most of my time) is the separation of the PostgreSQL code from the CL client. You have been asking for Sqlite support, I heard you and I'm getting everything ready so that an Sqlite plugin can be implemented! Currently, PostgreSQL is still the only backend you can choose, so please be patient... or even better, step in and contribute to dindexer! :)

[bitbucket.org/King_DuckZ/dindexer](https://bitbucket.org/King_DuckZ/dindexer)

\#dindexer #linux #opensource #cpp #sqlite
