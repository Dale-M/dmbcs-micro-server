>  This is the secondary hosting site for this package; to be sure to be
>  up to date and see all the details please visit
>  https://rdmp.org/dmbcs/micro-server.

# DMBCS Imbedded HTTP Server Library for C++

At DMBCS we like to do things in certain ways: all our code takes the form
of C++17 (or, lately, C++20) libraries built with GNU autotools, providing
user interaction through HTTP/HTML5/CSS3/EcmaScript web interfaces.  It is
thus a common requirement that our code links against a library of classes
which implement the HTML paradigm and provide the TCP/IP plumbing to allow
applications to easily take the form of self-contained web servers.  At
DMBCS we use NGINX at top-level to coordinate the set of such
micro-servers which make up a complete web site.

The library has been under constant development for over twenty years
(yes, really), and in heavy production use.  The code-base hasnʼt quite
been brought up to our expectations of full production-quality code yet
(it has always been a project on the side of other things), and so we
still regard it as beta-quality software.  We expect this situation to
change in the near future.


## Download

The *dmbcs-micro-server* source code is managed with *GIT* (configured
with *autotools*, built with *make* and a good C++17 compiler). Type

   git clone http://rdmp.org/dmbcs/micro-server.git dmbcs-micro-server

at the command line to obtain a copy.


## Documentation

The documentation, as yet incomplete, comes with the source, and you can
also read it [https://rdmp.org/dmbcs/micro-server/documentation](here).


## Contact

Please click [https://rdmp.org/dmbcs/contact](here) if you wish to send us
a message.

### Mailing list

If you would like to receive e-mail notices of matters arising about this
library, you may request this through the contact form.


## Contribution to development

We will happily consider contributions to the source code if you provide
the address of a GIT repository we can pull from, and will consider all
bug reports and feature requests, although onward development of this
library is not a primary concern of ours.

You may also, of course, fork this repository and send us pull requests
here on Github with your contributions.


## Donations

If you use this application please consider a bitcoin donation if you
can. A small amount informs us that there is interest and that we are
providing a useful service to the community; it will keep us motivated to
continue to make open source software. Donations can be made by bitcoin to
the address 1PWHez4zT2xt6PoyuAwKPJsgRznAKwTtF9.