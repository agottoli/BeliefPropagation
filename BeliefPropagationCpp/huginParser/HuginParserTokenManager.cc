/* HuginParserTokenManager.cc */
#include "./HuginParserTokenManager.h"
namespace hugin_parser {
static const unsigned long long jjbitVec0[] = {
   0xfffffffffffffffeULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const unsigned long long jjbitVec2[] = {
   0x0ULL, 0x0ULL, 0xffffffffffffffffULL, 0xffffffffffffffffULL
};
static const int jjnextStates[] = {
   12, 13, 14, 16, 17, 1, 2, 4, 6, 7, 9, 
};
static JAVACC_CHAR_TYPE jjstrLiteralChars_0[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_1[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_2[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_3[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_4[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_5[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_6[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_7[] = {0x6e, 0x65, 0x74, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_8[] = {0x7b, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_9[] = {0x6e, 0x6f, 0x64, 0x65, 0x5f, 0x73, 0x69, 0x7a, 0x65, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_10[] = {0x3d, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_11[] = {0x28, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_12[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_13[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_14[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_15[] = {0x29, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_16[] = {0x3b, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_17[] = {0x6c, 0x61, 0x62, 0x65, 0x6c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_18[] = {0x22, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_19[] = {0x6e, 0x61, 0x6d, 0x65, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_20[] = {0x6e, 0x6f, 0x64, 0x65, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_21[] = {0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_22[] = {0x73, 0x74, 0x61, 0x74, 0x65, 0x73, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_23[] = {0x70, 0x6f, 0x74, 0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_24[] = {0x64, 0x61, 0x74, 0x61, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_25[] = {0x7d, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_26[] = {0x22, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_27[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_28[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_29[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_30[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_31[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_32[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_33[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_34[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_35[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_36[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_37[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_38[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_39[] = {0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_40[] = {0x28, 0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_41[] = {0};
static JAVACC_CHAR_TYPE jjstrLiteralChars_42[] = {0x7c, 0};

static JAVACC_CHAR_TYPE jjstrLiteralChars_43[] = {0x29, 0};
static const JAVACC_STRING_TYPE jjstrLiteralImages[] = {
jjstrLiteralChars_0, 
jjstrLiteralChars_1, 
jjstrLiteralChars_2, 
jjstrLiteralChars_3, 
jjstrLiteralChars_4, 
jjstrLiteralChars_5, 
jjstrLiteralChars_6, 
jjstrLiteralChars_7, 
jjstrLiteralChars_8, 
jjstrLiteralChars_9, 
jjstrLiteralChars_10, 
jjstrLiteralChars_11, 
jjstrLiteralChars_12, 
jjstrLiteralChars_13, 
jjstrLiteralChars_14, 
jjstrLiteralChars_15, 
jjstrLiteralChars_16, 
jjstrLiteralChars_17, 
jjstrLiteralChars_18, 
jjstrLiteralChars_19, 
jjstrLiteralChars_20, 
jjstrLiteralChars_21, 
jjstrLiteralChars_22, 
jjstrLiteralChars_23, 
jjstrLiteralChars_24, 
jjstrLiteralChars_25, 
jjstrLiteralChars_26, 
jjstrLiteralChars_27, 
jjstrLiteralChars_28, 
jjstrLiteralChars_29, 
jjstrLiteralChars_30, 
jjstrLiteralChars_31, 
jjstrLiteralChars_32, 
jjstrLiteralChars_33, 
jjstrLiteralChars_34, 
jjstrLiteralChars_35, 
jjstrLiteralChars_36, 
jjstrLiteralChars_37, 
jjstrLiteralChars_38, 
jjstrLiteralChars_39, 
jjstrLiteralChars_40, 
jjstrLiteralChars_41, 
jjstrLiteralChars_42, 
jjstrLiteralChars_43, 
};

/** Lexer state names. */
static const JAVACC_CHAR_TYPE lexStateNames_arr_0[] = 
{0x44, 0x45, 0x46, 0x41, 0x55, 0x4c, 0x54, 0};
static const JAVACC_CHAR_TYPE lexStateNames_arr_1[] = 
{0x73, 0x74, 0x61, 0x74, 0x6f, 0x5f, 0x61, 0x6e, 0x79, 0x74, 0x68, 0x69, 0x6e, 0x67, 0};
static const JAVACC_CHAR_TYPE lexStateNames_arr_2[] = 
{0x73, 0x74, 0x61, 0x74, 0x6f, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x6e, 0x6f, 0x64, 0x65, 0};
static const JAVACC_CHAR_TYPE lexStateNames_arr_3[] = 
{0x73, 0x74, 0x61, 0x74, 0x6f, 0x5f, 0x70, 0x6f, 0x74, 0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c, 0};
static const JAVACC_STRING_TYPE lexStateNames[] = {
lexStateNames_arr_0, 
lexStateNames_arr_1, 
lexStateNames_arr_2, 
lexStateNames_arr_3, 
};

/** Lex State array. */
static const int jjnewLexState[] = {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1, 2, -1, -1, 3, -1, 
   -1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0, 
};
static const unsigned long long jjtoToken[] = {
   0xf040fffff81ULL, 
};
static const unsigned long long jjtoSkip[] = {
   0xfbf000007eULL, 
};

void  HuginParserTokenManager::setDebugStream(FILE *ds){ debugStream = ds; }

 int HuginParserTokenManager::jjStopStringLiteralDfa_0(int pos, unsigned long long active0){
   switch (pos)
   {
      default :
         return -1;
   }
}

int  HuginParserTokenManager::jjStartNfa_0(int pos, unsigned long long active0){
   return jjMoveNfa_0(jjStopStringLiteralDfa_0(pos, active0), pos + 1);
}

 int  HuginParserTokenManager::jjStopAtPos(int pos, int kind){
   jjmatchedKind = kind;
   jjmatchedPos = pos;
   return pos + 1;
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa0_0(){
   switch(curChar)
   {
      case 34:
         return jjStopAtPos(0, 18);
      case 40:
         return jjStopAtPos(0, 11);
      case 41:
         return jjStopAtPos(0, 15);
      case 59:
         return jjStopAtPos(0, 16);
      case 61:
         return jjStopAtPos(0, 10);
      case 100:
         return jjMoveStringLiteralDfa1_0(0x1000000ULL);
      case 108:
         return jjMoveStringLiteralDfa1_0(0x20000ULL);
      case 110:
         return jjMoveStringLiteralDfa1_0(0x180280ULL);
      case 112:
         return jjMoveStringLiteralDfa1_0(0xa00000ULL);
      case 115:
         return jjMoveStringLiteralDfa1_0(0x400000ULL);
      case 123:
         return jjStopAtPos(0, 8);
      case 125:
         return jjStopAtPos(0, 25);
      default :
         return jjMoveNfa_0(0, 0);
   }
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa1_0(unsigned long long active0){
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(0, active0);
      return 1;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 97:
         return jjMoveStringLiteralDfa2_0(active0, 0x10a0000ULL);
      case 101:
         return jjMoveStringLiteralDfa2_0(active0, 0x80ULL);
      case 111:
         return jjMoveStringLiteralDfa2_0(active0, 0xb00200ULL);
      case 116:
         return jjMoveStringLiteralDfa2_0(active0, 0x400000ULL);
      default :
         break;
   }
   return jjStartNfa_0(0, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa2_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(0, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(1, active0);
      return 2;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 97:
         return jjMoveStringLiteralDfa3_0(active0, 0x400000ULL);
      case 98:
         return jjMoveStringLiteralDfa3_0(active0, 0x20000ULL);
      case 100:
         return jjMoveStringLiteralDfa3_0(active0, 0x100200ULL);
      case 109:
         return jjMoveStringLiteralDfa3_0(active0, 0x80000ULL);
      case 115:
         return jjMoveStringLiteralDfa3_0(active0, 0x200000ULL);
      case 116:
         if ((active0 & 0x80ULL) != 0L)
            return jjStopAtPos(2, 7);
         return jjMoveStringLiteralDfa3_0(active0, 0x1800000ULL);
      default :
         break;
   }
   return jjStartNfa_0(1, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa3_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(1, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(2, active0);
      return 3;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 97:
         if ((active0 & 0x1000000ULL) != 0L)
            return jjStopAtPos(3, 24);
         break;
      case 101:
         if ((active0 & 0x80000ULL) != 0L)
            return jjStopAtPos(3, 19);
         else if ((active0 & 0x100000ULL) != 0L)
         {
            jjmatchedKind = 20;
            jjmatchedPos = 3;
         }
         return jjMoveStringLiteralDfa4_0(active0, 0x820200ULL);
      case 105:
         return jjMoveStringLiteralDfa4_0(active0, 0x200000ULL);
      case 116:
         return jjMoveStringLiteralDfa4_0(active0, 0x400000ULL);
      default :
         break;
   }
   return jjStartNfa_0(2, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa4_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(2, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(3, active0);
      return 4;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 95:
         return jjMoveStringLiteralDfa5_0(active0, 0x200ULL);
      case 101:
         return jjMoveStringLiteralDfa5_0(active0, 0x400000ULL);
      case 108:
         if ((active0 & 0x20000ULL) != 0L)
            return jjStopAtPos(4, 17);
         break;
      case 110:
         return jjMoveStringLiteralDfa5_0(active0, 0x800000ULL);
      case 116:
         return jjMoveStringLiteralDfa5_0(active0, 0x200000ULL);
      default :
         break;
   }
   return jjStartNfa_0(3, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa5_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(3, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(4, active0);
      return 5;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 105:
         return jjMoveStringLiteralDfa6_0(active0, 0x200000ULL);
      case 115:
         if ((active0 & 0x400000ULL) != 0L)
            return jjStopAtPos(5, 22);
         return jjMoveStringLiteralDfa6_0(active0, 0x200ULL);
      case 116:
         return jjMoveStringLiteralDfa6_0(active0, 0x800000ULL);
      default :
         break;
   }
   return jjStartNfa_0(4, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa6_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(4, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(5, active0);
      return 6;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 105:
         return jjMoveStringLiteralDfa7_0(active0, 0x800200ULL);
      case 111:
         return jjMoveStringLiteralDfa7_0(active0, 0x200000ULL);
      default :
         break;
   }
   return jjStartNfa_0(5, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa7_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(5, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(6, active0);
      return 7;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 97:
         return jjMoveStringLiteralDfa8_0(active0, 0x800000ULL);
      case 110:
         if ((active0 & 0x200000ULL) != 0L)
            return jjStopAtPos(7, 21);
         break;
      case 122:
         return jjMoveStringLiteralDfa8_0(active0, 0x200ULL);
      default :
         break;
   }
   return jjStartNfa_0(6, active0);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa8_0(unsigned long long old0, unsigned long long active0){
   if (((active0 &= old0)) == 0L)
      return jjStartNfa_0(6, old0);
   if (input_stream->endOfInput()) {
      jjStopStringLiteralDfa_0(7, active0);
      return 8;
   }
   curChar = input_stream->readChar();
   switch(curChar)
   {
      case 101:
         if ((active0 & 0x200ULL) != 0L)
            return jjStopAtPos(8, 9);
         break;
      case 108:
         if ((active0 & 0x800000ULL) != 0L)
            return jjStopAtPos(8, 23);
         break;
      default :
         break;
   }
   return jjStartNfa_0(7, active0);
}

int HuginParserTokenManager::jjMoveNfa_0(int startState, int curPos){
   int startsAt = 0;
   jjnewStateCnt = 22;
   int i = 1;
   jjstateSet[0] = startState;
   int kind = 0x7fffffff;
   for (;;)
   {
      if (++jjround == 0x7fffffff)
         ReInitRounds();
      if (curChar < 64)
      {
         unsigned long long l = 1ULL << curChar;
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if ((0x3ff000000000000ULL & l) != 0L)
                  {
                     if (kind > 12)
                        kind = 12;
                     jjCheckNAddStates(0, 4);
                  }
                  else if (curChar == 37)
                  {
                     if (kind > 5)
                        kind = 5;
                     jjCheckNAddStates(5, 7);
                  }
                  break;
               case 1:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 5)
                     kind = 5;
                  jjCheckNAddStates(5, 7);
                  break;
               case 2:
                  if ((0x2400ULL & l) != 0L && kind > 5)
                     kind = 5;
                  break;
               case 3:
                  if (curChar == 10 && kind > 5)
                     kind = 5;
                  break;
               case 4:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 3;
                  break;
               case 6:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 6)
                     kind = 6;
                  jjAddStates(8, 10);
                  break;
               case 7:
                  if ((0x2400ULL & l) != 0L && kind > 6)
                     kind = 6;
                  break;
               case 8:
                  if (curChar == 10 && kind > 6)
                     kind = 6;
                  break;
               case 9:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 8;
                  break;
               case 11:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 12)
                     kind = 12;
                  jjCheckNAddStates(0, 4);
                  break;
               case 12:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 12)
                     kind = 12;
                  jjCheckNAdd(12);
                  break;
               case 13:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     jjCheckNAddTwoStates(13, 14);
                  break;
               case 14:
                  if (curChar == 46)
                     jjCheckNAdd(15);
                  break;
               case 15:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 13)
                     kind = 13;
                  jjCheckNAdd(15);
                  break;
               case 16:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     jjCheckNAddTwoStates(16, 17);
                  break;
               case 17:
                  if (curChar == 46)
                     jjCheckNAdd(18);
                  break;
               case 18:
                  if ((0x3ff000000000000ULL & l) != 0L)
                     jjCheckNAddTwoStates(18, 19);
                  break;
               case 20:
                  if (curChar == 45)
                     jjCheckNAdd(21);
                  break;
               case 21:
                  if ((0x3ff000000000000ULL & l) == 0L)
                     break;
                  if (kind > 14)
                     kind = 14;
                  jjCheckNAdd(21);
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if (curChar == 72)
                     jjstateSet[jjnewStateCnt++] = 5;
                  break;
               case 1:
                  if (kind > 5)
                     kind = 5;
                  jjAddStates(5, 7);
                  break;
               case 5:
                  if (curChar != 82)
                     break;
                  if (kind > 6)
                     kind = 6;
                  jjCheckNAddStates(8, 10);
                  break;
               case 6:
                  if (kind > 6)
                     kind = 6;
                  jjCheckNAddStates(8, 10);
                  break;
               case 19:
                  if (curChar == 69)
                     jjstateSet[jjnewStateCnt++] = 20;
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else
      {
         int hiByte = (curChar >> 8);
         int i1 = hiByte >> 6;
         unsigned long long l1 = 1ULL << (hiByte & 077);
         int i2 = (curChar & 0xff) >> 6;
         unsigned long long l2 = 1ULL << (curChar & 077);
         do
         {
            switch(jjstateSet[--i])
            {
               case 1:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 5)
                     kind = 5;
                  jjAddStates(5, 7);
                  break;
               case 6:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 6)
                     kind = 6;
                  jjAddStates(8, 10);
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 || l2 == 0) break; else break;
            }
         } while(i != startsAt);
      }
      if (kind != 0x7fffffff)
      {
         jjmatchedKind = kind;
         jjmatchedPos = curPos;
         kind = 0x7fffffff;
      }
      ++curPos;
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 22 - startsAt)))
         return curPos;
      if (input_stream->endOfInput()) { return curPos; }
      curChar = input_stream->readChar();
   }
}

 int HuginParserTokenManager::jjStopStringLiteralDfa_1(int pos, unsigned long long active0){
   switch (pos)
   {
      default :
         return -1;
   }
}

int  HuginParserTokenManager::jjStartNfa_1(int pos, unsigned long long active0){
   return jjMoveNfa_1(jjStopStringLiteralDfa_1(pos, active0), pos + 1);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa0_1(){
   switch(curChar)
   {
      case 34:
         return jjStopAtPos(0, 26);
      default :
         return jjMoveNfa_1(0, 0);
   }
}

int HuginParserTokenManager::jjMoveNfa_1(int startState, int curPos){
   int startsAt = 0;
   jjnewStateCnt = 1;
   int i = 1;
   jjstateSet[0] = startState;
   int kind = 0x7fffffff;
   for (;;)
   {
      if (++jjround == 0x7fffffff)
         ReInitRounds();
      if (curChar < 64)
      {
         unsigned long long l = 1ULL << curChar;
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if ((0xfffffffbffffd9ffULL & l) == 0L)
                     break;
                  kind = 27;
                  jjstateSet[jjnewStateCnt++] = 0;
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  kind = 27;
                  jjstateSet[jjnewStateCnt++] = 0;
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else
      {
         int hiByte = (curChar >> 8);
         int i1 = hiByte >> 6;
         unsigned long long l1 = 1ULL << (hiByte & 077);
         int i2 = (curChar & 0xff) >> 6;
         unsigned long long l2 = 1ULL << (curChar & 077);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 27)
                     kind = 27;
                  jjstateSet[jjnewStateCnt++] = 0;
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 || l2 == 0) break; else break;
            }
         } while(i != startsAt);
      }
      if (kind != 0x7fffffff)
      {
         jjmatchedKind = kind;
         jjmatchedPos = curPos;
         kind = 0x7fffffff;
      }
      ++curPos;
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 1 - startsAt)))
         return curPos;
      if (input_stream->endOfInput()) { return curPos; }
      curChar = input_stream->readChar();
   }
}

 int HuginParserTokenManager::jjStopStringLiteralDfa_3(int pos, unsigned long long active0){
   switch (pos)
   {
      default :
         return -1;
   }
}

int  HuginParserTokenManager::jjStartNfa_3(int pos, unsigned long long active0){
   return jjMoveNfa_3(jjStopStringLiteralDfa_3(pos, active0), pos + 1);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa0_3(){
   switch(curChar)
   {
      case 40:
         return jjStopAtPos(0, 40);
      case 41:
         return jjStopAtPos(0, 43);
      case 124:
         return jjStopAtPos(0, 42);
      default :
         return jjMoveNfa_3(0, 0);
   }
}

int HuginParserTokenManager::jjMoveNfa_3(int startState, int curPos){
   int startsAt = 0;
   jjnewStateCnt = 6;
   int i = 1;
   jjstateSet[0] = startState;
   int kind = 0x7fffffff;
   for (;;)
   {
      if (++jjround == 0x7fffffff)
         ReInitRounds();
      if (curChar < 64)
      {
         unsigned long long l = 1ULL << curChar;
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if ((0xfffffcfaffffd9ffULL & l) != 0L)
                  {
                     if (kind > 41)
                        kind = 41;
                     jjCheckNAdd(5);
                  }
                  if (curChar == 37)
                  {
                     if (kind > 39)
                        kind = 39;
                     jjCheckNAddStates(5, 7);
                  }
                  break;
               case 1:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 39)
                     kind = 39;
                  jjCheckNAddStates(5, 7);
                  break;
               case 2:
                  if ((0x2400ULL & l) != 0L && kind > 39)
                     kind = 39;
                  break;
               case 3:
                  if (curChar == 10 && kind > 39)
                     kind = 39;
                  break;
               case 4:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 3;
                  break;
               case 5:
                  if ((0xfffffcfaffffd9ffULL & l) == 0L)
                     break;
                  if (kind > 41)
                     kind = 41;
                  jjCheckNAdd(5);
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
               case 5:
                  if ((0xefffffffffffffffULL & l) == 0L)
                     break;
                  if (kind > 41)
                     kind = 41;
                  jjCheckNAdd(5);
                  break;
               case 1:
                  if (kind > 39)
                     kind = 39;
                  jjAddStates(5, 7);
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else
      {
         int hiByte = (curChar >> 8);
         int i1 = hiByte >> 6;
         unsigned long long l1 = 1ULL << (hiByte & 077);
         int i2 = (curChar & 0xff) >> 6;
         unsigned long long l2 = 1ULL << (curChar & 077);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
               case 5:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 41)
                     kind = 41;
                  jjCheckNAdd(5);
                  break;
               case 1:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 39)
                     kind = 39;
                  jjAddStates(5, 7);
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 || l2 == 0) break; else break;
            }
         } while(i != startsAt);
      }
      if (kind != 0x7fffffff)
      {
         jjmatchedKind = kind;
         jjmatchedPos = curPos;
         kind = 0x7fffffff;
      }
      ++curPos;
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 6 - startsAt)))
         return curPos;
      if (input_stream->endOfInput()) { return curPos; }
      curChar = input_stream->readChar();
   }
}

 int HuginParserTokenManager::jjStopStringLiteralDfa_2(int pos, unsigned long long active0){
   switch (pos)
   {
      default :
         return -1;
   }
}

int  HuginParserTokenManager::jjStartNfa_2(int pos, unsigned long long active0){
   return jjMoveNfa_2(jjStopStringLiteralDfa_2(pos, active0), pos + 1);
}

 int  HuginParserTokenManager::jjMoveStringLiteralDfa0_2(){
   switch(curChar)
   {
      default :
         return jjMoveNfa_2(0, 0);
   }
}

int HuginParserTokenManager::jjMoveNfa_2(int startState, int curPos){
   int startsAt = 0;
   jjnewStateCnt = 12;
   int i = 1;
   jjstateSet[0] = startState;
   int kind = 0x7fffffff;
   for (;;)
   {
      if (++jjround == 0x7fffffff)
         ReInitRounds();
      if (curChar < 64)
      {
         unsigned long long l = 1ULL << curChar;
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if ((0xfffffcfaffffd9ffULL & l) != 0L)
                  {
                     if (kind > 34)
                        kind = 34;
                     jjCheckNAdd(11);
                  }
                  if (curChar == 37)
                  {
                     if (kind > 32)
                        kind = 32;
                     jjCheckNAddStates(5, 7);
                  }
                  break;
               case 1:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 32)
                     kind = 32;
                  jjCheckNAddStates(5, 7);
                  break;
               case 2:
                  if ((0x2400ULL & l) != 0L && kind > 32)
                     kind = 32;
                  break;
               case 3:
                  if (curChar == 10 && kind > 32)
                     kind = 32;
                  break;
               case 4:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 3;
                  break;
               case 6:
                  if ((0xffffffffffffdbffULL & l) == 0L)
                     break;
                  if (kind > 33)
                     kind = 33;
                  jjAddStates(8, 10);
                  break;
               case 7:
                  if ((0x2400ULL & l) != 0L && kind > 33)
                     kind = 33;
                  break;
               case 8:
                  if (curChar == 10 && kind > 33)
                     kind = 33;
                  break;
               case 9:
                  if (curChar == 13)
                     jjstateSet[jjnewStateCnt++] = 8;
                  break;
               case 11:
                  if ((0xfffffcfaffffd9ffULL & l) == 0L)
                     break;
                  if (kind > 34)
                     kind = 34;
                  jjCheckNAdd(11);
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else if (curChar < 128)
      {
         unsigned long long l = 1ULL << (curChar & 077);
         if (l == 1);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
                  if ((0xefffffffffffffffULL & l) != 0L)
                  {
                     if (kind > 34)
                        kind = 34;
                     jjCheckNAdd(11);
                  }
                  if (curChar == 72)
                     jjstateSet[jjnewStateCnt++] = 5;
                  break;
               case 1:
                  if (kind > 32)
                     kind = 32;
                  jjAddStates(5, 7);
                  break;
               case 5:
                  if (curChar != 82)
                     break;
                  if (kind > 33)
                     kind = 33;
                  jjCheckNAddStates(8, 10);
                  break;
               case 6:
                  if (kind > 33)
                     kind = 33;
                  jjCheckNAddStates(8, 10);
                  break;
               case 10:
                  if (curChar == 72)
                     jjstateSet[jjnewStateCnt++] = 5;
                  break;
               case 11:
                  if ((0xefffffffffffffffULL & l) == 0L)
                     break;
                  if (kind > 34)
                     kind = 34;
                  jjCheckNAdd(11);
                  break;
               default : break;
            }
         } while(i != startsAt);
      }
      else
      {
         int hiByte = (curChar >> 8);
         int i1 = hiByte >> 6;
         unsigned long long l1 = 1ULL << (hiByte & 077);
         int i2 = (curChar & 0xff) >> 6;
         unsigned long long l2 = 1ULL << (curChar & 077);
         do
         {
            switch(jjstateSet[--i])
            {
               case 0:
               case 11:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 34)
                     kind = 34;
                  jjCheckNAdd(11);
                  break;
               case 1:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 32)
                     kind = 32;
                  jjAddStates(5, 7);
                  break;
               case 6:
                  if (!jjCanMove_0(hiByte, i1, i2, l1, l2))
                     break;
                  if (kind > 33)
                     kind = 33;
                  jjAddStates(8, 10);
                  break;
               default : if (i1 == 0 || l1 == 0 || i2 == 0 || l2 == 0) break; else break;
            }
         } while(i != startsAt);
      }
      if (kind != 0x7fffffff)
      {
         jjmatchedKind = kind;
         jjmatchedPos = curPos;
         kind = 0x7fffffff;
      }
      ++curPos;
      if ((i = jjnewStateCnt), (jjnewStateCnt = startsAt), (i == (startsAt = 12 - startsAt)))
         return curPos;
      if (input_stream->endOfInput()) { return curPos; }
      curChar = input_stream->readChar();
   }
}

bool HuginParserTokenManager::jjCanMove_0(int hiByte, int i1, int i2, unsigned long long l1, unsigned long long l2){
   switch(hiByte)
   {
      case 0:
         return ((jjbitVec2[i2] & l2) != 0L);
      default :
         if ((jjbitVec0[i1] & l1) != 0L)
            return true;
         return false;
   }
}

/** Token literal values. */

Token * HuginParserTokenManager::jjFillToken(){
   Token *t;
   JAVACC_STRING_TYPE curTokenImage;
   int beginLine;
   int endLine;
   int beginColumn;
   int endColumn;
   JAVACC_STRING_TYPE im = jjstrLiteralImages[jjmatchedKind];
   curTokenImage = (im.length() == 0) ? input_stream->GetImage() : im;
   beginLine = input_stream->getBeginLine();
   beginColumn = input_stream->getBeginColumn();
   endLine = input_stream->getEndLine();
   endColumn = input_stream->getEndColumn();
   t = Token::newToken(jjmatchedKind, curTokenImage);
   t->specialToken = NULL;
   t->next = NULL;

   t->beginLine = beginLine;
   t->endLine = endLine;
   t->beginColumn = beginColumn;
   t->endColumn = endColumn;

   return t;
}
const int defaultLexState = 0;
/** Get the next Token. */

Token * HuginParserTokenManager::getNextToken(){
  Token *matchedToken;
  int curPos = 0;

  for (;;)
  {
   EOFLoop: 
   if (input_stream->endOfInput())
   {
      jjmatchedKind = 0;
      jjmatchedPos = -1;
      matchedToken = jjFillToken();
      return matchedToken;
   }
   curChar = input_stream->BeginToken();

   switch(curLexState)
   {
     case 0:
       { input_stream->backup(0);
          while (curChar <= 32 && (0x100002600ULL & (1ULL << curChar)) != 0L)
       {
       if (input_stream->endOfInput()) { goto EOFLoop; }
       curChar = input_stream->BeginToken();
       }
       }
       jjmatchedKind = 0x7fffffff;
       jjmatchedPos = 0;
       curPos = jjMoveStringLiteralDfa0_0();
       break;
     case 1:
       jjmatchedKind = 0x7fffffff;
       jjmatchedPos = 0;
       curPos = jjMoveStringLiteralDfa0_1();
       break;
     case 2:
       { input_stream->backup(0);
          while (curChar <= 32 && (0x100002600ULL & (1ULL << curChar)) != 0L)
       {
       if (input_stream->endOfInput()) { goto EOFLoop; }
       curChar = input_stream->BeginToken();
       }
       }
       jjmatchedKind = 0x7fffffff;
       jjmatchedPos = 0;
       curPos = jjMoveStringLiteralDfa0_2();
       break;
     case 3:
       { input_stream->backup(0);
          while (curChar <= 32 && (0x100002600ULL & (1ULL << curChar)) != 0L)
       {
       if (input_stream->endOfInput()) { goto EOFLoop; }
       curChar = input_stream->BeginToken();
       }
       }
       jjmatchedKind = 0x7fffffff;
       jjmatchedPos = 0;
       curPos = jjMoveStringLiteralDfa0_3();
       break;
   }
     if (jjmatchedKind != 0x7fffffff)
     {
        if (jjmatchedPos + 1 < curPos)
           input_stream->backup(curPos - jjmatchedPos - 1);
        if ((jjtoToken[jjmatchedKind >> 6] & (1ULL << (jjmatchedKind & 077))) != 0L)
        {
           matchedToken = jjFillToken();
       if (jjnewLexState[jjmatchedKind] != -1)
         curLexState = jjnewLexState[jjmatchedKind];
           return matchedToken;
        }
        else
        {
         if (jjnewLexState[jjmatchedKind] != -1)
           curLexState = jjnewLexState[jjmatchedKind];
           goto EOFLoop;
        }
     }
     int error_line = input_stream->getEndLine();
     int error_column = input_stream->getEndColumn();
     JAVACC_STRING_TYPE error_after;
     bool EOFSeen = false;
     if (input_stream->endOfInput()) {
        EOFSeen = true;
        error_after = curPos <= 1 ? EMPTY : input_stream->GetImage();
        if (curChar == '\n' || curChar == '\r') {
           error_line++;
           error_column = 0;
        }
        else
           error_column++;
     }
     if (!EOFSeen) {
        error_after = curPos <= 1 ? EMPTY : input_stream->GetImage();
     }
     lexicalError();
  }
}


void HuginParserTokenManager::jjCheckNAdd(int state){
   if (jjrounds[state] != jjround)
   {
      jjstateSet[jjnewStateCnt++] = state;
      jjrounds[state] = jjround;
   }
}

void HuginParserTokenManager::jjAddStates(int start, int end){
   do {
      jjstateSet[jjnewStateCnt++] = jjnextStates[start];
   } while (start++ != end);
}

void HuginParserTokenManager::jjCheckNAddTwoStates(int state1, int state2){
   jjCheckNAdd(state1);
   jjCheckNAdd(state2);
}


void HuginParserTokenManager::jjCheckNAddStates(int start, int end){
   do {
      jjCheckNAdd(jjnextStates[start]);
   } while (start++ != end);
}

  /** Reinitialise parser. */
  void HuginParserTokenManager::ReInit(CharStream *stream, int lexState, HuginParser *parserArg) {
    if (input_stream) delete input_stream;
    jjmatchedPos = jjnewStateCnt = 0;
    curLexState = lexState;
    input_stream = stream;
    ReInitRounds();
    debugStream = stdout; // init
    SwitchTo(lexState);
    parser = parserArg;
  }

  void HuginParserTokenManager::ReInitRounds() {
    int i;
    jjround = 0x80000001;
    for (i = 22; i-- > 0;)
      jjrounds[i] = 0x80000000;
  }

  /** Switch to specified lex state. */
  void HuginParserTokenManager::SwitchTo(int lexState) {
    if (lexState >= 4 || lexState < 0)
      assert(false);
      //throw 1;//new TokenMgrError("Error: Ignoring invalid lexical state : " + lexState + ". State unchanged.", TokenMgrError.INVALID_LEXICAL_STATE);
    else
      curLexState = lexState;
  }

  /** Constructor. */
  HuginParserTokenManager::HuginParserTokenManager (CharStream *stream, int lexState, HuginParser *parserArg){
    input_stream = NULL;
    ReInit(stream, lexState, parserArg);
  }

  // Destructor
  HuginParserTokenManager::~HuginParserTokenManager () {
    if (input_stream) delete input_stream;
  }
}
