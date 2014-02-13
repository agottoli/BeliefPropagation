/* Generated By:JavaCC: Do not edit this line. HuginParserConstants.java */

/**
 * Token literal values and constants.
 * Generated by org.javacc.parser.OtherFilesGen#start()
 */
#ifndef HUGINPARSERCONSTANTS_H
#define HUGINPARSERCONSTANTS_H

namespace hugin_parser {
  /** End of File. */
const  int _EOF = 0;
  /** RegularExpression Id. */
const  int NET = 7;
  /** RegularExpression Id. */
const  int LPARG = 8;
  /** RegularExpression Id. */
const  int NODESIZE = 9;
  /** RegularExpression Id. */
const  int EQUAL = 10;
  /** RegularExpression Id. */
const  int LPAR = 11;
  /** RegularExpression Id. */
const  int INTEGER = 12;
  /** RegularExpression Id. */
const  int DOUBLE = 13;
  /** RegularExpression Id. */
const  int DOUBLEE = 14;
  /** RegularExpression Id. */
const  int RPAR = 15;
  /** RegularExpression Id. */
const  int SEMICOLON = 16;
  /** RegularExpression Id. */
const  int LABEL = 17;
  /** RegularExpression Id. */
const  int DOUBLEQUOTE = 18;
  /** RegularExpression Id. */
const  int NAME = 19;
  /** RegularExpression Id. */
const  int NODE = 20;
  /** RegularExpression Id. */
const  int POSITION = 21;
  /** RegularExpression Id. */
const  int STATES = 22;
  /** RegularExpression Id. */
const  int POTENTIAL = 23;
  /** RegularExpression Id. */
const  int DATA = 24;
  /** RegularExpression Id. */
const  int RPARG = 25;
  /** RegularExpression Id. */
const  int DOUBLEQUOTE_EXIT = 26;
  /** RegularExpression Id. */
const  int ANYTHING = 27;
  /** RegularExpression Id. */
const  int ANYTHING_EXIT = 34;
  /** RegularExpression Id. */
const  int LPAR_P = 40;
  /** RegularExpression Id. */
const  int ANYTHING_P = 41;
  /** RegularExpression Id. */
const  int PIPE_P = 42;
  /** RegularExpression Id. */
const  int RPAR_P_EXIT = 43;

  /** Lexical state. */
const  int DEFAULT = 0;
  /** Lexical state. */
const  int stato_anything = 1;
  /** Lexical state. */
const  int stato_namenode = 2;
  /** Lexical state. */
const  int stato_potential = 3;

