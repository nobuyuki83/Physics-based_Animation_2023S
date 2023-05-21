# Task05: Gradient Descent Method for Mass-Spring Simulation

![preview](preview.png)

**Deadline: May 25th (Thu) at 15:00pm**

----

## Before Doing Assignment

If you have not done the [task00](../task00), do it first to set up the C++ graphics development environment.

Follow [this document](../doc/submit.md) to submit the assignment, In a nutshell, before doing the assignment,

- install `eigen` library following  [this document](../doc/setup_eigen.md)
- make sure you synchronized the `main ` branch of your local repository  to that of remote repository.
- make sure you created branch `task05` from `main` branch.
- make sure you are currently in the `task05` branch (use `git branch -a` command).

Now you are ready to go!

---

## Problem 1

Run the code as it is . Then, take a screenshot image of a program window. 
Save the screenshot image overwriting `task05/problem1.png`

![problem1](problem1.png)


## Problem 2

This program means to compute deformation of skirt as a mass-spring system. The total energy of the system is shown in the standard output. And that energy should be minimized.

The current code does not compute the gradient of the elastic energy of a spring. 
Hence, the energy minimization does not lead to elastic deformation.      

Write a few lines of code around `line #31` to accelerate the search. After running the program for a while, save the screenshot image overwriting `task05/problem2.png`
You may need to adjust the learning rate at `line #76` to reach the convergence. 

![problem2](problem2.png)

Write down the converged minimum value of the energy by updating the text below.

***minimum energy of the system = ????***


## After Doing the Assignment

After modifying the code, push the code and submit a pull request.




## Notes

- Do not submit multiple pull requests. Only the first pull request is graded
- Do not close the pull request by yourself. The instructor will close the pull request
- If you mistakenly merge the pull request, it's OK, but be careful not to merge next time. 
