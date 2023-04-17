# Physics-based Animation 4860-1081 2023S

[![Build_Win](https://github.com/PBA-2023S/pba/actions/workflows/windows.yml/badge.svg)](https://github.com/PBA-2023S/pba/actions/workflows/windows.yml)
[![Build_Linux](https://github.com/PBA-2023S/pba/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/PBA-2023S/pba/actions/workflows/ubuntu.yml)

![teaser](doc/rep_image.png)

Lecture at graduate school of information science and technology in the university of Tokyo, spring semester, 2023

#### ITC-LMS 

For Slack and GitHub Classroom invitaitons

- https://itc-lms.ecc.u-tokyo.ac.jp/lms/course?idnumber=20214860-10810F01

#### Instructor
Dr. Nobuyuki Umetani 
- email: umetani@ci.i.u-tokyo.ac.jp
- url: http://www.nobuyuki-umetani.com/
- lab's website: https://cgenglab.github.io/en/

#### Time
- Monday 2rd period, 10:25pm - 12:10pm

#### Course Description

Computer-generated images are everywhere in movies, video games, and VR. This course is an introduction to the techniques to animate objects in computer graphics based on the law of physics. The aim of the course is to get familiar with applied mathematics such as linear algebra, vector analysis, partial differential equations, variational principle, optimization, and numerical analysis through the animation techniques for particle systems, rigid bodies, elastic bodies. There are C++/Python programming assignments to acquire research-oriented graphics programming skills. 

Topics:
- mass-spring simulation
- rigid body simulation
- elastic body simulation
- cloth and hair modeling & simulation
- collision-detection using spatial hashing
- finite boundary method



## Lecture Schedule

| Day | Topic | Assignment | Slide |
|:----|:---|:---|-----|
| (1)<br> Apr. 10 | **Introduction**<br> |  | [[1]](http://www.nobuyuki-umetani.com/pba2023s/introduction.pdf) |
| (2)<br> Apr. 17 | **Data Structure**<br>data structure for simulation<br/>Implicit surface | [task00](task00/README.md) | [[2]](http://www.nobuyuki-umetani.com/pba2023s/data_structure.pdf) |
| (3)<br> Apr. 24 | **Time Integration**<br/> Newtonian-mechanics<br/> backward & forward Euler method,<br/> particle system | task01 | |
| (4)<br> May 1 | **Broad-phase Collision Detection**<br/>princepal component analysis<br>sort & sweep method <br/>bounding volume hierarchy | task02 |  |
| (5)<br> May 8 | **Numerical Optimization**<br/>Hessian & Jacobian,<br>Newton-Raphson method | task03 |  |
| (6)<br> May 15 | **Simple Deformation Energy**<br/>mass-spring system | task04 |  |
| (7)<br> May 22 | **Solving Large Linear System**<br/>Sparse matrix data structure<br/>Conjugate gradient method | task05 |  |
| (8)<br> Jun. 5 | **Optimization with Constraint**<br/>Lagrange multiplier method<br>**Rigid Body Dynamics**<br>Rotation representation | task06 |  |
| (9)<br> Jun. 12 | **Rigid Body Dynamics2**<br/>inertia tensor, <br/>angular velocity<br>impulse based method | task07 |  |
| (10)<br> Jun. 19 | Lagrangian mechanics<br />Variational time integration<br />**Continuum Mechanics**<br> tensor | task08 | |
| (11)<br> Jun. 26 | **Continuum Mechanics2**<br>Mesh interpolation<br />Tensor | task09 |  |
| (12)<br> Jul. 3 | **Geometric Deformation**<br> singular value decomposition,<br> shape matching method,<br> linear blend skinning,<br> as-rigid-as possible deformation | task10 |  |
| (13)<br> Jul. 10 | **Advanced Interpolation**<br> mean value coordinate,<br> radial based function |  |  |

#### Slides

- [[1] introduction](http://www.nobuyuki-umetani.com/pba2023s/introduction.pdf)
- [[2] data_structure](http://www.nobuyuki-umetani.com/pba2023s/data_structure.pdf)
- [[3] cpp](http://www.nobuyuki-umetani.com/pba2023s/cpp.pdf)
- [[4] git](http://www.nobuyuki-umetani.com/pba2023s/git.pdf)
- [[5]newtonian_mechanics ](http://www.nobuyuki-umetani.com/pba2023s/newtonian_mechanics.pdf)

  


## Grading

- 20% lecture attendance
  - Attendance is counted based on writing a secret keyword on LMS. The keyword is announced for each lecture.  
- 80% small assignemnts
  - see below

#### Assignemnts

There are many small programming assignments. To do the assignments, you need to create your own copy of this repository through **GitHub Classroom**.  These assignements needs to be submitted using **pull request** functionality of the GitHub. Look at the following document. 

[How to Submit the Assignments](doc/submit.md)

| Task ID                    | Title                        | Thumbnail                                  |
| :------------------------- | :--------------------------- | :----------------------------------------- |
| [task00](task00/README.md) | build C++ Program with CMake | <img src="task00/preview.png" width=100px> |

#### Policy

- Do the assignment by yourself. Don't share the assignments with others.
- Don't post the answers of the assignment on Slack 
- Late submission of an assignment is subject to grade deduction
- Score each assignment will not be open soon (instructor needs to adjust weights of the score later)



## Reading Material

- [Ten Min Physics (Youtube channel)](https://www.youtube.com/@TenMinutePhysics/videos)
- [Physically Based Modeling: Principles and Practice, Siggraph '97 Course notes by Dr. Baraff](http://www.cs.cmu.edu/~baraff/sigcourse/index.html)
- [Physics-Based Animation  by Kenny Erleben et al. (free textobook about rigid body dynamics)](https://iphys.wordpress.com/2020/01/12/free-textbook-physics-based-animation/)
- [Dynamic Deformables: Implementation and Production Practicalities, SIGGRAPH 2020 Courses](http://www.tkim.graphics/DYNAMIC_DEFORMABLES/)
- [Awesome Computer Graphics (GitHub)](https://github.com/luisnts/awesome-computer-graphics)
- [Skinning: Real-time Shape Deformation SIGGRAPH 2014 Course](https://skinning.org/)


#### My Past Lectures
- [Physics-based Animation 2021S](https://github.com/nobuyuki83/Physics-based_Animation_2021S)
- [Applied Computer Graphics 2022S](https://github.com/nobuyuki83/Applied_Computer_Graphics_2022S)
