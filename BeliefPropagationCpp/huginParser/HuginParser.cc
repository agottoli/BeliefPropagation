/* HuginParser.cc */
#include "./HuginParser.h"
namespace hugin_parser {
  unsigned int jj_la1_0[] = {
0x100000,0x800000,0x200,0x20000,0x80000,0x20000,0x200000,0x40000,0x0,0x0,0x800,0x7000,0x7000,0x8000,0x800,};
  unsigned int jj_la1_1[] = {
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x200,0x400,0x0,0x0,0x0,0x0,0x0,};

  /** Constructor with user supplied TokenManager. */



// define the parser

/*
* Productions
*/
void HuginParser::Start() {
    Net();
    while (true) {
      Node();
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case NODE:{
        ;
        break;
        }
      default:
        jj_la1[0] = jj_gen;
        goto end_label_1;
      }
    }
    end_label_1: ;
    while (true) {
      Potential();
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case POTENTIAL:{
        ;
        break;
        }
      default:
        jj_la1[1] = jj_gen;
        goto end_label_2;
      }
    }
    end_label_2: ;
    jj_consume_token(0);

  }


void HuginParser::Net() {
    jj_consume_token(NET);
    jj_consume_token(LPARG);
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case NODESIZE:{
      jj_consume_token(NODESIZE);
      jj_consume_token(EQUAL);
      jj_consume_token(LPAR);
      jj_consume_token(INTEGER);
      jj_consume_token(INTEGER);
      jj_consume_token(RPAR);
      jj_consume_token(SEMICOLON);
      break;
      }
    default:
      jj_la1[2] = jj_gen;
      ;
    }
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LABEL:{
      jj_consume_token(LABEL);
      jj_consume_token(EQUAL);
      jj_consume_token(DOUBLEQUOTE);
      jj_consume_token(ANYTHING);
      jj_consume_token(DOUBLEQUOTE_EXIT);
      jj_consume_token(SEMICOLON);
      break;
      }
    default:
      jj_la1[3] = jj_gen;
      ;
    }
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case NAME:{
      jj_consume_token(NAME);
      jj_consume_token(EQUAL);
      jj_consume_token(DOUBLEQUOTE);
      jj_consume_token(ANYTHING);
      jj_consume_token(DOUBLEQUOTE_EXIT);
      jj_consume_token(SEMICOLON);
      break;
      }
    default:
      jj_la1[4] = jj_gen;
      ;
    }
    jj_consume_token(RPARG);
net = new BayesianNetwork();
  }


void HuginParser::Node() {Token* node;
        Token* state;
    jj_consume_token(NODE);
    node = jj_consume_token(ANYTHING_EXIT);
    jj_consume_token(LPARG);
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case LABEL:{
      jj_consume_token(LABEL);
      jj_consume_token(EQUAL);
      jj_consume_token(DOUBLEQUOTE);
      jj_consume_token(ANYTHING);
      jj_consume_token(DOUBLEQUOTE_EXIT);
      jj_consume_token(SEMICOLON);
      break;
      }
    default:
      jj_la1[5] = jj_gen;
      ;
    }
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case POSITION:{
      jj_consume_token(POSITION);
      jj_consume_token(EQUAL);
      jj_consume_token(LPAR);
      jj_consume_token(INTEGER);
      jj_consume_token(INTEGER);
      jj_consume_token(RPAR);
      jj_consume_token(SEMICOLON);
      break;
      }
    default:
      jj_la1[6] = jj_gen;
      ;
    }
    jj_consume_token(STATES);
states = new States(5);
    jj_consume_token(EQUAL);
    jj_consume_token(LPAR);
    while (true) {
      jj_consume_token(DOUBLEQUOTE);
      state = jj_consume_token(ANYTHING);
states->push_back(state->toString());
      jj_consume_token(DOUBLEQUOTE_EXIT);
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case DOUBLEQUOTE:{
        ;
        break;
        }
      default:
        jj_la1[7] = jj_gen;
        goto end_label_3;
      }
    }
    end_label_3: ;
    jj_consume_token(RPAR);
    jj_consume_token(SEMICOLON);
    jj_consume_token(RPARG);
var = new Variable(node->toString(), states); net->addVariable(var); vs[node->toString()] = var;
  }


void HuginParser::Potential() {Token* t;
        Variable* var;
        Variable* father;
        vcp = new VecMap(1); // da dare una dimensione
        std::size_t dim = 1;
        //std::vector<double>* cpTable;
        double* cpTable;
        Probability* prob;
        //ProbabilityMD* prob;
        std::size_t i = 0;
    jj_consume_token(POTENTIAL);
    jj_consume_token(LPAR_P);
    t = jj_consume_token(ANYTHING_P);
var = vs[t->toString()];
                dim *= var->getStates()->size();
    switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
    case PIPE_P:{
      jj_consume_token(PIPE_P);
      while (true) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case ANYTHING_P:{
          ;
          break;
          }
        default:
          jj_la1[8] = jj_gen;
          goto end_label_4;
        }
        t = jj_consume_token(ANYTHING_P);
father = vs[t->toString()];
                dim *= father->getStates()->size();
                vcp->push_back(father);
                Variable::linkFatherToSon(father, var);
      }
      end_label_4: ;
      break;
      }
    default:
      jj_la1[9] = jj_gen;
      ;
    }
    jj_consume_token(RPAR_P_EXIT);
