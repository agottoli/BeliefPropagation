#ifndef HUGINPARSER_H
#define HUGINPARSER_H
#include "JavaCC.h"
#include "CharStream.h"
#include "Token.h"
#include "TokenManager.h"
#include "iostream"
#include "../BayesianNetwork.h"
#include "../States.h"
#include "stdio.h"
#include "stdlib.h"
#include "sstream"

#include "HuginParserConstants.h"
#include "ErrorHandler.h"
namespace hugin_parser {
  struct JJCalls {
    int gen;
    Token *first;
    int arg;
    JJCalls *next;
    ~JJCalls() { if (next) delete next; }
    JJCalls() { next = NULL; arg = 0; gen = -1; first = NULL; }
  };

class HuginParser {
   public:

// define the parser

/*
* Productions
*/
void Start();

void Net();

void Node();

void Potential();
  public: TokenManager *token_source;
  public: CharStream *jj_input_stream;
  /** Current token. */
  public: Token *token;
  /** Next token. */
  public: Token *jj_nt;
  private: int jj_ntk;
  private: JJCalls jj_2_rtns[1];
  private: bool jj_rescan;
  private: int jj_gc;
  private: Token *jj_scanpos, *jj_lastpos;
  private: int jj_la;
  /** Whether we are looking ahead. */
  private: bool jj_lookingAhead;
  private: bool jj_semLA;
  private: int jj_gen;
  private: int jj_la1[16];
  private: ErrorHandler *errorHandler;
  private: bool errorHandlerCreated;
  public: void setErrorHandler(ErrorHandler *eh) {
    if (errorHandlerCreated) delete errorHandler;
    errorHandler = eh;
    errorHandlerCreated = false;
  }
 Token *head; 
 public: 

 HuginParser(TokenManager *tm);
   public: virtual ~HuginParser();

void ReInit(TokenManager *tm);

Token * jj_consume_token(int kind);

Token * getNextToken();

Token * getToken(int index);

int jj_ntk_f();
 private: int jj_kind;
  int **jj_expentries;
  int *jj_expentry;

  /** Generate ParseException. */

protected: virtual void  parseError();
  private: int trace_indent;
  private: bool trace_enabled;
  /** Enable tracing. */

public: void enable_tracing();

public: void disable_tracing();
// campi privati
BayesianNetwork* net;
std::unordered_map<std::string, Variable*> vs;
States* states;
Variable* var;
VecMap* vcp;
//std::vector<double>* cpTable;
//double* cpTable;


BayesianNetwork* getBayesianNetwork() {
        BayesianNetwork* currentNet = net;
        net = new BayesianNetwork();
        vs.clear();
        return currentNet;
}private: bool jj_done;

};
}
#endif
