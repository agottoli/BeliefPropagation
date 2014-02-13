#ifndef HUGINPARSERTOKENMANAGER_H
#define HUGINPARSERTOKENMANAGER_H
#include "JavaCC.h"
#include "CharStream.h"
#include "Token.h"
#include "TokenManager.h"
#include "HuginParserConstants.h"
namespace hugin_parser {
class HuginParser;

/** Token Manager. */
class HuginParserTokenManager : public TokenManager {
   public:

  /** Debug output. */
  FILE *debugStream;
  /** Set debug output. */

void  setDebugStream(FILE *ds);

 int jjStopStringLiteralDfa_0(int pos, unsigned long long active0);

int  jjStartNfa_0(int pos, unsigned long long active0);

 int  jjStopAtPos(int pos, int kind);

 int  jjMoveStringLiteralDfa0_0();

 int  jjMoveStringLiteralDfa1_0(unsigned long long active0);

 int  jjMoveStringLiteralDfa2_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa3_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa4_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa5_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa6_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa7_0(unsigned long long old0, unsigned long long active0);

 int  jjMoveStringLiteralDfa8_0(unsigned long long old0, unsigned long long active0);

int jjMoveNfa_0(int startState, int curPos);

 int jjStopStringLiteralDfa_1(int pos, unsigned long long active0);

int  jjStartNfa_1(int pos, unsigned long long active0);

 int  jjMoveStringLiteralDfa0_1();

int jjMoveNfa_1(int startState, int curPos);

 int jjStopStringLiteralDfa_3(int pos, unsigned long long active0);

int  jjStartNfa_3(int pos, unsigned long long active0);

 int  jjMoveStringLiteralDfa0_3();

int jjMoveNfa_3(int startState, int curPos);

 int jjStopStringLiteralDfa_2(int pos, unsigned long long active0);

int  jjStartNfa_2(int pos, unsigned long long active0);

 int  jjMoveStringLiteralDfa0_2();

int jjMoveNfa_2(int startState, int curPos);

bool jjCanMove_0(int hiByte, int i1, int i2, unsigned long long l1, unsigned long long l2);

Token * jjFillToken();

public: int curLexState;
public: int jjnewStateCnt;
public: int jjround;
public: int jjmatchedPos;
public: int jjmatchedKind;


Token * getNextToken();

void jjCheckNAdd(int state);

void jjAddStates(int start, int end);

void jjCheckNAddTwoStates(int state1, int state2);

void jjCheckNAddStates(int start, int end);
  private: HuginParser*parser;
  private: void ReInitRounds();
  public: HuginParserTokenManager(CharStream *stream, int lexState = 0, HuginParser *parserArg = NULL);
  public: virtual ~HuginParserTokenManager();
  void ReInit(CharStream *stream, int lexState = 0, HuginParser *parserArg = NULL);
  void SwitchTo(int lexState);
  const JAVACC_SIMPLE_STRING jjKindsForBitVector(int i, unsigned long long vec);
  const JAVACC_SIMPLE_STRING jjKindsForStateVector(int lexState, int vec[], int start, int end);
    CharStream  *input_stream;
    int jjrounds[22];
    int jjstateSet[2 * 22];
    JAVACC_STRING_TYPE jjimage;
    JAVACC_STRING_TYPE image;
    int jjimageLen;
    int lengthOfMatch;
    JAVACC_CHAR_TYPE curChar;
};
}
#endif
