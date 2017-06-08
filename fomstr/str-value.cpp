//------------------------------------------------------------------------------
// �����]���̃T���v��(�ϐ����Ɗ֐���)
//------------------------------------------------------------------------------
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------
// break �t���̃L�[���[�h
//------------------------------------------------
#define CASE        break;case
#define DEFAULT     break;default

//------------------------------------------------
// �֐��̃v���g�^�C�v�錾
//------------------------------------------------
static double funcAddSub();

//--[�����]��]------------------------------------------------------------------

//------------------------------------------------
// �L���萔
//------------------------------------------------
#define BLKTOP      '('
#define BLKEND      ')'
#define SPC         ' '
#define TAB         '\t'

//------------------------------------------------
// �񋓒萔(�G���[�R�[�h)
//------------------------------------------------
enum eERRCODE {
    ERRCODE_SUCCESS,        // ����
    ERRCODE_FACTOR,         // ���T�|�[�g�̉��Z�q�ł��B
    ERRCODE_FUNCTION,       // �֐�����������܂���B
    ERRCODE_VARIABLE,       // �ϐ�����������܂���B
    MAX_ERRCODE,
};

//------------------------------------------------
// �O���[�o���ϐ�
//------------------------------------------------
static const char *g_parse;
static       long  g_error;
double x;
double y;

//------------------------------------------------
// �֐��̎��s
//------------------------------------------------
static double funcFunction( const char string[] )
{
    g_error = ERRCODE_FUNCTION;
    string;
    return 0;
}

//------------------------------------------------
// �ϐ��̎Q��
//------------------------------------------------

static double funcVariable(const char string[] )
{
	
	const char xstr[]="x"; 	//�ϐ���x�̔�r�p�̕���
	const char ystr[]="y";  //�ϐ���y�̔�r�p�̕���
	
	
	//�ϐ�����x�Ȃ炻�̕ϐ���x�̒l��Ԃ�
	if((strcmp(string,xstr))==0)	   return  x;
	
	//�ϐ�����y�Ȃ炻�̕ϐ���y�̒l��Ԃ�
	else if((strcmp(string,ystr))==0)	return y;

	g_error = ERRCODE_VARIABLE;
    
	string;
		
    return 0;
}

//------------------------------------------------
// �ϐ��E�֐��̕]��
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
// ���l������̕]��
//------------------------------------------------
static double funcValue()
{
    return strtod( g_parse, (char**)&g_parse );
}

//------------------------------------------------
// ���ʁE���l�E�V���{���̕]��
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
// ��Z�E���Z�E��]�̕]��
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
// ���Z�E���Z�̕]��
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
// ����������̕]��
//------------------------------------------------
extern double calcExpress( const char string[] )
{
    g_parse = string;
    g_error = ERRCODE_SUCCESS;
    return funcAddSub();
}

//------------------------------------------------
// ����������̃G���[�擾
//------------------------------------------------
extern long calcErrCode()
{
    return g_error;
}

//------------------------------------------------
// ���C���֐�
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