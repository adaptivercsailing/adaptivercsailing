You should symlink (symbolically link) the entire Arduino source code and libraries into here!

This allows easy navigation of the Arduino core code and libraries via Eclipse or other IDEs, for instance. 

Example to create the symlinks:

```bash
cd adaptivercsailing/arduino_src

# 1. Symlink main Arduino source code (change this to obtain whatever
#    source code version you have)
ln -si ~/Downloads/Install_Files/Arduino/arduino-1.8.13 .

# 2. Symlink Arduino libraries
ln -si ~/Arduino/libraries .
```

Now, you'll see something like this tree:
```bash
adaptivercsailing/arduino_src$ tree
.
├── arduino-1.8.13 -> /home/username/Downloads/Install_Files/Arduino/arduino-1.8.13
├── libraries -> /home/username/Arduino/libraries
└── README.md

2 directories, 1 file
```

## See also:
1. https://stackoverflow.com/questions/9272882/eclipse-cdt-c-c-include-a-header-file-from-another-project/65975086#65975086
1. https://github.com/ElectricRCAircraftGuy/AmboVent/blob/master/3-Software/Arduino/arduino_core/readme.md
