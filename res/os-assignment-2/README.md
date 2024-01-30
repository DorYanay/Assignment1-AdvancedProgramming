# os-assignment-2

Assignment 1 for "Operation Systems"

* For more information, you can see in the PDF in the docs directory.

## Requirements

* make
* gcc

## Build

For install this tools run the following command in the terminal:

```bash
make all
```

Or if is alreay builded

```bash
make rebuild
```

## Get Started

After the build the 5 tools and 2 dynamic libs. \
Now you can use the new tools 'cmp', 'copy', 'encode', 'decoud' and stshell.

* stshell support multi pipes | (more then only 2).

-v option in stshell: \
stshell support -v option for printing the command after it gets. \
it usefull for call the tool from file. for example: \

```bash
./stshell -v < test/stshell-simple.txt
```

## Authors

- Omer Priel

## License

MIT
