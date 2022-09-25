/* 
 * File:   main.cpp
 * Author: chien yu
 *
 * Created on 2017�~4��16��, �W�� 12:40
 */

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

enum TokenType {
  IDENTIFIER = 34512, CONSTANT = 87232, SPECIAL = 29742
};

typedef char * CharPtr;

struct Column {
  int column; // ��token(���r��)�b��line���X�{��column
  Column * next; // �U�@�Ӧ�token(�b��line)���X�{��column
}; // Column

typedef Column * ColumnPtr;

struct Line { // struct Line �O���F4�ظ�T
  int line; // ��token��b���@ line
  ColumnPtr firstAppearAt; // ���즹token�Ĥ@���X�{�b��line�����@column
  ColumnPtr lastAppearAt; // ���즹token�̫�X�{�b��line�����@column
  Line * next; // �P�@ token �U�@���X�{�b���@ line
};

typedef Line * LinePtr;

struct Token { // Struct token �����F4�ظ�T
  CharPtr tokenStr; // tokenStr ��A���U�Ӫ�token
  TokenType type; // type ������token�O�ݩ���@��case
  LinePtr firstAppearOn; // �� token �Ĥ@���X�{�b���@ line
  LinePtr lastAppearOn; // �� token �̫�X�{�b���@ line
  Token * next; // ���V�U�@��token
}; // struct Token

typedef Token * TokenPtr;
TokenPtr gFront = NULL, gRear = NULL; // ���O���VToken��C���Y���
typedef char Str100[ 100 ]; // �����O�O����Ψӫŧi�@�Ӱ}�C�ܼơB�H��Ū�r�ꤧ��
int gLine = 1; // �u�U�@�ӭnŪ�i�Ӫ��r���v�Ҧb��line number
int gColumn = 1; // �u�U�@�ӭnŪ�i�Ӫ��r���v�Ҧb��column number
int gfirstcolumn = 1;

void Star( char & word ) {
  if ( word == '\n' ) gLine++;
  if (word == '*') {
    if ((word = getchar()) == '/');
    else return Star(word);
  }// if

  else return Star(word = getchar());
} // Star()

int Case(char word) {
  if (word == ' ' || word == '\n' || word == '\t') return 40;
  if ((word >= 'a' && word <= 'z') || (word >= 'A' && word <= 'Z') || word == '_')
    return 10;
  if ((word >= '0' && word <= '9')) return 20;
  if (word == '=' || word == '>' || word == '<' || word == '!') return 31;
  if ( word == '"' ) return 21;
  if (word == '+') return 32;
  if (word == '-') return 33;
  if (word == '&') return 34;
  if (word == '|') return 35;
  else return 30;
} // Case()

int New( Str100 & t, char & word, int n) {  // �|�i�ӥN�� case < 40
  gfirstcolumn = gColumn;
  t[0] = word;
  int i = 1;
  if (word == '"') {
    for (; (word = getchar()) != '"'; gColumn++) {
      t[i] = word;
      i++;
      // cout << "1: " << word << "\n" ;
    } // for
    
    t[i] = word;
    gColumn++;
  }// if

  else if ( word == '/' ) {
    // cout << "2: " << word << "\n" ;
    if ((word = getchar()) == '/') {
      while ((word = getchar()) != '\n');
      return 0 ;
    } // if
    
    else if (word == '*') {
      Star(word = getchar());
      return 0 ;
    } // else if
    
    else {
      gColumn++;
      return 2;
    } // if // �hŪ���@��
  }// else if
  
    // ( word = getchar()) != '\n' && ( word = getchar() )!= ' ' && ( word = getchar() ) != '\t'
  else { // ( word != ' ' && word != '\n' && word != '\t' )
    // cout << "3: " << word << "\n" ;
    word = getchar();
    for (  gColumn++ ; word != '\n' && word != ' ' &&
            word != '\t'; gColumn++ ) {
      if (  n != 30 && (
              (n == 20 && ( Case(word) == 20 || word == '.')) ||
              (n == 10 && ( Case(word) == 10 || Case(word) == 20)) ||
              (n == 33 && (word == '-' || word == '>')) || Case(word) == n  ) ) {
        t[i] = word;
        i++;
        word = getchar();
        // cout << t << " " << n <<" �����i++\n" ;
      }// if
      
      else return 2; // �hŪ���@��
    } // for
    
  } // else 

  return 1;
} // New()

