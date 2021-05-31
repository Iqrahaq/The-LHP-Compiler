<meta name="author" content="Iqra Haq">

<p align="center">
  <img src="lhpCompilerIcon.png" alt="lhpCompilerIcon">
</p>

<h1 align="center" style="font-weight: bold;">
  The LHP Compiler
</h1>

<p align="center">
  <a href="https://github.com/Iqrahaq/The-LHP-Compiler/blob/main/LICENSE">
    <img alt="GitHub license" src="https://img.shields.io/github/license/Iqrahaq/The-LHP-Compiler">
  </a>  
  <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/Iqrahaq/The-LHP-Compiler">
  <img alt="Codacy grade" src="https://img.shields.io/codacy/grade/ea3a99f3ce1f43d1a65d8252ba0fa3ea"> <br/>
  <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/Iqrahaq/The-LHP-Compiler">
</p>

<p align="center">This is a prototype software program written in C that takes the contents of an LHP File (either HTML + C with FastCGI elements or just pure C with FastCGI elements) and compiles the contents into an EXE File compatible for a CGI enabled web-server.</p>

## Getting Started

<p align="center"> These instructions will get you a copy of the project up and running on your local machine. </p>
<p align="center"> Note: Your machine must be <b> Unix-based </b> for this program to work! </p>

### Prerequisites

This program requires various prerequisite packages and so it is recommended that these are installed on an up-to-date system.

To update:

```
sudo apt-get update -y && sudo apt-get upgrade -y
```

To download the prerequisite packages:

```
sudo apt-get install mysql-server libmysqlclient-dev libfcgi libfcgi-dev gcc -y
```

### Installing

First you must retrieve the C source code from this repository and then compile the program.
To do this, follow the steps below:

1. Retrieve the C Source Code.

```
wget https://github.com/Iqrahaq/lhpCompiler/raw/master/lhpCompiler.c
```

2. Compile the program.

```
gcc lhpCompiler.c –o lhpCompiler
```
3. Run the program (with the following command structure).
```
./lhpCompiler [lhpFile]
```

<p align="center"> Note: Any LHP Files used must be in <b> Unix format </b>, to convert to unix format run the following command: </p>

```
dos2unix [lhpFile]
```

## Authors
**Iqra Haq** - Student @ Northumbria University, Newcastle upon Tyne, UK.

## Credits
Dynamic File Reading - http://man7.org/linux/man-pages/man3/getline.3.html

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE) file for details

## Acknowledgments
**Dr. Nicholas Dalton** - Project Supervisor @ Northumbria University, Newcastle upon Tyne, UK.

