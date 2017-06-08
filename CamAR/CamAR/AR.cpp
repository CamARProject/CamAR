/**************************************--�d�l--***************************************
������
�}�[�J�uHiro�v�uSample1�v�uKanji�v��F�����ĕʂ̃I�u�W�F�N�g��\��������v���O����
�}�[�J�Ԃ̋����E�p�x�̍����R���\�[���Ō��邱�Ƃ��ł���B
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

#define _USE_MATH_DEFINES    // math.h��M_PI���g������
#include <math.h>            // �p�x�v�Z�p

//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#define MAXDATASIZE 1682
#include <AR/ar.h>
#include <AR/param.h>
#include <AR/video.h>
#include <AR/gsub.h>

// �O���[�o���ϐ�
/* �J�����\�� */

char	*vconf_name = "Data/WDM_camera_flipV.xml";	// �r�f�I�f�o�C�X�̐ݒ�t�@�C��

int  xsize;                                            // �E�B���h�E�T�C�Y
int  ysize;                                            // �E�B���h�E�T�C�Y
int  thresh = 100;                                    // 2�l����臒l
int  count = 0;                                        // �����t���[����

													   /* �J�����p�����[�^ */
char *cparam_name = "Data/camera_para_ex1.dat";            // �J�����p�����[�^�t�@�C��
ARParam cparam;                                        // �J�����p�����[�^

													   /* �p�^�[���t�@�C�� */
#define MARK_NUM        3                        // �g�p����}�[�J�[�̌�
													   //-----
#define MARK1_MARK_ID    1                        // �}�[�J�[ID
#define MARK1_PATT_NAME    "Data/patt.a"        // �p�^�[���t�@�C����
#define MARK1_SIZE        80.0                    // �p�^�[���̕��i80mm�j
													   //-----
#define MARK2_MARK_ID    2                        // �}�[�J�[ID
#define MARK2_PATT_NAME    "Data/patt.b"    // �p�^�[���t�@�C����
#define MARK2_SIZE        80.0                   // �p�^�[���̕��i80mm�j
													   //-----
#define MARK3_MARK_ID    3                        // �}�[�J�[ID
#define MARK3_PATT_NAME    "Data/patt.c"        // �p�^�[���t�@�C����
#define MARK3_SIZE        80.0                    // �p�^�[���̕��i80mm�j
													   //-----
typedef struct {
	char   *patt_name;            // �p�^�[���t�@�C��
	int    patt_id;                // �p�^�[����ID
	int    mark_id;                // �}�[�J�[ID
	int    visible;                // ���o�t���O
	double patt_width;            // �p�^�[���̃T�C�Y�i�P�ʁF�����j
	double patt_center[2];        // �p�^�[���̒��S���W
	double patt_trans[3][4];    // ���W�ϊ��s��
} MARK_T;
//-----
MARK_T   marker[MARK_NUM] = {
	{ MARK1_PATT_NAME, -1, MARK1_MARK_ID, 0, MARK1_SIZE,{ 0.0, 0.0 } },
	{ MARK2_PATT_NAME, -1, MARK2_MARK_ID, 0, MARK2_SIZE,{ 0.0, 0.0 } },
	{ MARK3_PATT_NAME, -1, MARK3_MARK_ID, 0, MARK3_SIZE,{ 0.0, 0.0 } }
};

// �v���g�^�C�v�錾
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

