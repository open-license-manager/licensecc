# Developer's environment setup
This guide is just an help in case you haven't decided your development environment or your development environment match ours. 
We don't have any specific dependency on development tools, you can choose the one you prefer. Though if you want to contribute
you're required to  format the code using `clang-format` before you submit the pull request.

## Linux setup

First of all download the source code and compile it from command line as described in [build the library](Build-the-library)
Download prerequisites:

```
sudo curl -L "https://github.com/google/styleguide/blob/gh-pages/cpplint/cpplint.py" -o /usr/bin/cpplint.py
sudo chmod a+x /usr/local/bin/cpplint.py
sudo apt-get install clang-format ninja-build
```

## Windows setup
(To be done ...)