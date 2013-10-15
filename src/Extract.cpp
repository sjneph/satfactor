#include <algorithm>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>


/*
    An application of very limited use.  Can extract factors of number N
    as found by a sat solver whose input was the output of the ifactor
    executable.  But, the solver's output solution must follow 1 or 2 below:
    (1) Contain the syntax
        solution = 1 -2 ...
    Other information can be in the file, but the solution must look like
    this, with no new lines.
    (2) Just the solution, with no other information in the file
         1 -2 ...

    If both are unfound, this app will assume that the WFF was
    unsatisfiable and report that N is a prime.

    You can modify your solution file to have one of these formats, or you can 
    make adjustments in main() below to suit your solver's output needs.
*/


struct ByLine : std::string {
  friend std::istream& operator>>(std::istream &is, ByLine &toGet) {
    std::getline(is, toGet);
    return(is);
  }
};

std::vector<std::string> SplitString(const std::string& s, char delim);
void RemoveFrontSpace(std::string&);

std::string Error() {
  std::string rtn = "Bad Input: Expect 2 arguments";
  rtn += "\nArg1 = WFF input file";
  rtn += "\nArg2 = Solution file";
  rtn += "\n\nuse extract-sat -h for more help";
  return(rtn);
}

std::string Usage() {
  std::string rtn = "extract-sat <WFF> <SAT-Solution>";
  rtn += "\n<WFF> is the original file of the solved SAT problem";
  rtn += "\n<SAT-Solution> is the SAT problem's solution file";
  rtn += "\n  Note that <SAT-Solution> must contain only a list of integers:";
  rtn += "\n    Example:  1 -2 3 4 -5 ...";
  return(rtn);
}


//========
// main()
//========
int main(int argc, char* argv[]) {
  try {
    if ( argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help" ) ) {
      std::cout << Usage() << std::endl;
      return(EXIT_SUCCESS);
    } else if ( argc != 3 ) {
      throw(std::string("Wrong # arguments"));
    } else if ( std::string(argv[2]) == "-h" || std::string(argv[2]) == "--help" ) {
      std::cout << Usage() << std::endl;
      return(EXIT_SUCCESS);
    }
  
    std::ifstream factorFile(argv[1]), solutionFile(argv[2]);
    if ( !factorFile ) {
      throw(std::string("Can't find WFF input file: ") + argv[1]);
    } else if ( !solutionFile ) {
      throw(std::string("Can't find solution file: ") + argv[2]);
    }
  
    // Read in WFF input file; determine its "half size"
    std::string toFind = "c half size = ";
    int multSize = -1;
    ByLine tmp;
    while ( factorFile >> tmp ) {
      std::string::size_type pos = tmp.find(toFind);
      if ( pos != std::string::npos ) {
        pos += toFind.size();
        std::string stmp = tmp.substr(pos);
        if ( stmp.find_first_not_of("0123456789") != std::string::npos ) {
          throw(std::string("Could not find half size value in WFF input file"));
        }
        std::stringstream convert(stmp);
        convert >> multSize;
        break;
      }
    } // while

    if ( multSize <= 0 )
      throw(std::string("Unable to find: '") + toFind + std::string("' in WFF input file"));

    // Read in solution file; find "solution ="; using the "half size", 
    // construct the two found factors and display them (in binary)
    std::vector<int> mult1, mult2;
    std::vector<std::string> fileContents;
    std::istream_iterator<ByLine> eof, sf(solutionFile);
    std::copy(sf, eof, std::back_inserter(fileContents));
    std::vector<std::string>::iterator i = fileContents.begin();
    toFind = "solution =";
    std::string solution1, solution2;
    while ( i != fileContents.end() ) {
      std::string stmp = *i;
      RemoveFrontSpace(stmp);
      std::string::size_type pos = stmp.find(toFind);
      if ( 1 == fileContents.size() && pos == std::string::npos ) {
        std::string nums = " -0123456789";
        if ( !stmp.empty() && stmp.find_first_not_of(nums) == std::string::npos ) {
          pos = 0;
          toFind = "";
        }
      }

      if ( pos != std::string::npos ) {
        pos += toFind.size();
        stmp = stmp.substr(pos);
        std::vector<std::string> split = SplitString(stmp, ' ');
        if ( split.size() < static_cast<std::size_t>(multSize * 2) )
          throw(std::string("Size of 'Multiple' given > # literals in solution??? - something is wrong"));
  
        int val;
        std::stringstream toConvert(stmp);
        for ( int idx = 0; idx < multSize; ++idx ) {
          toConvert >> val;
          mult1.push_back(val);
        } // for
  
        for ( int idx = multSize; idx < multSize * 2; ++idx ) {
          toConvert >> val;
          mult2.push_back(val);
        } // for
  
        std::reverse(mult1.begin(), mult1.end());
        std::reverse(mult2.begin(), mult2.end());
        std::vector<int>::iterator iter = mult1.begin();
        while ( iter != mult1.end() ) {
          if ( *iter > 0 )
            solution1 += "1";
          else
            solution1 += "0";
          ++iter;
        } // while
  
        iter = mult2.begin();
        while ( iter != mult2.end() ) {
          if ( *iter > 0 )
            solution2 += "1";
          else
            solution2 += "0";
          ++iter;
        } // while
        break;
      }
      ++i;
    } // while
  
    if ( !solution1.empty() ) {
      std::cout << "First Factor:  " << solution1 << std::endl;
      std::cout << "Second Factor: " << solution2 << std::endl;
    } else {
      std::cout << "No Solution Found: Number is PRIME" << std::endl;
    }

    return(EXIT_SUCCESS);
  } catch(std::string& s) {
    std::cerr << Error() << std::endl;
    std::cerr << s << std::endl;
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown runtime error" << std::endl;
  }
  return(EXIT_FAILURE);
}


