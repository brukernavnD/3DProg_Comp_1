#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <cmath>
#include <math.h>

#include "Camera.h"
#include "Shaders.h"
#include "VertexLoader.h"
#include "VertexWriter.h"
#include "glm/ext/matrix_clip_space.hpp"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

// Window dimensions
float resolutions[2] = { 800, 600 };

// camera (from https://learnopengl.com/Getting-started/Camera)
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = resolutions[0] / 2.0f;
float lastY = resolutions[1] / 2.0f;
bool firstMouse = true;

// timing (from https://learnopengl.com/Getting-started/Camera)
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int FunctionPicker = 0;

//function used in task 1
float VertexFunction1(const float Param)
{
	//get the value
	const float Value = Param * Param - 2 * Param + 4;
	return Value;
}

//derivative function to be used in task 1
float DerivativeFunction1(const float Param)
{
	//get the value
	const float Value = 2 * Param - 2;
	return Value;
}

//function to be used in task 2
std::vector<Vertex> VertexFunction2(const int LastIndex, const int LevelOfDetail)
{
    //vector to store vertices
    std::vector<Vertex> Vertices;

    //reserve space for the vertices
    Vertices.reserve(LastIndex);

    //colour of the vertices
    glm::vec3 Colour = glm::vec3(1.0f, 1.0f, 1.0f);

    //iterate through each index
	for (int Index = 0; Index < (LastIndex * LevelOfDetail) + 1; Index += 1)
	{
        //use cosine and sine to get the x and y coordinates
        float X = cos(float(Index) / float(LevelOfDetail));
        float Y = sin(float(Index) / float(LevelOfDetail));
        float Z = float(Index) / (float(LevelOfDetail) * float(LastIndex)) * 6.f * glm::pi<float>();

		//generate a spiral
		Vertices.emplace_back(glm::vec3(X, Y, Z), Colour);
	}

    return Vertices;
}

//helper function to generate the float value for the z coordinate in task 3
float ZFunction(const float X, const float Y)
{
	//get the value
	const float Value = X + Y;
	return Value;
}

