#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "Shaders.h"
#include "VertexLoader.h"
#include "VertexWriter.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Window dimensions
float resolutions[2] = { 800, 600 };

//function used in task 1
float VertexFunction1(const float Param)
{
	//get the value
	const float Value = Param * Param - 2 * Param + 4;
	return Value;
}

//function to be used in task 2
std::vector<Vertex> VertexFunction2(int Amount, int Stride)
{
    //vector to store vertices
    std::vector<Vertex> Vertices;

    //reserve space for the vertices
    Vertices.reserve(Amount);

    //iterate through each index
	for (int Index = 0; Index < Amount; ++Index)
	{
		//generate a spiral
		Vertices.emplace_back(glm::vec3(cos(Index * 0.1f), sin(Index * 0.1f), Index * 0.1f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

    return Vertices;
}

//function to be used in task 3
std::vector<Vertex> VertexFunction3(int InVal1, int InVal2, int Stride1, int Stride2, int Amount)
{
	//vector to store vertices
    std::vector<Vertex> Vertices;

    //reserve space for the vertices
    Vertices.reserve(Amount);

    //iterate through each index
    for (int Index = 0; Index < Amount; ++Index)
    {
        //the xyz coordinates of the vertex
        const int X = InVal1 + Stride1 * Index;
        const int Y = InVal2 + Stride2 * Index;
        const int Z = X + Y;

	    //add the vertex to the vector
		Vertices.emplace_back(glm::vec3(X, Y, Z), glm::vec3(1.0f, 1.0f, 1.0f));
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

    return Window;
}

//render function that gets called every frame
void RenderFunction(int Amount)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    //using glDrawArrays with GL_LINE_STRIP
    glDrawArrays(GL_LINE_STRIP, 0, Amount);
}

int main()
{
    //create vertex data variables' from the mathematical functions
    const std::vector<Vertex> VertexData1 = VertexWriter::WriteVertexDataFromFunc(VertexFunction1, 0, 3, 100);
    const std::vector<Vertex> VertexData2 = VertexFunction2(100, 1);
    const std::vector<Vertex> VertexData3 = VertexFunction3(0, 0, 1, 1, 100);

    //write the vertex data to a file
    VertexWriter::WriteVertexData("C:/School/2024-2025/3D-Programming/Compulsories/Compulsory 1/VertexData.txt", VertexData1, false);
    VertexWriter::WriteVertexData("C:/School/2024-2025/3D-Programming/Compulsories/Compulsory 1/VertexData.txt", VertexData2, true);
    VertexWriter::WriteVertexData("C:/School/2024-2025/3D-Programming/Compulsories/Compulsory 1/VertexData.txt", VertexData3, true);

	//create the vertex data and convert it to floats
	const std::vector<float> Vertices = VertexWriter::ConvertVertexDataToFloats(VertexData1);

    //setup glfw
    GLFWSetup();

    //glfw window creation
    GLFWwindow* Window = CreateWindow();

    //glad: load all OpenGL function pointers
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //create the shader program
    const unsigned int shaderProgram = Shaders::CreateShader();

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float), Vertices.data(), GL_STATIC_DRAW);

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

    // render loop
    while (!glfwWindowShouldClose(Window))
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // input
		processInput(Window);

        // render
        RenderFunction(Vertices.size() / 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        std::cout << "A key pressed ..." << std::endl;
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
