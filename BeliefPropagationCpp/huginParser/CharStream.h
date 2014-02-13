/* Generated By:JavaCC: Do not edit this line. CharStream.h Version 6.1 */
/* JavaCCOptions:STATIC=true,SUPPORT_CLASS_VISIBILITY_PUBLIC=true */
#ifndef CHARSTREAM_H
#define CHARSTREAM_H
#include "JavaCC.h"

#ifndef INITIAL_BUFFER_SIZE
#define INITIAL_BUFFER_SIZE 4096
#endif

namespace hugin_parser {

/**
 * This class describes a character stream that maintains line and
 * column number positions of the characters.  It also has the capability
 * to backup the stream to some extent.  An implementation of this
 * class is used in the TokenManager implementation generated by
 * JavaCCParser.
 *
 * All the methods except backup can be implemented in any fashion. backup
 * needs to be implemented correctly for the correct operation of the lexer.
 * Rest of the methods are all used to get information like line number,
 * column number and the string that constitutes a token and are not used
 * by the lexer. Hence their implementation won't affect the generated lexer's
 * operation.
 */


class CharStream {
 public:
  void setTabSize(int i) { tabSize = i; }
  int getTabSize(int i) { return tabSize; }
  virtual int getColumn() { return bufcolumn[bufpos]; }
  virtual int getLine() { return bufline[bufpos]; }
  virtual int getEndColumn() { return bufcolumn[bufpos]; }
  virtual int getEndLine() { return bufline[bufpos]; }
  virtual int getBeginColumn() { return bufcolumn[tokenBegin]; }
  virtual int getBeginLine() { return bufline[tokenBegin]; }

  /**
   * Returns the next character from the selected input.  The method
   * of selecting the input is the responsibility of the class
   * implementing this class.
   */
  virtual JAVACC_CHAR_TYPE readChar();

  /**
   * Backs up the input stream by amount steps. Lexer calls this method if it
   * had already read some characters, but could not use them to match a
   * (longer) token. So, they will be used again as the prefix of the next
   * token and it is the implemetation's responsibility to do this right.
   */
  virtual void backup(int amount);

  /**
   * Returns the next character that marks the beginning of the next token.
   * All characters must remain in the buffer between two successive calls
   * to this method to implement backup correctly.
   */
  virtual JAVACC_CHAR_TYPE BeginToken();

  virtual void ExpandBuff(bool wrapAround);
  virtual void FillBuff();

  /**
   * Returns a string made up of characters from the marked token beginning
   * to the current buffer position. Implementations can return
   * anything that they want to. For example, for efficiency, one might decide
   * to just return NULL, which is a valid implementation.
   */
  virtual JAVACC_STRING_TYPE GetImage() {
    if (bufpos >= tokenBegin)
      return JAVACC_STRING_TYPE(buffer + tokenBegin, bufpos - tokenBegin + 1);
    else
      return JAVACC_STRING_TYPE(buffer + tokenBegin, bufsize - tokenBegin)
             .append(buffer, bufpos + 1);
  }

  /**
   * Returns an array of characters that make up the suffix of length 'len' for
   * the currently matched token. This is used to build up the matched string
   * for use in actions in the case of MORE. A simple and inefficient
   * implementation of this is as follows :
   */
  virtual JAVACC_STRING_TYPE GetSuffix(int len) {
    if ((bufpos + 1) >= len) {
      return JAVACC_STRING_TYPE(buffer + bufpos - len + 1, len);
    }
    return JAVACC_STRING_TYPE(buffer + bufsize - (len - bufpos - 1), len - bufpos - 1)
           .append(buffer, bufpos + 1);
  }

  /**
   * The lexer calls this function to indicate that it is done with the stream
   * and hence implementations can free any resources held by this class.
   */
  virtual void DeleteBuffers();

  virtual ~CharStream() {
    if (deleteStream) {
     delete inputStream;
    }
    DeleteBuffers();
  }

  bool endOfInput() {
    return inBuf == 0 && bufpos + 1 >= maxNextCharInd &&
           inputStream->endOfInput();
  }

  CharStream(const JAVACC_CHAR_TYPE *buf, int sz, int startline,
                      int startcolumn, int buffersize) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(JAVACC_STRING_TYPE(buf, sz), startline, startcolumn, buffersize);
  }

  CharStream(const JAVACC_CHAR_TYPE *buf, int sz, int startline, int startcolumn) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(JAVACC_STRING_TYPE(buf, sz), startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  CharStream(const JAVACC_STRING_TYPE& str, int startline,
                      int startcolumn, int buffersize) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(str, startline, startcolumn, buffersize);
  }

  CharStream(const JAVACC_STRING_TYPE& str, int startline, int startcolumn) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0) ,tabSize(8){
    ReInit(str, startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  CharStream(ReaderStream *input_stream, int startline,
             int startcolumn, int buffersize) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(input_stream, startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  CharStream(ReaderStream *input_stream, int startline, int startcolumn) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(input_stream, startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  CharStream(ReaderStream *input_stream) :
    bufline(NULL), bufcolumn(NULL), inputStream(NULL), deleteStream(false),
    buffer(NULL), bufpos(0), bufsize(0), tokenBegin(0), column(0), line(0),
    prevCharIsCR (false), prevCharIsLF (false), available(0), maxNextCharInd(0),
    inBuf(0),tabSize(8) {
    ReInit(input_stream, 1, 1, INITIAL_BUFFER_SIZE);
  }

  virtual void ReInit(ReaderStream *input_stream, int startline, int startcolumn,
                      int buffersize);

  virtual void ReInit(ReaderStream *input_stream, int startline,
                      int startcolumn) {
    ReInit(input_stream, startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  virtual void ReInit(ReaderStream *input_stream) {
    ReInit(input_stream, 1, 1, INITIAL_BUFFER_SIZE);
  }

  virtual void ReInit(const JAVACC_STRING_TYPE& str, int startline,
                      int startcolumn, int buffersize);

  virtual void ReInit(const JAVACC_STRING_TYPE& str, int startline,
                      int startcolumn) {
    ReInit(str, startline, startcolumn, INITIAL_BUFFER_SIZE);
  }

  virtual void adjustBeginLineColumn(int newLine, int newCol);

 protected:
  virtual void UpdateLineColumn(JAVACC_CHAR_TYPE c);

  int *bufline;
  int *bufcolumn;
  ReaderStream *inputStream;
  bool deleteStream;
  JAVACC_CHAR_TYPE * buffer;
  int bufpos;
  int bufsize;
  int tokenBegin;
  int column;
  int line;
  bool prevCharIsCR ;
  bool prevCharIsLF ;
  int available;
  int maxNextCharInd;
  int inBuf ;
  int tabSize ;
};

}
#endif
/* JavaCC - OriginalChecksum=61267a724cdee6b95d8b8326c00aa04e (do not edit this line) */