GLfloat v[41][41][3], n[41][41][3];// ���_�A�@���x�N�g���i�[�p�̔z��
GLfloat vv[41][41][3], nn[41][41][3];// ���_�A�@���x�N�g���i�[�p�̔z��
GLfloat vvv[41][41][3], nnn[41][41][3];// ���_�A�@���x�N�g���i�[�p�̔z��
GLfloat angle = 0.0; // �p�x
unsigned char wireFrag = GL_TRUE;       //���̖̂�--�t���[���\����؂�ւ��邽�߂̃t���O

										// �Ȗʂ̒��_�Ɩ@���x�N�g�����v�Z���A�z��Ɋi�[
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
// main�֐�
//=======================================================
int main(int argc, char **argv)
{




	compute();


	// GLUT�̏�����
	glutInit(&argc, argv);

	// AR�A�v���P�[�V�����̏�����
	Init();

	// �r�f�I�L���v�`���̊J�n
	arVideoCapStart();

	// ���C�����[�v�̊J�n
	argMainLoop(MouseEvent, KeyEvent, MainLoop);

	return 0;
}


//=======================================================
// �������֐�
//=======================================================
void Init(void)
{
	ARParam wparam;        // �J�����p�����[�^

						   // �r�f�I�f�o�C�X�̐ݒ�
	if (arVideoOpen(vconf_name) < 0) {
		printf("�r�f�I�f�o�C�X�̃G���[\n");
		exit(0);
	}

	// �E�B���h�E�T�C�Y�̎擾
	if (arVideoInqSize(&xsize, &ysize) < 0) exit(0);
	printf("Image size (x,y) = (%d,$d)\n", xsize, ysize);

	// �J�����p�����[�^�̐ݒ�
	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("�J�����p�����[�^�̓ǂݍ��݂Ɏ��s���܂���\n");
		exit(0);
	}

	// �J�����p�����[�^�̃T�C�Y����
	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	// �J�����p�����[�^�̏�����
	arInitCparam(&cparam);
	printf("*** Camera Parameter ***\n");
	arParamDisp(&cparam);

	// �p�^�[���t�@�C���̃��[�h
	for (int i = 0; i<MARK_NUM; i++) {
		if ((marker[i].patt_id = arLoadPatt(marker[i].patt_name)) < 0) {
			printf("�p�^�[���t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
			printf("%s\n", marker[i].patt_name);
			exit(0);
		}
	}

	// gsub���C�u�����̏�����
	argInit(&cparam, 1.0, 0, 0, 0, 0);

	// �E�B���h�E�^�C�g���̐ݒ�
	glutSetWindowTitle("ARTK_MultiMarker");
}


