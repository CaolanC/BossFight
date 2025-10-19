# School of Computing &mdash; Year 4 Project Proposal Form

> Edit (then commit and push) this document to complete your proposal form.
> Make use of figures / diagrams where appropriate.
>
> Do not rename this file.

## SECTION A

|                     |                 |
|---------------------|-----------------|
|Project Title:       | Perfect         |
|Student 1 Name:      | Caolan Cochrane |
|Student 1 ID:        | 22490802        |
|Student 2 Name:      | Ethan Doyle     |
|Student 2 ID:        | 22497082        |
|Project Supervisor:  | Mark Humphrys   |

> Ensure that the Supervisor formally agrees to supervise your project; this is only recognised once the
> Supervisor assigns herself/himself via the project Dashboard.
>
> Project proposals without an assigned
> Supervisor will not be accepted for presentation to the Approval Panel.

## SECTION B

> Guidance: This document is expected to be approximately 3 pages in length, but it can exceed this page limit.
> It is also permissible to carry forward content from this proposal to your later documents (e.g. functional
> specification) as appropriate.
>
> Your proposal must include *at least* the following sections.


### Introduction

This project covers areas and fields such as real-time 3D graphics and collaborative software development. 
It will involve interactive system design as well as visual computing, audio visualisation and will serve as an educational tool for subjects such as mathematics. 
It will involve a co-operative learning aspect, as collaborative spaces will allow multiple users to interact with each other in shared spaces for development.

### Outline

Our proposed project, Perfect, involves the development of a C++ framework built on OpenGL and SDL3 (for rendering, graphics and textures) for collaborative work, through a collaborative space with shared access, which will allow co-operative interaction and visualisation in 3D environments. 
This will result in useful application in many areas, such as 3D model design, game development, audio visualisation, as an educational tool for 3D maths visualisation and interaction with a possible core focus on interior design.

A key goal of this project is to support real-time collaboration, editing, and audio visualisation within 3D spaces. 
We want users to be able to view and edit 3D scenes collaboratively, so the project will contain shared state management. 
We will include importing and exporting models, as well as tools for object manipulation, transformation and colouring + texturing. 
The project should also provide users with a way to demonstrate geometric transformations, vector operations and/or physics simulations to help with educational endeavors.

To facilitate the above, the system will be designed with modularity in mind and will include core components for rendering, input management, scene management, as well as components for models, meshes and textures. 
Models will be imported and exported through the use of glTF and tinyglTF. 
We will also achieve collaborative synchronization through the use of a C++ WebSockets library.

Overall, the project will be a framework suitable for creative collaboration and teaching in real-time 3D interactive environments.

### Background

Last year our third year project was about a procedurally generated world game in the web using Three.js and WebSockets for a listen-server architecture. 
For our project this year we wanted to follow suit in the realm of computer graphics, but not strictly game related. 
We decided to use a different language, C++, and a different graphics library, OpenGL.

After some decision-making, we decided to go at the project from a more practical / educational angle and decided on the collaborative spaces idea.

### Achievements

The completed project will deliver a working framework for interactive, collaborative work & visualisation. 
The system will support a wide range of core functionalities such as graphics rendering, model importing, viewing, editing and exporting, user input, scene handling & rendering and real-time updates to shared environments.

The intended users of this project range from software developers to educators & students. 
Developers can use it as a base for apps, tools or games while educators may use it to demonstrate geometry in an engaging format.

### Justification

The project will be useful in creative & educational contexts. 
In creative and technical fields such as model design, simulation & games, the framework offers a base for experimentation and furthering design. 
It supports projects that require real-time collaboration & rendering.

In educational contexts, it offers a platform for interacting with complex mathematical concepts and problems, as well as physical concepts. 
Interactive 3D environments allow learners to explore concepts like geometry and transformations.

### Programming language(s)

Perfect will be written in C++.

### Programming tools / Tech stack

Software that will be used in this project includes:

Simple DirectMedia Layer (SDL): SDL is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL. 
[QUOTE NUM site: https://www.libsdl.org/] 

OpenGL: OpenGL is a cross-platform API that enables developers of software for PC, workstation, and supercomputing hardware to create high-performance, visually compelling graphics software applications. 
[QUOTE NUM site: https://www.khronos.org/opengl/] 

WebSockets: WebSockets provides a persistent full-duplex communication channel between the client and server - essentially, a two-way communication session between a user’s client and a server. 
Helps to enable the listen-server architecture.

CMake: CMake is an open source, cross-platform family of tools designed to build, test, and package software. 
[QUOTE NUM site: https://cmake.org/about/] We are using this to build all necessary components in our project.

Compilers used:

GNU Compiler Collection (GCC): A collection of compilers that support various programming languages. 
We will be using this for C++.

MinGW: An open-source and free compiler that brings the GNU Compiler Collection to Windows. 
In this project, it is used for its ability to use the GCC compiler for C++ in Windows.

IDEs:

CLion: CLion is a cross-platform IDE for C and C++ development. 
We will be using it for minimal bugs and natural CMake + MinGW support.

### Hardware

Might need a computer with a powerful GPU to show off the project (specifically for demo), but not 100% necessary.

### Learning Challenges

Since we are using new libraries that are unfamiliar to us in the realm of computer graphics, OpenGL and SDL will have to be learnt to successfully work through the project. 
These software are required for the rendering of mostly everything in our system, from the window, to objects and models.

We will also have to learn libraries that we are using in conjunction with these, such as glTF and tinyglTF, and how they read + format files.

### Breakdown of work

Caolan:

Ethan:

### Risk Register

Description:  Programming overheads. 
These can be caused by many different aspects of our project. 
One to watch out for is an overhead caused by physics detection - physics detection needs to be as optimised as possible. 
Another possible overhead is when many models are loaded onto a scene, which could happen if someone is using the space for a very large project.

Likelihood: Almost guaranteed at some point with a project of this manner.

Severity: Depends on the overhead.

Mitigation: Reducing the overheads through optimisation and faster methods and using techniques such as occlusion culling for rendering.

\

Description: Hardware constraints. 
This risk can be tied with the above, as poorer hardware can cause lag in areas that better hardware may not experience.

Likelihood: Depends on the level of graphics used and the respective GPU of the hardware.

Severity: Same as above.

Mitigation: Building the game with lower end computers/hardware in mind.


#### Student 1

> *Student 1 should complete this section.*

#### Student 2

> *Student 2 should complete this section.*

## Example

> Example: Here's how you can include images in markdown documents...

<!-- Basically, just use HTML! -->

<p align="center">
  <img src="./res/cat.png" width="300px">
</p>

