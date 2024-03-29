Arth
====
it is software, made up of bugs.


It is a C++/Drogon/Svelte/CockroachDB/Redis QnA web application.
The main reason of writing it in C++ is speed and electricity
saving. Just imagine the amount of electircity saved if all
web apps were in C++. Humanity will have far better time
handling environmental issues.

A piece of code is written once, modified few times, read
many times and ran even more number of times. Thus performance
matters.

CockroachDB has been chosen to scale horizontally and Redis for
providing speed.

Svelte has been because of its simplicity and ease of use.
Another UI can be implemented in parallel with this as backend is
totally decoupled from Svelte.

Please read LICENSE carefully. At the bottom there are additional
clauses. The license applies for all files in this repository unless
file has a specific license mentioned.

Goals
=====
1. Be very fast but not necessarily fastest.
2. Be minimalistic in UI and data collection. Do not generate
   unnecessary metadata. Usually web application generate metadata
   for no good reason.

Features
========
1. Built with C++/Drogon which is one of the fastest web frameworks.
   So the application can handle quite high load with moderate hardware.
   Also,it is fully asynchronous app.
2. UI code is small, simple and snappy. Thanks to Svelte.
3. Password is encrypted using award winner argon algorithm.

What Works
==========
1. Registration
2. Login
3. New topic creation
4. Answering a topic
5. Voting
6. Accepting an answer
7. Editing a topic
8. Questions by tag
9. Viewing and editing tag info.

TODO
====
1. Implement Redis LRU cache.
2. Imlpement Elasticsearch for searching questions.
3. Implement reputation system.
4. Implement badges.
5. Implement more items on profile.

Installation
============
Check `CMakeLists.txt` file in backend folder which tells what packages you will need.
For ui folder you need node js and just run `npm i` and then you can build frontend.