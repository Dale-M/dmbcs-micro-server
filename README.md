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


## Simplest example

The following is provided as the simplest code to demonstrate use of the
dmbcs-micro-server library, not to inform of any coding style or quality
system approach.  We assume a standard GNU system with recent
`make`, `bash`, `gcc`, etc.

Start with the HTML file `calc.html`

```html
  <html>
    <head><title>Multiplier</title></head>
    <body><h1>Multiplier</h1>
      <form action='compute' method='GET' id='calc'>
        <input type='text' id='arg_1'/> x <input type='text' id='arg_2'/>
              = <input type='text' id='result'>[result/]</input>
        <br>
        <input type='submit'>CALCULATE</input>
      </form>
    </body>
  </html>
```

And the C++ source file `calc.cc`

```c++
  #include <dmbcs-micro-server.h>

  using namespace DMBCS::Micro_Server;


  /*  This function both serves up the basic HTML page, and
   *  performs the multiplication and injects the result into the
   *  HTML. */
  void  home_page  (Query_String const &query, int const socket)
  {
      auto  tags  =  Hyper_Tags {};

      add  (tags,
            'result',
            query.get ('arg_1', 0) * query.get ('arg_2', 0));

      Http_Server::return_html (substitute (tags,
                                            slurp_file ('calc.html')));
  }


  int main ()
  {
     auto server  =  Http_Server {2022,
                                  { {'', home_page},
                                    {'compute', home_page} }};

     for (;;)   tick  (server, 1000000);

     return 0;
  }
```

then the `makefile`

    CXXFLAGS = `pkg-config --cflags dmbcs-micro-server`
    LDFLAGS  = `pkg-config  --libs  dmbcs-micro-server`

Then at the command line type

    make calc
    ./calc

then point a browser at `http://localhost:2022/` and use the simple
calculator (donʼt try to do anything funny: the code has been kept
deliberately simple and doesnʼt do any error checking).  Note that an
answer can be obtained directly with a URL like
`http://localhost:2022/compute?arg_1=3&arg_2=4`.


## Download

The *dmbcs-micro-server* source code is managed with *GIT* (configured
with *autotools*, built with *make* and a good C++17 compiler). Type

>   git clone http://rdmp.org/dmbcs/micro-server.git dmbcs-micro-server

at the command line to obtain a copy.


## Documentation

The documentation, as yet incomplete, comes with the source, and you can
also read it [here](https://rdmp.org/dmbcs/micro-server/documentation).


## Contact

Please click [here](https://rdmp.org/dmbcs/contact) if you wish to send us
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
