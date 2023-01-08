# UU

A library of c++ code for writing programs on unix-like computers. The library includes useful concepts like assertions, logging, platforms, types, data structures, and utility functions. 

For some of these, I started by cribbing from open source code from other projects and organizations like WebKit, LLVM, and IBM, and then where appropriate, I added or changed some details to suit my own needs. 

For other parts, I started from scratch and wrote code that seemed like it would be useful to me. Some of these include:

* __Any__: a neat (I think) implementation of the notion
* __Spread__: a class that maintains a set of numbers that represents sweeps and individual elements, like 1..5,7,9..11
* __TextRef__: a class that models a reference to a line in text file.
* The __Like__ set of utilities that offer pleasant functions for doing tedious business, with examples in __FileLike__, __MathLike__, __StringLike__, and __UnixLike__.

I used these to make the __iota__ tools at `https://github.com/kocienda/iota`. I had fun writing this code and those tools.

If you want to compile and use this code yourself, I regret to say that you're on your own. I didn't take any time or make any substantial effort to make this code usable on anyone's computer by my own. That said, if you have cmake and a c++ compiler handy (I used clang-15 and c++17), you might have some luck with these steps:

1. Get the UU project source code from `https://github.com/kocienda/UU`.
2. Compile and install it with: `cmake -S . -B build; cmake --build build --target install`

You may also wish to look at the source code, just for fun, and if you do, I hope you enjoy.

— Ken
