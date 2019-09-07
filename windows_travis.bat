@echo off
if not exist "C:/local/boost_1_68_0/build/stage" ( 
        echo "Boost not cached, compiling it"
		wget https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.7z
		7z x boost_1_68_0.7z -oC:/local
		cd "C:/local/boost_1_68_0"
		dir 
		C:/local/boost_1_68_0/bootstrap.bat 
		C:/local/boost_1_68_0/b2.exe --with-date_time --with-test --with-filesystem --with-program_options --with-regex --with-serialization --with-system runtime-link=static 
)