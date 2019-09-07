@echo off
if not exist "C:/local/boost/include" (
        echo "Boost not cached, compiling it"
		wget -q https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.7z
		echo "Boost downloaded"
		7z x boost_1_68_0.7z -bb1 -oC:/local
		cd "C:/local/boost_1_68_0"
		dir 
		C:/local/boost_1_68_0/bootstrap.bat 
		C:/local/boost_1_68_0/b2.exe --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static --prefix=C:/local/boost stage
		C:/local/boost_1_68_0/b2.exe --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static --prefix=C:/local/boost install > a.txt 2>&1 
		cd C:/local/boost
		dir
)