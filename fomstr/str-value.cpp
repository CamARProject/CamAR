//------------------------------------------------------------------------------
// 数式評価のサンプル(変数名と関数名)
//------------------------------------------------------------------------------
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------
// break 付きのキーワード
//------------------------------------------------
#define CASE        break;case
#define DEFAULT     break;default

//------------------------------------------------
// 関数のプロトタイプ宣言
//------------------------------------------------
static double funcAddSub();

//--[数式評価]------------------------------------------------------------------

//------------------------------------------------
// 記号定数
//------------------------------------------------
#define BLKTOP      '('
#define BLKEND      ')'
#define SPC         ' '
#define TAB         '\t'

//------------------------------------------------
// 列挙定数(エラーコード)
//------------------------------------------------
enum eERRCODE {
    ERRCODE_SUCCESS,        // 正常
    ERRCODE_FACTOR,         // 未サポートの演算子です。
    ERRCODE_FUNCTION,       // 関数名が見つかりません。
    ERRCODE_VARIABLE,       // 変数名が見つかりません。
    MAX_ERRCODE,
};

//------------------------------------------------
// グローバル変数
//------------------------------------------------
static const char *g_parse;
static       long  g_error;
double x;
double y;

//------------------------------------------------
// 関数の実行
//------------------------------------------------
static double funcFunction( const char string[] )
{
    g_error = ERRCODE_FUNCTION;
    string;
    return 0;
}

//------------------------------------------------
// 変数の参照
//------------------------------------------------

static double funcVariable(const char string[] )
{
	
	const char xstr[]="x"; 	//変数名xの比較用の文字
	const char ystr[]="y";  //変数名yの比較用の文字
	
	
	//変数名がxならその変数にxの値を返す
	if((strcmp(string,xstr))==0)	   return  x;
	
	//変数名がyならその変数にyの値を返す
	else if((strcmp(string,ystr))==0)	return y;

	g_error = ERRCODE_VARIABLE;
    
	string;
		
    return 0;
}

//------------------------------------------------
// 変数・関数の評価
//------------------------------------------------
static double funcSymbol()
{
    char    name[ 256 ];
    char*   tail = name;
    
    while ( isalnum(*g_parse) ){
        *tail++ = *g_parse++;
    }
    for ( *tail = '\0' ; ; )
	{

		switch ( *g_parse )
		{
            CASE SPC:       g_parse++;
            CASE TAB:       g_parse++;
            CASE BLKTOP:    return funcFunction( name );
			DEFAULT:		return funcVariable( name );
        }
    }
}

//------------------------------------------------
// 数値文字列の評価
//------------------------------------------------
static double funcValue()
{
    return strtod( g_parse, (char**)&g_parse );
}

//------------------------------------------------
// 括弧・数値・シンボルの評価
//------------------------------------------------
static double funcFactor()
{
    double ans;
    
    while ( (*g_parse == SPC) || (*g_parse == TAB) ){
        g_parse++;
    }
    if ( *g_parse == BLKTOP ){
        g_parse++;
        ans = funcAddSub();
        g_parse++;
        return ans;
    }
    if ( isdigit(*g_parse) || (*g_parse == '+') || (*g_parse == '-') || (*g_parse == '.') ){
        return funcValue();
    }
    if ( isalpha(*g_parse) ){
        return funcSymbol();
    }
    g_error = ERRCODE_FACTOR;
    return 0;
}

//------------------------------------------------
// 乗算・除算・剰余の評価
//------------------------------------------------
static double funcMulDiv()
{
    double ans = funcFactor();
    
    for ( ; ; ){
        switch ( *g_parse ){
            CASE '*':       g_parse++; ans *= funcFactor();
            CASE '/':       g_parse++; ans /= funcFactor();
            CASE '%':       g_parse++; ans  = fmod( ans, funcFactor() );
            CASE SPC:       g_parse++;
            CASE TAB:       g_parse++;
            DEFAULT:        return ans;
        }
    }
}

//------------------------------------------------
// 加算・減算の評価
//------------------------------------------------
static double funcAddSub()
{
    double ans = funcMulDiv();
    
    for ( ; ; ){
        switch ( *g_parse )
		{
            CASE '+':       g_parse++; ans += funcMulDiv();
            CASE '-':       g_parse++; ans -= funcMulDiv();
            CASE SPC:       g_parse++;
            CASE TAB:       g_parse++;
            DEFAULT:        return ans;
        }
    }
}

//------------------------------------------------
// 数式文字列の評価
//------------------------------------------------
extern double calcExpress( const char string[] )
{
    g_parse = string;
    g_error = ERRCODE_SUCCESS;
    return funcAddSub();
}

//------------------------------------------------
// 数式文字列のエラー取得
//------------------------------------------------
extern long calcErrCode()
{
    return g_error;
}

//------------------------------------------------
// メイン関数
//------------------------------------------------
int main( void )
{
    char    buff[ 256 ];
    char*   find;
    
	printf("x-value:");
	scanf("%lf",&x);
	printf("y-value:");
	scanf("%lf",&y);
	
	printf("function:");
	
    while ( fgets(buff,sizeof(buff),stdin) != NULL ){
        if ( (find = strchr(buff,'\n')) != NULL ){
            *find = '\0';
        }
        if ( buff[0] != '\0' ){
            printf( "%s = %f\n", buff, calcExpress(buff) );
            
            switch ( calcErrCode() ){
                CASE ERRCODE_FACTOR:        printf( "ERRCODE_FACTOR\n\n" );
                CASE ERRCODE_FUNCTION:      printf( "ERRCODE_FUNCTION\n\n" );
                CASE ERRCODE_VARIABLE:      printf( "ERRCODE_VARIABLE\n\n" );
                DEFAULT:                    printf( "\n" );
            }
        }
    }
    return 0;
}