//function to be used in task 3
std::vector<Vertex> VertexFunction3(int Amount1, int Amount2, int SqareSize = 1)
{
	//vector to store vertices
    std::vector<Vertex> Vertices;

	for (int XCoord = 0; XCoord < Amount1; ++XCoord)
	{
		for (int YCoord = 0; YCoord < Amount2; ++YCoord)
		{
            //generate the first triangle of the square
            Vertex CurrentVertex1 = Vertex(glm::vec3(XCoord * SqareSize, YCoord * SqareSize, ZFunction(XCoord * SqareSize, YCoord * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));
            Vertex CurrentVertex2 = Vertex(glm::vec3(XCoord * SqareSize, (YCoord + 1) * SqareSize, ZFunction(XCoord * SqareSize, (YCoord + 1) * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));
            Vertex CurrentVertex3 = Vertex(glm::vec3((XCoord + 1) * SqareSize, YCoord * SqareSize, ZFunction((XCoord + 1) * SqareSize, YCoord * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));

            //generate the second triangle of the square
            Vertex CurrentVertex4 = Vertex(glm::vec3(XCoord * SqareSize, (YCoord + 1) * SqareSize, ZFunction(XCoord * SqareSize, (YCoord + 1) * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));
            Vertex CurrentVertex5 = Vertex(glm::vec3((XCoord + 1) * SqareSize, YCoord * SqareSize, ZFunction((XCoord + 1) * SqareSize, YCoord * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));
            Vertex CurrentVertex6 = Vertex(glm::vec3((XCoord + 1) * SqareSize, (YCoord + 1) * SqareSize, ZFunction((XCoord + 1) * SqareSize, (YCoord + 1) * SqareSize)), glm::vec3(1.0f, 0.0f, 0.0f));

            //add the vertex to the vector
            Vertices.push_back(CurrentVertex1);
            Vertices.push_back(CurrentVertex2);
            Vertices.push_back(CurrentVertex3);
            Vertices.push_back(CurrentVertex4);
            Vertices.push_back(CurrentVertex5);
            Vertices.push_back(CurrentVertex6);
		}
	}


	return Vertices;
}
	
//setup function
void GLFWSetup()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

//function to create a glfw window
GLFWwindow* CreateWindow()
{
    //create the window
	GLFWwindow* Window = glfwCreateWindow(resolutions[0], resolutions[1], "Compulsory 1", NULL, NULL);

    //set the window to the current context
	glfwMakeContextCurrent(Window);

    //set the on resize callback
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetScrollCallback(Window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return Window;
}

int main()
{
    //create vertex data variables' from the mathematical functions
    const std::vector<Vertex> VertexData1 = VertexWriter::WriteVertexDataFromFunc(VertexFunction1, 0, 3, 100);
    const std::vector<Vertex> VertexData2 = VertexFunction2(100, 10);
    const std::vector<Vertex> VertexData3 = VertexFunction3(10, 10);

    //write the vertex data to a file
    VertexWriter::WriteVertexData("VertexData.txt", VertexData1, false);
    VertexWriter::WriteVertexData("VertexData.txt", VertexData2, true);
    VertexWriter::WriteVertexData("VertexData.txt", VertexData3, true);

	//convert vertex data to floats
    const std::vector<float> VertexDataFloats1 = VertexWriter::ConvertVertexDataToFloats(VertexData1);
    const std::vector<float> VertexDataFloats2 = VertexWriter::ConvertVertexDataToFloats(VertexData2);
    const std::vector<float> VertexDataFloats3 = VertexWriter::ConvertVertexDataToFloats(VertexData3);

    //combine the vertex data
	std::vector<float> TotalVertices = VertexDataFloats1;
    TotalVertices.insert(TotalVertices.end(), VertexDataFloats2.begin(), VertexDataFloats2.end());
    TotalVertices.insert(TotalVertices.end(), VertexDataFloats3.begin(), VertexDataFloats3.end());

    //setup glfw
    GLFWSetup();

    //glfw window creation
    GLFWwindow* Window = CreateWindow();

    //glad: load all OpenGL function pointers
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //create the shader program
    const unsigned int ShaderProgram = Shaders::CreateShader();

    //glEnable(GL_DEPTH_TEST);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, TotalVertices.size() * sizeof(float), TotalVertices.data(), GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    //colour attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    //storage variables for the uniform names
    const std::string name1 = "projection";
    const std::string name2 = "view";
    const std::string name3 = "model";

    // render loop
    while (!glfwWindowShouldClose(Window))
    {
        // per-frame time logic (from https://learnopengl.com/Getting-started/Camera)
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
		processInput(Window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);

        //(modified)from https://learnopengl.com/Getting-started/Camera
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)resolutions[0] / (float)resolutions[1], 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, name1.c_str()), 1, false, &projection[0][0]);

        // camera/view transformation
        glm::mat4 View = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram,name2.c_str()), 1, false, &View[0][0]);

         //model matrix
        glm::mat4 model = glm::mat4(1.0f);

        //set the model matrix uniform
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, name3.c_str()), 1, false, &model[0][0]);

        //draw the graph of function 1
	    if (FunctionPicker == 0)
        {
        	//draw the first function
        	glDrawArrays(GL_LINE_STRIP, 0, VertexDataFloats1.size() / 6);
	    }
	    else if (FunctionPicker == 1)
	    {
		    //draw the second function
			glDrawArrays(GL_LINE_STRIP, VertexDataFloats1.size() / 6, VertexDataFloats2.size() / 6);
	    }
	    else if (FunctionPicker == 2)
	    {
			//draw the third function
			glDrawArrays(GL_TRIANGLES, VertexDataFloats1.size() / 6 + VertexDataFloats2.size() / 6, VertexDataFloats3.size() / 6);
	    }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(ShaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
	    glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
	    camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
	    camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
	    camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
	    camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        FunctionPicker = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
	    FunctionPicker = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
	    FunctionPicker = 2;
    }
}

//called whenever the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    std::cout << " windows resized with " << width << " Height " << height << std::endl;
}


// glfw: whenever the mouse moves, this callback is called (from https://learnopengl.com/Getting-started/Camera)
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called (from https://learnopengl.com/Getting-started/Camera)
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}