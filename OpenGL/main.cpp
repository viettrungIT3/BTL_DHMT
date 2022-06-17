/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
//#include "unistd.h" // time 

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[10] = { 0, 0, 0 };
GLfloat dr = 5;

mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
mat4 instance;
/* Khởi tạo các tham số chiếu sáng - tô bóng*/
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;
color4 diffuse_product = light_diffuse * material_diffuse;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

void toMau(string mau) {
		//material_diffuse = vec4(0.0, 1.0, 0.9, 0.5);  // mau xanh
	if( mau == "black")
		material_diffuse = vec4(0.0, 0.0, 0.0, 1.0); // black
	else if( mau == "red")
		material_diffuse = vec4(1.0, 0.0, 0.0, 1.0); // red
	else if( mau == "yellow")
		material_diffuse = vec4(1.0, 1.0, 0.0, 1.0); // yellow
	else if( mau == "green")
		material_diffuse = vec4(0.0, 1.0, 0.0, 1.0); // green
	else if( mau == "blue")
		material_diffuse = vec4(0.0, 0.0, 1.0, 1.0); // blue
	else if( mau == "magenta")
		material_diffuse = vec4(1.0, 0.0, 1.0, 1.0); // magenta
	else if( mau == "orange")
		material_diffuse = vec4(1.0, 0.5, 0.0, 1.0); // orange
	else if( mau == "cyan")
		material_diffuse = vec4(0.0, 1.0, 1.0, 1.0); // cyan
	else 
		material_diffuse = vec4(0.0, 1.0, 0.9, 0.5);  // mau xanh

	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}

mat4 instance_trans;
GLfloat DEPTH_house = 0.02, HEIGH_house = 0.4, LONG_house = 1.0;
mat4 instance_house;

// Nhà
void houseFrame() {
	instance_house = identity();
	// mặt 1
	instance_house = Translate( DEPTH_house / 2.0, HEIGH_house/2.0, LONG_house/2.0) * Scale(DEPTH_house, HEIGH_house, LONG_house);
	toMau("magenta");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_house);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt 2
	instance_house = Translate(LONG_house/2.0, HEIGH_house/2.0, DEPTH_house/2.0) * Scale(LONG_house, HEIGH_house, DEPTH_house);
	toMau("magenta");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_house);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// nền
	instance_house = Translate(LONG_house/2.0, DEPTH_house/2.0, LONG_house/2.0) * Scale(LONG_house, DEPTH_house, LONG_house);
	toMau("err");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_house);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// Bàn bếp
GLfloat WIDTH_stove = 0.18, HEIGH_stove = 0.2, LONG_stove = 0.4, DEPTH_stove = 0.01;
GLfloat WIDTH_sink = 0.18, HEIGH_sink = 0.04, LONG_sink = 0.2, DEPTH_sink = 0.03;
GLfloat WIDTH_eStove = 0.15, HEIGH_eStove = 0.03, LONG_eStove = 0.2, r_eStove;
mat4 instance_stove;
mat4 instance_fire;

