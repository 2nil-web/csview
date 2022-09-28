
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> args, envs;
std::string prog_basename;

// tail, see : https://stackoverflow.com/questions/11876290/c-fastest-way-to-read-only-last-line-of-text-file

void tail1(std::string fn) {
  std::fstream fin(fn, std::ios::in);
	
  if (fin.is_open()) {
    int n=0;
    int pos=-1;
    char ch;
    std::string str;
    fin.seekg(0L, std::ios::end);
    
    while(n != 5) {
      fin.seekg(pos, std::ios::cur);
      pos--;
      
      fin.get(ch);
      if (ch=='\n') n++;
    }

    std::cout <<n << std::endl;
    
    while(fin.eof()) {
      std::getline(fin, str);
      std::cout << str << std::endl;
    }
	}
}

void tail2(std::string filename) {
  std::ifstream fs(filename);

  if (fs.is_open()) {
    //Got to the last character before EOF
    fs.seekg(-1, std::ios_base::end);

    if (fs.peek() == '\n') {
      //Start searching for \n occurrences
      fs.seekg(-1, std::ios_base::cur);
      int i = fs.tellg();
      for(;i > 0; i--) {
        if (fs.peek() == '\n') {
          //Found
          fs.get();
          break;
        }
        //Move one character back
        fs.seekg(i, std::ios_base::beg);
      }
    }

    std::string lastline;
    getline(fs, lastline);
    std::cout << lastline << std::endl;
  } else {
    std::cout << "Could not find end line character" << std::endl;
  }
}

unsigned int nl(std::string fn) {
  unsigned int _nl = 0;
  FILE *fp=fopen(fn.c_str(), "r");
  int ch;

  while (EOF != (ch=getc(fp)))
      if ('\n' == ch) ++_nl;
  return _nl;
}

unsigned int nl_cpp(std::string fn) {
  std::string ln;
  unsigned int _nl=0;
  std::ifstream fp(fn);

  if (fp.is_open()) {
    while (std::getline(fp, ln)) ++_nl;
    fp.close();
  }
  return _nl;
}

int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  for (auto fn:args) {
    std::cout << fn << ':' << nl(fn) << std::endl;
  }
  return 0;
}

