#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);	// �������


// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// vision
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// ������ɫ��
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model, view, projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";
// Ƭ����ɫ��
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main()
{
	// glfw��ʼ��������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawTriangle", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: ����OpenGL����ָ��
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return 0;
	}

	// ��ʼ��ImGui����
	const char * glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);	// ����Ⱦ��
	ImGui_ImplOpenGL3_Init(glsl_version);

	//-----------------------------------
	// ����������ɫ�����󣬱�����ɫ��Դ��
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// �����ɫ���������
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "���붥����ɫ��Դ����ִ���\n" << infoLog << std::endl;
	}

	// ����Ƭ����ɫ�����󣬱�����ɫ��Դ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// �����ɫ���������
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "����Ƭ����ɫ��Դ����ִ���\n" << infoLog << std::endl;
	}

	//-----------------------------------
	// ������ɫ��
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); // ����һ����ɫ���������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// ���������ɫ���������
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "������ɫ��������ִ���\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// �������� �� ��������
	//float vertices[] = {
	//	-2.0f, -2.0f, -2.0f,	0.4f, 1.0f, 0.4f,
	//	 2.0f, -2.0f, -2.0f,	1.0f, 0.4f, 0.4f,
	//	 2.0f,  2.0f, -2.0f,	0.4f, 0.4f, 1.0f,
	//	-2.0f,  2.0f, -2.0f,	0.5f, 0.5f, 0.5f,

	//	-2.0f, -2.0f,  2.0f,	1.0f, 0.4f, 0.4f,
	//	 2.0f, -2.0f,  2.0f,	0.4f, 1.0f, 0.4f,
	//	 2.0f,  2.0f,  2.0f,	0.5f, 0.5f, 0.5f,
	//	-2.0f,  2.0f,  2.0f,	0.4f, 0.4f, 1.0f
	//};

	//unsigned int indices[] = {
	//	0, 1, 2,	// ǰ��-1
	//	0, 2, 3,	// ǰ��-2
	//	4, 5, 6,	// ����-1
	//	4, 6, 7,	// ����-2

	//	0, 3, 7,	// ����-1
	//	0, 4, 7,	// ����-2
	//	1, 2, 6,	// ����-1
	//	1, 5, 6,	// ����-2

	//	0, 1, 5,	// ����-1
	//	0, 4, 5,	// ����-2
	//	2, 3, 6,	// ����-1
	//	3, 6, 7		// ����-2
	//};

	float vertices[] = {
		-1.0f, -1.0f, -1.0f,	0.8f, 0.6f, 0.4f,	// ��-1
		1.0f, -1.0f, -1.0f,		0.8f, 0.6f, 0.4f,
		1.0f,  1.0f, -1.0f,		0.8f, 0.6f, 0.4f,

		-1.0f, -1.0f, -1.0f,	0.8f, 0.6f, 0.4f,	// ��-2
		1.0f,  1.0f, -1.0f,		0.8f, 0.6f, 0.4f,
		-1.0f,  1.0f, -1.0f,	0.8f, 0.6f, 0.4f,

		-1.0f, -1.0f,  1.0f,	0.4f, 0.6f, 0.8f,	// ǰ-1
		1.0f, -1.0f,  1.0f,		0.4f, 0.6f, 0.8f,
		1.0f,  1.0f,  1.0f,		0.4f, 0.6f, 0.8f,

		-1.0f, -1.0f,  1.0f,	0.4f, 0.6f, 0.8f,	// ǰ-2
		1.0f,  1.0f,  1.0f,		0.4f, 0.6f, 0.8f,
		-1.0f,  1.0f,  1.0f,	0.4f, 0.6f, 0.8f,

		-1.0f, -1.0f, -1.0f,	0.6f, 0.4f, 0.8f,	// ��-1
		-1.0f,  1.0f, -1.0f,	0.6f, 0.4f, 0.8f,
		-1.0f,  1.0f,  1.0f,	0.6f, 0.4f, 0.8f,

		-1.0f, -1.0f, -1.0f,	0.6f, 0.4f, 0.8f,	// ��-2
		-1.0f, -1.0f,  1.0f,	0.6f, 0.4f, 0.8f,
		-1.0f,  1.0f,  1.0f,	0.6f, 0.4f, 0.8f,

		1.0f, -1.0f, -1.0f,		0.8f, 0.4f, 0.6f,	// ��-1
		1.0f,  1.0f, -1.0f,		0.8f, 0.4f, 0.6f,
		1.0f,  1.0f,  1.0f,		0.8f, 0.4f, 0.6f,

		1.0f, -1.0f, -1.0f,		0.8f, 0.4f, 0.6f,	// ��-2
		1.0f, -1.0f,  1.0f,		0.8f, 0.4f, 0.6f,
		1.0f,  1.0f,  1.0f,		0.8f, 0.4f, 0.6f,

		-1.0f, -1.0f, -1.0f,	0.4f, 0.8f, 0.6f,	// ��-1
		1.0f, -1.0f, -1.0f,		0.4f, 0.8f, 0.6f,
		1.0f, -1.0f,  1.0f,		0.4f, 0.8f, 0.6f,

		-1.0f, -1.0f, -1.0f,	0.4f, 0.8f, 0.6f,	// ��-2
		-1.0f, -1.0f,  1.0f,	0.4f, 0.8f, 0.6f,
		1.0f, -1.0f,  1.0f,		0.4f, 0.8f, 0.6f,

		1.0f,  1.0f, -1.0f,		0.6f, 0.8f, 0.4f,	// ��-1
		-1.0f,  1.0f, -1.0f,	0.6f, 0.8f, 0.4f,
		1.0f,  1.0f,  1.0f,		0.6f, 0.8f, 0.4f,

		-1.0f,  1.0f, -1.0f,	0.6f, 0.8f, 0.4f,	// ��-2
		1.0f,  1.0f,  1.0f,		0.6f, 0.8f, 0.4f,
		-1.0f,  1.0f,  1.0f,	0.6f, 0.8f, 0.4f
	};

	// �������㻺����� Vertex Buffer Objects, �洢����, ����������GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ��������������� Vertex Array Objects, ����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// ����EBO
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ������������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	bool isOrtho = false;
	bool isViewChange = false;
	bool movingCamera = false;
	float orthoLeft = -5.0f, orthoRight = 5.0f, orthoTop = 5.0f, orthoBottom = -5.0f, orthoNear = 0.1f, orthoFar = 100.0f;
	float fov = 45.0f;

	while (!glfwWindowShouldClose(window))
	{
		// ��������
		processInput(window);
		glfwSetCursorPosCallback(window, mouse_callback);

		// ��Ⱦ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);

		//-----------------------------------
		// ImGui��ʼ�� - ���� - ��Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Configuration");
		ImGui::PushItemWidth(150);
		ImGui::Checkbox("View Moving ", &isViewChange);
		ImGui::Checkbox("Using Orthogonal Projection    ", &isOrtho);
		if (isOrtho) {
			ImGui::SliderFloat("left", &orthoLeft, -20.0f, -5.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("right", &orthoRight, 5.0f, 20.0f);
			ImGui::SliderFloat("top  ", &orthoTop, 5.0f, 20.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("bottom", &orthoBottom, -20.0f, -5.0f);
			ImGui::SliderFloat("near", &orthoNear, 0.1f, 10.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("far", &orthoFar, 20.0f, 100.0f);
		}
		else
			ImGui::SliderFloat("fov", &fov, 30.0f, 60.0f);
		ImGui::Checkbox("Using Moving Camera", &movingCamera);
		ImGui::End();

		if (movingCamera) 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// ���ݴ��ڵĻ����С��ȡ�ߴ�
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//-----------------------------------
		glUseProgram(shaderProgram);

		
		// �۲����
		glm::mat4 view = glm::mat4(1.0f);
		float radius = 6.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		if (isViewChange) {
			view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),		// camera position
				glm::vec3(0.0f, 0.0f, 0.0f),					// camera target
				glm::vec3(0.0f, 1.0f, 0.0f));					// camera up vector
		}
		else if (movingCamera)
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		else 
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
		

		// ͶӰ����
		glm::mat4 projection = glm::mat4(1.0f);
		if (isOrtho)
			projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
		else
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// 

		// �任����
		glm::mat4 model = glm::mat4(1.0f);
		// model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));


		
		// ģ�;��󡢹۲����ͶӰ�����������������������ɫ��
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		// glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);		// �������� 36
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);


		// glfw: �������棬�����¼�
		// ------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// glfw: ��������GLFW��Դ
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 0.1f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// ���� ��� �ƶ��ӽ�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float sensitivity = 0.05;
	yaw   += sensitivity * (xpos - lastX);
	pitch += sensitivity * (lastY - ypos);
	lastX = xpos;
	lastY = ypos;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}