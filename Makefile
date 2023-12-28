CXX = g++
CXXFLAGS = -std=c++11 -arch arm64 -I$(shell brew --prefix glm)/include -I$(shell brew --prefix glew)/include -I$(shell brew --prefix glfw)/include -L$(shell brew --prefix glew)/lib -L$(shell brew --prefix glfw)/lib -lGLEW -lglfw -framework OpenGL


myOpenGLApp: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o bunnyRun