void Newcolumn(ColumnPtr & first, ColumnPtr & last) {
  if (first == NULL) {
    first = new Column;
    first->column = gfirstcolumn;
    first->next = NULL;
    last = first;
    // cout << "column : " << first->column << "\n";
  }// if

  else Newcolumn(first->next, last);
} // Newcolumn()

void Newline(LinePtr & first, LinePtr & last) {
  if (first == NULL) {
    first = new Line;
    first->line = gLine; // ��token��b���@ line
    first->firstAppearAt = NULL; // ���즹token�Ĥ@���X�{�b��line�����@column
    first->lastAppearAt = NULL; // ���즹token�̫�X�{�b��line�����@column
    Newcolumn(first->firstAppearAt, first->lastAppearAt);
    first->next = NULL; // �P�@ token �U�@���X�{�b���@ line
    last = first;
    // cout << "line : " << first->line << "\n";
  }// if 

  else if (first->line == gLine)
    Newcolumn(first->firstAppearAt, first->lastAppearAt);
  else Newline(first->next, last);
} // Newline()

void Newtoken(TokenPtr & previous, char * token, TokenType type, TokenPtr & head) {
  TokenPtr temp = head;
  previous = new Token;
  previous->tokenStr =  token ;
  // cout << token << "\n";
  previous->type = type;
  previous->firstAppearOn = NULL;
  previous->lastAppearOn = NULL;
  Newline(previous->firstAppearOn, previous->lastAppearOn);
  previous->next = temp;
  // previous->next = head;
  head = previous;
} // Newtoken()

TokenPtr gprevious = NULL;

void Addtoken(TokenPtr & head, char * token, TokenType type, bool & repeat ) {
  if (head != NULL && strcmp(head->tokenStr, token) == 0) {
    Newline(head->firstAppearOn, head->lastAppearOn );
    gprevious = NULL;
    repeat = true;
  }// if
  else if (head != NULL && strcmp(head->tokenStr, token) < 0) {
    gprevious = head;
    Addtoken( head->next, token, type, repeat);
  }// else if

  else {
    if (gprevious == NULL) Newtoken(gprevious, token, type, head); // �̫e
    else Newtoken(gprevious->next, token, type, head); //����.�̫�
    if (head == NULL) gRear = gprevious;
    gprevious = NULL;
  } // else

} // Addtoken()

void Printcolumn( LinePtr temp1, ColumnPtr temp2 ) {
  if ( temp2 != NULL ) {
    cout << "(" << temp1->line << "," ;
    cout << temp2->column << ")";
    Printcolumn( temp1, temp2->next ) ;
  } // if
  
  else return ;
} // Printcolumn()

void Printline( LinePtr temp ) {
  if ( temp != NULL ) {
    Printcolumn( temp, temp->firstAppearAt ) ;
    Printline( temp->next) ;
  } // if
  
  else return ;
} // Printline()

bool Sameline ( LinePtr temp, int line ) {
  if ( temp != NULL ) {
    if ( temp->line == line ) {
      // cout << "�i�ӤF!! line == " << line << "\n";
      return true ;
    } // if
    else return Sameline ( temp->next, line ) ;
  } // if
  
  else return false ;
} // Sameline()

void Printsameline ( TokenPtr temp, int line ) {
  if ( temp != NULL ) {
    bool yes = Sameline ( temp->firstAppearOn, line);
    if ( yes ) cout << temp->tokenStr << "\n" ;
    Printsameline ( temp->next, line ) ;
  } // if
  
  else  return ;
} // viod()

void Printsametoken ( TokenPtr temp, CharPtr token ){
  if ( temp != NULL && strcmp ( temp->tokenStr, token ) == 0 ) {
    Printline( temp->firstAppearOn ) ;
    cout << "\n";
    return ;
  } // if
  
  if ( temp != NULL && strcmp ( temp->tokenStr, token ) != 0 ) 
    Printsametoken (temp->next,token);
  else {
    cout << "�d�L��token : " << token ;
    return ;
  } // else
} // Printsametoken()