// ngọn lửa
void fire()
{
	for (float i = 0; i < 360; i++)
	{
		instance_fire = RotateY(i) * Scale(0.01, 0.01, 0.05);
		//material_diffuse = vec4(1, 0.4, 0.23, 0.3);  // mau vat
		//diffuse_product = light_diffuse * material_diffuse;
		//glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
		toMau("red");
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove * instance_fire);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
// Bếp 
void electricStove() {
	// mặt bếp
	instance_stove = Translate(LONG_eStove / 2.0 + 0.18, HEIGH_stove/2 + HEIGH_eStove / 2.0, WIDTH_eStove / 2.0) * Scale(LONG_eStove, HEIGH_eStove, WIDTH_eStove);
	toMau("err");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// ngọn lửa 1
	instance_stove = Translate(LONG_eStove / 2.0 + 0.18 - LONG_eStove /4, HEIGH_stove / 2 + HEIGH_eStove, WIDTH_eStove / 2.0);
	fire();

	// ngọn lửa 2
	instance_stove = Translate(LONG_eStove / 2.0 + 0.18 + LONG_eStove /4, HEIGH_stove / 2 + HEIGH_eStove, WIDTH_eStove / 2.0);
	fire();
}

// bồn rửa
void sink() {
	// mặt 1
	instance_stove = Translate(DEPTH_house + DEPTH_sink/2, HEIGH_stove/2 - HEIGH_sink/2, LONG_stove * 1.25) * Scale(DEPTH_sink, HEIGH_sink, LONG_sink);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt 3
	instance_stove = Translate(DEPTH_house - DEPTH_sink*1.5 + WIDTH_sink, HEIGH_stove / 2 - HEIGH_sink / 2, LONG_stove * 1.25) * Scale(DEPTH_sink, HEIGH_sink, LONG_sink);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt 2
	instance_stove = Translate(DEPTH_house + WIDTH_sink/2 - DEPTH_sink/2, HEIGH_stove / 2 - HEIGH_sink / 2, LONG_stove * 1.25 + LONG_sink/2) * Scale(WIDTH_sink - DEPTH_sink, HEIGH_sink, DEPTH_sink);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	
	// mặt 4
	instance_stove = Translate(DEPTH_house + WIDTH_sink / 2 - DEPTH_sink/2, HEIGH_stove / 2 - HEIGH_sink / 2, LONG_stove * 1.25 - LONG_sink / 2) * Scale(WIDTH_sink - DEPTH_sink, HEIGH_sink, DEPTH_sink);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt đáy
	instance_stove = Translate(DEPTH_house + WIDTH_sink / 2 - DEPTH_sink, HEIGH_stove / 2 - HEIGH_sink / 2 - DEPTH_sink, LONG_stove * 1.25) * Scale(WIDTH_sink - DEPTH_sink * 2, DEPTH_sink/2, LONG_sink);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// bàn bếp
void stoveTableFrame() {
	instance_stove = identity();
	// mặt bàn 1
	instance_stove = Translate(WIDTH_stove / 2.0, HEIGH_stove / 2.0, LONG_stove / 2.0) * Scale(WIDTH_stove, DEPTH_stove, LONG_stove);
	toMau("blue");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt bàn 2
	instance_stove = Translate(LONG_stove / 2.0, HEIGH_stove / 2.0, WIDTH_stove / 2.0) * Scale(LONG_stove, DEPTH_stove, WIDTH_stove);
	toMau("blue");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_stove);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	electricStove();	// bếp điện
	sink();	// bồn rửa
}

// Tủ lạnh
GLfloat cao_tuLanh = 0.2, rong_tuLanh = 0.12, dai_tuLanh = 0.13, day_tuLanh = 0.005;
mat4 instance_tuLanh, m;

void canhTuTren()
{
	//cánh tủ trên
	instance_tuLanh = Translate( -day_tuLanh/2, day_tuLanh/2, 0) * Scale(dai_tuLanh + day_tuLanh, cao_tuLanh/3 + day_tuLanh, day_tuLanh);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//tay nắm của
	instance_tuLanh = Translate(-dai_tuLanh/2 + 0.01, 0, day_tuLanh) * Scale(day_tuLanh, cao_tuLanh/8, day_tuLanh);
	toMau("yellow");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void trucTren()
{
	instance_tuLanh = Translate(0, 0, 0) * Scale(day_tuLanh, cao_tuLanh / 3, day_tuLanh);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canhTuDuoi()
{
	//cánh tủ dưới
	instance_tuLanh = Translate(-day_tuLanh / 2, day_tuLanh / 2, 0) * Scale(dai_tuLanh + day_tuLanh, cao_tuLanh * 2 / 3 + day_tuLanh, day_tuLanh);
	toMau("blue");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//tay nắm cửa
	instance_tuLanh = Translate(-dai_tuLanh / 2 + 0.01, 0, day_tuLanh) * Scale(day_tuLanh, cao_tuLanh / 6, day_tuLanh);
	toMau("yellow");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void trucDuoi()
{
	instance_tuLanh = Translate(0, 0, 0) * Scale(day_tuLanh, cao_tuLanh / 3, day_tuLanh);
	toMau("blue");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void voTuLanh()
{
	instance_tuLanh = identity();
	// mặt trái
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + day_tuLanh / 2.0, cao_tuLanh / 2.0, rong_tuLanh / 2.0) * Scale(day_tuLanh, cao_tuLanh, rong_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt phải
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + day_tuLanh / 2.0 + dai_tuLanh, cao_tuLanh / 2.0, rong_tuLanh / 2.0) * Scale(day_tuLanh, cao_tuLanh, rong_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt sau
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh / 2.0, cao_tuLanh / 2.0, day_tuLanh / 2.0) * Scale(dai_tuLanh, cao_tuLanh, day_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt dưới
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh / 2.0, day_tuLanh / 2.0, rong_tuLanh / 2) * Scale(dai_tuLanh + day_tuLanh, day_tuLanh, rong_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//mặt giữa
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh / 2.0, day_tuLanh / 2.0 + 2 * cao_tuLanh / 3.0, rong_tuLanh / 2) * Scale(dai_tuLanh + day_tuLanh, day_tuLanh, rong_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt trên
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh / 2.0 + day_tuLanh / 2, day_tuLanh / 2.0 + cao_tuLanh, rong_tuLanh / 2) * Scale(dai_tuLanh + day_tuLanh, day_tuLanh, rong_tuLanh);
	toMau("green");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat dtt, dtp;
void tuLanh()
{
	instance_tuLanh = identity();
	// đế
	instance_tuLanh = Translate(DEPTH_house, DEPTH_house, DEPTH_house) * Translate(LONG_stove + dai_tuLanh / 2.0 + day_tuLanh / 2, day_tuLanh * 2, rong_tuLanh / 2) * Scale(dai_tuLanh + day_tuLanh, day_tuLanh * 4, rong_tuLanh);
	toMau("err");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuLanh);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	voTuLanh();	// vỏ tủ lạnh

	// Cánh tủ trên của tủ lạnh
	m = identity();
	m = m * Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh + day_tuLanh/2, cao_tuLanh - cao_tuLanh/6, day_tuLanh / 2.0 + rong_tuLanh) * RotateY(theta[0]);
	trucTren();
	m = m * Translate(-dai_tuLanh/2, 0, 0);
	canhTuTren();

	// Cánh tủ dưới của tủ lạnh
	m = identity();
	m = m * Translate(DEPTH_house, DEPTH_house + day_tuLanh * 4, DEPTH_house) * Translate(LONG_stove + dai_tuLanh + day_tuLanh / 2,  cao_tuLanh * 2 / 6, day_tuLanh / 2.0 + rong_tuLanh) * RotateY(theta[1]);
	trucDuoi();
	m = m * Translate(-dai_tuLanh / 2, 0, 0);
	canhTuDuoi();
}

// Tủ bếp
GLfloat cao_tuBep = 0.12, rong_tuBep = 0.1, dai_tuBep = 0.15, day_tuBep = 0.005;
mat4 instance_tuBep, instance_dichTuBep;
void khungTuBep() {
	instance_tuBep = identity();

	// mặt phải
	instance_tuBep = instance_dichTuBep * Translate(rong_tuBep / 2, cao_tuBep / 2, day_tuBep / 2) * Scale(rong_tuBep, cao_tuBep, day_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);;

	// mặt trái
	instance_tuBep = instance_dichTuBep * Translate(rong_tuBep / 2, cao_tuBep / 2, day_tuBep / 2 + dai_tuBep) * Scale(rong_tuBep, cao_tuBep, day_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt dưới
	instance_tuBep = instance_dichTuBep * Translate(rong_tuBep / 2, day_tuBep / 2, dai_tuBep / 2) * Scale(rong_tuBep, day_tuBep, dai_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt giữa
	instance_tuBep = instance_dichTuBep * Translate(rong_tuBep / 2, day_tuBep / 2 + cao_tuBep / 2, dai_tuBep / 2) * Scale(rong_tuBep, day_tuBep, dai_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	// mặt trên
	instance_tuBep = instance_dichTuBep * Translate(rong_tuBep / 2, day_tuBep / 2 + cao_tuBep - day_tuBep, dai_tuBep / 2) * Scale(rong_tuBep, day_tuBep, dai_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	
	// mặt sau
	instance_tuBep = instance_dichTuBep * Translate(day_tuBep / 2, cao_tuBep / 2, dai_tuBep / 2) * Scale(day_tuBep, cao_tuBep, dai_tuBep);
	toMau("orange");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canhTuBep()
{
	//cánh tủ 
	instance_tuBep = Scale(day_tuBep, cao_tuBep, dai_tuBep/2);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void trucTuBep()
{
	instance_tuBep = Translate(0, 0, 0) * Scale(day_tuBep, cao_tuBep, day_tuBep);
	toMau("red");
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * m * instance_tuBep);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuBep(GLfloat xoay1, GLfloat xoay2) {
	instance_dichTuBep *= Translate(DEPTH_house, DEPTH_house + 0.18, DEPTH_house);
	khungTuBep();

	// cánh tủ phải
	m = identity();
	//instance_dichTuBep = identity();
	instance_dichTuBep *= Translate( rong_tuBep, cao_tuBep/2, 0);
	m = m * instance_dichTuBep * RotateY(xoay1);
	trucTuBep();
	m = m * Translate(0, 0, dai_tuBep / 4);
	canhTuBep();

	// cánh tủ trái
	m = identity();
	//instance_dichTuBep = Translate(DEPTH_house, DEPTH_house + 0.18, DEPTH_house);
	//instance_dichTuBep = identity();
	instance_dichTuBep *= Translate(0, 0, dai_tuBep);
	m = m * instance_dichTuBep * RotateY(-xoay2);
	trucTuBep();
	m = m * Translate(0, 0, -dai_tuBep / 4);
	canhTuBep();
}

void boTuBep() {
	instance_dichTuBep = identity();
	GLfloat z_dich = 0;
	instance_dichTuBep = Translate(0, 0, z_dich);
	tuBep(0, theta[2]);	// Tủ 2

	z_dich = dai_tuBep + day_tuBep;
	instance_dichTuBep = Translate(0, 0, z_dich);
	tuBep(theta[3], theta[3]);	// Tủ 3

	z_dich = dai_tuBep * 2 + day_tuBep;
	instance_dichTuBep = Translate(0, 0, z_dich);
	tuBep(theta[4], theta[4]);	// Tủ 4

	z_dich = dai_tuBep * 3 + day_tuBep;
	instance_dichTuBep = Translate(0, 0, z_dich);
	tuBep(theta[5], theta[5]);	// Tủ 5

	z_dich = 0;
	instance_dichTuBep = Translate(dai_tuBep + WIDTH_stove + 2*DEPTH_house, 0, z_dich) * RotateY(-90);
	tuBep(theta[6], theta[6]);	// Tủ 1
}


GLfloat l = -0.5, r = 0.5;
GLfloat bottom = -0.5, top = 0.5;
GLfloat zNear = 2, zFar = 10.0;
GLfloat XeyeTemp = LONG_house * 2, HEIGH_Temp = HEIGH_house * 3;
GLfloat Xeye = XeyeTemp, Yeye = HEIGH_Temp, Zeye = XeyeTemp;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec4 eye(Xeye, Yeye, Zeye, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	houseFrame();	// nhà
	stoveTableFrame();	// bàn bếp
	tuLanh();	// tủ lạnh
	boTuBep();	// tủ bếp

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'x':
		model *= RotateX(dr);
		glutPostRedisplay();
		break;
	case 'X':
		model *= RotateX(-dr);
		glutPostRedisplay();
		break;
	case 'y':
		model *= RotateY(dr);
		glutPostRedisplay();
		break;
	case 'Y':
		model *= RotateY(-dr);
		glutPostRedisplay();
		break;
	case 'z':
		model *= RotateZ(dr);
		glutPostRedisplay();
		break;
	case 'Z':
		model *= RotateZ(-dr);
		glutPostRedisplay();
		break;
	case 'e':
		if (Xeye > 0.25)
		{
			Xeye -= 0.1;
			Zeye = sqrt(2 * XeyeTemp * XeyeTemp - Xeye * Xeye);
		}

		glutPostRedisplay();
		break;
	case 'E':
		if (Xeye < XeyeTemp * sqrt(2) - 0.1)
		{
			Xeye += 0.1;
			Zeye = sqrt(2 * XeyeTemp * XeyeTemp - Xeye * Xeye);
		}
		glutPostRedisplay();
		break;
	case 'r':
		Xeye = XeyeTemp;
		Yeye = HEIGH_Temp;
		Zeye = XeyeTemp;
		glutPostRedisplay();
		break;
	case 'R':
		if (Xeye >= 0.4 && Yeye >= 0.4 && Zeye >= 0.4)
		{
			Xeye -= XeyeTemp / 10.0;
			Yeye -= HEIGH_Temp / 10.0;
			Zeye -= XeyeTemp / 10.0;
		}
		glutPostRedisplay();
		break;
	case 't':
		if (Yeye >= 0.4)
		{
			Yeye -= HEIGH_Temp / 10.0;
		}
		glutPostRedisplay();
		break;
	case 'T':
		if (Yeye <= 3)
		{
			Yeye += HEIGH_Temp / 10.0;
		}
		glutPostRedisplay();
		break;
	case 'a':
		if (theta[0] >= 0 && theta[0] < 100)
		{
			theta[0] += 10;
			cout << "\nDang mo canh tren tu lanh voi goc: " << theta[0];
		}
		glutPostRedisplay();
		break;

	case 'A':
		if (theta[0] > 0 && theta[0] <= 100)
		{
			theta[0] -= 10;
			cout << "\nDang dong canh tren tu lanh voi goc: " << theta[0];
		}
		glutPostRedisplay();
		break;
	case 's':
		if (theta[1] >= 0 && theta[1] < 100)
		{
			theta[1] += 10;
			cout << "\nDang mo canh duoi tu lanh voi goc: " << theta[1];
		}
		glutPostRedisplay();
		break;

	case 'S':
		if (theta[1] > 0 && theta[1] <= 100)
		{
			theta[1] -= 10;
			cout << "\nDang dong canh duoi tu lanh voi goc: " << theta[1];
		}
		glutPostRedisplay();
		break;
	case 'd':
		if (theta[6] >= 0 && theta[6] < 90)
		{
			theta[6] += 10;
			cout << "\nDang mo canh tu bep 1 voi goc: " << theta[6];
			glutPostRedisplay();
		}
		break;

	case 'D':
		if (theta[6] > 0 && theta[6] <= 90)
		{
			theta[6] -= 10;
			cout << "\nDang dong canh tu bep 1 voi goc: " << theta[6];
			glutPostRedisplay();
		}
		break;
	case 'f':
		if (theta[2] >= 0 && theta[2] < 90)
		{
			theta[2] += 10;
			cout << "\nDang mo canh tu bep 2 voi goc: " << theta[2];
			glutPostRedisplay();
		}
		break;

	case 'F':
		if (theta[2] > 0 && theta[2] <= 90)
		{
			theta[2] -= 10;
			cout << "\nDang dong canh tu bep 2 voi goc: " << theta[2];
			glutPostRedisplay();
		}
		break;
	case 'g':
		if (theta[3] >= 0 && theta[3] < 90)
		{
			theta[3] += 10;
			cout << "\nDang mo canh tu bep 3 voi goc: " << theta[3];
			glutPostRedisplay();
		}
		break;

	case 'G':
		if (theta[3] > 0 && theta[3] <= 90)
		{
			theta[3] -= 10;
			cout << "\nDang dong canh tu bep 3 voi goc: " << theta[3];
			glutPostRedisplay();
		}
		break;
	case 'h':
		if (theta[4] >= 0 && theta[4] < 90)
		{
			theta[4] += 10;
			cout << "\nDang mo canh tu bep 4 voi goc: " << theta[4];
			glutPostRedisplay();
		}
		break;

	case 'H':
		if (theta[4] > 0 && theta[4] <= 90)
		{
			theta[4] -= 10;
			cout << "\nDang dong canh tu bep 4 voi goc: " << theta[4];
			glutPostRedisplay();
		}
		break;
	case 'j':
		if (theta[5] >= 0 && theta[5] < 90)
		{
			theta[5] += 10;
			cout << "\nDang mo canh tu bep 5 voi goc: " << theta[5];
			glutPostRedisplay();
		}
		break;

	case 'J':
		if (theta[5] > 0 && theta[5] <= 90)
		{
			theta[5] -= 10;
			cout << "\nDang dong canh tu bep 5 voi goc: " << theta[5];
			glutPostRedisplay();
		}
		break;
	}
}

/* Keywork
	e, E: thay đổi góc nhìn sang trái/phải
	r: đặt lại góc nhìn mặc định
	R: zoom góc quay
	t, T: góc nhìn xuống/lên theo chiều y
	a, A: Mở/đóng cánh trên tủ lạnh
	a, S: Mở/đóng cánh dưới tủ lanh 
	d, D: Mở/đóng cảnh tủ bếp 1
	f, F: Mở/đóng cảnh tủ bếp 2
	g, G: Mở/đóng cảnh tủ bếp 3
	h, H: Mở/đóng cảnh tủ bếp 4
	j, J: Mở/đóng cảnh tủ bếp 5
*/


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
