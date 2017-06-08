/**************************************--仕様--***************************************
■説明
マーカ「Hiro」「Sample1」「Kanji」を認識して別のオブジェクトを表示させるプログラム
マーカ間の距離・角度の差をコンソールで見ることができる。
*************************************************************************************/

#pragma warning(disable:4819)
#pragma comment(lib,"shell32.lib")

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#include    <windows.h>
STARTUPINFO          si;
PROCESS_INFORMATION  pi;

#include <GL/gl.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES    // math.hのM_PIを使うため
#include <math.h>            // 角度計算用

//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#define MAXDATASIZE 1682
#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>

// グローバル変数
/* カメラ構成 */

char	*vconf_name = "Data/WDM_camera_flipV.xml";	// ビデオデバイスの設定ファイル

int  xsize;                                            // ウィンドウサイズ
int  ysize;                                            // ウィンドウサイズ
int  thresh = 100;                                    // 2値化の閾値
int  count = 0;                                        // 処理フレーム数

													   /* カメラパラメータ */
char *cparam_name = "Data/camera_para_ex1.dat";            // カメラパラメータファイル
ARParam cparam;                                        // カメラパラメータ

													   /* パターンファイル */
#define MARK_NUM        3                        // 使用するマーカーの個数
													   //-----
#define MARK1_MARK_ID    1                        // マーカーID
#define MARK1_PATT_NAME    "Data/patt.a"        // パターンファイル名
#define MARK1_SIZE        80.0                    // パターンの幅（80mm）
													   //-----
#define MARK2_MARK_ID    2                        // マーカーID
#define MARK2_PATT_NAME    "Data/patt.b"    // パターンファイル名
#define MARK2_SIZE        80.0                   // パターンの幅（80mm）
													   //-----
#define MARK3_MARK_ID    3                        // マーカーID
#define MARK3_PATT_NAME    "Data/patt.c"        // パターンファイル名
#define MARK3_SIZE        80.0                    // パターンの幅（80mm）
													   //-----
typedef struct {
	char   *patt_name;            // パターンファイル
	int    patt_id;                // パターンのID
	int    mark_id;                // マーカーID
	int    visible;                // 検出フラグ
	double patt_width;            // パターンのサイズ（単位：ｍｍ）
	double patt_center[2];        // パターンの中心座標
	double patt_trans[3][4];    // 座標変換行列
} MARK_T;
//-----
MARK_T   marker[MARK_NUM] = {
	{ MARK1_PATT_NAME, -1, MARK1_MARK_ID, 0, MARK1_SIZE,{ 0.0, 0.0 } },
	{ MARK2_PATT_NAME, -1, MARK2_MARK_ID, 0, MARK2_SIZE,{ 0.0, 0.0 } },
	{ MARK3_PATT_NAME, -1, MARK3_MARK_ID, 0, MARK3_SIZE,{ 0.0, 0.0 } }
};

// プロトタイプ宣言
void Init(void);
void MainLoop(void);
void SetupLighting1(void);
void SetupLighting2(void);
void SetupMaterial1(void);
void SetupMaterial2(void);
void KeyEvent(unsigned char key, int x, int y);
void MouseEvent(int button, int state, int x, int y);
void Cleanup(void);
void DrawObject(int mark_id, double patt_trans[3][4]);

void mySetLight1(void);
void mySetLight3(void);
void mySetMaterial(void);
void axis1(GLfloat d, GLfloat s);
void axis2(GLfloat d, GLfloat s);
void axis3(GLfloat d, GLfloat s);
void subaxis(GLfloat d, GLfloat s);

GLfloat v[41][41][3], n[41][41][3];// 頂点、法線ベクトル格納用の配列
GLfloat vv[41][41][3], nn[41][41][3];// 頂点、法線ベクトル格納用の配列
GLfloat vvv[41][41][3], nnn[41][41][3];// 頂点、法線ベクトル格納用の配列
GLfloat angle = 0.0; // 角度
unsigned char wireFrag = GL_TRUE;       //物体の面--フレーム表示を切り替えるためのフラグ

										// 曲面の頂点と法線ベクトルを計算し、配列に格納
