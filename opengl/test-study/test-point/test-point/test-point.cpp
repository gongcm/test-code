
#include <gl/glew.h>
#include <gl/glut.h>

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
void drawCricel();
const int n = 70;
const GLfloat Pi = 3.1415926536f;
const GLfloat R = 0.5f;

static GLubyte Mask[128] =

{

	0x00, 0x00, 0x00, 0x00,    //   这是最下面的一行

	0x00, 0x00, 0x00, 0x00,

	0x03, 0x80, 0x01, 0xC0,    //   麻

	0x06, 0xC0, 0x03, 0x60,    //   烦

	0x04, 0x60, 0x06, 0x20,    //   的

	0x04, 0x30, 0x0C, 0x20,    //   初

	0x04, 0x18, 0x18, 0x20,    //   始

	0x04, 0x0C, 0x30, 0x20,    //   化

	0x04, 0x06, 0x60, 0x20,    //   ，

	0x44, 0x03, 0xC0, 0x22,    //   不

	0x44, 0x01, 0x80, 0x22,    //   建

	0x44, 0x01, 0x80, 0x22,    //   议

	0x44, 0x01, 0x80, 0x22,    //   使

	0x44, 0x01, 0x80, 0x22,    //   用

	0x44, 0x01, 0x80, 0x22,

	0x44, 0x01, 0x80, 0x22,

	0x66, 0x01, 0x80, 0x66,

	0x33, 0x01, 0x80, 0xCC,

	0x19, 0x81, 0x81, 0x98,

	0x0C, 0xC1, 0x83, 0x30,

	0x07, 0xE1, 0x87, 0xE0,

	0x03, 0x3F, 0xFC, 0xC0,

	0x03, 0x31, 0x8C, 0xC0,

	0x03, 0x3F, 0xFC, 0xC0,

	0x06, 0x64, 0x26, 0x60,

	0x0C, 0xCC, 0x33, 0x30,

	0x18, 0xCC, 0x33, 0x18,

	0x10, 0xC4, 0x23, 0x08,

	0x10, 0x63, 0xC6, 0x08,

	0x10, 0x30, 0x0C, 0x08,

	0x10, 0x18, 0x18, 0x08,

	0x10, 0x00, 0x00, 0x08    // 这是最上面的一行

};
void drawPolgon()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_POLYGON_STIPPLE);

	glPolygonStipple(Mask);

	glRectf(-0.5f, -0.5f, 0.0f, 0.0f);   // 在左下方绘制一个有镂空效果的正方形

	glDisable(GL_POLYGON_STIPPLE);

	glRectf(0.0f, 0.0f, 0.5f, 0.5f);     // 在右上方绘制一个无镂空效果的正方形

	glFlush();
}

void drawColor()
{
	int i;
	glShadeModel(GL_FLAT);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	for (i = 0; i <= 8; ++i)
	{
		glColor3f(i & 0x04, i & 0x02, i & 0x01);
		glVertex2f(cos(i*Pi / 4), sin(i*Pi / 4));
	}
	glEnd();
	glFlush();
}
void drawOther()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_FILL); // 设置正面为填充模式

	glPolygonMode(GL_BACK, GL_LINE);   // 设置反面为线形模式

	glFrontFace(GL_CCW);               // 设置逆时针方向为正面

	glBegin(GL_POLYGON);               // 按逆时针绘制一个正方形，在左下方

	glVertex2f(-0.5f, -0.5f);

	glVertex2f(0.0f, -0.5f);

	glVertex2f(0.0f, 0.0f);

	glVertex2f(-0.5f, 0.0f);

	glEnd();

	glBegin(GL_POLYGON);               // 按顺时针绘制一个正方形，在右上方

	glVertex2f(0.0f, 0.0f);

	glVertex2f(0.0f, 0.5f);

	glVertex2f(0.5f, 0.5f);

	glVertex2f(0.5f, 0.0f);

	glEnd();
}

// 太阳、地球和月亮
// 假设每个月都是30天
// 一年12个月，共是360天
static int day = 200; // day的变化：从0到359
void myDisplay(void)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, 1, 1, 400000000);
	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	gluLookAt(0, -200000000, 200000000, 0, 0, 0, 0, 0, 1);

	// 绘制红色的“太阳”
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidSphere(69600000, 20, 20);
	// 绘制蓝色的“地球”
	glColor3f(0.0f, 0.0f, 1.0f);
	glRotatef(day / 360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(150000000, 0.0f, 0.0f);
	glutSolidSphere(15945000, 20, 20);
	// 绘制黄色的“月亮”
	glColor3f(1.0f, 1.0f, 0.0f);
	glRotatef(day / 30.0*360.0 - day / 360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(38000000, 0.0f, 0.0f);
	glutSolidSphere(4345000, 20, 20);

	glFlush();
}
void drawStripLine()
{

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0x333);

	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f,0.0f);
	glVertex2f(0.0f,0.5f);
	glEnd();

}