  /** Literal token values. */
  static JAVACC_CHAR_TYPE tokenImage_arr_0[] = 
{0x3c, 0x45, 0x4f, 0x46, 0x3e, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_1[] = 
{0x22, 0x20, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_2[] = 
{0x22, 0xa, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_3[] = 
{0x22, 0xd, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_4[] = 
{0x22, 0x9, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_5[] = 
{0x22, 0x3c, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x6b, 0x69, 0x6e, 0x64, 0x20, 0x35, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_6[] = 
{0x22, 0x3c, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x6b, 0x69, 0x6e, 0x64, 0x20, 0x36, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_7[] = 
{0x22, 0x6e, 0x65, 0x74, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_8[] = 
{0x22, 0x7b, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_9[] = 
{0x22, 0x6e, 0x6f, 0x64, 0x65, 0x5f, 0x73, 0x69, 0x7a, 0x65, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_10[] = 
{0x22, 0x3d, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_11[] = 
{0x22, 0x28, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_12[] = 
{0x22, 0x3c, 0x49, 0x4e, 0x54, 0x45, 0x47, 0x45, 0x52, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_13[] = 
{0x22, 0x3c, 0x44, 0x4f, 0x55, 0x42, 0x4c, 0x45, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_14[] = 
{0x22, 0x3c, 0x44, 0x4f, 0x55, 0x42, 0x4c, 0x45, 0x45, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_15[] = 
{0x22, 0x29, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_16[] = 
{0x22, 0x3b, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_17[] = 
{0x22, 0x6c, 0x61, 0x62, 0x65, 0x6c, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_18[] = 
{0x22, 0x22, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_19[] = 
{0x22, 0x6e, 0x61, 0x6d, 0x65, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_20[] = 
{0x22, 0x6e, 0x6f, 0x64, 0x65, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_21[] = 
{0x22, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_22[] = 
{0x22, 0x73, 0x74, 0x61, 0x74, 0x65, 0x73, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_23[] = 
{0x22, 0x70, 0x6f, 0x74, 0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_24[] = 
{0x22, 0x64, 0x61, 0x74, 0x61, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_25[] = 
{0x22, 0x7d, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_26[] = 
{0x22, 0x22, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_27[] = 
{0x22, 0x3c, 0x41, 0x4e, 0x59, 0x54, 0x48, 0x49, 0x4e, 0x47, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_28[] = 
{0x22, 0x20, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_29[] = 
{0x22, 0xa, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_30[] = 
{0x22, 0xd, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_31[] = 
{0x22, 0x9, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_32[] = 
{0x22, 0x3c, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x6b, 0x69, 0x6e, 0x64, 0x20, 0x33, 0x32, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_33[] = 
{0x22, 0x3c, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x6b, 0x69, 0x6e, 0x64, 0x20, 0x33, 0x33, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_34[] = 
{0x22, 0x3c, 0x41, 0x4e, 0x59, 0x54, 0x48, 0x49, 0x4e, 0x47, 0x5f, 0x45, 0x58, 0x49, 0x54, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_35[] = 
{0x22, 0x20, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_36[] = 
{0x22, 0xa, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_37[] = 
{0x22, 0xd, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_38[] = 
{0x22, 0x9, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_39[] = 
{0x22, 0x3c, 0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x6b, 0x69, 0x6e, 0x64, 0x20, 0x33, 0x39, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_40[] = 
{0x22, 0x28, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_41[] = 
{0x22, 0x3c, 0x41, 0x4e, 0x59, 0x54, 0x48, 0x49, 0x4e, 0x47, 0x5f, 0x50, 0x3e, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_42[] = 
{0x22, 0x7c, 0x22, 0};
  static JAVACC_CHAR_TYPE tokenImage_arr_43[] = 
{0x22, 0x29, 0x22, 0};
  static JAVACC_STRING_TYPE tokenImage[] = {
tokenImage_arr_0, 
tokenImage_arr_1, 
tokenImage_arr_2, 
tokenImage_arr_3, 
tokenImage_arr_4, 
tokenImage_arr_5, 
tokenImage_arr_6, 
tokenImage_arr_7, 
tokenImage_arr_8, 
tokenImage_arr_9, 
tokenImage_arr_10, 
tokenImage_arr_11, 
tokenImage_arr_12, 
tokenImage_arr_13, 
tokenImage_arr_14, 
tokenImage_arr_15, 
tokenImage_arr_16, 
tokenImage_arr_17, 
tokenImage_arr_18, 
tokenImage_arr_19, 
tokenImage_arr_20, 
tokenImage_arr_21, 
tokenImage_arr_22, 
tokenImage_arr_23, 
tokenImage_arr_24, 
tokenImage_arr_25, 
tokenImage_arr_26, 
tokenImage_arr_27, 
tokenImage_arr_28, 
tokenImage_arr_29, 
tokenImage_arr_30, 
tokenImage_arr_31, 
tokenImage_arr_32, 
tokenImage_arr_33, 
tokenImage_arr_34, 
tokenImage_arr_35, 
tokenImage_arr_36, 
tokenImage_arr_37, 
tokenImage_arr_38, 
tokenImage_arr_39, 
tokenImage_arr_40, 
tokenImage_arr_41, 
tokenImage_arr_42, 
tokenImage_arr_43, 
  };

 }
#endif