void compute(void)
{

	FILE *ff, *fvz, *fnx, *fny, *fnz;
	//ShellExecute(NULL, NULL, "fdel.exe", NULL, NULL, SW_HIDE);
	//Sleep(1000);
	//ShellExecute(NULL, NULL, "formula.exe", NULL, NULL, SW_SHOWNORMAL);
	//Sleep(1000);

	/*while (1)
	{
	fopen_s(&ff, "formula.txt", "r");
	if (ff != NULL)
	{
	break;
	}

	else		fclose(ff);

	}
	fclose(ff);*/

	///*

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	//si.dwFlags= STARTF_USESHOWWINDOW;
	//si.dwFlags= SW_SHOWNORMAL;
	CreateProcess(NULL, "formula.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (pi.hProcess)
	{
		DWORD   dwExitCode = STILL_ACTIVE;
		while (dwExitCode == STILL_ACTIVE)
		{
			WaitForSingleObject(pi.hProcess, 1000);
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
		}
	}

	//*/







	ShellExecute(NULL, NULL, "C:/Program Files/R/R-3.3.1/bin/Rscript.exe", "script.R", NULL, SW_HIDE);
	Sleep(2000);

	char datavz[MAXDATASIZE];
	char datanx[MAXDATASIZE], datany[MAXDATASIZE], datanz[MAXDATASIZE];


	////fopen_s(&fvx,"output_vx.txt", "r");
	////fopen_s(&fvy,"output_vy.txt", "r");
	fopen_s(&fvz, "output_vz.txt", "r");

	fopen_s(&fnx, "output_nx.txt", "r");
	fopen_s(&fny, "output_ny.txt", "r");
	fopen_s(&fnz, "output_nz.txt", "r");

	int i, j;
	float x, y;


	for (i = 0; i <= 40; i++)
	{


		for (j = 0; j <= 40; j++)
		{

			x = ((float)i) / 10.0 - 2.0;
			y = ((float)j) / 10.0 - 2.0;
			///fgets(datavx, MAXDATASIZE, fvx);
			///fgets(datavy, MAXDATASIZE, fvy);
			fgets(datavz, MAXDATASIZE, fvz);

			fgets(datanx, MAXDATASIZE, fnx);
			fgets(datany, MAXDATASIZE, fny);
			fgets(datanz, MAXDATASIZE, fnz);

			v[i][j][0] = y * 20;
			v[i][j][1] = atof(datavz);
			v[i][j][2] = x * 20;
			n[i][j][0] = atof(datany);
			n[i][j][1] = atof(datanz);
			n[i][j][2] = atof(datanx);

		}
	}

	fclose(fvz);
	fclose(fnx);
	fclose(fny);
	fclose(fnz);


}

//=======================================================
// main関数
//=======================================================
int main(int argc, char **argv)
{




	compute();


	// GLUTの初期化
	glutInit(&argc, argv);

	// ARアプリケーションの初期化
	Init();

	// ビデオキャプチャの開始
	arVideoCapStart();

	// メインループの開始
	argMainLoop(MouseEvent, KeyEvent, MainLoop);

	return 0;
}


//=======================================================
// 初期化関数
//=======================================================
void Init(void)
{
	ARParam wparam;        // カメラパラメータ

						   // ビデオデバイスの設定
	if (arVideoOpen(vconf_name) < 0) {
		printf("ビデオデバイスのエラー\n");
		exit(0);
	}

	// ウィンドウサイズの取得
	if (arVideoInqSize(&xsize, &ysize) < 0) exit(0);
	printf("Image size (x,y) = (%d,$d)\n", xsize, ysize);

	// カメラパラメータの設定
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("カメラパラメータの読み込みに失敗しました\n");
		exit(0);
	}

	// カメラパラメータのサイズ調整
	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	// カメラパラメータの初期化
	arInitCparam(&cparam);
	printf("*** Camera Parameter ***\n");
	arParamDisp(&cparam);

	// パターンファイルのロード
	for (int i = 0; i<MARK_NUM; i++) {
		if ((marker[i].patt_id = arLoadPatt(marker[i].patt_name)) < 0) {
			printf("パターンファイルの読み込みに失敗しました\n");
			printf("%s\n", marker[i].patt_name);
			exit(0);
		}
	}

	// gsubライブラリの初期化
	argInit(&cparam, 1.0, 0, 0, 0, 0);

	// ウィンドウタイトルの設定
	glutSetWindowTitle("ARTK_MultiMarker");
}