void drawTriangle()
{

	//shader  
//	GLSLShader shader;

	//顶点数组对象和顶点缓冲区对象
	GLuint vaoID;
	GLuint vboVerticesID;
	GLuint vboIndicesID;
#if 1
	// 设置视口参数
	glViewport(0, 0, 400,400);


	/////////////////// Create the VBO ////////////////////
	float positionData[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f };

	float colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };



	// Create and set-up the vertex array object
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);


	// Create and populate the buffer objects
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);  // Vertex position
	glEnableVertexAttribArray(1);  // Vertex color

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	// Section2 准备着色器程序
	// Step1: 准备着色器源程序
	const GLchar* vertexShaderSource = "#version 330\n"
		"layout(location = 0) in vec3 position;\n"
		"void main()\n"
		"{\n gl_Position = vec4(position, 1.0);\n}";
	const GLchar* fragShaderSource = "#version 330\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n color = vec4(0.8, 0.8, 0.0, 1.0);\n}";
	// Step2 创建Shader object
	// 顶点着色器
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderId);
	GLint compileStatus = 0;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileStatus); // 检查编译状态
	if (compileStatus == GL_FALSE) // 获取错误报告
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetShaderInfoLog(vertexShaderId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader vertex shader compile failed," << &errLog[0] << std::endl;
	}
	// 片元着色器
	GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderId, 1, &fragShaderSource, NULL);
	glCompileShader(fragShaderId);
	glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragShaderId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetShaderInfoLog(fragShaderId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader fragment shader compile failed," << &errLog[0] << std::endl;
	}
	// Step3 链接形成 shader program object
	GLuint shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId, vertexShaderId);
	glAttachShader(shaderProgramId, fragShaderId);
	glLinkProgram(shaderProgramId);
	GLint linkStatus;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetProgramInfoLog(shaderProgramId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader link failed," << &errLog[0] << std::endl;
	}
	// 链接完成后detach
	glDetachShader(shaderProgramId, vertexShaderId);
	glDetachShader(shaderProgramId, fragShaderId);
	// 不需要连接到其他程序时 释放空间
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragShaderId);

	// 清除颜色缓冲区 重置为指定颜色
	glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 这里填写场景绘制代码
	glBindVertexArray(vaoID);
	glUseProgram(shaderProgramId);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);
#endif
}
void display(void)
{	

#if 0
	glPointSize(2.0f);
	glLineWidth(2.0f);
	glLineStipple(33,2);
	glBegin(GL_LINES); // GL_LINES 点的个数必须是2 的倍数
	glVertex2f(0.0f,0.0f);
	glVertex2f(0.0f, 0.3f);
	glVertex2f(-0.3f,0.0f);
	glVertex2f(0.0f, 0.3f);
	glVertex2f(-0.3f, 0.0f);
	glVertex2f(0.0f, 0.0f);
//	glVertex2f(-1.0f,1.0f);
	glEnd();
#endif
//	drawCricel();
	//glutFullScreen();
	//drawStripLine();
	//drawOther();
	//drawPolgon();
//	drawColor();
	drawTriangle();

	glutFullScreen();
	glFlush();
}
void drawCricel()
{
	int i;
	glBegin(GL_POLYGON);
	for (i = 0; i<n; ++i)
		glVertex2f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i));
	glEnd();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);

	glutInitWindowPosition(100,100);

	glutInitWindowSize(800,600);
	
	glutCreateWindow("opengl point");

	const GLubyte * name = glGetString(GL_VENDOR);
	const GLubyte * biaoshifu = glGetString(GL_RENDERER);
	const GLubyte * OpenGLVersion = glGetString(GL_VERSION);
	const GLubyte * gluVersion = gluGetString(GLU_VERSION);
	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OOpenGL实现的版本号：%s\n", OpenGLVersion);
	printf("OGLU工具库版本：%s\n", gluVersion);
	glutDisplayFunc(&myDisplay);

	glutMainLoop();

	return 0;
}