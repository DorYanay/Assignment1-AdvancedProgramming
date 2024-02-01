# assignment-1-advanced-programming

Assignment 1 for "Advanced Programming"

* For more information, you can see in the PDF in the docs directory.

## Requirements

* make
* g++

## Build

For install this tools run the following command in the terminal:

```bash
make all
```

Or if is already built

```bash
make rebuild
```

## Get Started

For open the server run the following commands in the terminal:

```bash
./shell   
hello: date >> myfile
hello: cat myfile
IST 19:21:11 2024 פבר 01 ה'
hello: date -u >> myfile
hello: cat myfile
IST 19:21:11 2024 פבר 01 ה'
UTC 17:21:31 2024 פבר 01 ה'
hello: wc -l < myfile
2
hello: prompt = hi
hi: mkdir mydir
hi: cd mydir
hi: pwd
/home/omer/projects/assignment-1-advanced-programming/mydir
hi: touch file1 file2 file3
hi: ls
file1  file2  file3
hi: !!
hi: ls
file1  file2  file3
hi: echo abc xyz
abc xyz
hi: ls
file1  file2  file3
hi: echo $?
0
hi: ls no_such_file
ls: cannot access 'no_such_file': No such file or directory
hi: echo $?
2
hi: ls no_such_file 2> file
hi: ^C
You typed Control-C
hi: cat > colors.txt
blue
black
red
red
green
blue
green
red
red
blue
hi: cat colors.txt
blue
black
red
red
green
blue
green
red
red
blue
hi: cat colors.txt | cat | cat | cat
blue
black
red
red
green
blue
green
red
red
blue
hi: sort colors.txt | uniq –c | sort –r | head -3
sort: cannot read: –r: No such file or directory
uniq: –c: No such file or directory
hi: quit
```

## Authors

- Omer Priel
- Dor Yanay

## License

MIT
