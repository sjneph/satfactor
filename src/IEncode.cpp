/*
  -----ifactor-----
  Creator: Shane J. Neph
  Input:   Binary number N that you want to factor
  Output:  WFF whereby a solution represents 2 divisors of N, whose
           product is exactly N

  If N is prime, then the output from ifactor is unsatisfiable
  Note that N*1=N is not a possible solution
*/


#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


enum { EndClause = 0 };

struct Clause {
  typedef std::set<long> LitType;

  explicit Clause(const LitType& literals) : lits_(literals)
  { /* */ }

  operator LitType() const { return(lits_); }

private:
  LitType lits_;
};

namespace { // unnamed
  typedef std::vector<Clause> Container;
  Container outputCNF;
  bool BAD = false;

  struct UseAbs {
    bool operator()(long a, long b) const {
      return(abs(a) < abs(b));
    }
  };

  std::vector<long> ConvertBinary(const std::string& str) {
    if ( str.find_first_not_of("01") != std::string::npos ) {
      std::cerr << "Non Binary # passed in" << std::endl;
      throw(BAD);
    }
    std::vector<long> toRtn;
    for ( std::size_t idx = 0; idx < str.size(); ++idx ) {
      std::string next;
      next += str[idx];
      std::stringstream convert(next);
      long value;
      convert >> value;
      toRtn.push_back(value);
    } // for
    std::reverse(toRtn.begin(), toRtn.end());
    return(toRtn);
  }

  void KeepCNF(const Container& v) {
    std::copy(v.begin(), v.end(), std::back_inserter(outputCNF));
  }

  void KeepCNF(const Clause& c) {
    outputCNF.push_back(c);
  }

  void KeepCNF(long unitResolution) {
    Clause::LitType lt;
    lt.insert(unitResolution);
    Clause tmp(lt);
    outputCNF.push_back(tmp);
  }

  void OutputCNF(std::ostream& os, long halfSize) {
    std::set<long> toSort;
    Container::const_iterator i = outputCNF.begin();
    while ( i != outputCNF.end() ) {
      Clause::LitType tmp = *i++;
      std::copy(tmp.begin(), tmp.end(),
                std::inserter(toSort, toSort.begin()));
    } // while
    toSort.erase(toSort.begin(), toSort.lower_bound(0));

    os << "c half size = " << halfSize << std::endl;
    os << "p cnf " << toSort.size() << " " << outputCNF.size() << std::endl;
    i = outputCNF.begin();
    while ( i != outputCNF.end() ) {
      Clause::LitType lt = *i;
      std::copy(lt.begin(), lt.end(),
                std::ostream_iterator<long>(os, " "));
      os << 0 << std::endl;
      ++i;
    } // while
  }
} // unnamed


struct NotGreaterEqualN {
  typedef std::vector<long> Values;

  // Implementation: !(inputs > outputs) or !(+multiplicand > +product)
  NotGreaterEqualN(long start, long end,
                   const Values& outputs, long& nextValue) {
    Values inputs;
    for ( long x = start; x <= end; ++x )
      inputs.push_back(x);

    if ( inputs.size() != outputs.size() || outputs.empty() ) {
      std::cerr << "Bad Args: NotGreaterEqualN Construction" << std::endl;
      throw(BAD);
    }

    Clause::LitType lt;
    Values cache;
    long sz = static_cast<long>(inputs.size()) - 1;
    for ( long idx = sz; idx >= 0; --idx ) {
      lt.clear();
      lt.insert(-inputs[idx]);
      lt.insert(outputs[idx]);
      std::copy(cache.begin(), cache.end(),
                std::inserter(lt, lt.begin()));
      KeepCNF(Clause(lt));
      cache.push_back(-(++nextValue));

      lt.clear();
      lt.insert(-nextValue);
      lt.insert(-inputs[idx]);
      lt.insert(outputs[idx]);
      KeepCNF(Clause(lt));

      lt.clear();
      lt.insert(-nextValue);
      lt.insert(inputs[idx]);
      lt.insert(-outputs[idx]);
      KeepCNF(Clause(lt));

      lt.clear();
      lt.insert(nextValue);
      lt.insert(inputs[idx]);
      lt.insert(outputs[idx]);
      KeepCNF(Clause(lt));

      lt.clear();
      lt.insert(nextValue);
      lt.insert(-inputs[idx]);
      lt.insert(-outputs[idx]);
      KeepCNF(Clause(lt));
    } // for

    // Ensure we do not get 1*N=N solutions
    lt.clear();
    for ( long i = start+1; i <= end; ++i )
      lt.insert(i);
    lt.insert(-start);
    KeepCNF(Clause(lt));
  }
};


