# declare -g, -pg for debug symbols and enable gprof profiling
rm build -rf 
mkdir -p build && cd build
cmake ..
make
./testy ../test_input_files/test_input_1.txt # generate gmon.out
gprof ./testy gmon.out > profile.txt #generate profiling repot
# Show only lines with >1% time
grep -E '^[ ]*[0-9]+\.[0-9]+' profile.txt | awk '$1>1 {print}'


