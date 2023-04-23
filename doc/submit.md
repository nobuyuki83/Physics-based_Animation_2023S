# How to Submit the Assignment 

There are many small programming assignments. These assignments need to be submitted using **pull request** functionality of GitHub. 



## Making Your Repository using GitHub Classroom

![](../doc/githubclassroom.png)

The assignments need to be submitted using the "pullrequest" functionality of GitHub. Using the system called "GitHub Classroom", each student makes his/her own private repository that is a copy of "https://github.com/PBA-2023S/pba". If a student has a GitHub account named `<username>`, the name of the repository will be `pba-<username>`. The private repository is only visible to the student and the instructor. In the second class, you will be shown how to make your class repository using GitHub Classroom. 



## Overview

![](../doc/branchstructure.png)

Let's assume you are looking at the repository `pba-<username>` (e.g., pba-nobuyuki83) and the  `task<task number>` is the assignment (e.g., task2). The submission is made by

1. create a branch of the name `task<task number>`
2. follow the instruction written in `pba-<username>/task<task number>/README.md`
3. push the repository with the branch `task<task number>`
4. make a pull request on the GitHub page
5. Instructor will close the pull request after grading. 

   

## Setup C++ Programming Environment

First of all, you need to set up C++ Programming environment (git, cmake, c++ compiler)

- [How to Set Up C++ Programming Environment](../doc/setup_env.md)



## Clone the Repository

If you don't have the local repository in your computer, clone it from the remote repository

```bash
$ git clone https://github.com/PBA-2023S/pba-<username>.git
```

**Before doing each assignment**, synchronize the local repository to the remote repository.

```bash
$ cd pba-<username>   # go to the local repository
$ git checkout main   # set the main branch as the current branch
$ git fetch origin main   # download the main branch from the remote repository
$ git reset --hard origin/main   # reset the local main branch same as remote repository
```



## Setup Libraries

For all the assignments, we use GLFW Library for OpenGL visualization. Please take a look at the following document

- [How to Set Up GLFW Library](../doc/setup_glfw.md)

Install [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page), which is a library for matrix operation. Please follow the following document for setting up.    

- [How to set up the Eigen Library](../doc/setup_eigen.md)  



## Make a Branch for Each Assignment

Create the `task<number>` branch and set it as the current branch. For `task01` the commands look like

```bash
$ git branch task01    # create task1 branch
$ git checkout task01  # switch into the task1 branch
$ git branch -a       # make sure you are in the task1 branch
```

Now, you are ready to edit the code and do the assignment!



## Do the Assignment

 Edit the code and this markdown document.



## Upload the Change

After you finish editing, you submit the updates pushing to the `task<number>` branch of the remote repository. For `task1` the commands look like

```bash
cd pba-<username>    # Go to the top of the repository
git branch -a  # Make sure again you are in the task1 branch
git status  # check the changes (typically few files are shown to be "updated")
git add .   # stage the changes
git status  # check the staged changes (typically few files are shown to be "staged")
git commit -m "task1 finished"   # The comment can be anything
git push --set-upstream origin task01  # update the task1 branch of the remote repository
```



## Make a Pull Request

Go to the GitHub webpage `https://github.com/PBA-2023S/pba-<username>` . If everything looks good on this page, make a pull request. 

![](../doc/pullrequest.png)

### Notes

- Do not submit multiple pull requests. Only the first pull request is graded
- Do not close the pull request by yourself. The instructor will close the pull request
- If you mistakenly merge the pull request, it's OK, but be careful not to merge next time. 



## Tips for Git / GitHub
- Do not put any binary files (e.g., executable, binary files) except for image files. Put only readable files in the repository.

- If many binary files are shown when I type `git status`, write the files/directories you want to ignore into `pba-<username>/.gitignore`.

- If something goes wrong, put the local repository somewhere and start again from **cloning the repository**.

- If you get errors, try to read them. If you do not understand what the error says, search for the error message on the web.

- If you mistakenly submit the assignment in the `main` branch, make a branch `task<number>` and submit again. 