vcp->push_back(var); // la variabile a cui ci si riferisce è sempre l'ultima che inserisco

                // Probability
                //cpTable = new std::vector<double>();
                //cpTable->reserve(dim);
                cpTable = new double[dim];
                // ProbabilityMD
                //prob = new ProbabilityMD(vcp);
                //

    jj_consume_token(LPARG);
    jj_consume_token(DATA);
    jj_consume_token(EQUAL);
    while (true) {
      while (true) {
        jj_consume_token(LPAR);
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case LPAR:{
          ;
          break;
          }
        default:
          jj_la1[10] = jj_gen;
          goto end_label_6;
        }
      }
      end_label_6: ;
      while (true) {
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case INTEGER:{
          t = jj_consume_token(INTEGER);
          break;
          }
        case DOUBLE:{
          t = jj_consume_token(DOUBLE);
          break;
          }
        case DOUBLEE:{
          t = jj_consume_token(DOUBLEE);
          break;
          }
        default:
          jj_la1[11] = jj_gen;
          jj_consume_token(-1);
          errorHandler->handleParseError(token, getToken(1), __FUNCTION__, this);
        }
std::stringstream ss (t->toString());
                        double d;
                        ss >> d;

                        // Probability
                        //cpTable->push_back(d);
                        cpTable[i++] = d;
                        // ProbabilityMD
                        //prob->push_back(d);
                        //

        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case INTEGER:
        case DOUBLE:
        case DOUBLEE:{
          ;
          break;
          }
        default:
          jj_la1[12] = jj_gen;
          goto end_label_7;
        }
      }
      end_label_7: ;
      while (true) {
        jj_consume_token(RPAR);
        switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
        case RPAR:{
          ;
          break;
          }
        default:
          jj_la1[13] = jj_gen;
          goto end_label_8;
        }
      }
      end_label_8: ;
      switch ((jj_ntk==-1)?jj_ntk_f():jj_ntk) {
      case LPAR:{
        ;
        break;
        }
      default:
        jj_la1[14] = jj_gen;
        goto end_label_5;
      }
    }
    end_label_5: ;
    jj_consume_token(SEMICOLON);
    jj_consume_token(RPARG);
// setto la probabilità condizionale
                // Probability
                prob = new Probability(vcp, cpTable, dim);
                //

                var->setConditionalProbability(prob);
                //std::cout << "tabella: " << p->toString();

                //std::string stringa;
                //std::cin >> stringa;

  }


 HuginParser::HuginParser(TokenManager *tm){
    head = NULL;
    ReInit(tm);
}
   HuginParser::~HuginParser()
{
  if (token_source) delete token_source;
  if (head) {
    Token *next, *t = head;
    while (t) {
      next = t->next;
      delete t;
      t = next;
    }
  }
  if (errorHandlerCreated) {
    delete errorHandler;
  }
}

void HuginParser::ReInit(TokenManager *tm){
    if (head) delete head;
    errorHandler = new ErrorHandler();
    errorHandlerCreated = true;
    token_source = tm;
    head = token = new Token();
    token->kind = 0;
    token->next = NULL;
    jj_lookingAhead = false;
    jj_rescan = false;
    jj_done = false;
    jj_scanpos = jj_lastpos = NULL;
    jj_gc = 0;
    jj_kind = -1;
    trace_indent = 0;
    trace_enabled = false;
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 15; i++) jj_la1[i] = -1;
  }


Token * HuginParser::jj_consume_token(int kind)  {
    Token *oldToken;
    if ((oldToken = token)->next != NULL) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    if (token->kind == kind) {
      jj_gen++;
      return token;
    }
    token = oldToken;
    jj_kind = kind;
    JAVACC_STRING_TYPE image = kind >= 0 ? tokenImage[kind] : tokenImage[0];
    errorHandler->handleUnexpectedToken(kind, image.substr(1, image.size() - 2), getToken(1), this);
    return token;
  }


/** Get the next Token. */

Token * HuginParser::getNextToken(){
    if (token->next != NULL) token = token->next;
    else token = token->next = token_source->getNextToken();
    jj_ntk = -1;
    jj_gen++;
    return token;
  }

/** Get the specific Token. */

Token * HuginParser::getToken(int index){
    Token *t = token;
    for (int i = 0; i < index; i++) {
      if (t->next != NULL) t = t->next;
      else t = t->next = token_source->getNextToken();
    }
    return t;
  }


int HuginParser::jj_ntk_f(){
    if ((jj_nt=token->next) == NULL)
      return (jj_ntk = (token->next=token_source->getNextToken())->kind);
    else
      return (jj_ntk = jj_nt->kind);
  }


 void  HuginParser::parseError()   {
      fprintf(stderr, "Parse error at: %d:%d, after token: %s encountered: %s\n", token->beginLine, token->beginColumn, addUnicodeEscapes(token->image).c_str(), addUnicodeEscapes(getToken(1)->image).c_str());
   }


 void HuginParser::enable_tracing()  {
  }

  /** Disable tracing. */

 void HuginParser::disable_tracing()  {
  }


}
