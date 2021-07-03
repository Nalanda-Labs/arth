Arth
====
it is software, made up of bugs.


It is a C++/Drogon/Svelte/Postgres/Redis QnA web application.
The main reason of writing it in C++ is speed and electricity
saving. Just imagine the amount of electircity saved if all
web apps were in C++. Humanity will have far better time
handling environmental issues.

A piece of code is written once, modified few times, read
many times and ran even more number of times. Thus performance
matters.

Please read LICENSE carefully. At the bottom there are additional
clauses.

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

TODO
====
1. Implement Redis LRU cache.
2. Imlpement Elasticsearch for searching questions.
3. Implement reputation system.
4. Implement badges.
5. Implement more ittems on profile.
