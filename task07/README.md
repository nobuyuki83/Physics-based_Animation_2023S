# Task07: Dirichlet's Energy Minimization with Gauss-Seidel Method

![preview](preview.png)

**Deadline: June 15th (Thu) at 15:00pm**

----

## Before Doing Assignment

If you have not done the [task00](../task00), do it first to set up the C++ graphics development environment.

Follow [this document](../doc/submit.md) to submit the assignment, In a nutshell, before doing the assignment,

- install `eigen` library following  [this document](../doc/setup_eigen.md)
- make sure you synchronized the `main ` branch of your local repository  to that of remote repository.
- make sure you created branch `task07` from `main` branch.
- make sure you are currently in the `task07` branch (use `git branch -a` command).

Now you are ready to go!

---

## Problem 1

Then, run the code. Take a screenshot image. 
Save the screenshot image overwriting `task07/problem1.png`

![problem1](problem1.png)


## Problem 2

This code means to minimize Dirichlet's energy using Gauss-Seidel method. 

The Dirichlet's energy here is defined as the sum of the squared difference of the values at the end points of the edges of the grid (see ```line #81```).      

Write a few lines of code around `line #27` to minimize Dirichlet's energy using Gauss-Seidel method. Do not write complicated or long code. 

Save the screenshot image overwriting `task07/problem2.png`

![problem2](problem2.png)


## After Doing the Assignment

After modifying the code, push the code and submit a pull request.


## Notes

- Do not submit multiple pull requests. Only the first pull request is graded
- Do not close the pull request by yourself. The instructor will close the pull request
- If you mistakenly merge the pull request, it's OK, but be careful not to merge next time. 