//=======================================================
// メインループ関数
//=======================================================
void MainLoop(void)
{
	ARUint8          *image;            // カメラキャプチャ画像
	ARMarkerInfo     *marker_info;        // マーカ情報
	int              marker_num;        // 検出されたマーカの数
	int              i, j, k;

	// カメラ画像の取得
	if ((image = (ARUint8 *)arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}
	if (count == 0) arUtilTimerReset();
	count++;

	// カメラ画像の描画
	argDrawMode2D();
	argDispImage(image, 0, 0);

	// マーカの検出と認識
	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);
	}

	// 次の画像のキャプチャ指示
	arVideoCapNext();

	// 3Dオブジェクトを描画するための準備
	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glClearDepth(1.0);                    // デプスバッファの消去値
	glClear(GL_DEPTH_BUFFER_BIT);        // デプスバッファの初期化

										 // マーカの数分だけループ
	for (i = 0; i<MARK_NUM; i++) {
		// マーカの一致度の比較
		k = -1;
		for (j = 0; j<marker_num; j++) {
			if (marker[i].patt_id == marker_info[j].id) {
				if (k == -1) k = j;
				else if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}

		// マーカーが見つからなかったとき
		if (k == -1) {
			marker[i].visible = 0;
			continue;
		}

		// 座標変換行列を取得
		if (marker[i].visible == 0) {
			// 1フレームを使ってマーカの位置・姿勢（座標変換行列）の計算
			arGetTransMat(&marker_info[k], marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		else {
			// 前のフレームを使ってマーカの位置・姿勢（座標変換行列）の計算
			arGetTransMatCont(&marker_info[k], marker[i].patt_trans, marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		marker[i].visible = 1;

		// 3Dオブジェクトの描画
		DrawObject(marker[i].mark_id, marker[i].patt_trans);
	}

	// バッファの内容を画面に表示
	argSwapBuffers();

	// 2つのマーカ間の距離を表示（マーカ1[Hiro]とマーカ2[Sample1]を認識した場合）
	if (marker[0].visible > 0 && marker[1].visible > 0) {
		double wmat1[3][4], wmat2[3][4];

		// ビュー→マーカ行列（カメラ座標系を基準に考えたマーカの位置・姿勢）を取得
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// マーカ1座標系を基準に考えたマーカ2の位置・姿勢（＝マーカ1とマーカ2の距離・角度の差）を取得
		arUtilMatMul(wmat1, marker[1].patt_trans, wmat2);

		// 距離を表示(x, y, z)
		printf("%5.4lf[mm] %5.4lf[mm] %5.4lf[mm]\n", wmat2[0][3], wmat2[1][3], wmat2[2][3]);
	}

	// 2つのマーカ間の角度の差を表示（マーカ1[Hiro]とマーカ3[Kanji]を認識した場合）
	if (marker[0].visible > 0 && marker[2].visible > 0) {
		double wmat1[3][4], wmat2[3][4];
		double yaw, pitch, roll;

		// ビュー→マーカ行列（カメラ座標系を基準に考えたマーカの位置・姿勢）を取得
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// マーカ1座標系を基準に考えたマーカ3の位置（＝マーカ1とマーカ3の距離・姿勢の差）を取得
		arUtilMatMul(wmat1, marker[2].patt_trans, wmat2);

		// 姿勢の差を表示
		//for( i=0; i<3; i++ ) {
		//    for( j=0; j< 3; j++ ) printf("%5.4f ", wmat2[i][j]);
		//    printf("\n");
		//}
		//printf("\n");

		// 角度の差を表示（-180°～180°）
		yaw = atan2(wmat2[1][0], wmat2[0][0]);
		pitch = atan2(wmat2[2][1], wmat2[2][2]);
		roll = atan2(wmat2[2][0], sqrt(wmat2[2][1] * wmat2[2][1] + wmat2[2][2] * wmat2[2][2]));

		printf("yaw = %4.4lf pitch = %4.4lf roll = %4.4lf\n", 180.0*yaw / M_PI, 180.0*pitch / M_PI, 180.0*roll / M_PI);
	}
}


//=======================================================
// 3Dオブジェクトの描画を行う関数
//=======================================================
void DrawObject(int mark_id, double patt_trans[3][4])
{
	double gl_para[16];    // ARToolKit->OpenGL変換行列

						   // 陰面消去
	glEnable(GL_DEPTH_TEST);            // 陰面消去・有効
	glDepthFunc(GL_LEQUAL);            // デプステスト

									   // 変換行列の適用
	argConvGlpara(patt_trans, gl_para);    // ARToolKitからOpenGLの行列に変換
	glMatrixMode(GL_MODELVIEW);            // 行列変換モード・モデルビュー
	glLoadMatrixd(gl_para);                // 読み込む行列を指定

	switch (mark_id) {
	case MARK1_MARK_ID:
		// 3Dオブジェクトの描画
		glClear(GL_DEPTH_BUFFER_BIT);			// Zバッファの初期化
		glEnable(GL_DEPTH_TEST);				// 隠面処理の適用

		mySetLight1();							// 光源の設定
		glEnable(GL_LIGHTING);				// 光源の適用

											//	GLfloat d = 1.0, s = 80.0;              //軸を描画するための設定

		glScalef(0.8, 0.8, 0.8);
		glPushMatrix();
		glTranslatef(0.0, 0.0, 80.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を立てにするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	// 立体を回転させる

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis1(1.0, 80.0);			// 軸の描画
		mySetMaterial();	// 材質特性の適用


		if (wireFrag == GL_TRUE) {
			// 以下、曲面の描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// 曲面を三角形分割して表示
				for (j = 0; j <= 40; j++) {
					if (v[i][j][1] >= 200)	v[i][j][1] = 200;
					else if (v[i][j][1] <= -200)	v[i][j][1] = -200;
					glNormal3f(n[i][j][0], n[i][j][1], n[i][j][2]);
					glVertex3f(v[i][j][0], v[i][j][1], v[i][j][2]);
					glNormal3f(n[i + 1][j][0], n[i + 1][j][1], n[i + 1][j][2]);
					glVertex3f(v[i + 1][j][0], v[i + 1][j][1], v[i + 1][j][2]);


				}
				glEnd();
			}

		}
		else {
			// 以下、曲面をフレームで描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// 曲面の縦線を表示
				for (j = 0; j <= 40; j++) {
					if (v[i][j][1] >= 200)	v[i][j][1] = 200;
					else if (v[i][j][1] <= -200)	v[i][j][1] = -200;
					glNormal3f(n[i][j][0], n[i][j][1], n[i][j][2]);
					glVertex3f(v[i][j][0], v[i][j][1], v[i][j][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);


				}
				glEnd();
			}

			int k, l;
			for (l = 0; l<40; l++) {
				glBegin(GL_LINE_STRIP);// 曲面の横線を表示
				for (k = 0; k <= 40; k++) {
					if (v[k][l][1] >= 200)	v[k][l][1] = 200;
					else if (v[k][l][1] <= -200)	v[k][l][1] = -200;
					glNormal3f(n[k][l][0], n[k][l][1], n[k][l][2]);
					glVertex3f(v[k][l][0], v[k][l][1], v[k][l][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);


				}
				glEnd();
			}
		}


		glPopMatrix();

		glDepthMask(GL_TRUE);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		glPushMatrix();
		glTranslatef(0.0, 0.0, 80.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を縦にするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	    // 立体をを回転させる

												// 以下、曲面の中心を通る線の描画（空間内に曲線を描くルーチン）
		int k;
		glColor3d(0.5, 0.5, 0.0);               //黄色
		glBegin(GL_LINE_STRIP);
		for (k = 0; k <= 40; k++) {
			glVertex3f(v[20][k][0], v[20][k][1], v[20][k][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int l;
		glColor3d(0.0, 0.5, 0.5);              //緑色
		glBegin(GL_LINE_STRIP);
		for (l = 0; l <= 40; l++) {
			glVertex3f(v[l][20][0], v[l][20][1], v[l][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();

		break;

	case MARK2_MARK_ID:
		// 3Dオブジェクトの描画
		glClear(GL_DEPTH_BUFFER_BIT);			// Zバッファの初期化
		glEnable(GL_DEPTH_TEST);				// 隠面処理の適用

		mySetLight1();							// 光源の設定
		glEnable(GL_LIGHTING);				// 光源の適用

											//			GLfloat d = 1.0, s = 80.0;              //軸を描画するための設定

		glScalef(0.8, 0.8, 0.8);

		glPushMatrix();
		glTranslatef(0.0, 0.0, 30.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を立てにするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	// 立体を回転させる

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis2(1.0, 80.0);			// 軸の描画
		mySetMaterial();	// 材質特性の適用


		if (wireFrag == GL_TRUE) {
			// 以下、曲面の描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// 曲面を三角形分割して表示
				for (j = 0; j <= 40; j++) {
					glNormal3f(nn[i][j][0], nn[i][j][1], nn[i][j][2]);
					glVertex3f(vv[i][j][0], vv[i][j][1], vv[i][j][2]);
					glNormal3f(nn[i + 1][j][0], nn[i + 1][j][1], nn[i + 1][j][2]);
					glVertex3f(vv[i + 1][j][0], vv[i + 1][j][1], vv[i + 1][j][2]);


				}
				glEnd();
			}
		}

		else {
			// 以下、曲面をフレームで描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// 曲面の縦線を表示
				for (j = 0; j <= 40; j++) {
					glNormal3f(nn[i][j][0], nn[i][j][1], nn[i][j][2]);
					glVertex3f(vv[i][j][0], vv[i][j][1], vv[i][j][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);
				}
				glEnd();
			}

			int k, l;
			for (l = 0; l<40; l++) {
				glBegin(GL_LINE_STRIP);// 曲面の横線を表示
				for (k = 0; k <= 40; k++) {
					glNormal3f(nn[k][l][0], nn[k][l][1], nn[k][l][2]);
					glVertex3f(vv[k][l][0], vv[k][l][1], vv[k][l][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);
				}
				glEnd();
			}
		}


		glPopMatrix();

		glDepthMask(GL_TRUE);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		glPushMatrix();
		glTranslatef(0.0, 0.0, 30.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を縦にするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	    // 立体をを回転させる

												// 以下、曲面の中心を通る線の描画（空間内に曲線を描くルーチン）
		int m;
		glColor3d(0.5, 0.5, 0.0);               //黄色
		glBegin(GL_LINE_STRIP);
		for (m = 0; m <= 40; m++) {
			glVertex3f(vv[20][m][0], vv[20][m][1], vv[20][m][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int p;
		glColor3d(0.0, 0.5, 0.5);              //緑色
		glBegin(GL_LINE_STRIP);
		for (p = 0; p <= 40; p++) {
			glVertex3f(vv[p][20][0], vv[p][20][1], vv[p][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();
		break;

	case MARK3_MARK_ID:
		// 3Dオブジェクトの描画
		glClear(GL_DEPTH_BUFFER_BIT);			// Zバッファの初期化
		glEnable(GL_DEPTH_TEST);				// 隠面処理の適用

		mySetLight3();							// 光源の設定
		glEnable(GL_LIGHTING);				// 光源の適用

											//			GLfloat d = 1.0, s = 80.0;              //軸を描画するための設定

		glScalef(0.8, 0.8, 0.8);
		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を立てにするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	// 立体を回転させる

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis3(1.0, 80.0);			// 軸の描画
		mySetMaterial();	// 材質特性の適用


		if (wireFrag == GL_TRUE) {
			// 以下、曲面の描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// 曲面を三角形分割して表示
				for (j = 0; j <= 40; j++) {
					glNormal3f(nnn[i][j][0], nnn[i][j][1], nnn[i][j][2]);
					glVertex3f(vvv[i][j][0], vvv[i][j][1], vvv[i][j][2]);
					glNormal3f(nnn[i + 1][j][0], nnn[i + 1][j][1], nnn[i + 1][j][2]);
					glVertex3f(vvv[i + 1][j][0], vvv[i + 1][j][1], vvv[i + 1][j][2]);
				}
				glEnd();
			}
		}
		else {
			// 以下、曲面をフレームで描画（多面体を描くルーチン）
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// 曲面の縦線を表示
				for (j = 0; j <= 40; j++) {
					glNormal3f(nnn[i][j][0], nnn[i][j][1], nnn[i][j][2]);
					glVertex3f(vvv[i][j][0], vvv[i][j][1], vvv[i][j][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);
				}
				glEnd();
			}

			int k, l;
			for (l = 0; l<40; l++) {
				glBegin(GL_LINE_STRIP);// 曲面の横線を表示
				for (k = 0; k <= 40; k++) {
					glNormal3f(nnn[k][l][0], nnn[k][l][1], nnn[k][l][2]);
					glVertex3f(vvv[k][l][0], vvv[k][l][1], vvv[k][l][2]);
					//  glNormal3f(n[i+1][j][0],n[i+1][j][1],n[i+1][j][2]);
					//	glVertex3f(v[i+1][j][0],v[i+1][j][1],v[i+1][j][2]);
				}
				glEnd();
			}
		}


		glPopMatrix();

		glDepthMask(GL_TRUE);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);		// マーカに載せるために平行移動
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z軸を縦にするための回転
		glRotatef(angle, 0.0, 1.0, 0.0);	    // 立体をを回転させる

												// 以下、曲面の中心を通る線の描画（空間内に曲線を描くルーチン）
		int c;
		glColor3d(0.5, 0.5, 0.0);               //黄色
		glBegin(GL_LINE_STRIP);
		for (c = 0; c <= 40; c++) {
			glVertex3f(vvv[20][c][0], vvv[20][c][1], vvv[20][c][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int b;
		glColor3d(0.0, 0.5, 0.5);              //緑色
		glBegin(GL_LINE_STRIP);
		for (b = 0; b <= 40; b++) {
			glVertex3f(vvv[b][20][0], vvv[b][20][1], vvv[b][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();

		break;
	}


	// 終了処理
	glDisable(GL_LIGHTING);        // ライティング・無効
	glDisable(GL_DEPTH_TEST);        // デプステスト・無効
}

void axis1(GLfloat d, GLfloat s)
{
	glPushMatrix();
	subaxis(d, s);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	subaxis(d, s);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90.0, 0.0, 0.0, 1.0);
	subaxis(d, s);
	glPopMatrix();

}

void axis2(GLfloat d, GLfloat s)
{
	glPushMatrix();
	glTranslatef(0.0, 50.0, 0.0);
	subaxis(d, s);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	subaxis(d, s);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90.0, 0.0, 0.0, 1.0);
	subaxis(d, s);
	glPopMatrix();

}

void axis3(GLfloat d, GLfloat s)
{
	glPushMatrix();
	glTranslatef(0.0, 20.0, 0.0);
	subaxis(d, 40.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	subaxis(d, s);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90.0, 0.0, 0.0, 1.0);
	subaxis(d, s);
	glPopMatrix();

}


void subaxis(GLfloat d, GLfloat s)
{
	int i;
	double pi = 3.14, t;
	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= 6; i++) {
		t = i * 2 * pi / 6;
		glNormal3f((GLfloat)cos(t), 0.0, (GLfloat)sin(t));
		glVertex3f((GLfloat)(d*cos(t)), -s, (GLfloat)(d*sin(t)));
		glVertex3f((GLfloat)(d*cos(t)), s, (GLfloat)(d*sin(t)));
	}
	glEnd();
	glTranslatef(0.0, s, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.0*d, 4.0*d, 5, 5);
}


//===========================================================================
// 光源の設定を行う関数
//===========================================================================
void mySetLight1(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// 拡散反射光
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// 鏡面反射光
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// 環境光
														//GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// 位置と種類
	GLfloat light_position[] = { 200.0, 0.0, 100.0, 0.0 };	// 位置と種類

															// 光源の設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // 拡散反射光の設定
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // 鏡面反射光の設定
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // 環境光の設定
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 位置と種類の設定

	glShadeModel(GL_SMOOTH);	// シェーディングの種類の設定
	glEnable(GL_LIGHT0);		// 光源の有効化
}

void mySetLight3(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// 拡散反射光
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// 鏡面反射光
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// 環境光
														//	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// 位置と種類
	GLfloat light_position[] = { 200.0, 100.0, 100.0, 0.0 };	// 位置と種類

																// 光源の設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // 拡散反射光の設定
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // 鏡面反射光の設定
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // 環境光の設定
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 位置と種類の設定

	glShadeModel(GL_SMOOTH);	// シェーディングの種類の設定
	glEnable(GL_LIGHT0);		// 光源の有効化
}

//===========================================================================
// 材質特性の設定を行う関数
//===========================================================================
void mySetMaterial(void)
{

	GLfloat mat_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };	// 拡散反射
	GLfloat mat_specular1[] = { 1.0, 1.0, 1.0, 1.0 };	// 鏡面反射
	GLfloat mat_ambient1[] = { 0.7, 0.7, 0.7, 1.0 };	// 環境光反射
	GLfloat shininess1[] = { 50.0 };					// 鏡面反射の鋭さ

	GLfloat mat_diffuse[] = { 0.5, 0.9, 0.9, 0.90 };	// 拡散反射
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.90 };	// 鏡面反射
	GLfloat mat_ambient[] = { 0.1, 0.3, 0.9, 0.90 };	// 環境光反射
	GLfloat shininess[] = { 50.0 };					// 鏡面反射の鋭さ

	GLfloat mat_diffuse_back[] = { 0.05, 0.25, 0.40, 0.45 };	// 拡散反射
	GLfloat mat_specular_back[] = { 0.50, 0.50, 0.50, 0.45 };	// 鏡面反射
	GLfloat mat_ambient_back[] = { 0.25, 0.25, 0.35, 0.45 };	// 環境光反射
	GLfloat shininess_back[] = { 50.0 };					// 鏡面反射の鋭さ

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);	// 拡散反射の設定
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);	// 鏡面反射の設定
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);	// 環境光反射の設定
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess1);		// 鏡面反射の鋭さの設定

	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);	// 拡散反射の設定
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);	// 鏡面反射の設定
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);	// 環境光反射の設定
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);		// 鏡面反射の鋭さの設定

	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse_back);	// 拡散反射の設定
	glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular_back);	// 鏡面反射の設定
	glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient_back);	// 環境光反射の設定
	glMaterialfv(GL_BACK, GL_SHININESS, shininess_back);		// 鏡面反射の鋭さの設定

}

//===========================================================================
// キーボード入力処理関数
//===========================================================================
void KeyEvent(unsigned char key, int x, int y)
{
	/*if (key == 'r') {
	angle += 3.0;	// rキーを押す度に角度を3.0度ずつ増加する
	}*/
	/*if (key == 'w') {
	wireFrag = !wireFrag;	// フラグの値は、wキーを押すたびに0と1を繰り返す
	}*/
	// ESCキーを入力したらアプリケーション終了
	/*else */if (key == 0x1b) {
		Cleanup();
		exit(0);
	}
}



//=======================================================
// マウス入力処理関数
//=======================================================
void MouseEvent(int button, int state, int x, int y)
{
	// 入力状態を表示
	//printf("ボタン：%d 状態：%d 座標：(x,y)=(%d,%d) \n", button, state, x, y);


	if (state == GLUT_DOWN)
	{

		/*	else */if (button == GLUT_RIGHT_BUTTON)  wireFrag = !wireFrag;
	}
	else if (button == GLUT_LEFT_BUTTON)		angle += 6.0;

}


//=======================================================
// 終了処理関数
//=======================================================
void Cleanup(void)
{
	arVideoCapStop();    // ビデオキャプチャの停止
	arVideoClose();        // ビデオデバイスの終了
	argCleanup();        // ARToolKitの終了処理
}