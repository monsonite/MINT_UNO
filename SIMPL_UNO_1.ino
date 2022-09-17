// SIMPL_UNO - A Serial Interpreted Minimal Programming Language  
// Inspired by Txtzyme - by Ward Cunningham and STABLE - by Sandor Schneider
// This version runs on standard Arduino UNO with LED on Digital 13
// New stack manipulation/handling introduced 27-05-22 as per Sandor Schneider's STABLE tiny Forthlike language

    int d = 13 ;      // d is used to denote the digital port pin for I/O operations
    int i ;           // A working register used for evaluating numbers and some stack manipulations
    int v ;           // Variable number  
    int s = 100 ;     // Stack Pointer
    int r = 60  ;     // The return stack pointer
    int p =  0  ;     // The pointer to the character in the program buffer
    int n ;
    int mx ;          // The extent of the character string in memory   
    int f ;           // The function number calculated as ascii value - 35   
    int st[256] ;     // Stack
  
    char t=0 ;
    char c ;          // The character in the file record to be written into pr memory
    char u ;          // The character at the current program location in pr memory    
    char k=0 ;        // A flag that denotes that the character is a variable
    char pr[1000] ;   // The memory containing the program 
    char buf[64];  
    char* addr;

void setup()
{   Serial.begin(115200);   pinMode(d,OUTPUT); }

void loop()
{ 
  txtRead(pr, 64);     // Read input string  
  txtEval(pr);         // Evaluate instructions         
}
 
void txtRead (char *p, byte n) {
  byte i = 0;
  while (i < (n-1)) {
    while (!Serial.available());
    char ch = Serial.read();
    if (ch == '\r' || ch == '\n') break;
    if (ch >= ' ' && ch <= '~') {
      *p++ = ch;
      i++;
    }
  }
  *p = 0;                        // Put a null at the end of the input string
}

// ------------------------------------------------------------------------------------------------ 
  void txtEval (char *pr) {
  unsigned int k = 0;
  char ch;
  while ((ch = *pr++)) {
    u= ch ;
    p++;   
    switch (ch) {
//---------------------------------------------------------------------     
// Detect numerical characters and form a 32-bit integer to place on top of stack       
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    i = ch - '0'; while (*pr >= '0' && *pr <= '9') { i = i*10 + (*pr++ - '0'); } s++; st[s] = i ;  break ;
    
//---------------------------------------------------------------------                
      case 'A':            // Point the interpreter to the array containing the words
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':

      r++; st[r]=p; p=st[u-35]; u=pr[p]; p--;                  // CALL definition

      Serial.print("r= "); Serial.println(r);
      Serial.print("p= "); Serial.println(p);
      Serial.print("u= "); Serial.println(u);
      Serial.print("s= "); Serial.println(s);

      break;
// ----------------------------------------------------------------------------------------
// Variables
      case 'a':            
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':     
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':

     k=1; v=u-97;  break;                                             // VAR
// ----------------------------------------------------------------------------------------
//  Stack operations

case ' ':   break;                                                    // NOP
case '"':   s++; st[s]=st[s-1]; break;                                // DUP
case 39 :   s--; break;                                               // DROP
case '$' :  i=st[s]; st[s]=st[s-1]; st[s-1]=i; break;                 // SWAP
case '%' :  i=st[s];st[s]=st[s-2];st[s-2]=st[s-1];st[s-1]=i; break;   // OVER

// ----------------------------------------------------------------------------------------
//  Arithmetic and Logical Operations

case '+' :  st[s-1]+=st[s]; s--; break;         // ADD
case '-' :  st[s-1]-=st[s]; s--; break;         // SUB
case '*' :  st[s-1]*=st[s]; s--; break;         // MUL
case '/' :  st[s-1]/=st[s]; s--; break;         // DIV
case '&' :  st[s-1]&=st[s]; s--; break;         // AND
case '|' :  st[s-1]|=st[s]; s--; break;         // OR
case '^' :  st[s-1]^=st[s]; s--; break;         // XOR
case '_' :  st[s]=-st[s]; break;                // NEG
case '~' :  st[s]=  ~st[s]; break;              // NOT
case '}' :  st[s]=  st[s]>>1 ; break;           // SHR
case '{' :  st[s]=  st[s]<<1 ; break;           // SHL
// case '|' :  st[s-1]%=st[s]; s--; break;      // MOD
// case '+' :  if(k==0){st[s-1]+=st[s]; s--; }else{st[v]++;} break;       // ADD INC
// case '-' :  if(k==0){st[s-1]-=st[s]; s--; }else{st[v]--;} break;       // SUB DEC
// ----------------------------------------------------------------------------------------
// Comparisons and conditional execution

case '<' :  if(st[s]> st[s-1]){st[s]=-1;} else {st[s]=0;} break;       // LESS THAN
case '=' :  if(st[s]==st[s-1]){st[s]=-1;} else {st[s]=0;} break;       // EQUAL TO
case '>' :  if(st[s]< st[s-1]){st[s]=-1;} else {st[s]=0;} break;       // GREATER THAN
// case '(' :  if(st[s]==0){s--;p++;u=pr[p];while(u!=')'){p++;u=pr[p];}} else{s--;} break;  // Skip if Zero START CONDITIONAL CODE
// case ')' :  r++;st[r]=p;if(st[s]==0){p++;u=pr[p]; while(u!=')'){ p++; u=pr[p];}} break;  // BEGIN LOOP

// ----------------------------------------------------------------------------------------
//Input and Output

// case 39 :   p++;u=pr[p];while(u!='\''){s++;st[s]=u;p++;u=pr[p];} break;      // Copy string to stack
case '.':   Serial.println(st[s]); s-- ;                            break;      // PRINT TOS
case ',' :  Serial.write(st[s]); s--;                               break;      // EMIT ascii
case '`' :  p++;u=pr[p];while(u!='`'){Serial.print(u);p++;u=pr[p];} break;      // PRINT STRING
case '?' :  if((c=getc(stdin))==EOF) { c=0; } s++; st[s]=c;         break;      // KEY

// ----------------------------------------------------------------------------------------
// Variables and Memory

case '!' :  st[v]=st[s]; s--;  break;                // STORE
case '@' :  s++; st[s]=st[v];   break;               // FETCH
//fIFETCH()s++; st[s]=st[st[v]]; break;              // INDIRECT FETCH
//fISTORE()st[st[v]]=st[s]; s--;  break;             // INDIRECT STORE

// ----------------------------------------------------------------------------------------
// Colon Definition

case ':' :  f=pr[p+1]-35;st[f]=p+2;while(u!=';'){p++;u=pr[p];} break; // COLON DEF
case ';' :  p=st[r];r--;                                       break; // END COLON DEF

// ----------------------------------------------------------------------------------------
// Loops

case '(' :  r++;st[r]=p;if(st[s]==0){p++;u=pr[p]; while(u!=')'){ p++; u=pr[p];}} break; // BEGIN LOOP
case ')' :  if(st[s]!=0) p=st[r]; else r--; s--;  break;                                // END LOOP
case 92 :  Serial.print(u); break;                  // Alternate function.    
    }
  }
}
