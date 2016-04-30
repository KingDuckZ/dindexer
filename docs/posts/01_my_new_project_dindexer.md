#     My new project: dindexer     #

It’s only been a few months since I began working on #dindexer, about 3 and a half. When I started the project I thought it was going to be much quicker to write. In fact I started it because I needed to figure out what files on my hard disks had already been burned onto DVDs, and thus could be safely deleted to make room for new stuff, and what needed to go through K3B first. Unfortunately I’m still not at the point where I can do exactly what I needed, but I’m getting closer and closer to it.

My use case is this: I have a lot of files - old videos, holiday pictures, tar archives, source code, humble bundle games. Some of those files already made their way onto some DVD and to the box in the storage room, some others still only exist on my hard disk. How can I tell them apart? Re-burning everything could be a solution (and in fact that’s what I’ve always done), so I can be sure everything is on at least one DVD. But the downside is that my already large collection of backup would grow much faster. In the end, duplicate files only add to confusion.

Ideally, I should be able to simply put everything in a directory (or maybe in a K3B savefile), point some tool to that directory and have it filter out things that already exist on some DVD. That’s the idea behind dindexer.

dindexer is a #c++ command-line #tool for #linux systems. There is no GUI yet, although I’m hoping to have one at some point.

I’ll be posting updates to my project as the development goes forward, so stay tuned! In the meantime please try out the latest version of [dindexer on bitbucket](https://bitbucket.org/King_DuckZ/dindexer) and let me hear your feedback!