//=======================================================
// ���C�����[�v�֐�
//=======================================================
void MainLoop(void)
{
	ARUint8          *image;            // �J�����L���v�`���摜
	ARMarkerInfo     *marker_info;        // �}�[�J���
	int              marker_num;        // ���o���ꂽ�}�[�J�̐�
	int              i, j, k;

	// �J�����摜�̎擾
	if ((image = (ARUint8 *)arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}
	if (count == 0) arUtilTimerReset();
	count++;

	// �J�����摜�̕`��
	argDrawMode2D();
	argDispImage(image, 0, 0);

	// �}�[�J�̌��o�ƔF��
	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		Cleanup();
		exit(0);
	}

	// ���̉摜�̃L���v�`���w��
	arVideoCapNext();

	// 3D�I�u�W�F�N�g��`�悷�邽�߂̏���
	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glClearDepth(1.0);                    // �f�v�X�o�b�t�@�̏����l
	glClear(GL_DEPTH_BUFFER_BIT);        // �f�v�X�o�b�t�@�̏�����

										 // �}�[�J�̐����������[�v
	for (i = 0; i<MARK_NUM; i++) {
		// �}�[�J�̈�v�x�̔�r
		k = -1;
		for (j = 0; j<marker_num; j++) {
			if (marker[i].patt_id == marker_info[j].id) {
				if (k == -1) k = j;
				else if (marker_info[k].cf < marker_info[j].cf) k = j;
			}
		}

		// �}�[�J�[��������Ȃ������Ƃ�
		if (k == -1) {
			marker[i].visible = 0;
			continue;
		}

		// ���W�ϊ��s����擾
		if (marker[i].visible == 0) {
			// 1�t���[�����g���ă}�[�J�̈ʒu�E�p���i���W�ϊ��s��j�̌v�Z
			arGetTransMat(&marker_info[k], marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		else {
			// �O�̃t���[�����g���ă}�[�J�̈ʒu�E�p���i���W�ϊ��s��j�̌v�Z
			arGetTransMatCont(&marker_info[k], marker[i].patt_trans, marker[i].patt_center, marker[i].patt_width, marker[i].patt_trans);
		}
		marker[i].visible = 1;

		// 3D�I�u�W�F�N�g�̕`��
		DrawObject(marker[i].mark_id, marker[i].patt_trans);
	}

	// �o�b�t�@�̓��e����ʂɕ\��
	argSwapBuffers();

	// 2�̃}�[�J�Ԃ̋�����\���i�}�[�J1[Hiro]�ƃ}�[�J2[Sample1]��F�������ꍇ�j
	if (marker[0].visible > 0 && marker[1].visible > 0) {
		double wmat1[3][4], wmat2[3][4];

		// �r���[���}�[�J�s��i�J�������W�n����ɍl�����}�[�J�̈ʒu�E�p���j���擾
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// �}�[�J1���W�n����ɍl�����}�[�J2�̈ʒu�E�p���i���}�[�J1�ƃ}�[�J2�̋����E�p�x�̍��j���擾
		arUtilMatMul(wmat1, marker[1].patt_trans, wmat2);

		// ������\��(x, y, z)
		printf("%5.4lf[mm] %5.4lf[mm] %5.4lf[mm]\n", wmat2[0][3], wmat2[1][3], wmat2[2][3]);
	}

	// 2�̃}�[�J�Ԃ̊p�x�̍���\���i�}�[�J1[Hiro]�ƃ}�[�J3[Kanji]��F�������ꍇ�j
	if (marker[0].visible > 0 && marker[2].visible > 0) {
		double wmat1[3][4], wmat2[3][4];
		double yaw, pitch, roll;

		// �r���[���}�[�J�s��i�J�������W�n����ɍl�����}�[�J�̈ʒu�E�p���j���擾
		arUtilMatInv(marker[0].patt_trans, wmat1);
		// �}�[�J1���W�n����ɍl�����}�[�J3�̈ʒu�i���}�[�J1�ƃ}�[�J3�̋����E�p���̍��j���擾
		arUtilMatMul(wmat1, marker[2].patt_trans, wmat2);

		// �p���̍���\��
		//for( i=0; i<3; i++ ) {
		//    for( j=0; j< 3; j++ ) printf("%5.4f ", wmat2[i][j]);
		//    printf("\n");
		//}
		//printf("\n");

		// �p�x�̍���\���i-180���`180���j
		yaw = atan2(wmat2[1][0], wmat2[0][0]);
		pitch = atan2(wmat2[2][1], wmat2[2][2]);
		roll = atan2(wmat2[2][0], sqrt(wmat2[2][1] * wmat2[2][1] + wmat2[2][2] * wmat2[2][2]));

		printf("yaw = %4.4lf pitch = %4.4lf roll = %4.4lf\n", 180.0*yaw / M_PI, 180.0*pitch / M_PI, 180.0*roll / M_PI);
	}
}


//=======================================================
// 3D�I�u�W�F�N�g�̕`����s���֐�
//=======================================================
void DrawObject(int mark_id, double patt_trans[3][4])
{
	double gl_para[16];    // ARToolKit->OpenGL�ϊ��s��

						   // �A�ʏ���
	glEnable(GL_DEPTH_TEST);            // �A�ʏ����E�L��
	glDepthFunc(GL_LEQUAL);            // �f�v�X�e�X�g

									   // �ϊ��s��̓K�p
	argConvGlpara(patt_trans, gl_para);    // ARToolKit����OpenGL�̍s��ɕϊ�
	glMatrixMode(GL_MODELVIEW);            // �s��ϊ����[�h�E���f���r���[
	glLoadMatrixd(gl_para);                // �ǂݍ��ލs����w��

	switch (mark_id) {
	case MARK1_MARK_ID:
		// 3D�I�u�W�F�N�g�̕`��
		glClear(GL_DEPTH_BUFFER_BIT);			// Z�o�b�t�@�̏�����
		glEnable(GL_DEPTH_TEST);				// �B�ʏ����̓K�p

		mySetLight1();							// �����̐ݒ�
		glEnable(GL_LIGHTING);				// �����̓K�p

											//	GLfloat d = 1.0, s = 80.0;              //����`�悷�邽�߂̐ݒ�

		glScalef(0.8, 0.8, 0.8);
		glPushMatrix();
		glTranslatef(0.0, 0.0, 80.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z���𗧂Ăɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	// ���̂���]������

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis1(1.0, 80.0);			// ���̕`��
		mySetMaterial();	// �ގ������̓K�p


		if (wireFrag == GL_TRUE) {
			// �ȉ��A�Ȗʂ̕`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// �Ȗʂ��O�p�`�������ĕ\��
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
			// �ȉ��A�Ȗʂ��t���[���ŕ`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// �Ȗʂ̏c����\��
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
				glBegin(GL_LINE_STRIP);// �Ȗʂ̉�����\��
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
		glTranslatef(0.0, 0.0, 80.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z�����c�ɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	    // ���̂�����]������

												// �ȉ��A�Ȗʂ̒��S��ʂ���̕`��i��ԓ��ɋȐ���`�����[�`���j
		int k;
		glColor3d(0.5, 0.5, 0.0);               //���F
		glBegin(GL_LINE_STRIP);
		for (k = 0; k <= 40; k++) {
			glVertex3f(v[20][k][0], v[20][k][1], v[20][k][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int l;
		glColor3d(0.0, 0.5, 0.5);              //�ΐF
		glBegin(GL_LINE_STRIP);
		for (l = 0; l <= 40; l++) {
			glVertex3f(v[l][20][0], v[l][20][1], v[l][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();

		break;

	case MARK2_MARK_ID:
		// 3D�I�u�W�F�N�g�̕`��
		glClear(GL_DEPTH_BUFFER_BIT);			// Z�o�b�t�@�̏�����
		glEnable(GL_DEPTH_TEST);				// �B�ʏ����̓K�p

		mySetLight1();							// �����̐ݒ�
		glEnable(GL_LIGHTING);				// �����̓K�p

											//			GLfloat d = 1.0, s = 80.0;              //����`�悷�邽�߂̐ݒ�

		glScalef(0.8, 0.8, 0.8);

		glPushMatrix();
		glTranslatef(0.0, 0.0, 30.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z���𗧂Ăɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	// ���̂���]������

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis2(1.0, 80.0);			// ���̕`��
		mySetMaterial();	// �ގ������̓K�p


		if (wireFrag == GL_TRUE) {
			// �ȉ��A�Ȗʂ̕`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// �Ȗʂ��O�p�`�������ĕ\��
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
			// �ȉ��A�Ȗʂ��t���[���ŕ`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// �Ȗʂ̏c����\��
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
				glBegin(GL_LINE_STRIP);// �Ȗʂ̉�����\��
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
		glTranslatef(0.0, 0.0, 30.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z�����c�ɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	    // ���̂�����]������

												// �ȉ��A�Ȗʂ̒��S��ʂ���̕`��i��ԓ��ɋȐ���`�����[�`���j
		int m;
		glColor3d(0.5, 0.5, 0.0);               //���F
		glBegin(GL_LINE_STRIP);
		for (m = 0; m <= 40; m++) {
			glVertex3f(vv[20][m][0], vv[20][m][1], vv[20][m][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int p;
		glColor3d(0.0, 0.5, 0.5);              //�ΐF
		glBegin(GL_LINE_STRIP);
		for (p = 0; p <= 40; p++) {
			glVertex3f(vv[p][20][0], vv[p][20][1], vv[p][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();
		break;

	case MARK3_MARK_ID:
		// 3D�I�u�W�F�N�g�̕`��
		glClear(GL_DEPTH_BUFFER_BIT);			// Z�o�b�t�@�̏�����
		glEnable(GL_DEPTH_TEST);				// �B�ʏ����̓K�p

		mySetLight3();							// �����̐ݒ�
		glEnable(GL_LIGHTING);				// �����̓K�p

											//			GLfloat d = 1.0, s = 80.0;              //����`�悷�邽�߂̐ݒ�

		glScalef(0.8, 0.8, 0.8);
		glPushMatrix();
		glTranslatef(0.0, 0.0, 20.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z���𗧂Ăɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	// ���̂���]������

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ZERO);

		axis3(1.0, 80.0);			// ���̕`��
		mySetMaterial();	// �ގ������̓K�p


		if (wireFrag == GL_TRUE) {
			// �ȉ��A�Ȗʂ̕`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_TRIANGLE_STRIP);// �Ȗʂ��O�p�`�������ĕ\��
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
			// �ȉ��A�Ȗʂ��t���[���ŕ`��i���ʑ̂�`�����[�`���j
			int i, j;
			for (i = 0; i<40; i++) {
				glBegin(GL_LINE_STRIP);// �Ȗʂ̏c����\��
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
				glBegin(GL_LINE_STRIP);// �Ȗʂ̉�����\��
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
		glTranslatef(0.0, 0.0, 20.0);		// �}�[�J�ɍڂ��邽�߂ɕ��s�ړ�
		glRotatef(90.0, 1.0, 0.0, 0.0);	// z�����c�ɂ��邽�߂̉�]
		glRotatef(angle, 0.0, 1.0, 0.0);	    // ���̂�����]������

												// �ȉ��A�Ȗʂ̒��S��ʂ���̕`��i��ԓ��ɋȐ���`�����[�`���j
		int c;
		glColor3d(0.5, 0.5, 0.0);               //���F
		glBegin(GL_LINE_STRIP);
		for (c = 0; c <= 40; c++) {
			glVertex3f(vvv[20][c][0], vvv[20][c][1], vvv[20][c][2]);
			//    glVertex3f(v[20][k+1][0],  v[20][k+1][1],  v[20][k+1][2]);
		}
		glEnd();
		//	glPopMatrix();

		int b;
		glColor3d(0.0, 0.5, 0.5);              //�ΐF
		glBegin(GL_LINE_STRIP);
		for (b = 0; b <= 40; b++) {
			glVertex3f(vvv[b][20][0], vvv[b][20][1], vvv[b][20][2]);
			//     glVertex3f(v[20][l+1][0],  v[20][l+1][1],  v[20][l+1][2]);
		}
		glEnd();
		glPopMatrix();

		break;
	}


	// �I������
	glDisable(GL_LIGHTING);        // ���C�e�B���O�E����
	glDisable(GL_DEPTH_TEST);        // �f�v�X�e�X�g�E����
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
// �����̐ݒ���s���֐�
//===========================================================================
void mySetLight1(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// �g�U���ˌ�
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// ���ʔ��ˌ�
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// ����
														//GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// �ʒu�Ǝ��
	GLfloat light_position[] = { 200.0, 0.0, 100.0, 0.0 };	// �ʒu�Ǝ��

															// �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // �g�U���ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // ���ʔ��ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // �ʒu�Ǝ�ނ̐ݒ�

	glShadeModel(GL_SMOOTH);	// �V�F�[�f�B���O�̎�ނ̐ݒ�
	glEnable(GL_LIGHT0);		// �����̗L����
}

void mySetLight3(void)
{
	GLfloat light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };	// �g�U���ˌ�
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// ���ʔ��ˌ�
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 0.1 };	// ����
														//	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	// �ʒu�Ǝ��
	GLfloat light_position[] = { 200.0, 100.0, 100.0, 0.0 };	// �ʒu�Ǝ��

																// �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);	 // �g�U���ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // ���ʔ��ˌ��̐ݒ�
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	 // �����̐ݒ�
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // �ʒu�Ǝ�ނ̐ݒ�

	glShadeModel(GL_SMOOTH);	// �V�F�[�f�B���O�̎�ނ̐ݒ�
	glEnable(GL_LIGHT0);		// �����̗L����
}

//===========================================================================
// �ގ������̐ݒ���s���֐�
//===========================================================================
void mySetMaterial(void)
{

	GLfloat mat_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };	// �g�U����
	GLfloat mat_specular1[] = { 1.0, 1.0, 1.0, 1.0 };	// ���ʔ���
	GLfloat mat_ambient1[] = { 0.7, 0.7, 0.7, 1.0 };	// ��������
	GLfloat shininess1[] = { 50.0 };					// ���ʔ��˂̉s��

	GLfloat mat_diffuse[] = { 0.5, 0.9, 0.9, 0.90 };	// �g�U����
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.90 };	// ���ʔ���
	GLfloat mat_ambient[] = { 0.1, 0.3, 0.9, 0.90 };	// ��������
	GLfloat shininess[] = { 50.0 };					// ���ʔ��˂̉s��

	GLfloat mat_diffuse_back[] = { 0.05, 0.25, 0.40, 0.45 };	// �g�U����
	GLfloat mat_specular_back[] = { 0.50, 0.50, 0.50, 0.45 };	// ���ʔ���
	GLfloat mat_ambient_back[] = { 0.25, 0.25, 0.35, 0.45 };	// ��������
	GLfloat shininess_back[] = { 50.0 };					// ���ʔ��˂̉s��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);	// �g�U���˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);	// ���ʔ��˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);	// �������˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess1);		// ���ʔ��˂̉s���̐ݒ�

	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);	// �g�U���˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);	// ���ʔ��˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);	// �������˂̐ݒ�
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);		// ���ʔ��˂̉s���̐ݒ�

	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse_back);	// �g�U���˂̐ݒ�
	glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular_back);	// ���ʔ��˂̐ݒ�
	glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient_back);	// �������˂̐ݒ�
	glMaterialfv(GL_BACK, GL_SHININESS, shininess_back);		// ���ʔ��˂̉s���̐ݒ�

}

//===========================================================================
// �L�[�{�[�h���͏����֐�
//===========================================================================
void KeyEvent(unsigned char key, int x, int y)
{
	/*if (key == 'r') {
	angle += 3.0;	// r�L�[�������x�Ɋp�x��3.0�x����������
	}*/
	/*if (key == 'w') {
	wireFrag = !wireFrag;	// �t���O�̒l�́Aw�L�[���������т�0��1���J��Ԃ�
	}*/
	// ESC�L�[����͂�����A�v���P�[�V�����I��
	/*else */if (key == 0x1b) {
		Cleanup();
		exit(0);
	}
}



//=======================================================
// �}�E�X���͏����֐�
//=======================================================
void MouseEvent(int button, int state, int x, int y)
{
	// ���͏�Ԃ�\��
	//printf("�{�^���F%d ��ԁF%d ���W�F(x,y)=(%d,%d) \n", button, state, x, y);


	if (state == GLUT_DOWN)
	{

		/*	else */if (button == GLUT_RIGHT_BUTTON)  wireFrag = !wireFrag;
	}
	else if (button == GLUT_LEFT_BUTTON)		angle += 6.0;

}


//=======================================================
// �I�������֐�
//=======================================================
void Cleanup(void)
{
	arVideoCapStop();    // �r�f�I�L���v�`���̒�~
	arVideoClose();        // �r�f�I�f�o�C�X�̏I��
	argCleanup();        // ARToolKit�̏I������
}