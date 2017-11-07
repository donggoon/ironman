/*
Authored by Prof. Jeong-Mo Hong, CSE Dongguk University
for Introduction to Computer Graphics, 2017 Spring
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <GL/freeglut.h>
#include <cstring>
#include <stdlib.h>		  // srand, rand
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "math.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include "Geometry/TriangularSurface/StaticTriangularSurface.h"
#include "GL2/GL2_Object.h"
#include "GL2/GL2_World.h"
#include "GL2/GL2_Light.h"
#include "GL2/GL2_Material.h"

int width_window = 640;
int height_window = 480;

bool game_over = false;

GL2_World gl_world;

// (-2.285, 1.19)		(2.285, 1.19)
// (-2.285, -1.19)		(2.285, -1.19)
bool direction[4] = { false, false, false, false }; // left, right, up, down
bool z_direction[2] = { false, false }; // near, far

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	gl_world.camera_.UpdateDolly(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		// direction[1] = direction[2] = direction[3] = false;
		direction[0] = true; // moving left
	}
	else if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
		direction[0] = true; // moving left
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		direction[0] = false; // stop moving left 
	}


	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		// direction[0] = direction[2] = direction[3] = false;
		direction[1] = true; // moving right
	}
	else if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
		direction[1] = true; // moving right
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		direction[1] = false; // stop moving right
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		// direction[0] = direction[1] = direction[3] = false;
		direction[2] = true; // moving up
	}
	else if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		direction[2] = true; // moving up
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		direction[2] = false; // stop moving up 
	}


	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		// direction[0] = direction[1] = direction[2] = false;
		direction[3] = true; // moving down
	}
	else if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		direction[3] = true; // moving down
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		direction[3] = false; // stop moving down
	}

	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		z_direction[1] = true;
	}
	else if (key == GLFW_KEY_Q && action == GLFW_REPEAT) {
		z_direction[1] = true;
	}
	else if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
		z_direction[1] = false;
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		z_direction[0] = true;
	}
	else if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
		z_direction[0] = true;
	}
	else if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
		z_direction[0] = false;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{ 
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.StartMouseRotation(xpos, ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		gl_world.camera_.EndMouseRotation(xpos, ypos);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.StartMousePan(xpos, ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_REPEAT) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		std::cout << "mouse left button repeat : " << xpos << ", " << ypos << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		gl_world.camera_.EndMousePan(xpos, ypos);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	gl_world.camera_.ProcessMouseMotion(xpos, ypos);	
}

int main(void)
{
	GLFWwindow *window = nullptr;

	const int number_of_star = 30;

	StaticTriangularSurface ironman, ver_enemy, bomb, hor_enemy, barrel, ufo, rocket,
		turkey, tank, star[number_of_star];

	TV trans_ironman, trans_ver_enemy, trans_bomb, trans_hor_enemy, trans_barrel, trans_ufo,
		trans_rocket, trans_turkey, trans_tank, trans_star[number_of_star];

	float origin_x = 0.0, origin_y = 0.0, origin_z = 0.0,
		border_x = 2.285, border_y = 1.19,
		ver_enemy_x = 0.0, ver_enemy_y = -2.0, ver_enemy_z = 0.0,
		bomb_x = 0.0, bomb_y = 0.0, bomb_z = 0.0,
		hor_enemy_x = -3.0, hor_enemy_y = 0.0, hor_enemy_z = 0.0,
		barrel_x = 0.0, barrel_y = 0.0, barrel_z = 0.0,
		ufo_x = 0.0, ufo_y = 0.0, ufo_z = -10.0,
		turkey_x = -3.0, turkey_y = 0.0, turkey_z = 0.0,
		tank_x = 0.0, tank_y = 0.0, tank_z = 0.0,
		rocket_x = 0.0, rocket_y = 0.0, rocket_z = 0.0;

	float ver_col_range = 0.25, hor_col_range = 0.25, ufo_col_range = 0.2, tank_col_range = 0.4,
		turkey_col_range = 0.2, rocket_col_range = 0.2;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_SAMPLES, 32);

	// window resolution
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width_window = mode->width * 0.8f;
	height_window = mode->height * 0.8f;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width_window, height_window, "Ironman does not die!", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// callbacks here
	glfwSetKeyCallback(window, key_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetCursorPosCallback(window, cursor_position_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glClearColor(0.15, 0.15, 0.15, 0); // while background

	printf("%s\n", glGetString(GL_VERSION));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	const float aspect = (float)width / (float)height;
	glViewport(0, 0, width, height);
	glOrtho(-1.2*aspect, 1.2*aspect, -1.2, 1.2, -100.0, 100.0);
	gluLookAt(0.1, 0.1, 0.1, 0.0, 0.0, 0.0, 0, 1, 0);//camera takes care of it

	gl_world.camera_.UpdateDolly(-10);

	const float zNear = 0.001, zFar = 100.0, fov = 45.0;			// UI
	gl_world.camera_.Resize(width, height, fov, zNear, zFar);
	gl_world.initShaders();

//	glEnable(GL_MULTISAMPLE);
	// speed setting
	float ironman_speed = 0.01, ver_enemy_speed = 0.012, hor_enemy_speed = 0.012,
		ufo_speed = 0.01, tank_speed = 0.005, turkey_speed = 0.008, rocket_speed = 0.009;

	// player ironman plane setting
	origin_x = 0.0;
	origin_y = -2.0;
	trans_ironman.assign(origin_x, origin_y, 0.0);
	ironman.readObj("ironmanout.obj", true, true);
	ironman.scale(0.3 * 2.0f * 0.99);
	ironman.translate(trans_ironman);

	// star setting
	srand((unsigned)time(NULL));
	for (int i = 0; i < number_of_star; i++) {
		float rand_star_x = (float)(rand() % 2205) / 1000 + 0.004;
		float rand_star_y = (float)(rand() % 1190) / 1000 + 0.004;
		float rand_star_scale = (float)(rand() % 5) / 100;

		float star_x_decision = rand() % 2;
		if (star_x_decision) {
			rand_star_x = -rand_star_x;
		}
		float star_y_decision = rand() % 2;
		if (star_y_decision) {
			rand_star_y = -rand_star_y;
		}

		trans_star[i].assign(rand_star_x, rand_star_y, 0.0);
		star[i].readObj("star.obj", true, true);
		star[i].scale(rand_star_scale * 2.0f * 0.99);
		star[i].translate(trans_star[i]);
		star[i].advanceOneSubdivisionStep(0);
		star[i].advanceOneSubdivisionStep(0);
		star[i].advanceOneSubdivisionStep(0);
	}

	// vertical enemy setting
	srand((unsigned)time(NULL));
	float rand_xpos = (float) (rand() % 2205) / 1000 + 0.004;
	//std::cout << rand_xpos << std::endl;
	int ver_decision = rand() % 2;
	//std::cout << ver_decision << std::endl;
	if (ver_decision) {
		rand_xpos = -rand_xpos;
	}

	ver_enemy_x = rand_xpos;
	ver_enemy_y = 2.0;

	trans_ver_enemy.assign(rand_xpos, 2.0, 0.0);
	ver_enemy.readObj("vertical_enemy.obj", true, true);
	ver_enemy.scale(0.4 * 2.0f * 0.99);
	ver_enemy.translate(trans_ver_enemy);

	// rocket setting
	srand((unsigned)time(NULL));
	float rocket_xpos = (float)(rand() % 2205) / 1000 + 0.004;
	//std::cout << rand_xpos << std::endl;
	int rocket_ver_decision = rand() % 2;
	//std::cout << ver_decision << std::endl;
	
	if (rocket_ver_decision) {
		rocket_xpos = -rocket_xpos;
	}

	rocket_x = rocket_xpos;
	rocket_y = -5.0;

	trans_rocket.assign(rocket_x, rocket_y, 0.0);
	rocket.readObj("F22.obj", true, true);
	rocket.scale(0.2 * 2.0f * 0.99);
	rocket.translate(trans_rocket);

	// horizontal enemy setting
	srand((unsigned)time(NULL));
	float rand_ypos = (float)(rand() % 1190) / 1000 + 0.004;
	int hor_decision = rand() % 2;
	if (hor_decision) {
		rand_ypos = -rand_ypos;
	}

	hor_enemy_x = 3.0;
	hor_enemy_y = rand_ypos;

	trans_hor_enemy.assign(3.0, rand_ypos, 0.0);
	hor_enemy.readObj("horizontal_enemy.obj", true, true);
	hor_enemy.scale(0.4 * 2.0f * 0.99);
	hor_enemy.translate(trans_hor_enemy);

	// ufo setting
	srand((unsigned)time(NULL));
	float rand_ufo_x = (float)(rand() % 2205) / 1000 + 0.004;
	float rand_ufo_y = (float)(rand() % 1190) / 1000 + 0.004;

	float ufo_x_decision = rand() % 2;
	if (ufo_x_decision) {
		rand_ufo_x = -rand_ufo_x;
	}
	float ufo_y_decision = rand() % 2;
	if (ufo_y_decision) {
		rand_ufo_y = -rand_ufo_y;
	}
	ufo_x = rand_ufo_x;
	ufo_y = rand_ufo_y;
	ufo_z = -2.0;

	trans_ufo.assign(ufo_x, ufo_y, ufo_z);
	ufo.readObj("ufo.obj", true, true);
	ufo.scale(0.1 * 2.0f * 0.99);
	ufo.translate(trans_ufo);

	// turkey setting
	srand((unsigned)time(NULL));
	float turkey_ypos = (float)(rand() % 1190) / 1000 + 0.004;
	float turkey_decision = rand() % 2;
	if (turkey_decision) {
		turkey_ypos = -turkey_ypos;
	}

	turkey_x = -3.0;
	turkey_y = turkey_ypos;

	trans_turkey.assign(turkey_x, turkey_y, 0.0);
	turkey.readObj("turkey.obj", true, true);
	turkey.scale(0.3 * 2.0f * 0.99);
	turkey.translate(trans_turkey);

	// tank setting
	tank_x = 3.0;
	tank_y = -1.19;

	trans_tank.assign(tank_x, tank_y, 0.0);
	tank.readObj("tank.obj", true, true);
	tank.scale(0.4 * 2.0f * 0.99);
	tank.translate(trans_tank);

	GL2_Object gl_ironman, gl_ver_enemy, gl_bomb, gl_hor_enemy, gl_barrel,
		gl_rocket, gl_ufo, gl_tank, gl_turkey, gl_star[number_of_star];
	gl_ironman.initPhongSurface(ironman);
	gl_ver_enemy.initPhongSurface(ver_enemy);
	gl_hor_enemy.initPhongSurface(hor_enemy);
	gl_rocket.initPhongSurface(rocket);
	gl_ufo.initPhongSurface(ufo);
	gl_tank.initPhongSurface(tank);
	gl_turkey.initPhongSurface(turkey);
	for (int i = 0; i < number_of_star; i++) {
		gl_star[i].initPhongSurface(star[i]);
	}

	gl_ironman.mat_.setRed();
	gl_ver_enemy.mat_.setBlue();
	gl_hor_enemy.mat_.setGreen();
	gl_ufo.mat_.setSky();
	gl_tank.mat_.setDarkgreen();
	gl_turkey.mat_.setWhite();
	gl_rocket.mat_.setPurple();
	for (int i = 0; i < number_of_star; i++) {
		gl_star[i].mat_.setYellow();
	}

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);

	glLoadIdentity();

	GL2_Light light;

	bool ver_enem_col = false, hor_enem_col = false, ver_hor_col = false, barrel_exist = false,
		tank_col = false, ufo_col = false, turkey_col = false, rocket_col = false; // set flag

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/*ver_enem_col = false;
		hor_enem_col = false;
		ver_hor_col = false;
		tank_col = false;
		ufo_col = false;*/

		int give_barrel = rand() % 1000;
		std::cout << give_barrel << std::endl;
		if (give_barrel < 3) { // 1000분의 3 확률로 연료 제공
			barrel_exist = true;
			barrel_x = (float)(rand() % 2205) / 1000 + 0.004;
			barrel_y = (float)(rand() % 1190) / 1000 + 0.004;

			if (ver_decision) {
				barrel_y = -barrel_y;
			}
			if (hor_decision) {
				barrel_x = -barrel_x;
			}

			std::cout << "barrel x : " << barrel_x << " barrel y : " << barrel_y << std::endl;
			trans_barrel.assign(barrel_x, barrel_y, 0.0);
			barrel.readObj("barrell.obj", true, true);
			barrel.scale(0.1 * 2.0f * 0.99);
			barrel.translate(trans_barrel);

			gl_barrel.initPhongSurface(barrel);
			gl_barrel.mat_.setBrown();
		}

		if (ver_enemy_y <= -1.5) { // range out of vertical enemy, setting start position
			rand_xpos = (float)(rand() % 2205) / 1000 + 0.004;
			//std::cout << rand_xpos << std::endl;
			ver_decision = rand() % 2;
			if (ver_decision) {
				rand_xpos = -rand_xpos;
			}
			ver_enemy_x = rand_xpos;
			ver_enemy_y = 2.0;
			ver_enemy.startToOrigin(ver_enemy_x, ver_enemy_y, 0.0);
			trans_ver_enemy.assign(ver_enemy_x, ver_enemy_y, 0.0);
		}

		if (rocket_y >= 1.5) { // range out of vertical enemy, setting start position
			rocket_xpos = (float)(rand() % 2205) / 1000 + 0.004;
			//std::cout << rand_xpos << std::endl;			
			rocket_ver_decision = rand() % 2;
			if (rocket_ver_decision) {
				rocket_xpos = -rocket_xpos;
			}
			rocket_x = rocket_xpos;
			rocket_y = -2.0;
			rocket.startToOrigin(rocket_x, rocket_y, 0.0);
			trans_rocket.assign(rocket_x, rocket_y, 0.0);
		}

		if (hor_enemy_x <= -2.5) { // range out of horizontal enemy, setting start position
			rand_ypos = (float)(rand() % 1190) / 1000 + 0.004;
			hor_decision = rand() % 2;
			if (hor_decision) {
				rand_ypos = -rand_ypos;
			}
			hor_enemy_x = 3.0;
			hor_enemy_y = rand_ypos;
			hor_enemy.startToOrigin(hor_enemy_x, hor_enemy_y, 0.0);
			trans_hor_enemy.assign(hor_enemy_x, hor_enemy_y, 0.0);
		}

		if (tank_x <= -2.5 ) { // range out of tank, setting start position
			tank_x = 3.0;
			tank.startToOrigin(tank_x, tank_y, 0.0);
			trans_tank.assign(tank_x, tank_y, 0.0);
		}


		if (ufo_z >= 2.0) { // range out of horizontal enemy, setting start position
			rand_ufo_x = (float)(rand() % 2205) / 1000 + 0.004;
			rand_ufo_y = (float)(rand() % 1190) / 1000 + 0.004;
			ufo_x_decision = rand() % 2;
			if (ufo_x_decision) {
				rand_ufo_x = -rand_ufo_x;
			}
			ufo_y_decision = rand() % 2;
			if (hor_decision) {
				rand_ufo_y = -rand_ufo_y;
			}
			ufo_x = rand_ufo_x;
			ufo_y = rand_ufo_y;
			ufo_z = -2.0;
			ufo.startToOrigin(ufo_x, ufo_y, ufo_z);
			trans_ufo.assign(ufo_x, ufo_y, ufo_z);
		}

		if (turkey_x >= 3.0) { // range out of horizontal enemy, setting start position
			turkey_ypos = (float)(rand() % 1190) / 1000 + 0.004;
			turkey_decision = rand() % 2;
			if (turkey_decision) {
				turkey_ypos = -turkey_ypos;
			}
			turkey_x = -3.0;
			turkey_y = turkey_ypos;
			turkey.startToOrigin(turkey_x, turkey_y, 0.0);
			trans_turkey.assign(turkey_x, turkey_y, 0.0);
		}
		gl_world.camera_.ContinueRotation();
		//std::cout << "camera x : " << gl_world.camera_.x_ << " camera y : " << gl_world.camera_.y_ << std::endl;
		//std::cout << "origin x : " << origin_x << " origin y : " << origin_y << std::endl;
		glm::mat4 vp = gl_world.camera_.GetWorldViewMatrix();

		if (origin_y < -1.19) { // 시작할 때 비행기 등장을 위한 조건문 (안보이는 곳에서 서서히 등장)
			origin_y += ironman_speed;
			trans_ironman.assign(0.0, ironman_speed, 0.0);
			ironman.translate(trans_ironman);
			gl_ironman.updatePhongSurface(ironman);
			for (int i = 0; i < 4; i++) direction[i] = false;
		}

		if (origin_x <= -border_x) direction[0] = false; // 좌측 경계 => 왼쪽 방향으로 갈 수 없게함
		if (origin_y >= border_y) direction[2] = false; // 상측 경계 => 위쪽 방향으로 갈 수 없게함
		if (origin_x >= border_x) direction[1] = false; // 우측 경계 => 오른쪽 방향으로 갈 수 없게함
		if (origin_y <= -border_y) direction[3] = false; // 하측 경계 => 아래쪽 방향으로 갈 수 없게함

		if (direction[0]) {
			origin_x -= ironman_speed;
			trans_ironman.assign(-ironman_speed, 0.0, 0.0);
			ironman.translate(trans_ironman);
		}
		if (direction[1]) {
			origin_x += ironman_speed;
			trans_ironman.assign(ironman_speed, 0.0, 0.0);
			ironman.translate(trans_ironman);
		}
		if (direction[2]) {
			origin_y += ironman_speed;
			trans_ironman.assign(0.0, ironman_speed, 0.0);
			ironman.translate(trans_ironman);
		}
		if (direction[3]) {
			origin_y -= ironman_speed;
			trans_ironman.assign(0.0, -ironman_speed, 0.0);
			ironman.translate(trans_ironman);
		}

		if (z_direction[0]) { // near
			origin_z += ironman_speed;
			trans_ironman.assign(0.0, 0.0, ironman_speed);
			ironman.translate(trans_ironman);
		}
		else if (z_direction[1]) { // far
			origin_z -= ironman_speed;
			trans_ironman.assign(0.0, 0.0, -ironman_speed);
			ironman.translate(trans_ironman);
		}

		// std::cout << "origin x : " << origin_x << " origin y : " << origin_y << std::endl;
		// std::cout << "ver_enemy x : " << ver_enemy_x << " ver_enemy y : " << ver_enemy_y << std::endl;

		// ironman collision with vertical enemy
		if (origin_x > ver_enemy_x - ver_col_range && origin_x < ver_enemy_x + ver_col_range &&
			origin_y > ver_enemy_y - ver_col_range && origin_y < ver_enemy_y + ver_col_range &&
			origin_z > ver_enemy_z - ver_col_range && origin_z < ver_enemy_z + ver_col_range) {
			bomb_x = (origin_x + ver_enemy_x) / 2;
			bomb_y = (origin_y + ver_enemy_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			ver_enem_col = true;
		}

		// ironman collision with horizontal enemy
		if (origin_x > hor_enemy_x - hor_col_range && origin_x < hor_enemy_x + hor_col_range &&
			origin_y > hor_enemy_y - hor_col_range && origin_y < hor_enemy_y + hor_col_range &&
			origin_z > hor_enemy_z - hor_col_range && origin_z < hor_enemy_z + hor_col_range) {
			bomb_x = (origin_x + hor_enemy_x) / 2;
			bomb_y = (origin_y + hor_enemy_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			hor_enem_col = true;
		}

		// ironman collision with tank
		if (origin_x > tank_x - tank_col_range && origin_x < tank_x + tank_col_range &&
			origin_y > tank_y - tank_col_range && origin_y < tank_y + tank_col_range &&
			origin_z > tank_z - tank_col_range && origin_z < tank_z + tank_col_range) {
			bomb_x = (origin_x + tank_x) / 2;
			bomb_y = (origin_y + tank_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			tank_col = true;
		}
		
		// ironman collision with turkey
		if (origin_x > turkey_x - turkey_col_range && origin_x < turkey_x + turkey_col_range &&
			origin_y > turkey_y - turkey_col_range && origin_y < turkey_y + turkey_col_range &&
			origin_z > turkey_z - turkey_col_range && origin_z < turkey_z + turkey_col_range) {
			bomb_x = (origin_x + turkey_x) / 2;
			bomb_y = (origin_y + turkey_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			turkey_col = true;
		}

		// ironman collision with ufo
		if (origin_x > ufo_x - ufo_col_range && origin_x < ufo_x + ufo_col_range &&
			origin_y > ufo_y - ufo_col_range && origin_y < ufo_y + ufo_col_range &&
			origin_z > ufo_z - ufo_col_range && origin_z < ufo_z + ufo_col_range) {
			bomb_x = (origin_x + ufo_x) / 2;
			bomb_y = (origin_y + ufo_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			ufo_col = true;
		}

		// ironman collision with rocket
		if (origin_x > rocket_x - rocket_col_range && origin_x < rocket_x + rocket_col_range &&
			origin_y > rocket_y - rocket_col_range && origin_y < rocket_y + rocket_col_range &&
			origin_z > rocket_z - rocket_col_range && origin_z < rocket_z + rocket_col_range) {
			bomb_x = (origin_x + rocket_x) / 2;
			bomb_y = (origin_y + rocket_y) / 2;
			trans_bomb.assign(bomb_x, bomb_y, 0.0);
			bomb.readObj("bomb.obj", true, true);
			bomb.scale(0.2 * 2.0f * 0.99);
			bomb.translate(trans_bomb);

			gl_bomb.initPhongSurface(bomb);
			gl_bomb.mat_.setBlack();

			rocket_col = true;
		}
		//// vertical enemy collision with horizontal enemy
		//if (ver_enemy_x > hor_enemy_x - 0.15 && ver_enemy_x < hor_enemy_x + 0.15 &&
		//	ver_enemy_y > hor_enemy_y - 0.15 && ver_enemy_y < hor_enemy_y + 0.15) {
		//	bomb_x = (ver_enemy_x + hor_enemy_x) / 2;
		//	bomb_y = (ver_enemy_y + hor_enemy_y) / 2;
		//	trans_bomb.assign(bomb_x, bomb_y, 0.0);
		//	bomb.readObj("bomb.obj", true, true);
		//	bomb.scale(0.2 * 2.0f * 0.99);
		//	bomb.translate(trans_bomb);

		//	gl_bomb.initPhongSurface(bomb);
		//	gl_bomb.mat_.setBlack();

		//	ver_hor_col = true;
		//}

		ver_enemy_y -= ver_enemy_speed;
		trans_ver_enemy.assign(0.0, -ver_enemy_speed, 0.0);
		ver_enemy.translate(trans_ver_enemy);
		gl_ver_enemy.updatePhongSurface(ver_enemy);

		hor_enemy_x -= hor_enemy_speed;
		trans_hor_enemy.assign(-hor_enemy_speed, 0.0, 0.0);
		hor_enemy.translate(trans_hor_enemy);
		gl_hor_enemy.updatePhongSurface(hor_enemy);

		turkey_x += turkey_speed;
		trans_turkey.assign(turkey_speed, 0.0, 0.0);
		turkey.translate(trans_turkey);
		gl_turkey.updatePhongSurface(turkey);

		tank_x -= tank_speed;
		trans_tank.assign(-tank_speed, 0.0, 0.0);
		tank.translate(trans_tank);
		gl_tank.updatePhongSurface(tank);

		ufo_z += ufo_speed;
		trans_ufo.assign(0.0, 0.0, ufo_speed);
		ufo.translate(trans_ufo);
		gl_ufo.updatePhongSurface(ufo);

		rocket_y += rocket_speed;
		trans_rocket.assign(0.0, rocket_speed, 0.0);
		rocket.translate(trans_rocket);
		gl_rocket.updatePhongSurface(rocket);

		// don't need to update if there is no change
		if(direction[0] || direction[1] || direction[2] || direction[3]) gl_ironman.updatePhongSurface(ironman); 

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gl_world.shaders_.program_id_);

		gl_world.shaders_.sendUniform(vp, "mvp");

		//gl_obj.drawPhongSurface();
		gl_ironman.applyLighting(light);
		gl_ironman.drawWithShader(gl_world.shaders_);
		//gl_obj.drawPhongSurface();

		gl_ver_enemy.applyLighting(light);
		gl_ver_enemy.drawWithShader(gl_world.shaders_);

		gl_hor_enemy.applyLighting(light);
		gl_hor_enemy.drawWithShader(gl_world.shaders_);

		gl_tank.applyLighting(light);
		gl_tank.drawWithShader(gl_world.shaders_);

		gl_ufo.applyLighting(light);
		gl_ufo.drawWithShader(gl_world.shaders_);

		gl_turkey.applyLighting(light);
		gl_turkey.drawWithShader(gl_world.shaders_);

		gl_rocket.applyLighting(light);
		gl_rocket.drawWithShader(gl_world.shaders_);

		for (int i = 0; i < number_of_star; i++) {
			gl_star[i].applyLighting(light);
			gl_star[i].drawWithShader(gl_world.shaders_);
		}

		/*gl_star.applyLighting(light);
		gl_star.drawWithShader(gl_world.shaders_);*/

		if (ver_enem_col || hor_enem_col || tank_col || ufo_col || turkey_col || rocket_col) {
			gl_bomb.applyLighting(light);
			gl_bomb.drawWithShader(gl_world.shaders_);
		}

		if (barrel_exist) {
			gl_barrel.applyLighting(light);
			gl_barrel.drawWithShader(gl_world.shaders_);
		}

		// ironman take barrel, speed up
		if (origin_x > barrel_x - 0.15 && origin_x < barrel_x + 0.15 &&
			origin_y > barrel_y - 0.15 && origin_y < barrel_y + 0.15 && barrel_exist) {
			trans_barrel.assign(-3.0, -2.0, 0.0);
			barrel.startToOrigin(-3.0, -2.0, 0.0);

			gl_barrel.initPhongSurface(barrel);
			gl_barrel.mat_.setGold();

			ironman_speed += 0.0005;
			barrel_exist = false;
		}

		glUseProgram(0);

		// Old-style rendering

		glLoadMatrixf(&vp[0][0]);

		//glLineWidth(2);
		//gl_world.drawAxes();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		if (ver_enem_col || hor_enem_col || tank_col || ufo_col || turkey_col || rocket_col) {
			game_over = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			glfwTerminate();
		}

		if (game_over) return 0;
	}
	glfwTerminate();
	
	return 0;
}