void Printtoken( TokenPtr temp ) {
  if ( temp != NULL ) {
    cout << temp->tokenStr << " " ;
    Printline( temp->firstAppearOn ) ;
    cout << "\n";
    Printtoken( temp->next ) ;
  } // if
  
  else return ;
} // Printtoken()

int main() { // IDENTIFIER = 34512, CONSTANT = 87232, SPECIAL = 29742
  char word = ' ';
  int num1 = 0, num2 = 0, num3 = 0;
  int process = 0; 
  Str100 token = { 0 };
  word = getchar(); // gColumn++ C++ Ū�r�����y�k 
  for (   ; strcmp( token, "END_OF_FILE" ) != 0 ; ) {
    for ( int i = 0 ; i != 100 ; i++ ) token[i] = '\0';
    TokenType type;
    if ( Case(word) == 10 ) {
      process = New( token, word, Case(word) );
      type = IDENTIFIER;
      if ( strcmp( token, "END_OF_FILE" ) != 0 ) num1++;
      // cout << "num1: " << token << " " << num1 << "\n";
    }// if
 
    else if ( Case( word ) < 30 ) {
      process = New(token, word, Case( word ) );
      type = CONSTANT;
      num2++;
      // cout << "num2: " << token << " " << num2 << "\n";
    }// else if

    else if ( Case( word ) < 40 ) {
      process = New(token, word, Case( word ) );
      type = SPECIAL;
      if ( process != 0 ) num3++;
     // cout << "num3: " << token << " " << num3 << process << "\n";
    } // else if

    
    if ( strcmp( token, "END_OF_FILE" ) != 0 && process >  0 ) {
      bool repeat = false ;
      char * t = new char[ strlen(token) + 1 ];
      // cout << "token : " << token << "\n";
      strcpy(t, token);
      // cout << "t : " << t << "\n";
      Addtoken( gFront, t, type, repeat);
      if ( repeat ) {
        if ( Case( t[0] ) == 10 ) num1--;
        else if ( Case( t[0] ) < 30 ) num2--;
        else if ( Case( t[0] ) < 40 ) num3--;
      } // if
      
      process--; // 1 or 2 -> 0 or 1
    } // else if

    // if ( word == ' ' ) gColumn++;
    if (word == '\n') {
      // cout << "line++ : " << gLine << " " << gColumn << "\n" ;
      gLine++;
      gColumn = 0;
    } // if
    
    if ( strcmp( token, "END_OF_FILE" ) != 0 &&
          ( word == ' ' || word == '\n' || word == '\t' || process == 0 ) ) {
      word = getchar();
      gColumn++;
    } // if
  } // for

  
  Printtoken( gFront ) ;

  cout << "\n�п�J���O�G\n" << "1.�`�@���h�ֺ� token\n" << "2.�T��case�U���h�� token\n"
          << "3.�L�X���w token ����m (line number,column number) (�n�Ƨ�)\n"
          << "4.�L�X�ҫ��w�����@ line �X�{���� token (�n�Ƨ�)\n"
          << "5.����\n\n";


  
    while ( cin >> word ) {
    if (word == '1') cout << "�`�@" << num1 + num2 + num3 << "��\n\n";
    if (word == '2') cout << "Case1 �@ " << num1 << "��\n" << "Case2 �@ "
            << num2 << "��\n" << "Case3 �@ " << num3 << "��\n\n";
    if (word == '3') {
      cout << "�п�J�n�j�M�� token :\n";
      cin >> token;
      cout << token << " " ;
      Printsametoken ( gFront, token );
      cout << "\n";
    } // if
    
    if (word == '4') {
      cout << "�п�J�n���w�� line :\n";
      int line ;
      cin >> line ;
      Printsameline ( gFront,  line );
      cout << "\n";
    } // if

    if ( word == '5' ) {
      cout << "byebye";
      return 0;
    } // if

  } // while

}