struct FullAdder {
  /*
     Note that this FullAdder is specific to this application and
     is not generalized.  In particular, x (carry-in) may be zero but
     it cannot be 1, meaning asserted, as 1 represents a literal.
  */
  FullAdder() : ok_(false) { /* */ };
  FullAdder(long x, long y, long z)
             : x_(x), y_(y), z_(z), ok_(true) { /* */ };

  void Set(long x, long y, long z) {
    x_ = x;
    y_ = y;
    z_ = z;
    ok_ = true;
  }

  std::pair<Container, Container> Equivalence(long sVar, long cVar) {
    if ( !ok_ ) {
      std::cerr << "FullAdder::Equivalence(): uninitialized" << std::endl;
      throw(BAD);
    }

    // Remember 'x_' represents the carry-input

    std::pair<Container, Container> toRtn;
    Container first, second;
    Clause::LitType cont;

    // S-out
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(-sVar); cont.insert(y_); cont.insert(z_);
    Clause c1(cont); first.push_back(c1);

    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(-sVar); cont.insert(-y_); cont.insert(-z_);
    Clause c2(cont); first.push_back(c2);
 
    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(-sVar); cont.insert(y_); cont.insert(-z_);
      cont.insert(-x_);
      Clause c3(cont); first.push_back(c3);
    }

    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(-sVar); cont.insert(-y_); cont.insert(z_);
      cont.insert(-x_);
      Clause c4(cont); first.push_back(c4);
    }

    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(sVar); cont.insert(y_), cont.insert(z_);
      cont.insert(-x_);
      Clause c5(cont); first.push_back(c5);
    }

    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(sVar); cont.insert(-y_); cont.insert(z_);
    Clause c6(cont); first.push_back(c6);

    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(sVar); cont.insert(-y_); cont.insert(-z_);
      cont.insert(-x_);
      Clause c7(cont); first.push_back(c7);
    }

    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(sVar); cont.insert(y_); cont.insert(-z_);
    Clause c8(cont); first.push_back(c8);


    // C-out
    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(-cVar); cont.insert(y_);
    Clause c9(cont); second.push_back(c9);

    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(-cVar); cont.insert(z_);
    Clause c10(cont); second.push_back(c10);

    cont.clear();
    cont.insert(-cVar); cont.insert(y_); cont.insert(z_);
    Clause c11(cont); second.push_back(c11);

    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(cVar); cont.insert(y_); cont.insert(-z_);
      cont.insert(-x_); 
      Clause c12(cont); second.push_back(c12);
    }

    cont.clear();
    if ( 0 != x_ )
      cont.insert(x_);
    cont.insert(cVar); cont.insert(-y_); cont.insert(-z_);
    Clause c13(cont); second.push_back(c13);

    if ( 0 != x_ ) {
      cont.clear();
      cont.insert(cVar); cont.insert(-y_);
      cont.insert(-x_);
      Clause c14(cont); second.push_back(c14);
    }

    return(std::make_pair(first, second));
  }

private:
  long x_, y_, z_;
  bool ok_;
};


struct BitAdder {
  typedef std::vector<long> IOType;

  BitAdder(const IOType& it, long lastVar, long inCarry = 0) 
                                            : nextVar_(lastVar) {
    if ( it.empty() || (it.size() % 2) ) {
      std::cerr << "BitAdder::Constructor --> BadArg" << std::endl;
      throw(BAD);
    }

    std::size_t sz = it.size();
    FullAdder toAdd(inCarry, it[0], it[1]);
    typedef std::pair<Container, Container> PType;
    long Sout = ++nextVar_, Cout = ++nextVar_;
    PType pType = toAdd.Equivalence(Sout, Cout);
    KeepCNF(pType.first); KeepCNF(pType.second);

    outputs_.push_back(Sout); // sout's
    for ( std::size_t i = 2; i < sz; i += 2 ) {
      toAdd = FullAdder(Cout, it[i], it[i+1]);
      Sout = ++nextVar_; Cout = ++nextVar_;
      pType = toAdd.Equivalence(Sout, Cout);
      KeepCNF(pType.first); KeepCNF(pType.second);
      outputs_.push_back(Sout);
    } // for
    outputs_.push_back(Cout);
  }

  long NextVar() const {
    return(nextVar_);
  }

  IOType Output() const {
    return(outputs_);
  }

private:
  std::vector<FullAdder> comp_;
  IOType outputs_;
  long nextVar_;
};