//===============================
// simple string cleanup helpers
//===============================
void RemoveBackSpace(std::string& s) {
  bool done = false;
  while ( ! done ) {
    if ( s.empty() ) break;
    if ( s[s.size()-1] == ' ' ) {
      if ( s.size() > 1 )
        s = s.substr(0, s.size()-1);
      else
        s = "";
    }
    else 
      break;
  } // while
}

void RemoveFrontSpace(std::string& s) {
  bool done = false;
  while ( ! done ) {
    if ( s.empty() ) 
      break;
    if ( s[0] == ' ' ) 
      s = s.substr(1);
    else 
      break;
  } // while
}

void RemoveFrontBackSpace(std::string& s) {
  RemoveBackSpace(s);
  RemoveFrontSpace(s);
}

void RemoveTabs(std::string& s) {
  bool done = false;
  std::string::size_type idx = 0;
  while ( ! done ) {
    if ( s.empty() ) break;
    if ( s[idx] == '\t' ) {
      s = s.substr(0, idx) + s.substr(idx+1);
      idx = 0;
    }
    if ( ++idx == s.size() ) break;
  } // while
}

std::vector<std::string> SplitString(const std::string& s, char delim) {
  // Split string around each delim instance
  std::vector<std::string> toRtn;
  if ( s.empty() )
    return(toRtn);
    
  std::size_t j = 0, i = 0;
  for ( ; i < s.size(); ) {
    if ( s[i] == delim ) {
      if ( 0 != i ) 
        toRtn.push_back(s.substr(j, i-j));
      j = ++i;         
    }
    else
      ++i;
  } // for
  if ( j != i )
    toRtn.push_back(s.substr(j));
 
  // Remove leading/trailing spaces, any tabs and any newlines
  std::vector<std::string>::iterator k = toRtn.begin();
  while ( k != toRtn.end() ) {
    RemoveTabs(*k);
    RemoveFrontBackSpace(*k);
    if ( ! k->empty() )
      ++k;
    else {
      toRtn.erase(k);
      k = toRtn.begin();
    }
  } // while
  return(toRtn);
}
