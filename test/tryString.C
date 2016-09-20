#define DEBUG(x) (TString("")+x)
TString operator<<( TString s, const char *so ){ 
  TString st = s; return st+so;
}
void tryString(){
  //cout<<( TString("abc")<<"def" )<<endl;
  cout<<DEBUG( "abc" + 123l + "cde")<<endl;

}
