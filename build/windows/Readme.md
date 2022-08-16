## How to build - windows platform

### Requirements

- Visual Studio :-)
- [Inno Setup](https://jrsoftware.org/isdl.php#stable)

### The easy way

If you want to build a complete eCAL setup with the default Visual Studio  Version 2019 then simply call:

```win_make_all.bat```

### Step by step

If you want to cmake / build eCAL step by step do the following:

for Visual Studio 2019:

```
win_make_cmake.bat v142
win_make_build.bat
```

for Visual Studio 2022:

```
win_make_cmake.bat v143
win_make_build.bat
```

To create a windows setup finally call:

```
win_make_setup.bat
```