struct CreateMultiplier {
  explicit CreateMultiplier(std::vector<long> soln) : nextVar_(0) {
    if ( soln.empty() ) {
      std::cerr << "CreateMultiplier::Constructor: BadArg" << std::endl;
      throw(BAD);
    }
       
    // Make multiplier inputs
    std::size_t multBits = soln.size();
    std::vector<long> Ax(multBits), Bx(multBits);
    for ( std::size_t idx = 0; idx < multBits; ++idx )
      Ax[idx] = ++nextVar_;
    for ( std::size_t idx = 0; idx < multBits; ++idx )
      Bx[idx] = ++nextVar_;
    max_ = nextVar_;

    // 'first' represents the fact that the input to the first bit adder
    //  comes from a set of AND'ed results, while any other bit adder
    //  receives half of its inputs from the preceding bit adder.
    long nextSoln = 0;
    bool first = true;
    std::vector<long> outputs;
    BitAdder::IOType inputs;
    for ( std::size_t idx = 0; idx < multBits; ++idx ) {
      long marker = nextVar_ + 1;
      for ( std::size_t inner = 0; inner < multBits; ++inner ) {
        makeAndEq(Ax[idx], Bx[inner]);
        if ( !idx && !inner ) {
          outputs.push_back(nextVar_);
          setValue(nextVar_, soln[nextSoln++]);
        }
      } // for

      if ( !idx )
        ++marker;
      for ( long x = marker; x <= nextVar_; ++x )
        inputs.push_back(x);

      if ( !idx ) {
        inputs.push_back(++nextVar_);
        setValue(nextVar_, 0);
      }
      else
        first = false;

      if ( !first ) {
        // Re-arrange inputs as expected by BitAdder
        BitAdder::IOType tmp(inputs);
        std::size_t half = tmp.size() / 2, cntr = 0;
        for ( std::size_t tIdx = 0; tIdx < half; ++tIdx ) {
          inputs[tIdx + cntr]     = tmp[tIdx];
          inputs[tIdx + 1 + cntr] = tmp[tIdx + half];
          ++cntr;
        } // for

        // Add inputs to a BitAdder and grab results
        BitAdder ba(inputs, nextVar_);
        nextVar_ = ba.NextVar();
        BitAdder::IOType out = ba.Output();
        setValue(out[0], soln[nextSoln++]);
        outputs.push_back(out[0]);
        inputs.clear();
        BitAdder::IOType::iterator iter = ++out.begin();
        std::copy(iter, out.end(), std::back_inserter(inputs));
      }
    } // for

    // Declare values of remaining bits of soln
    //   --> comes from output of last BitAdder
    BitAdder::IOType::iterator iterA = inputs.begin();
    while ( iterA != inputs.end() ) {
      if ( nextSoln < static_cast<long>(soln.size()) )
        setValue(*iterA, soln[nextSoln++]);
      else
        setValue(*iterA, 0);
      ++iterA;
    } // while
    NotGreaterEqualN m1gen(Ax[0], Ax[multBits-1], outputs, nextVar_);
    NotGreaterEqualN m2gen(Bx[0], Bx[multBits-1], outputs, nextVar_);
  }

  long GetTotalInputBits() const {
    return(max_);
  }

private:
  void makeAndEq(long a, long b) {
    ++nextVar_;
    Clause::LitType lt;
    lt.insert(-nextVar_); lt.insert(a);
    Clause c1(lt);
    lt.clear();

    lt.insert(-nextVar_); lt.insert(b);
    Clause c2(lt);
    lt.clear();

    lt.insert(-a); lt.insert(-b); lt.insert(nextVar_);
    Clause c3(lt);

    Container cont;
    cont.push_back(c1);
    cont.push_back(c2);
    cont.push_back(c3);
    KeepCNF(cont);
  }

  void setValue(long var, long value) {
    if ( value )
      KeepCNF(var);
    else
      KeepCNF(-var);
  }

private:
  long nextVar_, max_;
};


//========
// main()
//========
int main(int argc, char* argv[])
{
  try {
    if ( argc != 2 ) {
      std::cerr << "Wrong # parameters - expect 1 binary integer" << std::endl;
      std::cerr << "Example: iencode 10101" << std::endl;
      std::cerr << "  to encode decimal 21 as a SATISFIABILITY problem." << std::endl;
      std::cerr << "use iencode -h for help" << std::endl;
      throw(BAD);
    } else if ( argv[1] == std::string("--help") || argv[1] == std::string("-h") ) {
      std::cout << "Expect single integer input in binary form" << std::endl;
      std::cout << "Example: iencode 10101" << std::endl;
      std::cout << "  to encode decimal 21 as a SATISFIABILITY problem." << std::endl;
      return(EXIT_SUCCESS);
    }
    std::vector<long> soln = ConvertBinary(std::string(argv[1]));
    CreateMultiplier fm(soln);

    OutputCNF(std::cout, fm.GetTotalInputBits() / 2);
    return(EXIT_SUCCESS);
  } catch(bool) {
    /* already output error msg */
  } catch(...) {
    std::cerr << "Unknown runtime error" << std::endl;
  }
  return(EXIT_FAILURE);
}


