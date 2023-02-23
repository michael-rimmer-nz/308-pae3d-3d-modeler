# PAE3D - 3D Modeler Project #
**Authors:      Henry P, Michael R**

### Description ###

**COMP 308 Introduction to Computer Graphics - Final Project**

2014 final project for my computer graphics class. The project objective was to create a design interface for modelling unique functions/operations on 3D objects using OpenGL.

The project was done in pairs, my partner was Henry P. We decided to make a 3D Modeler to discover the process involved in creating an interface that allows an inexperienced user to create complex models by using the interface’s tools/operations. Henry built the core model structure, while my focus was individual tools/operations and the user interface. The project is intended to be a proof of concept as it is a first attempt at making a 3D Modeler. 

This repository is for historical and storage purposes. 

#### Edits ####

2023
 - Basic code formatting to be more readable. 
 - Code comments for historical purposes.
 - Minor fix to the selection processing that auto-selects edges, faces when vertices are selected (2 lines changed). 
 - No major functional changes. 
 
#### Potential Improvements ####
 
 - Improve naming for some functions, definitions, global and local variable. Some don't describe themselves as well as they could. There needs to be more consistency in the name formatting. 
 - Look into optimising functions and their design.
 - Duplicate is not on the toolbar.
 - Can only extrude one face at a time. Cannot extrude multiple faces, nor any edges or vertices. 
 - Model.cpp is large and could have some of its transformation operations moved to a new 'ModelTransform' class. Functions: subdivide/smooth, extrude, merge, duplicate etc.
 - There is only one Model class instance. We have not looked at how it behaves with more than one model instance. 
 - Duplicate would be better named "copy" since it copies vertices, edges and faces within the model instance. A 'duplicate' would ideally create a copy instance of the model instead. 
 - With multiple instances of a model we could add an 'object dashboard' and include tools for creating new objects and managing them. 

### Build and Run ###

**Compile:**

	$ make

**Run with default generated cube:**

	$ ./build/PAE3D

**Run with loaded model object file:**

	S ./build/PAE3D -o models/Teapot.obj

### Usage ###

Top toolbar buttons have short cut keys written on them; They work as follows.

**Camera:**

	middle click + drag to rotate.
	ctrl + middle click + drag to zoom.
	shift + middle click + drag to pan.

**Selection:**
	f, e, v toggle selection mode to faces, edges and vertices respectively.
	a to select all.
	right click to select face, edge or vertex.

**Move/Scale(M,S):**

	(M) to toggle to move mode
	(S) to toggle to scale mode
	left click + drag handles to move/scale

**Regional Extrude(X):**

	(X) to extrude selected faces, note you need to move or scale after as it extrudes in place.

**Merge(L):**

	(L) to merge all selected vertices, edges, faces to the center of the selection (handle position)

**Subdivide(D):**

	(D) to subdivide selected faces, if none are selected will subdivide the whole model

**Smooth(G):**

	(G) to smooth selected faces, if none are selected will smooth the whole model

**Colour(C):**

	c to enable/disable colouring by left click. colouring a face will set it to the material selected in the colour window
	in the bottom left of the screen.

	+ button to add new material.
	<, > buttons to navigate the materials.
	sliders to change material parameters.

**Renderer(P):**

	p to switch to render mode, only faces are drawn with gourad shading. A skybox is added for aesthetics.

**Delete(del):**

	(del) to delete all selected faces, edges and vertices and any edges and faces attached to selected verticies

**Duplicate(1):**

	(1) number key to duplicate all selected faces, edges and vertices


## WSL2 Installation ##

### Using WSL2 ###

**Install:** https://learn.microsoft.com/en-us/windows/wsl/install-manual 

The Windows Subsystem for Linux (WSL) lets developers install a Linux distribution (e.g. Ubuntu).
You can use Linux applications, utilities, and Bash command-line tools directly on Windows, unmodified, without the overhead of a traditional virtual machine or dual-boot setup. 
Alternatively you can use a Linux OS or Virtual Machine. For this setup I used Ubuntu with WSL 2. 

Notes: When using WSL you will also need to install and set up VcXsrv for generating a display window as explained below. 

### STEPS ###

**1. Install a distribution to WSL (e.g., Ubuntu)**

Commands:

	- List available distributions:
	
		$ wsl --list --online
	
	- Install a distribution:
	
		$ wsl.exe --install -d <Distribution Name> 
		
		for example/
		
		$ wsl.exe --install -d Ubuntu

**2. Copy the project into your distribution file directory.**

With WSL your distribution file-system is accessible via the window explorer. 
Put the project in a sensible place e.g. \\wsl.localhost\Ubuntu\home\<user>\

**3. Open your distribution command line prompt.**

Navigate to the project location. The command line should open to your user directory by default. 

e.g. 
 
 $ cd \\wsl.localhost\Ubuntu\home\<user>\<project> 		
 
 or/
 
 $ cd ~\<project>


**4. Install required project libraries:**

	$ sudo apt-get install g++-mingw-w64
	$ sudo apt-get install freeglut3-dev
	$ sudo apt-get install libjpeg62-dev
	$ sudo apt-get install libpng++-dev
	
**5. In the project directory run make**

	$ Make

	This should build the project as long as the libraries are correct. 


**6. Setup Application Display Windows**

WSL won't have a window to display the application when it is run. 
Here we use vcxsrv as a display server for Windows to display our Linux apps. 
It listens for application connections and will display them when connected to. 

Resource: https://github.com/microsoft/WSL/issues/4106

a. Install vcxsrv on Windows

	https://sourceforge.net/projects/vcxsrv/files/latest/download

b. Start XLaunch on Windows

	*Note: For this application to work with WSL apps it has to be allowed on public and private networks.*

	- Select Multiple Windows (default)
	- Select Start no client (default)
	- Check✅ Disable access control
	- Uncheck ☐ Native opengl
	
	XLaunch should show in your windows system tray. 

c. In your distribution command line, set your DISPLAY env variable to your WSL IP address and set LIBGL_ALWAYS_INDIRECT. 

	Example:
	
	$ cat /etc/resolv.conf
	  # This file was automatically generated by WSL. To stop automatic generation of this file, add the following entry to /etc/wsl.conf:
      # [network]
	  # generateResolvConf = false
	  nameserver 172.25.240.1
	$ export DISPLAY=172.25.240.1:0.0
	$ export LIBGL_ALWAYS_INDIRECT=1
	
d. [Optional] Also put the above export commands in ~/.bashrc

e. Install test apps and run a test

	$ run sudo apt install x11-apps -y
	$ xeyes
	
	Xeyes will produce a window with eyes that follow the mouse cursor. 

7. Run the application

	$ ./build/PAE3D